#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include "Semantic_types.h"

class ExpList;
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
class IdentifierList;
class TopLevelDecl;
class Declaration;
class FunctionDecl;
class MethodDecl;
class ParameterList;
class Type;
class TypeSpec;
class ConstSpec;
class VarSpec;
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
    list<TopLevelDecl*> listatopleveldecl;
    void accept(Visitor* visitor);
    ~Programa();
    Programa();
};

class TopLevelDecl {
public: 
    virtual void accept(Visitor* visitor);
    virtual ~TopLevelDecl();
    
};

class Declaration : public TopLevelDecl {
public:
    virtual void accept(Visitor* visitor);
    virtual ~Declaration();
};
class FunctionDecl : public TopLevelDecl {
public:
    string name;
    ParameterList* lista_de_parametros;
    Type* tipo;
    Block* cuerpo;
    void accept(Visitor* visitor);
    FunctionDecl();
    ~FunctionDecl();
};
class MethodDecl : public TopLevelDecl {
public:

    ParameterList* lista_de_parametros;
    Type* tipo;
    Block* cuerpo;
    string nombreMethod;
    string nombreId;
    bool puntero;
    string NombreTipoBase;
    void accept(Visitor* visitor);
    MethodDecl();
    ~MethodDecl();

};

class ConstDecl : public Declaration {
public:
    list<ConstSpec*> constspecList;
    void accept(Visitor* visitor) override;
    ConstDecl();
    ~ConstDecl();
};

class ConstSpec {
public:
    IdentifierList* identifierList;
    Type* tipo;
    ExpList* expresionlist;
    void accept(Visitor* visitor);
    ConstSpec();
    ~ConstSpec();
};

class TypeDecl : public Declaration {
public:
    list<TypeSpec*> typespecList;
    void accept(Visitor* visitor) override;
    TypeDecl();
    ~TypeDecl();
};

class TypeSpec {
public:
    string id;
    Type* tipo;
    void accept(Visitor* visitor);
    TypeSpec();
    ~TypeSpec();
};

class VarDecl : public Declaration {
public:
    list<VarSpec*> varspecList;
    void accept(Visitor* visitor) override;
    VarDecl();
    ~VarDecl();
};

class VarSpec {
public:

    IdentifierList* identifierlist; 
    Type* tipo;
    ExpList* expresionlist;
    void accept(Visitor* visitor);
    VarSpec();
    ~VarSpec();
};

class FieldDecl {
public:
    IdentifierList* identifierlist; 
    Type* type;
    void accept(Visitor* visitor);
    FieldDecl();
    ~FieldDecl() = default;
};


// -------------- Types -----------------------
class Type {
public:
    virtual void accept(Visitor* visitor);
    virtual ~Type() = default;
};

class BasicType : public Type{
public:

    string tipo;
    void accept(Visitor* visitor) override;
  
    BasicType(string t);
    ~BasicType();
};

class ArrayType: public Type {
public:
    Exp* length;
    Type* elementtype;
    void accept(Visitor *visitor) override;
    ArrayType();
    ~ArrayType() = default;
};

class StructType: public Type {
public:
    list<FieldDecl*> declaraciones;
    void accept(Visitor *visitor) override;
    StructType();
    ~StructType();
};


class PointerType: public Type {
public:
    Type* basetype;
    void accept(Visitor *visitor) override;
    PointerType();
    ~PointerType() override;
};

class ParameterList {
public:
    list<ParameterDecl*> parameterList;
    void accept(Visitor* visitor);
    ParameterList();
    ~ParameterList();
};


class ParameterDecl {
public:
    IdentifierList* identifierlist;
    Type* type;
    void accept(Visitor* visitor);
    ParameterDecl();
    ~ParameterDecl();
};

class IdentifierList {
public:
    list<string> lista_ids;
    void accept(Visitor* visitor);
    IdentifierList();
    ~IdentifierList();
};
class ExpList {
public:
    list<Exp*> lista_exp;
    Semantic_types accept(Visitor* visitor);
    ExpList();
    ~ExpList();
};

// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
class Block {
public:
    StmtList* lista_statements;
    Semantic_types accept(Visitor* visitor);
    Block();
    ~Block();
};

class StmtList {
public:
    list<Stmt*> statements;
    Semantic_types accept(Visitor* visitor);
    StmtList();
    ~StmtList();
};

class Stmt{
public:
    virtual Semantic_types accept(Visitor* visitor) = 0;
    virtual ~Stmt() = 0;
};

