#include "ast.h"
#include <iostream>

using namespace std;
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

// BinaryExp ──────────────────────────────────────────

BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o) : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}

// UnaryExp ───────────────────────────────────────────

UnaryExp::UnaryExp(Exp* e, UnaryOp o, bool _postfix) : exp(e), op(o) , postfix(_postfix){}

UnaryExp::~UnaryExp() {
    delete exp;
}

// IntExp ─────────────────────────────────────────────

IntExp::IntExp(int v) : value(v) {}

IntExp::~IntExp() {}

// FloatExp ───────────────────────────────────────────

FloatExp::FloatExp(float v) : value(v) {}

FloatExp::~FloatExp() {}

// BoolExp ────────────────────────────────────────────

BoolExp::BoolExp(bool v) : value(v) {}

BoolExp::~BoolExp() {}

// StringExp ──────────────────────────────────────────

StringExp::StringExp(string v) : value(v) {}

StringExp::~StringExp() {}

// IdExp ──────────────────────────────────────────────
/*
IdExp::IdExp(string n) : name(n) {}

IdExp::~IdExp() {}
*/
// Programa ───────────────────────────────────────────

Programa::Programa() {}

Programa::~Programa() {}

// Stmt ───────────────────────────────────────────────


// DeclStmt ───────────────────────────────────────────



// AutoDeclStmt ───────────────────────────────────────

// AssignStmt ─────────────────────────────────────────


// BlockStmt ──────────────────────────────────────────
/*
BlockStmt::BlockStmt(list<Stmt*> _stmts) : stmts(_stmts) {}

BlockStmt::~BlockStmt() {
    for (Stmt* s : stmts){
        delete s;
    }
}
*/

// ExpStmt ────────────────────────────────────────────
/*
ExpStmt::ExpStmt(Exp* _exp) : exp(_exp) {}

ExpStmt::~ExpStmt() {
    delete exp;
}
*/

// PrintStmt  ─────────────────────────────────────────
/*
PrintStmt::PrintStmt(Exp* e) : exp(e) {}
PrintStmt::~PrintStmt() { delete exp; }*/