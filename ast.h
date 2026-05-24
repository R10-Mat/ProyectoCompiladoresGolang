#ifndef AST_H
#define AST_H

#include <string>
#include <unordered_map>
#include <list>
#include <ostream>
#include "token.h"

using namespace std;

class Visitor; 
class Stmt;

enum BinaryOp {
    PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, MOD_OP, POW_OP,
    EQ_OP, NEQ_OP, LT_OP, GT_OP, LTE_OP, GTE_OP,
    AND_OP, OR_OP
};

enum UnaryOp {
    NEG_OP,   
    NOT_OP,   
    INC_OP,   
    DEC_OP    
};

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

class IdExp : public Exp {
public:
    string name;
    void accept(Visitor* visitor) override;
    IdExp(string _name);
    ~IdExp();
};

class Programa {
public:
    list<Stmt*> slist;
    void accept(Visitor* visitor);
    ~Programa();
    Programa();

};

class Stmt{
public:
    virtual void accept(Visitor* visitor) = 0;
    virtual ~Stmt() = 0;
};

class DeclStmt : public Stmt {
public:
    string name;
    Token::Type type;  
    Exp* init;  
    void accept(Visitor* visitor) override;
    DeclStmt(string _name, Token::Type _type, Exp* _init);
    ~DeclStmt();
       
};

class AutoDeclStmt : public Stmt {
public:
    string name;
    Exp* init; 
    void accept(Visitor* visitor) override;
    AutoDeclStmt(string _name, Exp* _init);
    ~AutoDeclStmt();        
};

class AssignStmt : public Stmt {
public:
    string name;
    Token::Type op;   
    Exp* exp;
    void accept(Visitor* visitor) override;
    AssignStmt(string _name, Token::Type _op, Exp* _exp);
    ~AssignStmt();
};

class BlockStmt : public Stmt {
public:
    list<Stmt*> stmts;
    void accept(Visitor* visitor) override;
    BlockStmt(list<Stmt*> stmts_);
    ~BlockStmt();
};

class ExpStmt : public Stmt {
public:
    Exp* exp;
    void accept(Visitor* visitor) override;
    ExpStmt(Exp* _exp);
    ~ExpStmt();
};


class PrintStmt : public Stmt {
public:
    Exp* exp;
    void accept(Visitor* visitor) override;
    PrintStmt(Exp* e);
    ~PrintStmt();
};


#endif // AST_H
