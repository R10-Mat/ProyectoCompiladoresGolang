//
// Created by Usuario on 21/6/2026.
//

#include "GenCode.h"
#include "ast.h"
#include <iostream>
#include <sstream>
#include <unordered_set>

using namespace std;

// =============================================================================
// GenCode — Generacion de codigo ensamblador x86-64 (sintaxis AT&T)
// =============================================================================
// Simplificaciones deliberadas de esta primera version del backend (ver
// grammar.txt "Notas de implementacion" y el plan de la sesion):
//   - Todo valor cabe en 8 bytes: int/bool/string(puntero)/pointer viajan en
//     un registro entero; float64 viaja en %xmm0 (SSE real).
//   - struct y array son SIEMPRE representados como un puntero de 8 bytes a
//     un bloque reservado con malloc (slots*8 bytes, 8 bytes por campo o
//     elemento). Esto simplifica mucho el codigo (parametros, asignaciones y
//     receptores de metodo no necesitan casos especiales por tamaño) al
//     costo de no tener semantica de copia por valor real de Go para
//     structs/arrays (una asignacion "m2 = m" copia el puntero, no los
//     datos). Es un tradeoff deliberado de alcance, no un bug.
//   - Cada funcion reserva locales + un margen fijo de "temporales" (para
//     literales compuestos evaluados como sub-expresion) en vez de contar
//     temporales con precision.
// =============================================================================

namespace {

const char* ARG_REGS_INT[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

// La ABI SysV exige %rsp alineado a 16 bytes en cada "call". pushq %rbp ya
// deja el frame en 0 mod 16 (el "call" que entra a la funcion desalinea en
// 8); el propio subq del prologo debe conservar ese multiplo de 16, o cada
// "call" dentro de la funcion (incluido printf, que hace spills SSE
// alineados internamente) puede fallar con SIGSEGV.
int tamanoFrameAlineado(int slots) {
    int bytes = slots * 8;
    return (bytes + 15) & ~15;
}

// Forward decl: usado por emitirArgumentos, definido mas abajo junto con emitirValorFloat.
void emitirConversionAType(GenCode* gc, const Semantic_types& origen, const Semantic_types& destino);

// ---- Helpers que no forman parte del patron Visitor (funciones libres) ----

void registrarVariable(GenCode* gc, const string& nombre, const Semantic_types& tipo) {
    if (gc->entornoFuncion) {
        gc->tipos_var[nombre] = tipo;
        gc->memoria[nombre] = gc->reservarSlots(1);
    } else {
        gc->tipos_var_globales[nombre] = tipo;
        gc->memoriaGlobal[nombre] = true;
    }
}

// Almacena el valor actual (en %rax o %xmm0, segun tipoValor) en la
// ubicacion de `target`. Soporta variables simples, campos de struct,
// elementos de arreglo y dereferencia de puntero.
void emitirAlmacenar(GenCode* gc, Exp* target, const Semantic_types& tipoValor) {
    bool esFloat = tipoValor.getKind() == Semantic_types::FLOAT64;

    if (auto on = dynamic_cast<OperandNameExp*>(target)) {
        bool esGlobal = gc->memoriaGlobal.count(on->name) > 0;
        string ubicacion = esGlobal ? (on->name + "(%rip)")
                                     : (to_string(gc->memoria[on->name]) + "(%rbp)");
        if (esFloat) gc->out << "  movsd %xmm0, " << ubicacion << "\n";
        else gc->out << "  movq %rax, " << ubicacion << "\n";
        return;
    }

    // Caso general (campo/elemento/dereferencia): preservar el valor,
    // calcular la direccion destino, y luego almacenar.
    if (esFloat) gc->out << "  subq $8, %rsp\n  movsd %xmm0, (%rsp)\n";
    else gc->out << "  pushq %rax\n";

    gc->emitirDireccion(target); // %rax = direccion destino
    gc->out << "  movq %rax, %r10\n";

    if (esFloat) {
        gc->out << "  movsd (%rsp), %xmm0\n  addq $8, %rsp\n";
        gc->out << "  movsd %xmm0, (%r10)\n";
    } else {
        gc->out << "  popq %rax\n";
        gc->out << "  movq %rax, (%r10)\n";
    }
}

// Evalua cada expresion en `args` (dejando el resultado en pila) y luego
// carga los registros de la ABI SysV respetando que enteros/punteros y
// flotantes tienen contadores de registro independientes. `offsetInt`
// permite reservar los primeros N registros enteros (usado por llamadas a
// metodo, donde %rdi ya esta ocupado por el receptor). Si `tiposEsperados`
// no es null, se usa el tipo DECLARADO de cada parametro (no el tipo del
// argumento) para decidir clase de registro y aplicar la conversion
// INT->FLOAT64 cuando el TypeChecker permitio una promocion.
void emitirArgumentos(GenCode* gc, const vector<Exp*>& args, int offsetInt,
                       const vector<Semantic_types>* tiposEsperados = nullptr) {
    size_t n = args.size();
    if (n == 0) return;

    vector<Semantic_types> tiposArgs(n);
    vector<int> regIndex(n);
    int ci = offsetInt, cf = 0;
    for (size_t i = 0; i < n; ++i) {
        Semantic_types tipoDestino = (tiposEsperados && i < tiposEsperados->size())
            ? (*tiposEsperados)[i] : gc->tipoDe(args[i]);
        tiposArgs[i] = tipoDestino;
        if (tipoDestino.getKind() == Semantic_types::FLOAT64) regIndex[i] = cf++;
        else regIndex[i] = ci++;
    }

    for (size_t i = 0; i < n; ++i) {
        Semantic_types tipoOrigen = gc->tipoDe(args[i]);
        args[i]->accept(gc);
        emitirConversionAType(gc, tipoOrigen, tiposArgs[i]);
        if (tiposArgs[i].getKind() == Semantic_types::FLOAT64)
            gc->out << "  subq $8, %rsp\n  movsd %xmm0, (%rsp)\n";
        else
            gc->out << "  pushq %rax\n";
    }
    for (size_t j = 0; j < n; ++j) {
        size_t i = n - 1 - j;
        if (tiposArgs[i].getKind() == Semantic_types::FLOAT64)
            gc->out << "  movsd (%rsp), %xmm" << regIndex[i] << "\n  addq $8, %rsp\n";
        else
            gc->out << "  popq " << ARG_REGS_INT[regIndex[i]] << "\n";
    }
}

// Emite una llamada a printf con el formato correcto segun el tipo del
// valor ya evaluado (en %rax o %xmm0).
void emitirPrintDeTipo(GenCode* gc, const Semantic_types& t) {
    switch (t.getKind()) {
        case Semantic_types::FLOAT64:
            gc->out << "  leaq print_fmt_f(%rip), %rdi\n  movq $1, %rax\n  call printf@PLT\n";
            break;
        case Semantic_types::STRING:
            gc->out << "  movq %rax, %rsi\n  leaq print_fmt_s(%rip), %rdi\n  movq $0, %rax\n  call printf@PLT\n";
            break;
        default: // INT, BOOL, POINTER, ARRAY, STRUCT (se imprimen como entero/direccion)
            gc->out << "  movq %rax, %rsi\n  leaq print_fmt_ld(%rip), %rdi\n  movq $0, %rax\n  call printf@PLT\n";
            break;
    }
}

// Evalua expresiones constantes enteras (mismo criterio que TypeCheker.cpp,
// pero leyendo `tipos.valores_const`, ya resuelto por el chequeo semantico).
bool tryConstFoldIntImpl(GenCode* gc, Exp* e, long& out) {
    if (!e) return false;
    if (auto lit = dynamic_cast<BasicLitExp*>(e)) {
        if (lit->tipoLiteral == Token::INT_LIT) { out = stol(lit->valor); return true; }
        return false;
    }
    if (auto on = dynamic_cast<OperandNameExp*>(e)) {
        auto it = gc->tipos.valores_const.find(on->name);
        if (it == gc->tipos.valores_const.end()) return false;
        out = it->second;
        return true;
    }
    if (auto pe = dynamic_cast<ParenExp*>(e)) return tryConstFoldIntImpl(gc, pe->expresion, out);
    if (auto un = dynamic_cast<UnaryExprExp*>(e)) {
        long v;
        if (!tryConstFoldIntImpl(gc, un->expresion, v)) return false;
        if (un->op == NEG_OP) { out = -v; return true; }
        if (un->op == POS_OP) { out = v; return true; }
        return false;
    }
    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        long l, r;
        if (!tryConstFoldIntImpl(gc, bin->left, l) || !tryConstFoldIntImpl(gc, bin->right, r)) return false;
        switch (bin->op) {
            case PLUS_OP:  out = l + r; return true;
            case MINUS_OP: out = l - r; return true;
            case MUL_OP:   out = l * r; return true;
            case DIV_OP:   if (r == 0) return false; out = l / r; return true;
            case MOD_OP:   if (r == 0) return false; out = l % r; return true;
            default: return false;
        }
    }
    return false;
}

bool tryConstFoldFloatImpl(Exp* e, double& out) {
    if (!e) return false;
    if (auto lit = dynamic_cast<BasicLitExp*>(e)) {
        if (lit->tipoLiteral == Token::FLOAT_LIT || lit->tipoLiteral == Token::INT_LIT) {
            out = stod(lit->valor);
            return true;
        }
        return false;
    }
    if (auto pe = dynamic_cast<ParenExp*>(e)) return tryConstFoldFloatImpl(pe->expresion, out);
    if (auto un = dynamic_cast<UnaryExprExp*>(e)) {
        double v;
        if (!tryConstFoldFloatImpl(un->expresion, v)) return false;
        if (un->op == NEG_OP) { out = -v; return true; }
        if (un->op == POS_OP) { out = v; return true; }
        return false;
    }
    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        double l, r;
        if (!tryConstFoldFloatImpl(bin->left, l) || !tryConstFoldFloatImpl(bin->right, r)) return false;
        switch (bin->op) {
            case PLUS_OP:  out = l + r; return true;
            case MINUS_OP: out = l - r; return true;
            case MUL_OP:   out = l * r; return true;
            case DIV_OP:   out = l / r; return true;
            default: return false;
        }
    }
    return false;
}

