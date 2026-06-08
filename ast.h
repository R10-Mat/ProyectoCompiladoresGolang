#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>

#include "ast.h"
#include "ast.h"
#include "ast.h"
#include "ast.h"
#include "token.h"

class ForClause;
class ExpCaseClause;
class TypeTerm;
class InterfaceElem;
class ParameterDecl;
class Signature;
class TypeArgs;
class TypeName;
class TypeList;
class IdExp;
class FieldDecl;
class Exp;
class StmtList;
class Block;
using namespace std;

class Visitor; 
class Stmt;

enum BinaryOp {
    PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, MOD_OP, POW_OP,
    EQ_OP, NEQ_OP, LT_OP, GT_OP, LTE_OP, GTE_OP,
    AND_OP, OR_OP
};

enum AssignOp {
    PLUS_ASSIGN, NEG_ASSIGN, MUL_ASSIGN, DIV_ASSIGN
};

enum UnaryOp {
    NEG_OP,   
    NOT_OP,   
    INC_OP,   
    DEC_OP    
};

// ----------------------------------------------------------------------
// Parte Rayhan: global y types
// ----------------------------------------------------------------------

class Programa {
public:
    Block* bloque;
    void accept(Visitor* visitor);
    ~Programa();
    Programa();
};

// -------------- Types -----------------------
class Type {
public:
    virtual void accept(Visitor* visitor);
    virtual ~Type() = default;
};

class TypeName_TypeArgs : public Type {
public:
    TypeName* nombres;
    TypeArgs* argumentos;
    void accept(Visitor *visitor) override;
    TypeName_TypeArgs();

    TypeName_TypeArgs(TypeName * tname, TypeArgs * targs);
    ~TypeName_TypeArgs() override;
};

class TypeName: public Type {
public:
    string nombre;
    string prefijo_paquete;
    void accept(Visitor *visitor) override;
    TypeName();
    ~TypeName() override;
};

class TypeArgs: public Type {
public:
    TypeList* lista;
    void accept(Visitor *visitor) override;
    TypeArgs();
    ~TypeArgs() override;
};

class TypeList: public Type {
public:
    list<Type*> lista_tipos;
    void accept(Visitor *visitor) override;
    TypeList();
    ~TypeList() override;
};

class TypeLiteral: public Type {
public:
    void accept(Visitor *visitor) override;
    ~TypeLiteral() = default;
};

class ArrayType: public TypeLiteral {
public:
    Exp* length;
    Type* elementtype;
    void accept(Visitor *visitor) override;
    ArrayType();
    ~ArrayType() = default;
};

class StructType: public TypeLiteral {
public:
    list<FieldDecl*> declaraciones;
    void accept(Visitor *visitor) override;
    StructType();
    ~StructType() = default;
};

class FieldDecl {
public:
    list<IdExp*> identifierlist;
    Type* type;
    bool tiene_mul;
    string tag;
    FieldDecl();
    ~FieldDecl() = default;
};

class PointerType: public TypeLiteral {
public:
    Type* basetype;
    void accept(Visitor *visitor) override;
    PointerType();
    ~PointerType() override;
};

class FunctionType: public TypeLiteral {
public:
    Signature* signature;
    void accept(Visitor *visitor) override;
    FunctionType();
    ~FunctionType() override;
};

class Signature {
public:
    list<ParameterDecl*> parameterlist;
    list<ParameterDecl*> result_parameters;
    void accept(Visitor *visitor);
    Signature();
    ~Signature();
};

class ParameterDecl {
public:
    list<IdExp*> identifierlist;
    Type* type;
    bool es_variadico;

    ParameterDecl();
    ~ParameterDecl();
};

class InterfaceType: public TypeLiteral {
public:
    list<InterfaceElem*> elementos;
    void accept(Visitor *visitor) override;
    InterfaceType();
    ~InterfaceType() override;
};

class InterfaceElem {
    virtual ~InterfaceElem() = default;
    virtual void accept(Visitor* visitor) = 0;
};

class MethodElem: public InterfaceElem {
public:
    string identifier;
    Signature* signature;
    void accept(Visitor *visitor) override;
    MethodElem();
    ~MethodElem() override;
};

class TypeElem: public InterfaceElem {
    list<TypeTerm> terminos;
    TypeElem();
    ~TypeElem() override;
    void accept(Visitor *visitor) override;
};

class TypeTerm {
    bool tiene_underline;
    Type* tipo;
    void accept(Visitor* visitor);
    TypeTerm();
    ~TypeTerm();
};

class SliceType: public TypeLiteral {
public:
    Type* elementtype;
    void accept(Visitor *visitor) override;
    SliceType();
    ~SliceType() override;
};

class ChannelType: public TypeLiteral {
public:
    Type* elementtype;
    void accept(Visitor *visitor) override;
    ChannelType();
    ~ChannelType() override;
};

