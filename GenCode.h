//
// Created by Usuario on 21/6/2026.
//

#ifndef GENCODE_H
#define GENCODE_H
#include "visitor.h"
#include "Semantic_types.h"

class GenCode: public Visitor {

    // Parte Rayhan //

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



#endif //GENCODE_H