// Carga el valor de `e` (ya conocido de tipo `tipoConocido`) en %xmm0,
// convirtiendo desde entero si hace falta (ej. "1 + 2.5").
void emitirValorFloat(GenCode* gc, Exp* e, const Semantic_types& tipoConocido) {
    e->accept(gc);
    if (tipoConocido.getKind() != Semantic_types::FLOAT64) {
        gc->out << "  cvtsi2sdq %rax, %xmm0\n";
    }
}

// Convierte el valor ya evaluado (en %rax o %xmm0, segun `origen`) al
// registro/formato de `destino`, si hace falta. Necesario en cualquier
// punto donde el TypeChecker permitio una promocion INT->FLOAT64 (ej.
// "var x float64 = 0", "return 0" en una funcion que devuelve float64, o
// pasar un literal entero a un parametro float64): el chequeo semantico
// valida que la promocion es legal, pero la conversion real (cvtsi2sd) la
// tiene que emitir el generador de codigo.
void emitirConversionAType(GenCode* gc, const Semantic_types& origen, const Semantic_types& destino) {
    if (origen.getKind() == Semantic_types::INT && destino.getKind() == Semantic_types::FLOAT64) {
        gc->out << "  cvtsi2sdq %rax, %xmm0\n";
    } else if (origen.getKind() == Semantic_types::FLOAT64 && destino.getKind() == Semantic_types::INT) {
        gc->out << "  cvttsd2siq %xmm0, %rax\n";
    }
}

} // namespace

// -----------------------------------------------------------------------------
// Helpers miembro
// -----------------------------------------------------------------------------

int GenCode::reservarSlots(int n) {
    if (n < 1) n = 1;
    int base = offset - (n - 1) * 8;
    offset = base - 8;
    return base;
}

bool GenCode::tryConstFoldInt(Exp* e, long& out) { return tryConstFoldIntImpl(this, e, out); }
bool GenCode::tryConstFoldFloat(Exp* e, double& out) { return tryConstFoldFloatImpl(e, out); }

string GenCode::etiquetaParaString(const string& valor) {
    auto it = stringLiterals.find(valor);
    if (it != stringLiterals.end()) return it->second;
    string etiqueta = "str_lit_" + to_string(stringLitCont++);
    stringLiterals[valor] = etiqueta;
    out << ".data\n" << etiqueta << ": .string \"" << valor << "\"\n.text\n";
    return etiqueta;
}

