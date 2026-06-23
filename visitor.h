#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include "token.h"
#include <list>
#include <unordered_map>

#include "Semantic_types.h"

class Visitor {
public:
    virtual ~Visitor() = default;

    // Parte Rayhan //
    virtual Semantic_types visit(Programa* programa) = 0;
    virtual Semantic_types visit(FunctionDecl* fun_decl) = 0;
    virtual Semantic_types visit(MethodDecl* method_decl) = 0;
    virtual Semantic_types visit(ConstDecl* const_decl) = 0;
    virtual Semantic_types visit(ConstSpec* const_spec) = 0;
    virtual Semantic_types visit(TypeDecl* type_decl) = 0;
    virtual Semantic_types visit(TypeSpec* type_spec) = 0;
    virtual Semantic_types visit(VarDecl* var_decl) = 0;
    virtual Semantic_types visit(VarSpec* var_spec) = 0;
    virtual Semantic_types visit(FieldDecl* field_decl) = 0;
    virtual Semantic_types visit(ArrayType* array_type) = 0;
    virtual Semantic_types visit(StructType* struct_type) = 0;
    virtual Semantic_types visit(PointerType* pointer_type) = 0;
    virtual Semantic_types visit(ParameterList* parameter_list) = 0;
    virtual Semantic_types visit(ParameterDecl* parameter_decl) = 0;
    //  Parte Bruno //
    virtual Semantic_types visit(Block* block) = 0;
    virtual Semantic_types visit(StmtList* stmt_list) = 0;
    virtual Semantic_types visit(DeclarationStmt* declaration_stmt) = 0;
    virtual Semantic_types visit(BlockStmt* block_stmt) = 0;
    virtual Semantic_types visit(ExpresionStmt* expresion_stmt) = 0;
    virtual Semantic_types visit(IncDecStmt* inc_dec_stmt) = 0;
    virtual Semantic_types visit(Assigment* assigment_stmt) = 0;
    virtual Semantic_types visit(ReturnStmt* return_stmt) = 0;
    virtual Semantic_types visit(BreakStmt* break_stmt) = 0;
    virtual Semantic_types visit(ContinueStmt* continue_stmt) = 0;
    virtual Semantic_types visit(IfStmt* if_stmt) = 0;
    virtual Semantic_types visit(SwitchStmt* switch_stmt) = 0;
    virtual Semantic_types visit(ExpCaseClause* exp_case_clause) = 0;
    virtual Semantic_types visit(ForStmt* for_stmt) = 0;
    virtual Semantic_types visit(ForClause* for_clause) = 0;
    //  Parte Nico //
};

#endif // VISITOR_H