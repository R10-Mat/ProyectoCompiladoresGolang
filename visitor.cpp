#include <iostream>
#include <fstream>
#include <cmath>
#include "ast.h"
#include "visitor.h"
#include "token.h"
using namespace std;

void BinaryExp::accept(Visitor* visitor) { visitor->visit(this); }
void UnaryExp::accept(Visitor* visitor)  { visitor->visit(this); }
void IntExp::accept(Visitor* visitor)    { visitor->visit(this); }
void FloatExp::accept(Visitor* visitor)  { visitor->visit(this); }
void BoolExp::accept(Visitor* visitor)   { visitor->visit(this); }
void StringExp::accept(Visitor* visitor) { visitor->visit(this); }
void IdExp::accept(Visitor* visitor)     { visitor->visit(this); }

void DeclStmt::accept(Visitor* visitor)     { visitor->visit(this); }
void PrintStmt::accept(Visitor* visitor)    { visitor->visit(this); }
void AutoDeclStmt::accept(Visitor* visitor) { visitor->visit(this); }
void AssignStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void BlockStmt::accept(Visitor* visitor)    { visitor->visit(this); }
void ExpStmt::accept(Visitor* visitor)      { visitor->visit(this); }
void Programa::accept(Visitor* visitor)     { visitor->visit(this); }

//PRINT-VISITOR
void PrintVisitor::imprimir(Programa* programa) {
    if (programa) {
        cout << "Codigo:" << endl;
        programa->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Programa* p) {
    for (auto s : p->slist)
        s->accept(this);
}

void PrintVisitor::visit(DeclStmt* stm) {
    switch (stm->type) {
        case Token::INT:    cout << "int ";    break;
        case Token::FLOAT:  cout << "float ";  break;
        case Token::DOUBLE: cout << "double "; break;
        case Token::CHAR:   cout << "char ";   break;
        case Token::BOOL:   cout << "bool ";   break;
        default:            cout << "? ";      break;
    }
    cout << stm->name;
    if (stm->init) {
        cout << " = ";
        stm->init->accept(this);
    }
    cout << ";" << endl;
}

void PrintVisitor::visit(AutoDeclStmt* stm) {
    cout << "auto " << stm->name << " = ";
    stm->init->accept(this);
    cout << ";" << endl;
}

void PrintVisitor::visit(AssignStmt* stm) {
    cout << stm->name << " ";
    switch (stm->op) {
        case Token::ASSIGN:       cout << "=";  break;
        case Token::PLUS_ASSIGN:  cout << "+="; break;
        case Token::MINUS_ASSIGN: cout << "-="; break;
        case Token::MUL_ASSIGN:   cout << "*="; break;
        case Token::DIV_ASSIGN:   cout << "/="; break;
        case Token::MOD_ASSIGN:   cout << "%="; break;
        default:                  cout << "?";  break;
    }
    cout << " ";
    stm->exp->accept(this);
    cout << ";" << endl;
}

void PrintVisitor::visit(BlockStmt* stm) {
    cout << "{" << endl;
    for (auto s : stm->stmts)
        s->accept(this);
    cout << "}" << endl;
}

void PrintVisitor::visit(ExpStmt* stm) {
    stm->exp->accept(this);
    cout << ";" << endl;
}

void PrintVisitor::visit(BinaryExp* exp) {
    cout << "(";
    exp->left->accept(this);
    cout << " " << Exp::binopToString(exp->op) << " ";
    exp->right->accept(this);
    cout << ")";
}

void PrintVisitor::visit(PrintStmt *stm) {
    cout << "print (";
    stm->exp->accept(this);
    cout << ")"<< endl;
}

void PrintVisitor::visit(UnaryExp* exp) {
    if (exp->postfix) {
        exp->exp->accept(this);
        cout << Exp::unopToString(exp->op);
    } else {
        cout << Exp::unopToString(exp->op);
        exp->exp->accept(this);
    }
}
void PrintVisitor::visit(IntExp* exp)    { cout << exp->value; }
void PrintVisitor::visit(FloatExp* exp)  { cout << exp->value; }
void PrintVisitor::visit(BoolExp* exp)   { cout << (exp->value ? "true" : "false"); }
void PrintVisitor::visit(StringExp* exp) { cout << "\"" << exp->value << "\""; }
void PrintVisitor::visit(IdExp* exp)     { cout << exp->name; }

void EVALVisitor::interprete(Programa* programa) {
    if (programa) {
        cout << "Interprete:" << endl;
        programa->accept(this);
        cout << endl;
    }
}

// EVAL-VISITOR

void EVALVisitor::visit(Programa* p) {
    for (auto s : p->slist)
        s->accept(this);
}


void EVALVisitor::visit(DeclStmt* stm) {
    if (stm->init) stm->init->accept(this);
    switch (stm->type) {
        case Token::INT:
        case Token::DOUBLE:
            memInt[stm->name]    = stm->init ? (int)memInt["__tmp"]   : 0; break;
        case Token::FLOAT:
            memFloat[stm->name]  = stm->init ? memFloat["__tmp"]      : 0; break;
        case Token::BOOL:
            memBool[stm->name]   = stm->init ? memBool["__tmp"]       : false; break;
        default: break;
    }
}

void EVALVisitor::visit(PrintStmt *stm) {
    stm->exp->accept(this); 

    if (memString.count("__tmp")){
        cout << memString["__tmp"];
    }
    else if (memFloat.count("__tmp")){
        cout << memFloat["__tmp"];
    }
    else if (memBool.count("__tmp")){
        cout << (memBool["__tmp"] ? "true" : "false");
    }
    else{
        cout << memInt["__tmp"];
    }
    cout << endl;
}


void EVALVisitor::visit(AutoDeclStmt* stm) {
    stm->init->accept(this);
    if (memInt.count("__tmp")){
        memInt[stm->name]    = memInt["__tmp"];
    }
    else if (memFloat.count("__tmp")){
        memFloat[stm->name] = memFloat["__tmp"];
    }
    else if (memBool.count("__tmp")){
        memBool[stm->name]  = memBool["__tmp"];
    }
}

void EVALVisitor::visit(AssignStmt* stm) {
    stm->exp->accept(this);
    if (memInt.count(stm->name)) {
        int val = memInt["__tmp"];
        switch (stm->op) {
            case Token::ASSIGN:       memInt[stm->name]  = val; break;
            case Token::PLUS_ASSIGN:  memInt[stm->name] += val; break;
            case Token::MINUS_ASSIGN: memInt[stm->name] -= val; break;
            case Token::MUL_ASSIGN:   memInt[stm->name] *= val; break;
            case Token::DIV_ASSIGN:   memInt[stm->name] /= val; break;
            case Token::MOD_ASSIGN:   memInt[stm->name] %= val; break;
            default: break;
        }
    }
    else if (memFloat.count(stm->name)) {
        float val = memFloat["__tmp"];
        switch (stm->op) {
            case Token::ASSIGN:       memFloat[stm->name]  = val; break;
            case Token::PLUS_ASSIGN:  memFloat[stm->name] += val; break;
            case Token::MINUS_ASSIGN: memFloat[stm->name] -= val; break;
            case Token::MUL_ASSIGN:   memFloat[stm->name] *= val; break;
            case Token::DIV_ASSIGN:   memFloat[stm->name] /= val; break;
            default: break;
        }
    }
}

void EVALVisitor::visit(BlockStmt* stm) {
    for (auto s : stm->stmts)
        s->accept(this);
}

void EVALVisitor::visit(ExpStmt* stm) {
    stm->exp->accept(this);
}

void EVALVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    int   li = memInt["__tmp"];
    float lf = memFloat["__tmp"];

    exp->right->accept(this);
    int   ri = memInt["__tmp"];
    float rf = memFloat["__tmp"];

    bool isFloat = memFloat.count("__lf") || memFloat.count("__rf");

    switch (exp->op) {
        case PLUS_OP:  memInt["__tmp"] = li + ri; memFloat["__tmp"] = lf + rf; break;
        case MINUS_OP: memInt["__tmp"] = li - ri; memFloat["__tmp"] = lf - rf; break;
        case MUL_OP:   memInt["__tmp"] = li * ri; memFloat["__tmp"] = lf * rf; break;
        case DIV_OP:
            memInt["__tmp"]   = ri != 0 ? li / ri : 0;
            memFloat["__tmp"] = rf != 0 ? lf / rf : 0;
            break;
        case MOD_OP:   memInt["__tmp"] = li % ri; break;
        case POW_OP:   memInt["__tmp"] = (int)pow(li, ri); memFloat["__tmp"] = pow(lf, rf); break;
        case EQ_OP:    memBool["__tmp"] = (li == ri); break;
        case NEQ_OP:   memBool["__tmp"] = (li != ri); break;
        case LT_OP:    memBool["__tmp"] = (li <  ri); break;
        case GT_OP:    memBool["__tmp"] = (li >  ri); break;
        case LTE_OP:   memBool["__tmp"] = (li <= ri); break;
        case GTE_OP:   memBool["__tmp"] = (li >= ri); break;
        case AND_OP:   memBool["__tmp"] = (memBool["__tmp"] && memBool["__tmp"]); break;
        case OR_OP:    memBool["__tmp"] = (memBool["__tmp"] || memBool["__tmp"]); break;
        default: break;
    }
}