Semantic_types GenCode::tipoDe(Exp* e) {
    if (!e) return Semantic_types(Semantic_types::NOTYPE);

    if (auto lit = dynamic_cast<BasicLitExp*>(e)) {
        switch (lit->tipoLiteral) {
            case Token::INT_LIT:  case Token::RUNE_LIT: return Semantic_types(Semantic_types::INT);
            case Token::FLOAT_LIT: return Semantic_types(Semantic_types::FLOAT64);
            case Token::STRING_LIT: return Semantic_types(Semantic_types::STRING);
            default: return Semantic_types(Semantic_types::NOTYPE);
        }
    }
    if (auto on = dynamic_cast<OperandNameExp*>(e)) {
        if (on->name == "true" || on->name == "false") return Semantic_types(Semantic_types::BOOL);
        auto it = tipos_var.find(on->name);
        if (it != tipos_var.end()) return it->second;
        auto itG = tipos_var_globales.find(on->name);
        if (itG != tipos_var_globales.end()) return itG->second;
        auto itFn = tipos.tabla_funciones.find(on->name);
        if (itFn != tipos.tabla_funciones.end()) return itFn->second.return_types;
        return Semantic_types(Semantic_types::NOTYPE);
    }
    if (auto pe = dynamic_cast<ParenExp*>(e)) return tipoDe(pe->expresion);
    if (auto un = dynamic_cast<UnaryExprExp*>(e)) {
        Semantic_types t = tipoDe(un->expresion);
        if (un->op == ADDR_OP) return Semantic_types::makePointer(t);
        if (un->op == DEREF_OP) return t.baseType() ? *t.baseType() : Semantic_types(Semantic_types::NOTYPE);
        return t;
    }
    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        switch (bin->op) {
            case AND_OP: case OR_OP:
            case EQ_OP: case NEQ_OP: case LT_OP: case GT_OP: case LTE_OP: case GTE_OP:
                return Semantic_types(Semantic_types::BOOL);
            default: {
                Semantic_types izq = tipoDe(bin->left);
                Semantic_types der = tipoDe(bin->right);
                if (izq.getKind() == Semantic_types::FLOAT64 || der.getKind() == Semantic_types::FLOAT64)
                    return Semantic_types(Semantic_types::FLOAT64);
                return izq;
            }
        }
    }
    if (auto sel = dynamic_cast<SelectorExp*>(e)) {
        Semantic_types base = tipoDe(sel->expresion);
        Semantic_types structTipo = base.isPointer() ? (base.baseType() ? *base.baseType() : base) : base;
        return structTipo.fieldType(sel->campo);
    }
    if (auto idx = dynamic_cast<IndexExp*>(e)) {
        Semantic_types base = tipoDe(idx->expresion);
        if (base.isArray()) return base.baseType() ? *base.baseType() : Semantic_types(Semantic_types::NOTYPE);
        if (base.getKind() == Semantic_types::STRING) return Semantic_types(Semantic_types::INT);
        return Semantic_types(Semantic_types::NOTYPE);
    }
    if (auto sl = dynamic_cast<SliceExp*>(e)) return tipoDe(sl->expresion);
    if (auto ta = dynamic_cast<TypeAssertionExp*>(e)) return ta->tipo->accept(&tipos);
    if (auto cl = dynamic_cast<CompositeLitExp*>(e)) return cl->tipo->accept(&tipos);
    if (auto fl = dynamic_cast<FunctionLit*>(e)) return fl->tipo ? fl->tipo->accept(&tipos) : Semantic_types(Semantic_types::VOID);
    if (auto ar = dynamic_cast<ArgumentsExp*>(e)) {
        if (auto on2 = dynamic_cast<OperandNameExp*>(ar->funcion)) {
            Semantic_types::Kind k = Semantic_types::string_to_kind(on2->name);
            if (k != Semantic_types::NOTYPE) return Semantic_types(k);
            auto itT = tipos.tabla_tipos.find(on2->name);
            if (itT != tipos.tabla_tipos.end()) return itT->second;
            if (on2->name == "print" || on2->name == "println") return Semantic_types(Semantic_types::VOID);
            auto itFn = tipos.tabla_funciones.find(on2->name);
            if (itFn != tipos.tabla_funciones.end()) return itFn->second.return_types;
        }
        if (auto sel2 = dynamic_cast<SelectorExp*>(ar->funcion)) {
            Semantic_types receptor = tipoDe(sel2->expresion);
            Semantic_types base = receptor.isPointer() ? (receptor.baseType() ? *receptor.baseType() : receptor) : receptor;
            if (base.getKind() == Semantic_types::STRUCT) {
                auto itFn = tipos.tabla_funciones.find(base.structName() + "_" + sel2->campo);
                if (itFn != tipos.tabla_funciones.end()) return itFn->second.return_types;
            }
        }
        return tipoDe(ar->funcion);
    }
    return Semantic_types(Semantic_types::NOTYPE);
}

// Direccion (no el valor) de una expresion lvalue: variable simple, campo de
// struct, elemento de arreglo, o dereferencia de puntero.
void GenCode::emitirDireccion(Exp* e) {
    if (auto on = dynamic_cast<OperandNameExp*>(e)) {
        if (memoriaGlobal.count(on->name)) out << "  leaq " << on->name << "(%rip), %rax\n";
        else out << "  leaq " << memoria[on->name] << "(%rbp), %rax\n";
        return;
    }
    if (auto idx = dynamic_cast<IndexExp*>(e)) {
        idx->expresion->accept(this); // ya es el puntero base (arreglos son punteros, ver notas arriba)
        out << "  pushq %rax\n";
        idx->indice->accept(this);
        out << "  movq %rax, %rdi\n";
        out << "  popq %rax\n";
        out << "  leaq (%rax, %rdi, 8), %rax\n";
        return;
    }
    if (auto sel = dynamic_cast<SelectorExp*>(e)) {
        Semantic_types base = tipoDe(sel->expresion);
        Semantic_types structTipo = base.isPointer() ? (base.baseType() ? *base.baseType() : base) : base;
        sel->expresion->accept(this); // %rax = puntero al struct (uniforme, ver notas arriba)
        int idxCampo = structTipo.fieldIndex(sel->campo);
        if (idxCampo > 0) out << "  addq $" << (idxCampo * 8) << ", %rax\n";
        return;
    }
    if (auto un = dynamic_cast<UnaryExprExp*>(e)) {
        if (un->op == DEREF_OP) {
            un->expresion->accept(this); // %rax = direccion apuntada
            return;
        }
    }
    if (auto pe = dynamic_cast<ParenExp*>(e)) { emitirDireccion(pe->expresion); return; }

    cerr << "Error interno de codegen: no se puede tomar la direccion de esta expresion\n";
    exit(1);
}

void GenCode::emitStrConcatHelperSiHaceFalta() {
    out << "\n.globl str_concat\n";
    out << "str_concat:\n";
    out << "  pushq %rbp\n  movq %rsp, %rbp\n";
    out << "  pushq %rbx\n  pushq %r12\n  pushq %r13\n";
    out << "  movq %rdi, %r12\n  movq %rsi, %r13\n";
    out << "  movq %r12, %rdi\n  call strlen@PLT\n  movq %rax, %rbx\n";
    out << "  movq %r13, %rdi\n  call strlen@PLT\n  addq %rbx, %rax\n  addq $1, %rax\n";
    out << "  movq %rax, %rdi\n  call malloc@PLT\n  movq %rax, %rbx\n";
    out << "  movq %rbx, %rdi\n  movq %r12, %rsi\n  call strcpy@PLT\n";
    out << "  movq %rbx, %rdi\n  movq %r13, %rsi\n  call strcat@PLT\n";
    out << "  movq %rbx, %rax\n";
    out << "  popq %r13\n  popq %r12\n  popq %rbx\n";
    out << "  leave\n  ret\n";
}

// -----------------------------------------------------------------------------
// generar — punto de entrada
// -----------------------------------------------------------------------------

void GenCode::generar(Programa* programa) {
    // Nota: no corre tipos.TypeCheker() aqui -- se asume que el llamador ya
    // lo ejecuto (ver main.cpp, que separa la fase semantica de esta fase
    // de codegen para poder reportar cada una por separado).
    programa->accept(this);
}

// Parte Rayhan //

