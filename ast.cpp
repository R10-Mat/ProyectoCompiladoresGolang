#include "ast.h"
#include <iostream>

using namespace std;

// ----------------------------------------------------------------------
// Parte Rayhan: global y types
// ----------------------------------------------------------------------

Programa::Programa() {}
Programa::~Programa() { for (auto d : listatopleveldecl) delete d; }

// TopLevelDecl y Declaration son bases abstractas (nunca se instancian
// directamente, siempre se crea una subclase concreta), pero el destructor
// virtual puro necesita cuerpo porque las subclases lo invocan en cadena.
TopLevelDecl::~TopLevelDecl() {}
Declaration::~Declaration() {}

FunctionDecl::FunctionDecl() : lista_de_parametros(nullptr), tipo(nullptr), cuerpo(nullptr) {}
FunctionDecl::~FunctionDecl() { delete lista_de_parametros; delete tipo; delete cuerpo; }

MethodDecl::MethodDecl() : lista_de_parametros(nullptr), tipo(nullptr), cuerpo(nullptr), puntero(false) {}
MethodDecl::~MethodDecl() { delete lista_de_parametros; delete tipo; delete cuerpo; }

ConstDecl::ConstDecl() {}
ConstDecl::~ConstDecl() { for (auto s : constspecList) delete s; }

ConstSpec::ConstSpec() : identifierList(nullptr), tipo(nullptr), expresionlist(nullptr) {}
ConstSpec::~ConstSpec() { delete identifierList; delete tipo; delete expresionlist; }

TypeDecl::TypeDecl() {}
TypeDecl::~TypeDecl() { for (auto s : typespecList) delete s; }

TypeSpec::TypeSpec() : tipo(nullptr) {}
TypeSpec::~TypeSpec() { delete tipo; }

VarDecl::VarDecl() {}
VarDecl::~VarDecl() { for (auto s : varspecList) delete s; }

VarSpec::VarSpec() : identifierlist(nullptr), tipo(nullptr), expresionlist(nullptr) {}
VarSpec::~VarSpec() { delete identifierlist; delete tipo; delete expresionlist; }

FieldDecl::FieldDecl() : identifierlist(nullptr), type(nullptr) {}
FieldDecl::~FieldDecl() { delete identifierlist; delete type; }

BasicType::BasicType(string t) : tipo(t) {}
BasicType::~BasicType() {}

ArrayType::ArrayType() : length(nullptr), elementtype(nullptr) {}
ArrayType::~ArrayType() { delete length; delete elementtype; }

StructType::StructType() {}
StructType::~StructType() { for (auto f : declaraciones) delete f; }

PointerType::PointerType() : basetype(nullptr) {}
PointerType::~PointerType() { delete basetype; }

ParameterList::ParameterList() {}
ParameterList::~ParameterList() { for (auto p : parameterList) delete p; }

ParameterDecl::ParameterDecl() : identifierlist(nullptr), type(nullptr) {}
ParameterDecl::~ParameterDecl() { delete identifierlist; delete type; }

IdentifierList::IdentifierList() {}
IdentifierList::~IdentifierList() {}

ExpList::ExpList() {}
ExpList::~ExpList() { for (auto e : lista_exp) delete e; }

// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
// Los campos opcionales de la gramatica (ej: los tres campos de ForClause,
// o cuerpo_else/if_anidado de IfStmt) pueden quedar sin asignar cuando esa
// parte simplemente no aparece en el codigo fuente (no es un error). Por
// eso todo puntero se inicializa en nullptr: delete nullptr no hace nada,
// pero delete sobre un puntero sin inicializar es undefined behavior.
Block::Block() : lista_statements(nullptr) {}
Block::~Block() { delete lista_statements; }

// Stmt es una base abstracta pura (accept = 0), pero su destructor virtual
// puro necesita cuerpo porque cada destructor derivado lo invoca en cadena.
Stmt::~Stmt() {}

StmtList::StmtList() {}
StmtList::~StmtList() { for (const auto i:statements) delete i;}

DeclarationStmt::DeclarationStmt() : declaration(nullptr) {}
DeclarationStmt::~DeclarationStmt() {delete declaration;}

BlockStmt::BlockStmt() : block(nullptr) {}
BlockStmt::~BlockStmt() {delete block;}

ExpresionStmt::ExpresionStmt() : expresion(nullptr) {}
ExpresionStmt::~ExpresionStmt() {delete expresion;}