void EVALVisitor::visit(UnaryExp* exp) {
    exp->exp->accept(this);

    switch (exp->op) {
        case NEG_OP:
            memInt["__tmp"]  = -memInt["__tmp"];
            memFloat["__tmp"] = -memFloat["__tmp"];
            break;
        case NOT_OP:
            memBool["__tmp"] = !memBool["__tmp"];
            break;
        case INC_OP:
            memInt["__tmp"]++;
            if (IdExp* id = dynamic_cast<IdExp*>(exp->exp)) {
                memInt[id->name] = memInt["__tmp"];
            }
            break;
        case DEC_OP:
            memInt["__tmp"]--;
            if (IdExp* id = dynamic_cast<IdExp*>(exp->exp)) {
                memInt[id->name] = memInt["__tmp"];
            }
            break;
        default: break;
    }
}
void EVALVisitor::visit(IntExp* exp)    { memInt["__tmp"]    = exp->value; }
void EVALVisitor::visit(FloatExp* exp)  { memFloat["__tmp"]  = exp->value; }
void EVALVisitor::visit(BoolExp* exp)   { memBool["__tmp"]   = exp->value; }
void EVALVisitor::visit(StringExp* exp) { memString["__tmp"] = exp->value; }

void EVALVisitor::visit(IdExp* exp) {
    if (memInt.count(exp->name))    memInt["__tmp"]    = memInt[exp->name];
    else if (memFloat.count(exp->name))  memFloat["__tmp"]  = memFloat[exp->name];
    else if (memBool.count(exp->name))   memBool["__tmp"]   = memBool[exp->name];
    else if (memString.count(exp->name)) memString["__tmp"] = memString[exp->name];
    else
        throw runtime_error("Variable no declarada: " + exp->name);
}