#include "ast.h"
#include <iostream>

using namespace std;

// ----------------------------------------------------------------------
// Parte Rayhan: global y types
// ----------------------------------------------------------------------



// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
Block::Block() {}
Block::~Block() { delete lista_statements; }

StmtList::StmtList() {}
StmtList::~StmtList() { for (const auto i:statements) delete i;}

DeclarationStmt::DeclarationStmt() {}
DeclarationStmt::~DeclarationStmt() {delete declaration;}

BlockStmt::BlockStmt() {}
BlockStmt::~BlockStmt() {delete block;}

ExpresionStmt::ExpresionStmt() {}
ExpresionStmt::~ExpresionStmt() {delete expresion;}

IncDecStmt::IncDecStmt() {}
IncDecStmt::~IncDecStmt() {delete expresion;}

Assigment::Assigment() {}
Assigment::~Assigment() {delete expresion_list_id; delete expresion_list_values;}

ReturnStmt::ReturnStmt() {}
ReturnStmt::~ReturnStmt() {delete expresion_list;}

BreakStmt::BreakStmt() {}
BreakStmt::~BreakStmt() {}

ContinueStmt::ContinueStmt() {}
ContinueStmt::~ContinueStmt() {}

IfStmt::IfStmt() {}
IfStmt::~IfStmt() {delete expresion; delete cuerpo_if; delete cuerpo_else; delete if_anidado;}

SwitchStmt::SwitchStmt() {}
SwitchStmt::~SwitchStmt() {delete expresion; for (auto i : exp_case_clause) delete i;}

ExpCaseClause::ExpCaseClause() {}
ExpCaseClause::~ExpCaseClause() {delete expresion_list;delete statement_list;}

ForStmt::ForStmt() {}
ForStmt::~ForStmt() {delete expresion; delete for_clause; delete block;}

ForClause::ForClause(){}
ForClause::~ForClause() {delete expresion; delete asignacion1; delete asignacion2; delete inc_dec_stmt;}

// ----------------------------------------------------------------------
// Parte Nico: Expresion , en proceso
// ----------------------------------------------------------------------
// Exp ────────────────────────────────────────────────

Exp::~Exp() {}

string Exp::binopToString(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case MOD_OP:   return "%";
        case POW_OP:   return "**";
        case EQ_OP:    return "==";
        case NEQ_OP:   return "!=";
        case LT_OP:    return "<";
        case GT_OP:    return ">";
        case LTE_OP:   return "<=";
        case GTE_OP:   return ">=";
        case AND_OP:   return "&&";
        case OR_OP:    return "||";
        default:       return "?";
    }
}

string Exp::unopToString(UnaryOp op) {
    switch (op) {
        case NEG_OP: return "-";
        case NOT_OP: return "!";
        case INC_OP: return "++";
        case DEC_OP: return "--";
        default:     return "?";
    }
}
