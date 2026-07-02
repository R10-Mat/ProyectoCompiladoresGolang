//
// Created by Usuario on 21/6/2026.
//

#include "TypeCheker.h"
#include "ast.h"

// =============================================================================
// TypeCheckerVisitor — Análisis semántico
// =============================================================================
// Responsabilidades:
//   1. Registrar las funciones y su aridad antes de analizar los cuerpos.
//   2. Verificar que cada variable usada haya sido declarada en el scope.
//   3. Verificar que cada función llamada exista y reciba el número correcto
//      de argumentos.
//   4. Contar las variables locales de cada función para que GenCodeVisitor
//      pueda reservar el espacio correcto en el stack frame.
//   5. Inferencia y chequeo de Tipos en Expresiones:
//   6. Validación de Tipos Compuestos y Structs
//   7. Validación de Punteros y Arreglos
//   8. Registro de métodos
// =============================================================================

namespace {
// Evalúa expresiones constantes enteras sin abortar el programa: literales
// int, nombres de const ya resueltas, y combinaciones con +,-,*,/,% entre
// constantes (incluye unario +/-). Se usa tanto para el tamaño de un
// ArrayType (que debe ser constante) como para ir registrando el valor de
// cada const entera declarada.
bool tryEvalConstInt(Exp *e, const unordered_map<string, long> &consts, long &out) {
    if (!e) return false;
    if (auto lit = dynamic_cast<BasicLitExp *>(e)) {
        if (lit->tipoLiteral == Token::INT_LIT) {
            out = stol(lit->valor);
            return true;
        }
        return false;
    }
    if (auto on = dynamic_cast<OperandNameExp *>(e)) {
        auto it = consts.find(on->name);
        if (it == consts.end()) return false;
        out = it->second;
        return true;
    }
    if (auto pe = dynamic_cast<ParenExp *>(e)) {
        return tryEvalConstInt(pe->expresion, consts, out);
    }
    if (auto un = dynamic_cast<UnaryExprExp *>(e)) {
        long v;
        if (!tryEvalConstInt(un->expresion, consts, v)) return false;
        if (un->op == NEG_OP) { out = -v; return true; }
        if (un->op == POS_OP) { out = v; return true; }
        return false;
    }
    if (auto bin = dynamic_cast<BinaryExp *>(e)) {
        long l, r;
        if (!tryEvalConstInt(bin->left, consts, l) || !tryEvalConstInt(bin->right, consts, r))
            return false;
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
} // namespace

long TypeChekerVisitor::evalConstIntExpr(Exp *e) {
    long resultado;
    if (tryEvalConstInt(e, valores_const, resultado)) return resultado;
    cerr << "Error semantico: se esperaba una expresion constante entera "
            "(por ejemplo, el tamaño de un arreglo)\n";
    exit(1);
}

// -----------------------------------------------------------------------------
// TypeChecker — punto de entrada
// -----------------------------------------------------------------------------

void TypeChekerVisitor::TypeCheker(Programa *programa) {
    visit(programa);
}

// Parte Rayhan //

Semantic_types TypeChekerVisitor::visit(Programa *programa) {
    // Pasada 1: reservar los nombres declarados con "type" para permitir
    // referencias cruzadas entre tipos (ej. un struct con puntero a si mismo,
    // o dos structs que se referencian mutuamente vía puntero).
    for (auto decl : programa->listatopleveldecl) {
        if (auto td = dynamic_cast<TypeDecl *>(decl)) {
            for (auto spec : td->typespecList) {
                if (tabla_tipos.count(spec->id)) {
                    cerr << "Error semantico: el tipo '" << spec->id << "' ya fue declarado\n";
                    exit(1);
                }
                tabla_tipos[spec->id] = Semantic_types();
            }
        }
    }

    // Pasada 2: resolver cada tipo ahora que todos los nombres existen.
    for (auto decl : programa->listatopleveldecl) {
        if (auto td = dynamic_cast<TypeDecl *>(decl)) {
            for (auto spec : td->typespecList) {
                Semantic_types resuelto = spec->tipo->accept(this);
                if (resuelto.getKind() == Semantic_types::STRUCT) resuelto.setStructName(spec->id);
                tabla_tipos[spec->id] = resuelto;
            }
        }
    }

    // Pasada 3: registrar firmas de funciones y métodos (permite llamadas
    // hacia adelante, sin importar el orden textual de declaración).
    for (auto decl : programa->listatopleveldecl) {
        if (auto fd = dynamic_cast<FunctionDecl *>(decl)) {
            FunctionSignature firma;
            if (fd->lista_de_parametros) {
                for (auto pd : fd->lista_de_parametros->parameterList) {
                    Semantic_types t = pd->type->accept(this);
                    for (size_t i = 0; i < pd->identifierlist->lista_ids.size(); ++i)
                        firma.param_types.push_back(t);
                }
            }
            firma.return_types = fd->tipo ? fd->tipo->accept(this) : Semantic_types(Semantic_types::VOID);
            tabla_funciones[fd->name] = firma;
        } else if (auto md = dynamic_cast<MethodDecl *>(decl)) {
            FunctionSignature firma;
            if (md->lista_de_parametros) {
                for (auto pd : md->lista_de_parametros->parameterList) {
                    Semantic_types t = pd->type->accept(this);
                    for (size_t i = 0; i < pd->identifierlist->lista_ids.size(); ++i)
                        firma.param_types.push_back(t);
                }
            }
            firma.return_types = md->tipo ? md->tipo->accept(this) : Semantic_types(Semantic_types::VOID);
            tabla_funciones[md->NombreTipoBase + "_" + md->nombreMethod] = firma;
        }
    }

    // Pasada 4: declaraciones globales (const/var) en el scope global.
    entorno.add_level();
    for (auto decl : programa->listatopleveldecl) {
        if (dynamic_cast<ConstDecl *>(decl) || dynamic_cast<VarDecl *>(decl)) {
            decl->accept(this);
        }
    }

    // Pasada 5: chequear los cuerpos de funciones y métodos.
    for (auto decl : programa->listatopleveldecl) {
        if (dynamic_cast<FunctionDecl *>(decl) || dynamic_cast<MethodDecl *>(decl)) {
            decl->accept(this);
        }
    }
    entorno.remove_level();

    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(FunctionDecl *fun_decl) {
    string nombreAnterior = nombre_funcion_actual;
    Semantic_types retornoAnterior = tipo_retorno_actual;
    int localesAnterior = locales_actual;

    nombre_funcion_actual = fun_decl->name;
    tipo_retorno_actual = fun_decl->tipo ? fun_decl->tipo->accept(this) : Semantic_types(Semantic_types::VOID);
    locales_actual = 0;

    entorno.add_level();
    int parametros = 0;
    if (fun_decl->lista_de_parametros) {
        for (auto pd : fun_decl->lista_de_parametros->parameterList) {
            Semantic_types t = pd->type->accept(this);
            for (auto &nombre : pd->identifierlist->lista_ids) {
                entorno.add_var(nombre, t);
                parametros++;
            }
        }
    }
    if (fun_decl->cuerpo) fun_decl->cuerpo->accept(this);
    entorno.remove_level();

    fun_var_contador[fun_decl->name] = parametros + locales_actual;

    nombre_funcion_actual = nombreAnterior;
    tipo_retorno_actual = retornoAnterior;
    locales_actual = localesAnterior;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(MethodDecl *method_decl) {
    string nombreAnterior = nombre_funcion_actual;
    Semantic_types retornoAnterior = tipo_retorno_actual;
    int localesAnterior = locales_actual;

    string clave = method_decl->NombreTipoBase + "_" + method_decl->nombreMethod;
    nombre_funcion_actual = clave;
    tipo_retorno_actual = method_decl->tipo ? method_decl->tipo->accept(this) : Semantic_types(Semantic_types::VOID);
    locales_actual = 0;

    auto itTipoBase = tabla_tipos.find(method_decl->NombreTipoBase);
    if (itTipoBase == tabla_tipos.end()) {
        cerr << "Error semantico: tipo receptor no declarado '" << method_decl->NombreTipoBase
             << "' en el metodo '" << method_decl->nombreMethod << "'\n";
        exit(1);
    }
    Semantic_types tipoReceptor = method_decl->puntero
        ? Semantic_types::makePointer(itTipoBase->second)
        : itTipoBase->second;

    entorno.add_level();
    entorno.add_var(method_decl->nombreId, tipoReceptor);
    int parametros = 1; // el receptor ocupa el primer slot del frame

    if (method_decl->lista_de_parametros) {
        for (auto pd : method_decl->lista_de_parametros->parameterList) {
            Semantic_types t = pd->type->accept(this);
            for (auto &nombre : pd->identifierlist->lista_ids) {
                entorno.add_var(nombre, t);
                parametros++;
            }
        }
    }
    if (method_decl->cuerpo) method_decl->cuerpo->accept(this);
    entorno.remove_level();

    fun_var_contador[clave] = parametros + locales_actual;

    nombre_funcion_actual = nombreAnterior;
    tipo_retorno_actual = retornoAnterior;
    locales_actual = localesAnterior;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ConstDecl *const_decl) {
    for (auto spec : const_decl->constspecList) spec->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ConstSpec *const_spec) {
    auto &nombres = const_spec->identifierList->lista_ids;
    auto &valores = const_spec->expresionlist->lista_exp;
    if (nombres.size() != valores.size()) {
        cerr << "Error semantico: la cantidad de identificadores y expresiones no coincide "
                "en la declaracion de constantes\n";
        exit(1);
    }

    Semantic_types tipoDeclarado;
    bool tieneTipo = false;
    if (const_spec->tipo) {
        tipoDeclarado = const_spec->tipo->accept(this);
        tieneTipo = true;
    }

    auto itNombre = nombres.begin();
    for (auto exp : valores) {
        Semantic_types tipoValor = exp->accept(this);
        Semantic_types tipoFinal = tieneTipo ? tipoDeclarado : tipoValor;
        if (tieneTipo && !tipoValor.match(&tipoDeclarado) && !tipoValor.canPromoteTo(tipoDeclarado)) {
            cerr << "Error semantico: la constante '" << *itNombre << "' no coincide con su tipo declarado ("
                 << tipoDeclarado.to_string() << " vs " << tipoValor.to_string() << ")\n";
            exit(1);
        }
        entorno.add_var(*itNombre, tipoFinal);
        long valorConst;
        if (tryEvalConstInt(exp, valores_const, valorConst)) valores_const[*itNombre] = valorConst;
        locales_actual++;
        ++itNombre;
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(TypeDecl *type_decl) {
    // Los tipos de nivel superior ya se resolvieron en la pasada 1-2 de
    // visit(Programa*); esto cubre el caso de un TypeDecl local (dentro de
    // un cuerpo de función), que no pasa por esa pre-pasada.
    for (auto spec : type_decl->typespecList) {
        if (!tabla_tipos.count(spec->id)) {
            spec->accept(this);
        }
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(TypeSpec *type_spec) {
    Semantic_types resuelto = type_spec->tipo->accept(this);
    if (resuelto.getKind() == Semantic_types::STRUCT) resuelto.setStructName(type_spec->id);
    tabla_tipos[type_spec->id] = resuelto;
    return resuelto;
}

Semantic_types TypeChekerVisitor::visit(VarDecl *var_decl) {
    for (auto spec : var_decl->varspecList) spec->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(VarSpec *var_spec) {
    auto &nombres = var_spec->identifierlist->lista_ids;

    Semantic_types tipoDeclarado;
    bool tieneTipo = false;
    if (var_spec->tipo) {
        tipoDeclarado = var_spec->tipo->accept(this);
        tieneTipo = true;
    }

    if (var_spec->expresionlist) {
        auto &valores = var_spec->expresionlist->lista_exp;
        if (nombres.size() != valores.size()) {
            cerr << "Error semantico: la cantidad de identificadores y expresiones no coincide "
                    "en la declaracion de variables\n";
            exit(1);
        }
        auto itNombre = nombres.begin();
        for (auto exp : valores) {
            Semantic_types tipoValor = exp->accept(this);
            Semantic_types tipoFinal = tieneTipo ? tipoDeclarado : tipoValor;
            if (tieneTipo && !tipoValor.match(&tipoDeclarado) && !tipoValor.canPromoteTo(tipoDeclarado)) {
                cerr << "Error semantico: no se puede inicializar '" << *itNombre << "' de tipo "
                     << tipoDeclarado.to_string() << " con un valor de tipo " << tipoValor.to_string() << "\n";
                exit(1);
            }
            entorno.add_var(*itNombre, tipoFinal);
            locales_actual++;
            ++itNombre;
        }
    } else {
        for (auto &nombre : nombres) {
            entorno.add_var(nombre, tipoDeclarado);
            locales_actual++;
        }
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(FieldDecl *field_decl) {
    return field_decl->type->accept(this);
}

Semantic_types TypeChekerVisitor::visit(BasicType *basic_type) {
    Semantic_types::Kind k = Semantic_types::string_to_kind(basic_type->tipo);
    if (k != Semantic_types::NOTYPE) return Semantic_types(k);

    auto it = tabla_tipos.find(basic_type->tipo);
    if (it != tabla_tipos.end()) return it->second;

    cerr << "Error semantico: tipo no declarado '" << basic_type->tipo << "'\n";
    exit(1);
}

Semantic_types TypeChekerVisitor::visit(ArrayType *array_type) {
    Semantic_types elemento = array_type->elementtype->accept(this);
    long longitud = evalConstIntExpr(array_type->length);
    if (longitud < 0) {
        cerr << "Error semantico: el tamaño de un arreglo no puede ser negativo\n";
        exit(1);
    }
    return Semantic_types::makeArray(elemento, longitud);
}

Semantic_types TypeChekerVisitor::visit(StructType *struct_type) {
    vector<pair<string, Semantic_types>> campos;
    for (auto fd : struct_type->declaraciones) {
        Semantic_types tipoCampo = fd->type->accept(this);
        for (auto &nombre : fd->identifierlist->lista_ids) {
            campos.emplace_back(nombre, tipoCampo);
        }
    }
    return Semantic_types::makeStruct("", campos);
}

Semantic_types TypeChekerVisitor::visit(PointerType *pointer_type) {
    Semantic_types base = pointer_type->basetype->accept(this);
    return Semantic_types::makePointer(base);
}

Semantic_types TypeChekerVisitor::visit(ParameterList *parameter_list) {
    for (auto pd : parameter_list->parameterList) pd->accept(this);
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ParameterDecl *parameter_decl) {
    return parameter_decl->type->accept(this);
}

//  Parte Bruno //
Semantic_types TypeChekerVisitor::visit(Block* block) {
    entorno.add_level();

    if (block->lista_statements != nullptr) {
        block->lista_statements->accept(this);
    }
    entorno.remove_level();
    return Semantic_types(Semantic_types::VOID);
}
Semantic_types TypeChekerVisitor::visit(StmtList* stmt_list) {
    for (auto stmt : stmt_list->statements) {
        if (stmt != nullptr) {
            stmt->accept(this);
        }
    }
    return Semantic_types(Semantic_types::VOID);
}
Semantic_types TypeChekerVisitor::visit(DeclarationStmt* declaration_stmt) {
    if (declaration_stmt->declaration != nullptr) {
        declaration_stmt->declaration->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(BlockStmt* block_stmt) {
    if (block_stmt->block != nullptr) {
        block_stmt->block->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ExpresionStmt* expresion_stmt) {
    if (expresion_stmt->expresion != nullptr) {
        expresion_stmt->expresion->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(IncDecStmt *inc_dec_stmt) {
    if (inc_dec_stmt->expresion != nullptr) {
        Semantic_types tipo_izq = inc_dec_stmt->expresion->accept(this);

        if (tipo_izq.getKind() != Semantic_types::INT) {
            cerr << "Error semantico: Los operadores '++' y '--' "
                 << "solo se pueden a aplciar a int. Se encontro: "
                 << tipo_izq.getKind() << "\n";
            exit(1);
        }
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(Assigment *assigment_stmt) {
    // 1. Validación de seguridad básica
    if (assigment_stmt->expresion_list_id == nullptr || assigment_stmt->expresion_list_values == nullptr) {
        return Semantic_types(Semantic_types::VOID);
    }

    auto& lista_izq = assigment_stmt->expresion_list_id->lista_exp;
    auto& lista_der = assigment_stmt->expresion_list_values->lista_exp;

    // 2. Regla 1: Validar que los tamaños coincidan (ej: x, y = 1, 2)
    if (lista_izq.size() != lista_der.size()) {
        cerr <<  "Desajuste en la cantidad de variables y valores de asignación";
        exit(1);
    }

    // 3. Regla 2: Si es un operador compuesto (+=, -=, *=, /=), Go prohíbe la asignación múltiple
    // Es decir, solo se permite 'a += 1', no 'a, b += 1, 2'
    if (assigment_stmt->op != AssignOp::ASSIGN && lista_izq.size() > 1) {
        cerr << "Operadores compuestos no permiten asignación múltiple";
        exit(1);
    }

    // 4. Iterar y chequear tipos par por par
    for (size_t i = 0; i < lista_izq.size(); ++i) {
        Semantic_types tipo_izq = lista_izq[i]->accept(this);
        Semantic_types tipo_der = lista_der[i]->accept(this);

        // Si el lado izquierdo es un identificador que no existe en el entorno, tipo_izq será un tipo de error
        if (tipo_izq.getKind() == Semantic_types::NOTYPE) {
            cerr << "La variable no ha sido declarada";
            exit(1);
            continue;
        }

        // Caso A: Asignación simple ( = )
        if (assigment_stmt->op == AssignOp::ASSIGN) {
            if (tipo_izq.getKind() != tipo_der.getKind()) {
                cerr << "No se puede asignar un tipo 'X' a una variable de tipo 'Y'";
                exit(1);
            }
        }
        // Caso B: Asignación compuesta ( += , -= , *= , /= )
        else {
            // Validar que ambos lados sean estrictamente numéricos y del mismo tipo
            if ((tipo_izq.getKind() != Semantic_types::INT && tipo_izq.getKind() != Semantic_types::FLOAT64) ||
                (tipo_der.getKind() != Semantic_types::INT && tipo_der.getKind() != Semantic_types::FLOAT64)) {
                cerr << "Los operadores compuestos solo se permiten en tipos numéricos";
            }
            else if (tipo_izq.getKind() != tipo_der.getKind()) {
                cerr << "Tipos no coinciden en la operación compuesta (Go no tiene casteo implícito)";
            }
        }
    }

    // Como es un Statement, su evaluación semántica siempre devuelve VOID
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ReturnStmt* return_stmt) {
    if (tipo_retorno_actual.getKind() == Semantic_types::VOID) {
        if (return_stmt->expresion_list != nullptr) {
            cerr << "Error semantico en la función '" << nombre_funcion_actual
                 << "': Una function que retorna void no debe devolver ninguna valor \n ";
            exit(1);
        }
        return Semantic_types(Semantic_types::VOID);
    }
    if (return_stmt->expresion_list == nullptr) {
        cerr << "Error semantico en funcion '" << nombre_funcion_actual
                << "': La funcion no es void, debe devolver un valor de tipo "
                << tipo_retorno_actual.to_string() << ".\n";
        exit(1);
    }
    Semantic_types tipo_expresion = return_stmt->expresion_list->accept(this);

    if (!tipo_retorno_actual.match(&tipo_expresion)) {
        cerr << "Error semantico en funcion '" << nombre_funcion_actual
             << "': El tipo de retorno (" << tipo_expresion.to_string()
             << ") no coincide con el tipo declarado ("
             << tipo_retorno_actual.to_string() << ").\n";
        exit(1);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(BreakStmt* break_stmt) {
    if (this->dentro_de_bucle <= 0) {
        cerr << "Error semantico: La sentencia break solo puede usarse dentro de un bucle";
        exit(1);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ContinueStmt* continue_stmt) {
    if (this->dentro_de_bucle <= 0) {
        cerr << "Error semantico: La sentencia continue solo puede usarse dentro de un bucle";
        exit(1);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(IfStmt *if_stmt) {
    if (if_stmt->expresion != nullptr) {
        Semantic_types tipo_cond = if_stmt->expresion->accept(this);

        if (tipo_cond.getKind() != Semantic_types::BOOL) {
            cerr << "Error semantico: La condifición del if debe ser de tipo bool"
                 << "Se encontro: " << tipo_cond.getKind() << endl;
            exit(1);
        }
    }
    if (if_stmt->cuerpo_if != nullptr) {
        if_stmt->cuerpo_if->accept(this);
    }
    if (if_stmt->if_anidado != nullptr) {
        if_stmt->if_anidado->accept(this);
    }
    if (if_stmt->cuerpo_else != nullptr) {
        if_stmt->cuerpo_else->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(SwitchStmt *switch_stmt) {
    Semantic_types tipo_base_switch(Semantic_types::BOOL);
    if (switch_stmt->expresion != nullptr) {
        tipo_base_switch = switch_stmt->expresion->accept(this);
    }
    Semantic_types tipo_anterior = this->tipo_switch_actual;
    this->tipo_switch_actual = tipo_base_switch;

    entorno.add_level();
    for (auto un_case : switch_stmt->exp_case_clause) {
        if (un_case != nullptr) {
            un_case->accept(this);
        }
    }
    entorno.remove_level();
    this->tipo_switch_actual = tipo_anterior;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ExpCaseClause *exp_case_clause) {
    if (exp_case_clause->expresion_list != nullptr) {
        for (auto exp: exp_case_clause->expresion_list->lista_exp) {
            Semantic_types tipo_exp = exp->accept(this);

            if (tipo_exp.getKind() != this->tipo_switch_actual.getKind()) {
                cerr  << "Error semantico: Tipo del case no coincide con el tipo del switch";
                exit(1);
            }
        }
    }

    if (exp_case_clause->statement_list != nullptr) {
        exp_case_clause->statement_list->accept(this);
    }

    return Semantic_types(Semantic_types::VOID);
}


Semantic_types TypeChekerVisitor::visit(ForStmt *for_stmt) {
    // Activamos la bandera para permitir 'break' y 'continue' dentro del bloque
    this->dentro_de_bucle++;

    // --- CASO 1: For con Cláusula (for init; cond; post { ... }) ---
    if (for_stmt->for_clause != nullptr) {
        // Abrimos el entorno ANTES de la cláusula porque el 'init' (asignacion1)
        // puede declarar variables locales (ej: i := 0) que solo viven en el bucle.
        entorno.add_level();

        for_stmt->for_clause->accept(this);

        if (for_stmt->block != nullptr) {
            for_stmt->block->accept(this);
        }

        entorno.remove_level();
    }
    else {
        if (for_stmt->expresion != nullptr) {
            // La condición se evalúa en el entorno actual (el del padre)
            Semantic_types tipo_condicion = for_stmt->expresion->accept(this);
            if (tipo_condicion.getKind() != Semantic_types::BOOL) {
                cerr << "La condición del bucle for debe ser de tipo bool";
                exit(1);
            }
        }

        // El bloque del 'for' condicional sí abre su propio nivel de variables
        if (for_stmt->block != nullptr) {
            entorno.add_level();
            for_stmt->block->accept(this);
            entorno.remove_level();
        }
    }
    this->dentro_de_bucle--;

    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ForClause *for_clause) {
    // 1. Validar la inicialización (ej: i := 0 o i = 0)
    // Las variables nuevas (si usa :=) se registrarán en el entorno actual
    if (for_clause->asignacion1 != nullptr) {
        for_clause->asignacion1->accept(this);
    }

    // 2. Validar la condición central (debe ser estrictamente un BOOL)
    if (for_clause->expresion != nullptr) {
        Semantic_types tipo_cond = for_clause->expresion->accept(this);
        if (tipo_cond.getKind() != Semantic_types::BOOL) {
            cerr << "La condicion central del for debe ser de tipo bool";
            exit(1);
        }
    }

    // 3. Validar el paso de post-ejecución (ej: i++ o i += 1)
    // Nota: Aquí se evalúa en el mismo entorno, por lo que modificará las variables del init
    if (for_clause->inc_dec_stmt != nullptr) {
        for_clause->inc_dec_stmt->accept(this);
    } else if (for_clause->asignacion2 != nullptr) {
        for_clause->asignacion2->accept(this);
    }

    return Semantic_types(Semantic_types::VOID);
}
// Parte Nico //

Semantic_types TypeChekerVisitor::visit(BasicLitExp *basic_lit_exp) {
    switch (basic_lit_exp->tipoLiteral) {
        case Token::INT_LIT:    return Semantic_types(Semantic_types::INT);
        case Token::FLOAT_LIT:  return Semantic_types(Semantic_types::FLOAT64);
        case Token::STRING_LIT: return Semantic_types(Semantic_types::STRING);
        case Token::RUNE_LIT:   return Semantic_types(Semantic_types::INT);
        case Token::IMAGINARY_LIT:
            cerr << "Error semantico: los numeros imaginarios no estan soportados\n";
            exit(1);
        default:
            cerr << "Error semantico: literal no reconocido\n";
            exit(1);
    }
}

Semantic_types TypeChekerVisitor::visit(OperandNameExp *operand_name_exp) {
    if (operand_name_exp->name == "true" || operand_name_exp->name == "false") {
        return Semantic_types(Semantic_types::BOOL);
    }
    Semantic_types tipo;
    if (entorno.lookup(operand_name_exp->name, tipo)) return tipo;

    auto itFn = tabla_funciones.find(operand_name_exp->name);
    if (itFn != tabla_funciones.end()) return itFn->second.return_types;

    cerr << "Error semantico: variable no declarada '" << operand_name_exp->name
         << "' usada en la funcion '" << nombre_funcion_actual << "'\n";
    exit(1);
}

Semantic_types TypeChekerVisitor::visit(ParenExp *paren_exp) {
    return paren_exp->expresion->accept(this);
}

Semantic_types TypeChekerVisitor::visit(UnaryExprExp *unary_exp) {
    Semantic_types tipo = unary_exp->expresion->accept(this);
    switch (unary_exp->op) {
        case POS_OP:
        case NEG_OP:
            if (!tipo.isNumeric()) {
                cerr << "Error semantico: el operador unario '" << Exp::unopToString(unary_exp->op)
                     << "' requiere un operando numerico\n";
                exit(1);
            }
            return tipo;
        case NOT_OP:
            if (tipo.getKind() != Semantic_types::BOOL) {
                cerr << "Error semantico: el operador '!' requiere un operando bool\n";
                exit(1);
            }
            return tipo;
        case ADDR_OP:
            return Semantic_types::makePointer(tipo);
        case DEREF_OP:
            if (!tipo.isPointer()) {
                cerr << "Error semantico: no se puede dereferenciar un valor que no es puntero\n";
                exit(1);
            }
            return *tipo.baseType();
        default:
            cerr << "Error semantico: operador unario no soportado en una expresion\n";
            exit(1);
    }
}

Semantic_types TypeChekerVisitor::visit(BinaryExp *binary_exp) {
    Semantic_types izq = binary_exp->left->accept(this);
    Semantic_types der = binary_exp->right->accept(this);

    switch (binary_exp->op) {
        case AND_OP:
        case OR_OP:
            if (izq.getKind() != Semantic_types::BOOL || der.getKind() != Semantic_types::BOOL) {
                cerr << "Error semantico: '" << Exp::binopToString(binary_exp->op)
                     << "' requiere operandos bool\n";
                exit(1);
            }
            return Semantic_types(Semantic_types::BOOL);

        case EQ_OP: case NEQ_OP: case LT_OP: case GT_OP: case LTE_OP: case GTE_OP:
            if (!izq.match(&der) && !izq.canPromoteTo(der) && !der.canPromoteTo(izq)) {
                cerr << "Error semantico: no se pueden comparar tipos " << izq.to_string()
                     << " y " << der.to_string() << "\n";
                exit(1);
            }
            return Semantic_types(Semantic_types::BOOL);

        case PLUS_OP:
            if (izq.getKind() == Semantic_types::STRING && der.getKind() == Semantic_types::STRING) {
                return Semantic_types(Semantic_types::STRING);
            }
            [[fallthrough]];
        case MINUS_OP: case MUL_OP: case DIV_OP: case MOD_OP: case POW_OP:
            if (!izq.isNumeric() || !der.isNumeric()) {
                cerr << "Error semantico: '" << Exp::binopToString(binary_exp->op)
                     << "' requiere operandos numericos\n";
                exit(1);
            }
            if (izq.match(&der)) return izq;
            if (izq.canPromoteTo(der)) return der;
            if (der.canPromoteTo(izq)) return izq;
            cerr << "Error semantico: tipos incompatibles en operacion aritmetica ("
                 << izq.to_string() << " vs " << der.to_string() << ")\n";
            exit(1);

        default:
            cerr << "Error semantico: operador binario no soportado\n";
            exit(1);
    }
}

Semantic_types TypeChekerVisitor::visit(CompositeLitExp *composite_lit_exp) {
    Semantic_types tipo = composite_lit_exp->tipo->accept(this);

    if (tipo.getKind() == Semantic_types::STRUCT) {
        bool conClaves = false;
        for (auto ke : composite_lit_exp->elementos) {
            if (ke->key) { conClaves = true; break; }
        }
        if (conClaves) {
            for (auto ke : composite_lit_exp->elementos) {
                if (!ke->key) {
                    cerr << "Error semantico: no se pueden mezclar elementos con y sin clave "
                            "en un literal de struct\n";
                    exit(1);
                }
                auto nombreCampo = dynamic_cast<OperandNameExp *>(ke->key);
                if (!nombreCampo) {
                    cerr << "Error semantico: la clave de un campo debe ser un identificador\n";
                    exit(1);
                }
                Semantic_types tipoCampo = tipo.fieldType(nombreCampo->name);
                if (tipoCampo.getKind() == Semantic_types::NOTYPE) {
                    cerr << "Error semantico: el struct '" << tipo.structName()
                         << "' no tiene el campo '" << nombreCampo->name << "'\n";
                    exit(1);
                }
                Semantic_types tipoValor = ke->value->accept(this);
                if (!tipoValor.match(&tipoCampo) && !tipoValor.canPromoteTo(tipoCampo)) {
                    cerr << "Error semantico: el campo '" << nombreCampo->name << "' espera "
                         << tipoCampo.to_string() << ", se dio " << tipoValor.to_string() << "\n";
                    exit(1);
                }
            }
        } else {
            // Literal posicional: Semantic_types no expone el orden de los
            // campos para validar posicion-a-posicion, asi que solo se
            // chequea cada expresion (simplificacion documentada).
            for (auto ke : composite_lit_exp->elementos) ke->value->accept(this);
        }
        return tipo;
    }

    if (tipo.getKind() == Semantic_types::ARRAY) {
        const Semantic_types *elemento = tipo.baseType();
        if (static_cast<long>(composite_lit_exp->elementos.size()) > tipo.arrayLength()) {
            cerr << "Error semantico: el literal tiene mas elementos que el tamaño del arreglo ("
                 << tipo.arrayLength() << ")\n";
            exit(1);
        }
        for (auto ke : composite_lit_exp->elementos) {
            Semantic_types tipoValor = ke->value->accept(this);
            if (elemento && !tipoValor.match(elemento) && !tipoValor.canPromoteTo(*elemento)) {
                cerr << "Error semantico: elemento del arreglo con tipo incompatible ("
                     << tipoValor.to_string() << " vs " << elemento->to_string() << ")\n";
                exit(1);
            }
        }
        return tipo;
    }

    cerr << "Error semantico: literal compuesto sobre un tipo que no es struct ni arreglo\n";
    exit(1);
}

Semantic_types TypeChekerVisitor::visit(FunctionLit *function_lit) {
    Semantic_types retornoAnterior = tipo_retorno_actual;
    tipo_retorno_actual = function_lit->tipo ? function_lit->tipo->accept(this) : Semantic_types(Semantic_types::VOID);

    entorno.add_level();
    if (function_lit->lista_de_parametros) {
        for (auto pd : function_lit->lista_de_parametros->parameterList) {
            Semantic_types t = pd->type->accept(this);
            for (auto &nombre : pd->identifierlist->lista_ids) entorno.add_var(nombre, t);
        }
    }
    if (function_lit->cuerpo) function_lit->cuerpo->accept(this);
    entorno.remove_level();

    // Simplificacion: Semantic_types no tiene un Kind para "funcion", asi
    // que se devuelve su tipo de retorno como aproximacion.
    Semantic_types tipoResultado = tipo_retorno_actual;
    tipo_retorno_actual = retornoAnterior;
    return tipoResultado;
}

Semantic_types TypeChekerVisitor::visit(SelectorExp *selector_exp) {
    Semantic_types base = selector_exp->expresion->accept(this);
    if (base.isPointer()) base = *base.baseType();
    if (base.getKind() != Semantic_types::STRUCT) {
        cerr << "Error semantico: no se puede acceder al campo '" << selector_exp->campo
             << "' de un tipo que no es struct\n";
        exit(1);
    }
    Semantic_types tipoCampo = base.fieldType(selector_exp->campo);
    if (tipoCampo.getKind() == Semantic_types::NOTYPE) {
        cerr << "Error semantico: el struct '" << base.structName() << "' no tiene el campo '"
             << selector_exp->campo << "'\n";
        exit(1);
    }
    return tipoCampo;
}

Semantic_types TypeChekerVisitor::visit(IndexExp *index_exp) {
    Semantic_types base = index_exp->expresion->accept(this);
    Semantic_types indice = index_exp->indice->accept(this);
    if (indice.getKind() != Semantic_types::INT) {
        cerr << "Error semantico: el indice debe ser de tipo int\n";
        exit(1);
    }
    if (base.isArray()) {
        return base.baseType() ? *base.baseType() : Semantic_types(Semantic_types::NOTYPE);
    }
    if (base.getKind() == Semantic_types::STRING) {
        return Semantic_types(Semantic_types::INT); // indexar un string da un byte (int)
    }
    cerr << "Error semantico: no se puede indexar un tipo que no es arreglo ni string\n";
    exit(1);
}

Semantic_types TypeChekerVisitor::visit(SliceExp *slice_exp) {
    // Extension fuera de grammar.txt (no hay slices en la gramatica base):
    // se valida el operando y los limites, y se devuelve el mismo tipo del
    // operando como simplificacion (en Go real produce un tipo "slice"
    // distinto, que Semantic_types no modela).
    Semantic_types base = slice_exp->expresion->accept(this);
    if (!base.isArray() && base.getKind() != Semantic_types::STRING) {
        cerr << "Error semantico: solo se puede hacer slice de un arreglo o string\n";
        exit(1);
    }
    if (slice_exp->low && slice_exp->low->accept(this).getKind() != Semantic_types::INT) {
        cerr << "Error semantico: el limite inferior del slice debe ser int\n";
        exit(1);
    }
    if (slice_exp->high && slice_exp->high->accept(this).getKind() != Semantic_types::INT) {
        cerr << "Error semantico: el limite superior del slice debe ser int\n";
        exit(1);
    }
    if (slice_exp->max && slice_exp->max->accept(this).getKind() != Semantic_types::INT) {
        cerr << "Error semantico: el limite maximo del slice debe ser int\n";
        exit(1);
    }
    return base;
}

Semantic_types TypeChekerVisitor::visit(TypeAssertionExp *type_assertion_exp) {
    type_assertion_exp->expresion->accept(this);
    return type_assertion_exp->tipo->accept(this);
}

Semantic_types TypeChekerVisitor::visit(ArgumentsExp *arguments_exp) {
    // Caso 1: conversion de tipo T(x), ej. int(x) o Point(x)
    if (auto on = dynamic_cast<OperandNameExp *>(arguments_exp->funcion)) {
        Semantic_types::Kind k = Semantic_types::string_to_kind(on->name);
        bool esTipo = (k != Semantic_types::NOTYPE) || tabla_tipos.count(on->name);

        if (esTipo) {
            if (!arguments_exp->args || arguments_exp->args->lista_exp.size() != 1) {
                cerr << "Error semantico: la conversion de tipo '" << on->name
                     << "' espera exactamente 1 argumento\n";
                exit(1);
            }
            Semantic_types destino = (k != Semantic_types::NOTYPE) ? Semantic_types(k) : tabla_tipos[on->name];
            Semantic_types origen = arguments_exp->args->lista_exp[0]->accept(this);
            if (!origen.isConvertibleTo(destino)) {
                cerr << "Error semantico: no se puede convertir " << origen.to_string()
                     << " a " << destino.to_string() << "\n";
                exit(1);
            }
            return destino;
        }

        // print/println son builtins predeclarados de Go (no hay import/fmt en la gramatica).
        if (on->name == "print" || on->name == "println") {
            if (arguments_exp->args)
                for (auto e : arguments_exp->args->lista_exp) e->accept(this);
            return Semantic_types(Semantic_types::VOID);
        }

        auto itFn = tabla_funciones.find(on->name);
        if (itFn == tabla_funciones.end()) {
            cerr << "Error semantico: funcion no declarada '" << on->name << "'\n";
            exit(1);
        }
        size_t recibidos = arguments_exp->args ? arguments_exp->args->lista_exp.size() : 0;
        if (recibidos != itFn->second.param_types.size()) {
            cerr << "Error semantico: '" << on->name << "' espera " << itFn->second.param_types.size()
                 << " argumento(s), se pasaron " << recibidos << "\n";
            exit(1);
        }
        for (size_t i = 0; i < recibidos; ++i) {
            Semantic_types t = arguments_exp->args->lista_exp[i]->accept(this);
            Semantic_types &esperado = itFn->second.param_types[i];
            if (!t.match(&esperado) && !t.canPromoteTo(esperado)) {
                cerr << "Error semantico: el argumento " << (i + 1) << " de '" << on->name
                     << "' tiene tipo incompatible (" << t.to_string() << " vs " << esperado.to_string() << ")\n";
                exit(1);
            }
        }
        return itFn->second.return_types;
    }

    // Caso 2: llamada a metodo, ej. p.Move(dx, dy)
    if (auto sel = dynamic_cast<SelectorExp *>(arguments_exp->funcion)) {
        Semantic_types receptor = sel->expresion->accept(this);
        Semantic_types receptorBase = receptor.isPointer() ? *receptor.baseType() : receptor;
        if (receptorBase.getKind() == Semantic_types::STRUCT) {
            string clave = receptorBase.structName() + "_" + sel->campo;
            auto itFn = tabla_funciones.find(clave);
            if (itFn != tabla_funciones.end()) {
                size_t recibidos = arguments_exp->args ? arguments_exp->args->lista_exp.size() : 0;
                if (recibidos != itFn->second.param_types.size()) {
                    cerr << "Error semantico: el metodo '" << sel->campo << "' espera "
                         << itFn->second.param_types.size() << " argumento(s), se pasaron "
                         << recibidos << "\n";
                    exit(1);
                }
                for (size_t i = 0; i < recibidos; ++i) arguments_exp->args->lista_exp[i]->accept(this);
                return itFn->second.return_types;
            }
        }
        // No es un metodo conocido: se resuelve como un campo funcional cualquiera.
    }

    // Caso 3: llamar al resultado de una expresion arbitraria.
    Semantic_types tipoFuncion = arguments_exp->funcion->accept(this);
    if (arguments_exp->args)
        for (auto e : arguments_exp->args->lista_exp) e->accept(this);
    return tipoFuncion;
}