Semantic_types GenCode::visit(Programa* programa) {
    out << ".data\n";
    out << "print_fmt_ld: .string \"%ld \\n\"\n";
    out << "print_fmt_f: .string \"%f \\n\"\n";
    out << "print_fmt_s: .string \"%s \\n\"\n";
    out << "nl_fmt: .string \"\\n\"\n";

    entornoFuncion = false;
    for (auto decl : programa->listatopleveldecl) {
        if (dynamic_cast<ConstDecl*>(decl) || dynamic_cast<VarDecl*>(decl)) {
            decl->accept(this);
        }
    }
    for (auto& kv : memoriaGlobal) {
        out << kv.first << ": .zero 8\n";
    }

    out << "\n.text\n";

    // __init_globals corre los inicializadores de var/const globales antes
    // de main (equivalente a la inicializacion de paquete de Go real).
    out << "\n__init_globals:\n";
    out << "  pushq %rbp\n  movq %rsp, %rbp\n";
    string nombreAnterior = nombreFuncion;
    nombreFuncion = "__init_globals";
    for (auto& ini : inicializacionesGlobales) {
        if (!ini.second) continue;
        Semantic_types tipoDestino = tipos_var_globales.count(ini.first)
            ? tipos_var_globales[ini.first] : Semantic_types(Semantic_types::INT);
        Semantic_types tipoOrigen = tipoDe(ini.second);
        ini.second->accept(this);
        emitirConversionAType(this, tipoOrigen, tipoDestino);
        if (tipoDestino.getKind() == Semantic_types::FLOAT64)
            out << "  movsd %xmm0, " << ini.first << "(%rip)\n";
        else
            out << "  movq %rax, " << ini.first << "(%rip)\n";
    }
    // struct/array globales SIN inicializador explicito tambien necesitan su
    // buffer reservado aqui (ver misma nota en visit(VarSpec*)): su slot
    // solo guarda un puntero, que arranca en 0 si no se le asigna nada.
    {
        unordered_set<string> conInicializador;
        for (auto& ini : inicializacionesGlobales) conInicializador.insert(ini.first);
        for (auto& kv : tipos_var_globales) {
            const string& nombre = kv.first;
            const Semantic_types& t = kv.second;
            if ((t.isStruct() || t.isArray()) && !conInicializador.count(nombre)) {
                int slots = t.isArray() ? (int)t.arrayLength() : (int)t.fieldCount();
                if (slots < 1) slots = 1;
                out << "  movq $" << slots << ", %rdi\n  movq $8, %rsi\n  call calloc@PLT\n";
                out << "  movq %rax, " << nombre << "(%rip)\n";
            }
        }
    }
    out << "  leave\n  ret\n";
    nombreFuncion = nombreAnterior;

    for (auto decl : programa->listatopleveldecl) {
        if (dynamic_cast<FunctionDecl*>(decl) || dynamic_cast<MethodDecl*>(decl)) {
            decl->accept(this);
        }
    }

    if (necesitaConcatString) emitStrConcatHelperSiHaceFalta();

    out << "\n.section .note.GNU-stack,\"\",@progbits\n";
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(FunctionDecl* fun_decl) {
    entornoFuncion = true;
    memoria.clear();
    tipos_var.clear();
    offset = -8;
    nombreFuncion = fun_decl->name;

    const int MARGEN_TEMPORALES = 64; // slots extra para composite literals evaluados como sub-expresion
    int totalLocales = tipos.fun_var_contador.count(fun_decl->name) ? tipos.fun_var_contador[fun_decl->name] : 0;

    out << "\n.globl " << fun_decl->name << "\n";
    out << fun_decl->name << ":\n";
    out << "  pushq %rbp\n  movq %rsp, %rbp\n";
    out << "  subq $" << tamanoFrameAlineado(totalLocales + MARGEN_TEMPORALES) << ", %rsp\n";

    int ci = 0, cf = 0;
    if (fun_decl->lista_de_parametros) {
        for (auto pd : fun_decl->lista_de_parametros->parameterList) {
            Semantic_types t = pd->type->accept(&tipos);
            for (auto& nombre : pd->identifierlist->lista_ids) {
                int slot = reservarSlots(1);
                memoria[nombre] = slot;
                tipos_var[nombre] = t;
                if (t.getKind() == Semantic_types::FLOAT64) out << "  movsd %xmm" << cf++ << ", " << slot << "(%rbp)\n";
                else out << "  movq " << ARG_REGS_INT[ci++] << ", " << slot << "(%rbp)\n";
            }
        }
    }

    if (fun_decl->name == "main") out << "  call __init_globals\n";

    if (fun_decl->cuerpo) fun_decl->cuerpo->accept(this);

    // Si el control llega aqui por caida natural (sin "return" explicito,
    // ej. una funcion void, o "main"), %rax puede tener basura de la ultima
    // llamada (ej. el valor de retorno de printf); se limpia antes de salir.
    // Un "return expr;" salta directo a la etiqueta de abajo, sin pasar por
    // aqui, asi que esto no pisa un valor de retorno real.
    out << "  movq $0, %rax\n";
    out << ".end_" << fun_decl->name << ":\n";
    out << "  leave\n  ret\n";

    entornoFuncion = false;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(MethodDecl* method_decl) {
    entornoFuncion = true;
    memoria.clear();
    tipos_var.clear();
    offset = -8;
    string clave = method_decl->NombreTipoBase + "_" + method_decl->nombreMethod;
    nombreFuncion = clave;

    const int MARGEN_TEMPORALES = 64;
    int totalLocales = tipos.fun_var_contador.count(clave) ? tipos.fun_var_contador[clave] : 0;

    out << "\n.globl " << clave << "\n";
    out << clave << ":\n";
    out << "  pushq %rbp\n  movq %rsp, %rbp\n";
    out << "  subq $" << tamanoFrameAlineado(totalLocales + MARGEN_TEMPORALES) << ", %rsp\n";

    int ci = 0, cf = 0;

    auto itTipoBase = tipos.tabla_tipos.find(method_decl->NombreTipoBase);
    Semantic_types tipoBase = itTipoBase != tipos.tabla_tipos.end() ? itTipoBase->second : Semantic_types(Semantic_types::NOTYPE);
    Semantic_types tipoReceptor = method_decl->puntero ? Semantic_types::makePointer(tipoBase) : tipoBase;

    int slotRecep = reservarSlots(1);
    memoria[method_decl->nombreId] = slotRecep;
    tipos_var[method_decl->nombreId] = tipoReceptor;
    out << "  movq " << ARG_REGS_INT[ci++] << ", " << slotRecep << "(%rbp)\n";

    if (method_decl->lista_de_parametros) {
        for (auto pd : method_decl->lista_de_parametros->parameterList) {
            Semantic_types t = pd->type->accept(&tipos);
            for (auto& nombre : pd->identifierlist->lista_ids) {
                int slot = reservarSlots(1);
                memoria[nombre] = slot;
                tipos_var[nombre] = t;
                if (t.getKind() == Semantic_types::FLOAT64) out << "  movsd %xmm" << cf++ << ", " << slot << "(%rbp)\n";
                else out << "  movq " << ARG_REGS_INT[ci++] << ", " << slot << "(%rbp)\n";
            }
        }
    }

    if (method_decl->cuerpo) method_decl->cuerpo->accept(this);

    out << "  movq $0, %rax\n"; // ver nota en visit(FunctionDecl*)
    out << ".end_" << clave << ":\n";
    out << "  leave\n  ret\n";

    entornoFuncion = false;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ConstDecl* const_decl) {
    for (auto spec : const_decl->constspecList) spec->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ConstSpec* const_spec) {
    auto& nombres = const_spec->identifierList->lista_ids;
    auto& valores = const_spec->expresionlist->lista_exp;
    auto itNombre = nombres.begin();
    for (auto exp : valores) {
        Semantic_types tipoFinal = const_spec->tipo ? const_spec->tipo->accept(&tipos) : tipoDe(exp);
        registrarVariable(this, *itNombre, tipoFinal);
        if (!entornoFuncion) {
            inicializacionesGlobales.emplace_back(*itNombre, exp);
        } else {
            Semantic_types tipoOrigen = tipoDe(exp);
            exp->accept(this);
            emitirConversionAType(this, tipoOrigen, tipoFinal);
            bool esFloat = tipoFinal.getKind() == Semantic_types::FLOAT64;
            if (esFloat) out << "  movsd %xmm0, " << memoria[*itNombre] << "(%rbp)\n";
            else out << "  movq %rax, " << memoria[*itNombre] << "(%rbp)\n";
        }
        ++itNombre;
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(TypeDecl* type_decl) {
    for (auto spec : type_decl->typespecList) spec->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(TypeSpec* type_spec) { return tipos.visit(type_spec); }

Semantic_types GenCode::visit(VarDecl* var_decl) {
    for (auto spec : var_decl->varspecList) spec->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(VarSpec* var_spec) {
    auto& nombres = var_spec->identifierlist->lista_ids;
    Semantic_types tipoDeclarado = var_spec->tipo ? var_spec->tipo->accept(&tipos) : Semantic_types(Semantic_types::NOTYPE);

    if (var_spec->expresionlist) {
        auto& valores = var_spec->expresionlist->lista_exp;
        auto itNombre = nombres.begin();
        for (auto exp : valores) {
            Semantic_types tipoFinal = tipoDeclarado.getKind() != Semantic_types::NOTYPE ? tipoDeclarado : tipoDe(exp);
            registrarVariable(this, *itNombre, tipoFinal);
            if (!entornoFuncion) {
                inicializacionesGlobales.emplace_back(*itNombre, exp);
            } else {
                Semantic_types tipoOrigen = tipoDe(exp);
                exp->accept(this);
                emitirConversionAType(this, tipoOrigen, tipoFinal);
                bool esFloat = tipoFinal.getKind() == Semantic_types::FLOAT64;
                if (esFloat) out << "  movsd %xmm0, " << memoria[*itNombre] << "(%rbp)\n";
                else out << "  movq %rax, " << memoria[*itNombre] << "(%rbp)\n";
            }
            ++itNombre;
        }
    } else {
        // Sin inicializador: escalares quedan en el "zero value" gratis
        // (.zero en .data para globales; en el stack no se garantiza, pero
        // no importa porque no son punteros). struct/array SI necesitan
        // reservar su buffer aqui mismo (con calloc, para que el "zero
        // value" sea real), porque su slot solo guarda un puntero: sin esto
        // quedaria un puntero sin inicializar y cualquier lectura
        // (arr[i], m.campo) dereferenciaria memoria invalida.
        for (auto& nombre : nombres) {
            registrarVariable(this, nombre, tipoDeclarado);
            if (entornoFuncion && (tipoDeclarado.isStruct() || tipoDeclarado.isArray())) {
                int slots = tipoDeclarado.isArray() ? (int)tipoDeclarado.arrayLength() : (int)tipoDeclarado.fieldCount();
                if (slots < 1) slots = 1;
                out << "  movq $" << slots << ", %rdi\n  movq $8, %rsi\n  call calloc@PLT\n";
                out << "  movq %rax, " << memoria[nombre] << "(%rbp)\n";
            }
        }
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(FieldDecl* field_decl) { return tipos.visit(field_decl); }
Semantic_types GenCode::visit(BasicType* basic_type) { return tipos.visit(basic_type); }
Semantic_types GenCode::visit(ArrayType* array_type) { return tipos.visit(array_type); }
Semantic_types GenCode::visit(StructType* struct_type) { return tipos.visit(struct_type); }
Semantic_types GenCode::visit(PointerType* pointer_type) { return tipos.visit(pointer_type); }
Semantic_types GenCode::visit(ParameterList* parameter_list) { return tipos.visit(parameter_list); }
Semantic_types GenCode::visit(ParameterDecl* parameter_decl) { return tipos.visit(parameter_decl); }

//  Parte Bruno //

Semantic_types GenCode::visit(Block* block) {
    if (block->lista_statements) block->lista_statements->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(StmtList* stmt_list) {
    for (auto stmt : stmt_list->statements) if (stmt) stmt->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(DeclarationStmt* declaration_stmt) {
    if (declaration_stmt->declaration) declaration_stmt->declaration->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(BlockStmt* block_stmt) {
    if (block_stmt->block) block_stmt->block->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ExpresionStmt* expresion_stmt) {
    if (expresion_stmt->expresion) expresion_stmt->expresion->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(IncDecStmt* inc_dec_stmt) {
    inc_dec_stmt->expresion->accept(this);
    out << (inc_dec_stmt->op == INC_OP ? "  addq $1, %rax\n" : "  subq $1, %rax\n");
    emitirAlmacenar(this, inc_dec_stmt->expresion, Semantic_types(Semantic_types::INT));
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(Assigment* assigment_stmt) {
    auto& targets = assigment_stmt->expresion_list_id->lista_exp;
    auto& values = assigment_stmt->expresion_list_values->lista_exp;

    for (size_t i = 0; i < targets.size(); ++i) {
        Exp* targetExp = targets[i];
        Exp* valueExp = values[i];
        Semantic_types tipoDestino = tipoDe(targetExp);
        bool esFloat = tipoDestino.getKind() == Semantic_types::FLOAT64;

        if (assigment_stmt->op != ASSIGN) {
            targetExp->accept(this);
            if (esFloat) out << "  subq $8, %rsp\n  movsd %xmm0, (%rsp)\n";
            else out << "  pushq %rax\n";
            valueExp->accept(this);
            if (esFloat) {
                out << "  movsd %xmm0, %xmm1\n  movsd (%rsp), %xmm0\n  addq $8, %rsp\n";
                switch (assigment_stmt->op) {
                    case PLUS_ASSIGN: out << "  addsd %xmm1, %xmm0\n"; break;
                    case NEG_ASSIGN:  out << "  subsd %xmm1, %xmm0\n"; break;
                    case MUL_ASSIGN:  out << "  mulsd %xmm1, %xmm0\n"; break;
                    case DIV_ASSIGN:  out << "  divsd %xmm1, %xmm0\n"; break;
                    default: break;
                }
            } else {
                out << "  movq %rax, %rcx\n  popq %rax\n";
                switch (assigment_stmt->op) {
                    case PLUS_ASSIGN: out << "  addq %rcx, %rax\n"; break;
                    case NEG_ASSIGN:  out << "  subq %rcx, %rax\n"; break;
                    case MUL_ASSIGN:  out << "  imulq %rcx, %rax\n"; break;
                    case DIV_ASSIGN:  out << "  cqto\n  idivq %rcx\n"; break;
                    default: break;
                }
            }
        } else {
            valueExp->accept(this);
        }
        emitirAlmacenar(this, targetExp, tipoDestino);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ReturnStmt* return_stmt) {
    if (return_stmt->expresion_list && !return_stmt->expresion_list->lista_exp.empty()) {
        return_stmt->expresion_list->lista_exp[0]->accept(this);
    } else {
        // "return;" sin valor (funcion void): limpiar %rax explicitamente,
        // igual que el fallthrough al final de la funcion (ver notas ahi) --
        // de lo contrario el codigo de salida queda con basura de la ultima
        // llamada emitida antes de este return.
        out << "  movq $0, %rax\n";
    }
    out << "  jmp .end_" << nombreFuncion << "\n";
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(BreakStmt* break_stmt) {
    out << "  jmp " << currentBreakLabel << "\n";
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ContinueStmt* continue_stmt) {
    out << "  jmp " << currentContinueLabel << "\n";
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(IfStmt* if_stmt) {
    int lbl = labelcont++;
    if (if_stmt->expresion) if_stmt->expresion->accept(this);
    out << "  cmpq $0, %rax\n";
    out << "  je else_" << lbl << "\n";
    if (if_stmt->cuerpo_if) if_stmt->cuerpo_if->accept(this);
    out << "  jmp endif_" << lbl << "\n";
    out << "else_" << lbl << ":\n";
    if (if_stmt->if_anidado) if_stmt->if_anidado->accept(this);
    else if (if_stmt->cuerpo_else) if_stmt->cuerpo_else->accept(this);
    out << "endif_" << lbl << ":\n";
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(SwitchStmt* switch_stmt) {
    int lbl = labelcont++;
    if (switch_stmt->expresion) switch_stmt->expresion->accept(this);
    else out << "  movq $1, %rax\n"; // switch sin tag == switch true
    out << "  movq %rax, %r10\n";

    string oldBreak = currentBreakLabel;
    currentBreakLabel = "endswitch_" + to_string(lbl);

    int casoIdx = 0;
    vector<string> etiquetasCaso;
    ExpCaseClause* clauseDefault = nullptr;
    for (auto clause : switch_stmt->exp_case_clause) {
        if (!clause->expresion_list) {
            clauseDefault = clause;
            etiquetasCaso.push_back("");
            casoIdx++;
            continue;
        }
        string etq = "case_" + to_string(lbl) + "_" + to_string(casoIdx);
        etiquetasCaso.push_back(etq);
        for (auto exp : clause->expresion_list->lista_exp) {
            exp->accept(this);
            out << "  cmpq %rax, %r10\n";
            out << "  je " << etq << "\n";
        }
        casoIdx++;
    }
    if (clauseDefault) out << "  jmp default_" << lbl << "\n";
    else out << "  jmp " << currentBreakLabel << "\n";

    casoIdx = 0;
    for (auto clause : switch_stmt->exp_case_clause) {
        if (clause == clauseDefault) out << "default_" << lbl << ":\n";
        else out << etiquetasCaso[casoIdx] << ":\n";
        if (clause->statement_list) clause->statement_list->accept(this);
        out << "  jmp " << currentBreakLabel << "\n";
        casoIdx++;
    }

    out << currentBreakLabel << ":\n";
    currentBreakLabel = oldBreak;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ExpCaseClause* exp_case_clause) {
    // Logica manejada directamente en visit(SwitchStmt*); existe solo para
    // cumplir la interfaz del Visitor.
    if (exp_case_clause->statement_list) exp_case_clause->statement_list->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ForStmt* for_stmt) {
    int lbl = labelcont++;
    string etqInicio = "for_" + to_string(lbl);
    string etqFin = "endfor_" + to_string(lbl);
    string etqPost = "forpost_" + to_string(lbl);

    string oldBreak = currentBreakLabel;
    string oldContinue = currentContinueLabel;
    currentBreakLabel = etqFin;

    if (for_stmt->for_clause) {
        ForClause* fc = for_stmt->for_clause;
        if (fc->asignacion1) fc->asignacion1->accept(this);
        currentContinueLabel = etqPost;

        out << etqInicio << ":\n";
        if (fc->expresion) {
            fc->expresion->accept(this);
            out << "  cmpq $0, %rax\n";
            out << "  je " << etqFin << "\n";
        }
        if (for_stmt->block) for_stmt->block->accept(this);
        out << etqPost << ":\n";
        if (fc->inc_dec_stmt) fc->inc_dec_stmt->accept(this);
        else if (fc->asignacion2) fc->asignacion2->accept(this);
        out << "  jmp " << etqInicio << "\n";
        out << etqFin << ":\n";
    } else {
        currentContinueLabel = etqInicio;
        out << etqInicio << ":\n";
        if (for_stmt->expresion) {
            for_stmt->expresion->accept(this);
            out << "  cmpq $0, %rax\n";
            out << "  je " << etqFin << "\n";
        }
        if (for_stmt->block) for_stmt->block->accept(this);
        out << "  jmp " << etqInicio << "\n";
        out << etqFin << ":\n";
    }

    currentBreakLabel = oldBreak;
    currentContinueLabel = oldContinue;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(ForClause* for_clause) {
    // Logica manejada directamente en visit(ForStmt*); existe solo para
    // cumplir la interfaz del Visitor.
    return Semantic_types(Semantic_types::VOID);
}

// Parte Nico //

Semantic_types GenCode::visit(BasicLitExp* basic_lit_exp) {
    switch (basic_lit_exp->tipoLiteral) {
        case Token::INT_LIT:
        case Token::RUNE_LIT:
            out << "  movq $" << basic_lit_exp->valor << ", %rax\n";
            return Semantic_types(Semantic_types::INT);
        case Token::FLOAT_LIT: {
            string etq = "flt_lit_" + to_string(labelcont++);
            out << ".data\n" << etq << ": .double " << basic_lit_exp->valor << "\n.text\n";
            out << "  movsd " << etq << "(%rip), %xmm0\n";
            return Semantic_types(Semantic_types::FLOAT64);
        }
        case Token::STRING_LIT: {
            // valor incluye las comillas originales (ver scanner.cpp); se recortan.
            string contenido = basic_lit_exp->valor.size() >= 2
                ? basic_lit_exp->valor.substr(1, basic_lit_exp->valor.size() - 2)
                : "";
            string etq = etiquetaParaString(contenido);
            out << "  leaq " << etq << "(%rip), %rax\n";
            return Semantic_types(Semantic_types::STRING);
        }
        default:
            cerr << "Error interno de codegen: literal no soportado\n";
            exit(1);
    }
}

Semantic_types GenCode::visit(OperandNameExp* operand_name_exp) {
    if (operand_name_exp->name == "true") { out << "  movq $1, %rax\n"; return Semantic_types(Semantic_types::BOOL); }
    if (operand_name_exp->name == "false") { out << "  movq $0, %rax\n"; return Semantic_types(Semantic_types::BOOL); }

    Semantic_types tipo = tipoDe(operand_name_exp);
    bool esGlobal = memoriaGlobal.count(operand_name_exp->name) > 0;
    if (tipo.getKind() == Semantic_types::FLOAT64) {
        if (esGlobal) out << "  movsd " << operand_name_exp->name << "(%rip), %xmm0\n";
        else out << "  movsd " << memoria[operand_name_exp->name] << "(%rbp), %xmm0\n";
    } else {
        if (esGlobal) out << "  movq " << operand_name_exp->name << "(%rip), %rax\n";
        else out << "  movq " << memoria[operand_name_exp->name] << "(%rbp), %rax\n";
    }
    return tipo;
}

Semantic_types GenCode::visit(ParenExp* paren_exp) {
    return paren_exp->expresion->accept(this);
}

Semantic_types GenCode::visit(UnaryExprExp* unary_exp) {
    Semantic_types tipoOperando = tipoDe(unary_exp->expresion);

    if (unary_exp->op == ADDR_OP) {
        if (tipoOperando.isStruct() || tipoOperando.isArray()) {
            // El "valor" de un struct/arreglo ya es su puntero base (ver
            // notas de simplificacion arriba); &m devuelve el mismo puntero.
            unary_exp->expresion->accept(this);
        } else {
            emitirDireccion(unary_exp->expresion);
        }
        return Semantic_types::makePointer(tipoOperando);
    }
    if (unary_exp->op == DEREF_OP) {
        unary_exp->expresion->accept(this); // %rax = direccion apuntada
        Semantic_types pointee = tipoOperando.baseType() ? *tipoOperando.baseType() : Semantic_types(Semantic_types::NOTYPE);
        if (pointee.getKind() == Semantic_types::FLOAT64) out << "  movsd (%rax), %xmm0\n";
        else out << "  movq (%rax), %rax\n";
        return pointee;
    }

    if (tipoOperando.getKind() == Semantic_types::FLOAT64) {
        unary_exp->expresion->accept(this);
        if (unary_exp->op == NEG_OP) {
            string etq = "neg_mask_" + to_string(labelcont++);
            out << ".data\n" << etq << ": .quad 0x8000000000000000\n.text\n";
            out << "  xorpd " << etq << "(%rip), %xmm0\n";
        }
        return tipoOperando;
    }

    unary_exp->expresion->accept(this);
    switch (unary_exp->op) {
        case NEG_OP: out << "  negq %rax\n"; break;
        case NOT_OP:
            out << "  cmpq $0, %rax\n  movq $0, %rax\n  sete %al\n  movzbq %al, %rax\n";
            break;
        default: break; // POS_OP: no-op
    }
    return tipoOperando;
}

Semantic_types GenCode::visit(BinaryExp* binary_exp) {
    if (binary_exp->op == AND_OP || binary_exp->op == OR_OP) {
        binary_exp->left->accept(this);
        out << "  pushq %rax\n";
        binary_exp->right->accept(this);
        out << "  movq %rax, %rcx\n  popq %rax\n";
        out << (binary_exp->op == AND_OP ? "  andq %rcx, %rax\n" : "  orq %rcx, %rax\n");
        return Semantic_types(Semantic_types::BOOL);
    }

    Semantic_types tipoIzq = tipoDe(binary_exp->left);
    Semantic_types tipoDer = tipoDe(binary_exp->right);
    bool esFloat = tipoIzq.getKind() == Semantic_types::FLOAT64 || tipoDer.getKind() == Semantic_types::FLOAT64;
    bool esString = tipoIzq.getKind() == Semantic_types::STRING && tipoDer.getKind() == Semantic_types::STRING;

    if (esString) {
        binary_exp->left->accept(this);
        out << "  pushq %rax\n";
        binary_exp->right->accept(this);
        out << "  movq %rax, %rsi\n  popq %rdi\n";
        necesitaConcatString = true;
        out << "  call str_concat\n";
        return Semantic_types(Semantic_types::STRING);
    }

    if (esFloat) {
        double cf;
        if (tryConstFoldFloat(binary_exp, cf)) {
            string etq = "flt_" + to_string(labelcont++);
            out << ".data\n" << etq << ": .double " << cf << "\n.text\n";
            out << "  movsd " << etq << "(%rip), %xmm0\n";
            return Semantic_types(Semantic_types::FLOAT64);
        }

        emitirValorFloat(this, binary_exp->left, tipoIzq);
        out << "  subq $8, %rsp\n  movsd %xmm0, (%rsp)\n";
        emitirValorFloat(this, binary_exp->right, tipoDer);
        out << "  movsd %xmm0, %xmm1\n  movsd (%rsp), %xmm0\n  addq $8, %rsp\n";

        switch (binary_exp->op) {
            case PLUS_OP:  out << "  addsd %xmm1, %xmm0\n"; return Semantic_types(Semantic_types::FLOAT64);
            case MINUS_OP: out << "  subsd %xmm1, %xmm0\n"; return Semantic_types(Semantic_types::FLOAT64);
            case MUL_OP:   out << "  mulsd %xmm1, %xmm0\n"; return Semantic_types(Semantic_types::FLOAT64);
            case DIV_OP:   out << "  divsd %xmm1, %xmm0\n"; return Semantic_types(Semantic_types::FLOAT64);
            case EQ_OP: case NEQ_OP: case LT_OP: case GT_OP: case LTE_OP: case GTE_OP: {
                out << "  comisd %xmm1, %xmm0\n  movq $0, %rax\n";
                switch (binary_exp->op) {
                    case EQ_OP:  out << "  sete %al\n"; break;
                    case NEQ_OP: out << "  setne %al\n"; break;
                    case LT_OP:  out << "  setb %al\n"; break;
                    case GT_OP:  out << "  seta %al\n"; break;
                    case LTE_OP: out << "  setbe %al\n"; break;
                    case GTE_OP: out << "  setae %al\n"; break;
                    default: break;
                }
                out << "  movzbq %al, %rax\n";
                return Semantic_types(Semantic_types::BOOL);
            }
            default: return Semantic_types(Semantic_types::FLOAT64);
        }
    }

    long cf;
    if (tryConstFoldInt(binary_exp, cf)) {
        out << "  movq $" << cf << ", %rax\n";
        return Semantic_types(Semantic_types::INT);
    }

    binary_exp->left->accept(this);
    out << "  pushq %rax\n";
    binary_exp->right->accept(this);
    out << "  movq %rax, %rcx\n  popq %rax\n";

    switch (binary_exp->op) {
        case PLUS_OP:  out << "  addq %rcx, %rax\n"; return tipoIzq;
        case MINUS_OP: out << "  subq %rcx, %rax\n"; return tipoIzq;
        case MUL_OP:   out << "  imulq %rcx, %rax\n"; return tipoIzq;
        case DIV_OP:   out << "  cqto\n  idivq %rcx\n"; return tipoIzq;
        case MOD_OP:   out << "  cqto\n  idivq %rcx\n  movq %rdx, %rax\n"; return tipoIzq;
        case EQ_OP:  out << "  cmpq %rcx, %rax\n  movq $0, %rax\n  sete %al\n  movzbq %al, %rax\n"; return Semantic_types(Semantic_types::BOOL);
        case NEQ_OP: out << "  cmpq %rcx, %rax\n  movq $0, %rax\n  setne %al\n  movzbq %al, %rax\n"; return Semantic_types(Semantic_types::BOOL);
        case LT_OP:  out << "  cmpq %rcx, %rax\n  movq $0, %rax\n  setl %al\n  movzbq %al, %rax\n"; return Semantic_types(Semantic_types::BOOL);
        case GT_OP:  out << "  cmpq %rcx, %rax\n  movq $0, %rax\n  setg %al\n  movzbq %al, %rax\n"; return Semantic_types(Semantic_types::BOOL);
        case LTE_OP: out << "  cmpq %rcx, %rax\n  movq $0, %rax\n  setle %al\n  movzbq %al, %rax\n"; return Semantic_types(Semantic_types::BOOL);
        case GTE_OP: out << "  cmpq %rcx, %rax\n  movq $0, %rax\n  setge %al\n  movzbq %al, %rax\n"; return Semantic_types(Semantic_types::BOOL);
        case POW_OP: {
            int lbl = labelcont++;
            out << "  movq $1, %rdx\n";
            out << "pow_loop_" << lbl << ":\n";
            out << "  cmpq $0, %rcx\n";
            out << "  jle pow_end_" << lbl << "\n";
            out << "  imulq %rax, %rdx\n";
            out << "  decq %rcx\n";
            out << "  jmp pow_loop_" << lbl << "\n";
            out << "pow_end_" << lbl << ":\n";
            out << "  movq %rdx, %rax\n";
            return tipoIzq;
        }
        default: return tipoIzq;
    }
}

Semantic_types GenCode::visit(CompositeLitExp* composite_lit_exp) {
    Semantic_types tipo = composite_lit_exp->tipo->accept(&tipos);
    int slots = tipo.isArray() ? (int)tipo.arrayLength() : tipo.isStruct() ? (int)tipo.fieldCount() : 1;
    if (slots < 1) slots = 1;

    // calloc (no malloc): los campos/elementos omitidos en el literal deben
    // quedar en el "zero value" de Go, no en basura sin inicializar.
    out << "  movq $" << slots << ", %rdi\n  movq $8, %rsi\n";
    out << "  call calloc@PLT\n";
    out << "  pushq %rax\n"; // conservar el puntero base mientras se evaluan los elementos

    if (tipo.isStruct()) {
        bool conClaves = false;
        for (auto ke : composite_lit_exp->elementos) if (ke->key) { conClaves = true; break; }
        size_t posicion = 0;
        for (auto ke : composite_lit_exp->elementos) {
            int idxCampo;
            Semantic_types tipoCampo;
            if (conClaves) {
                auto nombreCampo = dynamic_cast<OperandNameExp*>(ke->key);
                idxCampo = tipo.fieldIndex(nombreCampo->name);
                tipoCampo = tipo.fieldType(nombreCampo->name);
            } else {
                idxCampo = (int)posicion;
                tipoCampo = tipo.fieldType(tipo.fieldNameAt(posicion));
            }
            ke->value->accept(this);
            out << "  movq (%rsp), %r10\n"; // peek: puntero base, sin desapilar
            if (tipoCampo.getKind() == Semantic_types::FLOAT64) out << "  movsd %xmm0, " << (idxCampo * 8) << "(%r10)\n";
            else out << "  movq %rax, " << (idxCampo * 8) << "(%r10)\n";
            posicion++;
        }
    } else if (tipo.isArray()) {
        Semantic_types tipoElem = tipo.baseType() ? *tipo.baseType() : Semantic_types(Semantic_types::NOTYPE);
        size_t i = 0;
        for (auto ke : composite_lit_exp->elementos) {
            ke->value->accept(this);
            out << "  movq (%rsp), %r10\n";
            if (tipoElem.getKind() == Semantic_types::FLOAT64) out << "  movsd %xmm0, " << (i * 8) << "(%r10)\n";
            else out << "  movq %rax, " << (i * 8) << "(%r10)\n";
            i++;
        }
    }

    out << "  popq %rax\n"; // recuperar el puntero como resultado de la expresion
    return tipo;
}

Semantic_types GenCode::visit(FunctionLit* function_lit) {
    // Simplificacion: los literales de funcion (closures) no generan codigo
    // real en esta version del backend; solo se les asigna un valor
    // placeholder para no romper programas que los declaran pero no
    // dependen de invocarlos dinamicamente.
    out << "  movq $0, %rax\n";
    return function_lit->tipo ? function_lit->tipo->accept(&tipos) : Semantic_types(Semantic_types::VOID);
}

Semantic_types GenCode::visit(SelectorExp* selector_exp) {
    Semantic_types tipoBase = tipoDe(selector_exp->expresion);
    Semantic_types structTipo = tipoBase.isPointer() ? (tipoBase.baseType() ? *tipoBase.baseType() : tipoBase) : tipoBase;

    selector_exp->expresion->accept(this); // %rax = puntero al struct (uniforme)

    int idx = structTipo.fieldIndex(selector_exp->campo);
    Semantic_types tipoCampo = structTipo.fieldType(selector_exp->campo);
    int desplazamiento = idx > 0 ? idx * 8 : 0;

    if (tipoCampo.getKind() == Semantic_types::FLOAT64) out << "  movsd " << desplazamiento << "(%rax), %xmm0\n";
    else out << "  movq " << desplazamiento << "(%rax), %rax\n";
    return tipoCampo;
}

Semantic_types GenCode::visit(IndexExp* index_exp) {
    Semantic_types tipoBase = tipoDe(index_exp->expresion);

    if (tipoBase.getKind() == Semantic_types::STRING) {
        index_exp->expresion->accept(this);
        out << "  pushq %rax\n";
        index_exp->indice->accept(this);
        out << "  movq %rax, %rdi\n  popq %rax\n";
        out << "  addq %rdi, %rax\n";
        out << "  movzbq (%rax), %rax\n";
        return Semantic_types(Semantic_types::INT);
    }

    emitirDireccion(index_exp);
    Semantic_types tipoElem = tipoBase.baseType() ? *tipoBase.baseType() : Semantic_types(Semantic_types::NOTYPE);
    if (tipoElem.getKind() == Semantic_types::FLOAT64) out << "  movsd (%rax), %xmm0\n";
    else out << "  movq (%rax), %rax\n";
    return tipoElem;
}

Semantic_types GenCode::visit(SliceExp* slice_exp) {
    // Extension fuera de la gramatica base (no hay slices en grammar.txt):
    // se simplifica devolviendo la misma direccion base, sin longitud real.
    slice_exp->expresion->accept(this);
    return tipoDe(slice_exp->expresion);
}

Semantic_types GenCode::visit(TypeAssertionExp* type_assertion_exp) {
    type_assertion_exp->expresion->accept(this);
    return type_assertion_exp->tipo->accept(&tipos);
}

Semantic_types GenCode::visit(ArgumentsExp* arguments_exp) {
    // Caso 1: conversion de tipo T(x), o llamada a funcion / builtin print(ln)
    if (auto on = dynamic_cast<OperandNameExp*>(arguments_exp->funcion)) {
        Semantic_types::Kind k = Semantic_types::string_to_kind(on->name);
        bool esTipo = (k != Semantic_types::NOTYPE) || tipos.tabla_tipos.count(on->name);

        if (esTipo) {
            Semantic_types destino = (k != Semantic_types::NOTYPE) ? Semantic_types(k) : tipos.tabla_tipos[on->name];
            Exp* argExp = arguments_exp->args->lista_exp[0];
            Semantic_types origen = tipoDe(argExp);
            argExp->accept(this);
            if (origen.getKind() == Semantic_types::INT && destino.getKind() == Semantic_types::FLOAT64) {
                out << "  cvtsi2sdq %rax, %xmm0\n";
            } else if (origen.getKind() == Semantic_types::FLOAT64 && destino.getKind() == Semantic_types::INT) {
                out << "  cvttsd2siq %xmm0, %rax\n";
            }
            return destino;
        }

        if (on->name == "print" || on->name == "println") {
            if (arguments_exp->args) {
                for (auto e : arguments_exp->args->lista_exp) {
                    Semantic_types t = tipoDe(e);
                    e->accept(this);
                    emitirPrintDeTipo(this, t);
                }
            }
            if (on->name == "println") out << "  leaq nl_fmt(%rip), %rdi\n  movq $0, %rax\n  call printf@PLT\n";
            return Semantic_types(Semantic_types::VOID);
        }

        auto itFn = tipos.tabla_funciones.find(on->name);
        vector<Exp*> argsVec = arguments_exp->args ? arguments_exp->args->lista_exp : vector<Exp*>{};
        emitirArgumentos(this, argsVec, 0, itFn != tipos.tabla_funciones.end() ? &itFn->second.param_types : nullptr);
        out << "  call " << on->name << "\n";
        return itFn != tipos.tabla_funciones.end() ? itFn->second.return_types : Semantic_types(Semantic_types::NOTYPE);
    }

    // Caso 2: llamada a metodo p.Metodo(args)
    if (auto sel = dynamic_cast<SelectorExp*>(arguments_exp->funcion)) {
        Semantic_types receptor = tipoDe(sel->expresion);
        Semantic_types receptorBase = receptor.isPointer() ? (receptor.baseType() ? *receptor.baseType() : receptor) : receptor;
        if (receptorBase.getKind() == Semantic_types::STRUCT) {
            string clave = receptorBase.structName() + "_" + sel->campo;
            auto itFn = tipos.tabla_funciones.find(clave);
            if (itFn != tipos.tabla_funciones.end()) {
                sel->expresion->accept(this); // %rax = puntero al struct (uniforme)
                out << "  pushq %rax\n";
                vector<Exp*> argsVec = arguments_exp->args ? arguments_exp->args->lista_exp : vector<Exp*>{};
                emitirArgumentos(this, argsVec, 1, &itFn->second.param_types); // deja %rdi libre para el receptor
                out << "  popq %rdi\n";
                out << "  call " << clave << "\n";
                return itFn->second.return_types;
            }
        }
    }

    // Caso 3: llamar el resultado de una expresion arbitraria (no soportado
    // en codegen real; se evaluan los efectos y se devuelve un valor de 0).
    arguments_exp->funcion->accept(this);
    if (arguments_exp->args)
        for (auto e : arguments_exp->args->lista_exp) e->accept(this);
    out << "  movq $0, %rax\n";
    return tipoDe(arguments_exp);
}