class MapType: public TypeLiteral {
public:
    Type* keytype;
    Type* elementtype;
    void accept(Visitor *visitor) override;
    MapType();
    ~MapType() override;
};

class ExpList {
public:
    vector<Exp*> expressions;

};

// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
class Block {
public:
    StmtList* lista_statements;
    void accept(Visitor* visitor);
    Block();
    ~Block();
};

class StmtList {
public:
    list<Stmt*> statements;
    void accept(Visitor* visitor);
    StmtList();
    ~StmtList();
};

class Stmt{
public:
    virtual void accept(Visitor* visitor) = 0;
    virtual ~Stmt() = 0;
};

class DeclarationStmt: public Stmt {
public:
    Declaration* declaration;
    void accept(Visitor *visitor) override;
    DeclarationStmt();
    ~DeclarationStmt();
};


class BlockStmt : public Stmt {
public:
    Block* block;
    void accept(Visitor *visitor) override;
    BlockStmt();
    ~BlockStmt();
};

class ExpresionStmt : public Stmt {
public:
    Exp* expresion;
    void accept(Visitor *visitor) override;
    ExpresionStmt();
    ~ExpresionStmt();
};

class IncDecStmt : public Stmt {
public:
    Exp* expresion;
    UnaryOp op;
    void accept(Visitor *visitor) override;
    IncDecStmt();
    ~IncDecStmt();
};

class Assigment : public Stmt {
public:
    ExpList* expresion_list_id;
    ExpList* expresion_list_values;
    AssignOp op;
    void accept(Visitor *visitor) override;
    Assigment();
    ~Assigment();
};

class ReturnStmt : public Stmt {
public:
    ExpList* expresion_list;
    void accept(Visitor *visitor) override;
    ReturnStmt();
    ~ReturnStmt();
};

class BreakStmt: public Stmt {
public:
    void accept(Visitor *visitor) override;
    BreakStmt();
    ~BreakStmt();
};

class ContinueStmt: public Stmt {
public:
    void accept(Visitor *visitor) override;
    ContinueStmt();
    ~ContinueStmt();
};

class IfStmt: public Stmt {
public:
    Exp* expresion;
    Block* cuerpo_if;
    Block* cuerpo_else;
    IfStmt* if_anidado;
    void accept(Visitor *visitor) override;
    IfStmt();
    ~IfStmt();
};

class SwitchStmt: public Stmt {
public:
    Exp* expresion;
    list<ExpCaseClause*> exp_case_clause;
    void accept(Visitor *visitor) override;
    SwitchStmt();
    ~SwitchStmt();
};

class ExpCaseClause {
public:
    ExpList* expresion_list;
    StmtList* statement_list;
    void accept(Visitor *visitor);
    ExpCaseClause();
    ~ExpCaseClause();
};

class ForStmt: public Stmt {
public:
    Exp* expresion;
    ForClause* for_clause;
    Block* block;
    void accept(Visitor *visitor) override;
    ForStmt();
    ~ForStmt();
};

class ForClause {
public:
    Assigment* asignacion1;
    Exp* expresion;
    Assigment* asignacion2;
    IncDecStmt* inc_dec_stmt;
    void accept(Visitor *visitor);
    ForClause();
    ~ForClause();
};

// ----------------------------------------------------------------------
// Parte Nico: Expresions
// ----------------------------------------------------------------------
class Exp {
public:
    virtual void accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToString(BinaryOp op);
    static string unopToString(UnaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp*     left;
    Exp*     right;
    BinaryOp op;
    void accept(Visitor* visitor) override;
    BinaryExp(Exp* _left, Exp* _right, BinaryOp _op);
    ~BinaryExp();
};

class UnaryExp : public Exp {
public:
    Exp*    exp;
    UnaryOp op;
    bool    postfix;
    void accept(Visitor* visitor) override;
    UnaryExp(Exp* _exp, UnaryOp _op, bool _postfix = false);
    ~UnaryExp();
};

class IntExp : public Exp {
public:
    int value;
    void accept(Visitor* visitor) override;
    IntExp(int v);
    ~IntExp();
};

class IdExp : public Exp {
public:
    string name;
    void accept(Visitor* visitor) override;
    IdExp();
    ~IdExp();
};

class QualifiedIdent : public Exp {
public:
    Exp* prefijo;
    string subfijo;
    void accept(Visitor* visitor) override;
    QualifiedIdent();
    ~QualifiedIdent();
};
class FloatExp : public Exp {
public:
    float value;
    void accept(Visitor* visitor) override;
    FloatExp(float v);
    ~FloatExp();
};

class BoolExp : public Exp {
public:
    bool value;
    void accept(Visitor* visitor) override;
    BoolExp(bool v);
    ~BoolExp();
};


class StringExp : public Exp {
public:
    string value;
    void accept(Visitor* visitor) override;
    StringExp(string v);
    ~StringExp();
};



#endif // AST_H