class DeclarationStmt: public Stmt {
public:
    Declaration* declaration;
    Semantic_types accept(Visitor *visitor) override;
    DeclarationStmt();
    ~DeclarationStmt();
};


class BlockStmt : public Stmt {
public:
    Block* block;
    Semantic_types accept(Visitor *visitor) override;
    BlockStmt();
    ~BlockStmt();
};

class ExpresionStmt : public Stmt {
public:
    Exp* expresion;
    Semantic_types accept(Visitor *visitor) override;
    ExpresionStmt();
    ~ExpresionStmt();
};

class IncDecStmt : public Stmt {
public:
    Exp* expresion;
    UnaryOp op;
    Semantic_types accept(Visitor *visitor) override;
    IncDecStmt();
    ~IncDecStmt();
};

class Assigment : public Stmt {
public:
    ExpList* expresion_list_id;
    ExpList* expresion_list_values;
    AssignOp op;
    Semantic_types accept(Visitor *visitor) override;
    Assigment();
    ~Assigment();
};

class ReturnStmt : public Stmt {
public:
    ExpList* expresion_list;
    Semantic_types accept(Visitor *visitor) override;
    ReturnStmt();
    ~ReturnStmt();
};

class BreakStmt: public Stmt {
public:
    Semantic_types accept(Visitor *visitor) override;
    BreakStmt();
    ~BreakStmt();
};

class ContinueStmt: public Stmt {
public:
    Semantic_types accept(Visitor *visitor) override;
    ContinueStmt();
    ~ContinueStmt();
};

class IfStmt: public Stmt {
public:
    Exp* expresion;
    Block* cuerpo_if;
    Block* cuerpo_else;
    IfStmt* if_anidado;
    Semantic_types accept(Visitor *visitor) override;
    IfStmt();
    ~IfStmt();
};

class SwitchStmt: public Stmt {
public:
    Exp* expresion;
    list<ExpCaseClause*> exp_case_clause;
    Semantic_types accept(Visitor *visitor) override;
    SwitchStmt();
    ~SwitchStmt();
};

class ExpCaseClause {
public:
    ExpList* expresion_list;
    StmtList* statement_list;
    Semantic_types accept(Visitor *visitor);
    ExpCaseClause();
    ~ExpCaseClause();
};

class ForStmt: public Stmt {
public:
    Exp* expresion;
    ForClause* for_clause;
    Block* block;
    Semantic_types accept(Visitor *visitor) override;
    ForStmt();
    ~ForStmt();
};

class ForClause {
public:
    Assigment* asignacion1;
    Exp* expresion;
    Assigment* asignacion2;
    IncDecStmt* inc_dec_stmt;
    Semantic_types accept(Visitor *visitor);
    ForClause();
    ~ForClause();
};

// ----------------------------------------------------------------------
// Parte Nico: Expresion , en proceso
// ----------------------------------------------------------------------
class Exp {
public:
    virtual Semantic_types accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToString(BinaryOp op);
    static string unopToString(UnaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp*     left;
    Exp*     right;
    BinaryOp op;
    Semantic_types accept(Visitor* visitor) override;
    BinaryExp(Exp* _left, Exp* _right, BinaryOp _op);
    ~BinaryExp();
};

class UnaryExp : public Exp {
public:
    Exp*    exp;
    UnaryOp op;
    bool    postfix;
    Semantic_types accept(Visitor* visitor) override;
    UnaryExp(Exp* _exp, UnaryOp _op, bool _postfix = false);
    ~UnaryExp();
};

class IntExp : public Exp {
public:
    int value;
    Semantic_types accept(Visitor* visitor) override;
    IntExp(int v);
    ~IntExp();
};

class IdExp : public Exp {
public:
    string name;
    Semantic_types accept(Visitor* visitor) override;
    IdExp(string _name);
    ~IdExp();
};

class QualifiedIdent : public Exp {
public:
    Exp* prefijo;
    string subfijo;
    Semantic_types accept(Visitor* visitor) override;
    QualifiedIdent();
    ~QualifiedIdent();
};
class FloatExp : public Exp {
public:
    float value;
    Semantic_types accept(Visitor* visitor) override;
    FloatExp(float v);
    ~FloatExp();
};

class BoolExp : public Exp {
public:
    bool value;
    Semantic_types accept(Visitor* visitor) override;
    BoolExp(bool v);
    ~BoolExp();
};


class StringExp : public Exp {
public:
    string value;
    Semantic_types accept(Visitor* visitor) override;
    StringExp(string v);
    ~StringExp();
};



#endif // AST_H
