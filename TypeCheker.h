//
// Created by Usuario on 21/6/2026.
//

#ifndef TYPECHEKER_H
#define TYPECHEKER_H

#include "Enviroment.h"
#include "Semantic_types.h"
#include "visitor.h"

using namespace std;

class Exp;

// Estructura para almacenar la firma de una función en la tabla global de funciones
struct FunctionSignature {
    vector<Semantic_types> param_types; // Tipos de los parámetros en orden
    Semantic_types return_types;        // Tipo de retorno (puede ser VOID)
};

class TypeChekerVisitor: public Visitor{
public:
    // [key] = nombre de la función (o "Tipo_Metodo" para métodos) -> [value] = Firma
    unordered_map<string,FunctionSignature> tabla_funciones;

    // [key] = nombre de la función -> [value] = número total de variables locales que genera
    unordered_map<string,int> fun_var_contador;

    // [key] = nombre de tipo declarado con "type" (ej. "Point") -> [value] = su Semantic_types resuelto
    unordered_map<string,Semantic_types> tabla_tipos;

    // [key] = nombre de una const entera conocida en tiempo de compilación -> [value] = su valor
    // (usado para resolver el tamaño de un ArrayType, que la gramática exige constante)
    unordered_map<string,long> valores_const;

    //Entorno de variables
    Environment<Semantic_types> entorno;

    // Nombre de la function actual
    string nombre_funcion_actual;

    // tipo de retorno de la funcion actual
    Semantic_types tipo_retorno_actual;

    // tipo de retorno del switch actual
    Semantic_types tipo_switch_actual;

    // numero de fors anidados
    int dentro_de_bucle = 0;

    // contador de variables/consts locales declaradas en la funcion actual
    // (incluye las de bloques anidados: if/for/switch); se usa para que
    // GenCode reserve el espacio correcto en el stack frame
    int locales_actual = 0;

    void TypeCheker(Programa* programa);

    // Evalúa una expresión constante entera (literal, o const ya resuelta,
    // combinados con +,-,*,/ entre constantes). Lanza error si `e` no es
    // una expresión constante — Go exige esto para el tamaño de un arreglo.
    long evalConstIntExpr(Exp* e);

    // --------- métodos visit ------------------
    // Parte Rayhan //
    Semantic_types visit(Programa* programa) override;
    Semantic_types visit(FunctionDecl* fun_decl)override;
    Semantic_types visit(MethodDecl* method_decl)override;
    Semantic_types visit(ConstDecl* const_decl)override;
    Semantic_types visit(ConstSpec* const_spec)override;
    Semantic_types visit(TypeDecl* type_decl)override;
    Semantic_types visit(TypeSpec* type_spec)override;
    Semantic_types visit(VarDecl* var_decl)override;
    Semantic_types visit(VarSpec* var_spec)override;
    Semantic_types visit(FieldDecl* field_decl)override;
    Semantic_types visit(BasicType* basic_type)override;
    Semantic_types visit(ArrayType* array_type)override;
    Semantic_types visit(StructType* struct_type)override;
    Semantic_types visit(PointerType* pointer_type)override;
    Semantic_types visit(ParameterList* parameter_list)override;
    Semantic_types visit(ParameterDecl* parameter_decl)override;
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
    Semantic_types visit(BinaryExp* binary_exp)override;
    Semantic_types visit(UnaryExprExp* unary_exp)override;
    Semantic_types visit(ParenExp* paren_exp)override;
    Semantic_types visit(OperandNameExp* operand_name_exp)override;
    Semantic_types visit(BasicLitExp* basic_lit_exp)override;
    Semantic_types visit(CompositeLitExp* composite_lit_exp)override;
    Semantic_types visit(FunctionLit* function_lit)override;
    Semantic_types visit(SelectorExp* selector_exp)override;
    Semantic_types visit(IndexExp* index_exp)override;
    Semantic_types visit(SliceExp* slice_exp)override;
    Semantic_types visit(TypeAssertionExp* type_assertion_exp)override;
    Semantic_types visit(ArgumentsExp* arguments_exp)override;
};

#endif //TYPECHEKER_H
