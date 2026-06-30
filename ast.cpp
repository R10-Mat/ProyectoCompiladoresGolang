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

UnaryOp tokenToUnaryOp(Token::Type t) {
    switch (t) {
        case Token::NEG: return NEG_OP;
        case Token::NOT: return NOT_OP;
        default:
            throw runtime_error("Operador unario no soportado en este UnaryOp");
    }
}

BinaryOp tokenToBinaryOp(Token::Type t) {
    switch (t) {
        case Token::PLUS:      return PLUS_OP;
        case Token::NEG:       return MINUS_OP;
        case Token::OR:        return OR_OP;
        case Token::MUL:       return MUL_OP;
        case Token::DIV:       return DIV_OP;
        case Token::MOD:       return MOD_OP;
        case Token::AND:       return AND_OP;
        case Token::LES:       return LT_OP;
        case Token::LEQ:       return LTE_OP;
        case Token::GER:       return GT_OP;
        case Token::GEQ:       return GTE_OP;
        case Token::EQUAL:     return EQ_OP;
        case Token::DISTINCT:  return NEQ_OP;
        default:
            throw runtime_error("Token no es un operador binario valido");
    }
}

SliceExp::SliceExp(Exp* expresion, Exp* low, Exp* high, Exp* max) : expresion(expresion), low(low), high(high), max(max) {}

SliceExp::SliceExp() : expresion(nullptr), low(nullptr), high(nullptr), max(nullptr) {}


SliceExp::~SliceExp() {
    delete expresion;
    delete low;
    delete high;
    delete max;
}


TypeAssertionExp::TypeAssertionExp(Exp* expresion, Type* tipo) : expresion(expresion), tipo(tipo) {}

TypeAssertionExp::TypeAssertionExp() : expresion(nullptr), tipo(nullptr) {}

TypeAssertionExp::~TypeAssertionExp() {
    delete expresion;
    delete tipo;
}

ArgumentsExp::ArgumentsExp(Exp* funcion, vector<Exp*> args, bool variadic) : funcion(funcion), args(move(args)), variadic(variadic) {}

ArgumentsExp::ArgumentsExp() : funcion(nullptr), args(), variadic(false) {}

ArgumentsExp::~ArgumentsExp() {
    delete funcion;
    for (Exp* arg : args) delete arg;
}

IndexExp::IndexExp(Exp* expresion, Exp* indice) : expresion(expresion), indice(indice) {}

IndexExp::IndexExp() : expresion(nullptr), indice(nullptr) {}

IndexExp::~IndexExp() {
    delete expresion;
    delete indice;
}

SelectorExp::SelectorExp(Exp* expresion, const string& campo) : expresion(expresion), campo(campo) {}

SelectorExp::~SelectorExp() {
    delete expresion;
}

FunctionLit::FunctionLit(ParameterList* lista_de_parametros, Type* tipo, Block* cuerpo) : lista_de_parametros(lista_de_parametros), tipo(tipo), cuerpo(cuerpo) {}

FunctionLit::~FunctionLit() {
    delete lista_de_parametros;
    delete tipo;
    delete cuerpo;
}

CompositeLitExp::CompositeLitExp(Type* tipo, vector<KeyedElement*> elementos) : tipo(tipo), elementos(elementos) {}

CompositeLitExp::CompositeLitExp() {}

CompositeLitExp::~CompositeLitExp() {
    delete tipo;
    for (auto e : elementos) delete e;
}

KeyedElement::KeyedElement(Exp* key, Exp* value) : key(key), value(value) {}
KeyedElement::KeyedElement(){}

KeyedElement::~KeyedElement() {
    delete key;
    delete value;
}

BasicLitExp::BasicLitExp(Token::Type tipoLiteral, string valor) : tipoLiteral(tipoLiteral), valor(valor) {}

BasicLitExp::BasicLitExp() {}

BasicLitExp::~BasicLitExp() {}

OperandNameExp::OperandNameExp(string name, vector<Type*> typeArgs) : name(name), typeArgs(typeArgs) {}

OperandNameExp::OperandNameExp() {}

OperandNameExp::~OperandNameExp() {
    for (auto t : typeArgs) delete t;
}

ParenExp::ParenExp(Exp* expresion) : expresion(expresion) {}

ParenExp::~ParenExp() {
    delete expresion;
}