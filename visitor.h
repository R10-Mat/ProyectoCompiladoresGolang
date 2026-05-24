#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include "token.h"
#include <list>
#include <unordered_map>

class BinaryExp;
class UnaryExp;
class IntExp;
class FloatExp;
class BoolExp;
class StringExp;
class IdExp;

class Visitor {
public:
    virtual void visit(BinaryExp* exp) = 0;
    virtual void visit(UnaryExp* exp) = 0;
    virtual void visit(IntExp* exp) = 0;
    virtual void visit(FloatExp* exp) = 0;
    virtual void visit(BoolExp* exp) = 0;
    virtual void visit(StringExp* exp)= 0;
    virtual void visit(IdExp* exp) = 0;
    virtual void visit(DeclStmt* stm) = 0;
    virtual void visit(PrintStmt* stm) = 0;
    virtual void visit(AutoDeclStmt* stm) = 0;  
    virtual void visit(AssignStmt* stm) = 0;  
    virtual void visit(BlockStmt* stm) = 0;  
    virtual void visit(ExpStmt* stm) = 0;  
    virtual void visit(Programa* program) = 0;
};

class PrintVisitor : public Visitor {
public:
    void visit(BinaryExp* exp)  override;
    void visit(UnaryExp* exp)   override;
    void visit(IntExp* exp)     override;
    void visit(FloatExp* exp)   override;
    void visit(BoolExp* exp)    override;
    void visit(StringExp* exp)  override;
    void visit(IdExp* exp)      override;
    void visit(DeclStmt* stm)     override;
    void visit(AutoDeclStmt* stm) override;
    void visit(AssignStmt* stm)   override;
    void visit(PrintStmt* stm) override;
    void visit(BlockStmt* stm)    override;
    void visit(ExpStmt* stm)      override;
    void visit(Programa* program) override;
    void imprimir(Programa* program);
};

class EVALVisitor : public Visitor {
public:
    unordered_map<string, int>   memInt;       
    unordered_map<string, float> memFloat;  
    unordered_map<string, bool>  memBool;   
    unordered_map<string, string>memString; 

    void visit(BinaryExp* exp) override;
    void visit(UnaryExp* exp) override;
    void visit(IntExp* exp) override;
    void visit(FloatExp* exp) override;
    void visit(BoolExp* exp) override;
    void visit(StringExp* exp) override;
    void visit(IdExp* exp) override;
    void visit(DeclStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(AutoDeclStmt* stm) override;
    void visit(AssignStmt* stm) override;
    void visit(BlockStmt* stm) override;
    void visit(ExpStmt* stm) override;
    void visit(Programa* program) override;
    void interprete(Programa* program);
};

#endif // VISITOR_H