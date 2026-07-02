//
// Created by Usuario on 21/6/2026.
//

#ifndef GENCODE_H
#define GENCODE_H
#include <ostream>
#include <unordered_map>
#include <string>
#include "visitor.h"
#include "Semantic_types.h"
#include "TypeCheker.h"

// =============================================================================
// GenCode — Generacion de codigo ensamblador x86-64 (sintaxis AT&T)
// =============================================================================
// Convenciones (ver plan / grammar.txt "Notas de implementacion"):
//   - Asume que el llamador ya corrio tipos.TypeCheker(programa) antes de
//     llamar a generar() (ver main.cpp, que separa la fase semantica de la
//     fase de codegen para reportar cada una por separado).
//   - Cada slot (variable local, campo de struct, elemento de arreglo) mide
//     8 bytes; structs/arrays son valores (no punteros a heap) reservados en
//     el frame de la funcion o en .data si son globales.
//   - int/bool/pointer/string(puntero al buffer) viajan en registros de
//     proposito general; float64 viaja en %xmm0 (SSE real).
//   - Metodos se emiten como funciones normales "Tipo_Metodo" con el
//     receptor como primer argumento implicito.
// =============================================================================

class GenCode: public Visitor {
public:
    std::ostream& out;
    TypeChekerVisitor tipos; // resultado del analisis semantico (Programa completo)

    // Variable -> offset relativo a %rbp (locales) — flat, se limpia por funcion.
    std::unordered_map<std::string, int> memoria;
    // Variable global -> true (vive en .data, acceso RIP-relativo).
    std::unordered_map<std::string, bool> memoriaGlobal;
    // Variable -> su Semantic_types resuelto (flat, se limpia por funcion).
    std::unordered_map<std::string, Semantic_types> tipos_var;
    // Igual que tipos_var pero solo para variables globales (const/var de
    // nivel superior): persiste entre funciones, a diferencia de tipos_var.
    std::unordered_map<std::string, Semantic_types> tipos_var_globales;

    int offset = -8;             // proximo offset libre (mas negativo) del frame actual
    int labelcont = 0;            // contador de etiquetas unicas (if/for/switch)
    bool entornoFuncion = false;  // true mientras se procesan declaraciones dentro de una funcion
    std::string currentBreakLabel;
    std::string currentContinueLabel;
    std::string nombreFuncion;

    bool necesitaConcatString = false; // se emite el helper str_concat solo si se usa

    // Literales string ya emitidos en .data (valor -> etiqueta), para no duplicar.
    std::unordered_map<std::string, std::string> stringLiterals;
    int stringLitCont = 0;

    // Inicializadores de variables/consts globales (nombre, expresion valor);
    // se ejecutan en __init_globals, llamada al inicio de main (igual que la
    // inicializacion de paquete en Go real corre antes que main).
    std::vector<std::pair<std::string, Exp*>> inicializacionesGlobales;

    explicit GenCode(std::ostream& out) : out(out) {}

    // Emite el .s completo (asume que `tipos.TypeCheker` ya corrio antes).
    void generar(Programa* programa);

    // ---- Helpers internos (no forman parte del patron Visitor) ----
    // Reserva `n` slots de 8 bytes contiguos en el frame actual (n=1 para
    // escalares, n>1 para structs/arrays) y devuelve el offset del primero.
    int reservarSlots(int n);
    // Determina el tipo estatico de una expresion sin volver a recorrer el
    // entorno (con scopes) del TypeChecker, que ya se cerro: usa `tipos_var`
    // (flat, de la funcion actual) y las tablas globales de `tipos`.
    Semantic_types tipoDe(Exp* e);
    // Plegado de constantes minimo (folding), reutilizado en BinaryExp/UnaryExprExp.
    bool tryConstFoldInt(Exp* e, long& out);
    bool tryConstFoldFloat(Exp* e, double& out);
    // Registra (o reutiliza) un literal string en .data y devuelve su etiqueta.
    std::string etiquetaParaString(const std::string& valor);
    // Emite la direccion (no el valor) de una expresion en %rax; usado por
    // '&' y por el lado izquierdo de una asignacion.
    void emitirDireccion(Exp* e);
    // Emite una llamada al helper de concatenacion de strings (deja el
    // puntero resultante en %rax); asume que ambos operandos ya se evaluaron.
    void emitStrConcatHelperSiHaceFalta();

    // Parte Rayhan //
    Semantic_types visit(Programa* programa) override;
    Semantic_types visit(FunctionDecl* fun_decl) override;
    Semantic_types visit(MethodDecl* method_decl) override;
    Semantic_types visit(ConstDecl* const_decl) override;
    Semantic_types visit(ConstSpec* const_spec) override;
    Semantic_types visit(TypeDecl* type_decl) override;
    Semantic_types visit(TypeSpec* type_spec) override;
    Semantic_types visit(VarDecl* var_decl) override;
    Semantic_types visit(VarSpec* var_spec) override;
    Semantic_types visit(FieldDecl* field_decl) override;
    Semantic_types visit(BasicType* basic_type) override;
    Semantic_types visit(ArrayType* array_type) override;
    Semantic_types visit(StructType* struct_type) override;
    Semantic_types visit(PointerType* pointer_type) override;
    Semantic_types visit(ParameterList* parameter_list) override;
    Semantic_types visit(ParameterDecl* parameter_decl) override;

    //  Parte Bruno //
    Semantic_types visit(Block* block)override;
    Semantic_types visit(StmtList* stmt_list)override;
    Semantic_types visit(DeclarationStmt* declaration_stmt)override;
    Semantic_types visit(BlockStmt* block_stmt)override;
    Semantic_types visit(ExpresionStmt* expresion_stmt)override;
    Semantic_types visit(IncDecStmt* inc_dec_stmt)override;
    Semantic_types visit(Assigment* assigment_stmt)override;
    Semantic_types visit(ReturnStmt* return_stmt)override;
    Semantic_types visit(BreakStmt* break_stmt)override;
    Semantic_types visit(ContinueStmt* continue_stmt)override;
    Semantic_types visit(IfStmt* if_stmt)override;
    Semantic_types visit(SwitchStmt* switch_stmt)override;
    Semantic_types visit(ExpCaseClause* exp_case_clause)override;
    Semantic_types visit(ForStmt* for_stmt)override;
    Semantic_types visit(ForClause* for_clause)override;

    // Parte Nico //
    Semantic_types visit(BinaryExp* binary_exp) override;
    Semantic_types visit(UnaryExprExp* unary_exp) override;
    Semantic_types visit(ParenExp* paren_exp) override;
    Semantic_types visit(OperandNameExp* operand_name_exp) override;
    Semantic_types visit(BasicLitExp* basic_lit_exp) override;
    Semantic_types visit(CompositeLitExp* composite_lit_exp) override;
    Semantic_types visit(FunctionLit* function_lit) override;
    Semantic_types visit(SelectorExp* selector_exp) override;
    Semantic_types visit(IndexExp* index_exp) override;
    Semantic_types visit(SliceExp* slice_exp) override;
    Semantic_types visit(TypeAssertionExp* type_assertion_exp) override;
    Semantic_types visit(ArgumentsExp* arguments_exp) override;
};



#endif //GENCODE_H
