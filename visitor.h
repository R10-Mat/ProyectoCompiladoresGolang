#ifndef VISITOR_H
#define VISITOR_H

#include "Semantic_types.h"

class Programa;
class FunctionDecl;
class MethodDecl;
class ConstDecl;
class ConstSpec;
class TypeDecl;
class TypeSpec;
class VarDecl;
class VarSpec;
class FieldDecl;
class BasicType;
class ArrayType;
class StructType;
class PointerType;
class ParameterList;
class ParameterDecl;
class Block;
class StmtList;
class DeclarationStmt;
class BlockStmt;
class ExpresionStmt;
class IncDecStmt;
class Assigment;
class ReturnStmt;
class BreakStmt;
class ContinueStmt;
class IfStmt;
class SwitchStmt;
class ExpCaseClause;
class ForStmt;
class ForClause;
class BinaryExp;
class UnaryExprExp;
class ParenExp;
class OperandNameExp;
class BasicLitExp;
class CompositeLitExp;
class FunctionLit;
class SelectorExp;
class IndexExp;
class SliceExp;
class TypeAssertionExp;
class ArgumentsExp;

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
    virtual Semantic_types visit(BasicType* basic_type) = 0;
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
    virtual Semantic_types visit(BinaryExp* binary_exp) = 0;
    virtual Semantic_types visit(UnaryExprExp* unary_exp) = 0;
    virtual Semantic_types visit(ParenExp* paren_exp) = 0;
    virtual Semantic_types visit(OperandNameExp* operand_name_exp) = 0;
    virtual Semantic_types visit(BasicLitExp* basic_lit_exp) = 0;
    virtual Semantic_types visit(CompositeLitExp* composite_lit_exp) = 0;
    virtual Semantic_types visit(FunctionLit* function_lit) = 0;
    virtual Semantic_types visit(SelectorExp* selector_exp) = 0;
    virtual Semantic_types visit(IndexExp* index_exp) = 0;
    virtual Semantic_types visit(SliceExp* slice_exp) = 0;
    virtual Semantic_types visit(TypeAssertionExp* type_assertion_exp) = 0;
    virtual Semantic_types visit(ArgumentsExp* arguments_exp) = 0;
};

#endif // VISITOR_H
