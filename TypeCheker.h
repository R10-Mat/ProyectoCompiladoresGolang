//
// Created by Usuario on 21/6/2026.
//

#ifndef TYPECHEKER_H
#define TYPECHEKER_H

#include "Enviroment.h"
#include "Semantic_types.h"
#include "visitor.h"

using namespace std;

// Estructura para almacenar la firma de una función en la tabla global de funciones
struct FunctionSignature {
    vector<Semantic_types> param_types; // Tipos de los parámetros en orden
    Semantic_types return_types;        // Tipo de retorno (puede ser VOID)
};

class TypeChekerVisitor: public Visitor{
public:
    // [key] = nombre de la función -> [value] = Firma (tipos de parámetros y retorno)
    unordered_map<string,FunctionSignature> tabla_funciones;

    // [key] = nombre de la función -> [value] = número total de variables locales que genera
    unordered_map<string,int> fun_var_contador;

    //Entorno de variables
    Environment<Semantic_types> entorno;

    // Nombre de la function actual
    string nombre_funcion_actual;

    // tipo de retorno de la funcion actual
    Semantic_types tipo_retorno_actual;

    // numero de fors anidados
    int dentro_de_bucle = 0;

    void TypeCheker(Programa* programa);

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
};



#endif //TYPECHEKER_H