IncDecStmt::IncDecStmt() : expresion(nullptr), op(INC_OP) {}
IncDecStmt::~IncDecStmt() {delete expresion;}

Assigment::Assigment() : expresion_list_id(nullptr), expresion_list_values(nullptr), op(ASSIGN) {}
Assigment::~Assigment() {delete expresion_list_id; delete expresion_list_values;}

ReturnStmt::ReturnStmt() : expresion_list(nullptr) {}
ReturnStmt::~ReturnStmt() {delete expresion_list;}

BreakStmt::BreakStmt() {}
BreakStmt::~BreakStmt() {}

ContinueStmt::ContinueStmt() {}
ContinueStmt::~ContinueStmt() {}

IfStmt::IfStmt() : expresion(nullptr), cuerpo_if(nullptr), cuerpo_else(nullptr), if_anidado(nullptr) {}
IfStmt::~IfStmt() {delete expresion; delete cuerpo_if; delete cuerpo_else; delete if_anidado;}

SwitchStmt::SwitchStmt() : expresion(nullptr) {}
SwitchStmt::~SwitchStmt() {delete expresion; for (auto i : exp_case_clause) delete i;}

ExpCaseClause::ExpCaseClause() : expresion_list(nullptr), statement_list(nullptr) {}
ExpCaseClause::~ExpCaseClause() {delete expresion_list;delete statement_list;}

ForStmt::ForStmt() : expresion(nullptr), for_clause(nullptr), block(nullptr) {}
ForStmt::~ForStmt() {delete expresion; delete for_clause; delete block;}

ForClause::ForClause() : asignacion1(nullptr), expresion(nullptr), asignacion2(nullptr), inc_dec_stmt(nullptr) {}
ForClause::~ForClause() {delete expresion; delete asignacion1; delete asignacion2; delete inc_dec_stmt;}

// ----------------------------------------------------------------------
// Parte Nico: Expresion , en proceso
// ----------------------------------------------------------------------
// Exp ────────────────────────────────────────────────

Exp::~Exp() {}

// BinaryExp ──────────────────────────────────────────

BinaryExp::BinaryExp(Exp* left, Exp* right, BinaryOp op) : left(left), right(right), op(op) {}

BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}

// UnaryExprExp ───────────────────────────────────────

UnaryExprExp::UnaryExprExp(Exp* expresion, UnaryOp op, bool postfix) : expresion(expresion), op(op), postfix(postfix) {}

UnaryExprExp::~UnaryExprExp() {
    delete expresion;
}

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
        case POS_OP:   return "+";
        case NEG_OP:   return "-";
        case NOT_OP:   return "!";
        case DEREF_OP: return "*";
        case ADDR_OP:  return "&";
        case INC_OP:   return "++";
        case DEC_OP:   return "--";
        default:       return "?";
    }
}

UnaryOp tokenToUnaryOp(Token::Type t) {
    switch (t) {
        case Token::PLUS: return POS_OP;
        case Token::NEG:  return NEG_OP;
        case Token::NOT:  return NOT_OP;
        case Token::MUL:  return DEREF_OP;  // *p: dereferencia de puntero
        case Token::AND:  return ADDR_OP;   // &x: direccion de memoria
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

ArgumentsExp::ArgumentsExp(Exp* funcion, ExpList* args, bool variadic) : funcion(funcion), args(args), variadic(variadic) {}

ArgumentsExp::ArgumentsExp() : funcion(nullptr), args(), variadic(false) {}

ArgumentsExp::~ArgumentsExp() {
    delete funcion;
    delete args;
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

FunctionLit::FunctionLit() : lista_de_parametros(nullptr), tipo(nullptr), cuerpo(nullptr) {}

FunctionLit::~FunctionLit() {
    delete lista_de_parametros;
    delete tipo;
    delete cuerpo;
}

CompositeLitExp::CompositeLitExp(Type* tipo, vector<KeyedElement*> elementos) : tipo(tipo), elementos(elementos) {}

CompositeLitExp::CompositeLitExp() : tipo(nullptr) {}

CompositeLitExp::~CompositeLitExp() {
    delete tipo;
    for (auto e : elementos) delete e;
}

KeyedElement::KeyedElement(Exp* key, Exp* value) : key(key), value(value) {}
KeyedElement::KeyedElement() : key(nullptr), value(nullptr) {}

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