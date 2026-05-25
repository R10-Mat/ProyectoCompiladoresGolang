#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;
/*
Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Programa* Parser::parseProgram() {
    Programa* prog = new Programa();
    while (!isAtEnd()) {
        prog->slist.push_back(parseStmt());
    }
    return prog;
}

Stmt* Parser::parseStmt() {
    if (check(Token::INT)||check(Token::FLOAT)||check(Token::DOUBLE)||check(Token::CHAR)||check(Token::BOOL)) {
        advance();
        return parseDeclStmt();
    }
    else if (match(Token::AUTO)) {
        return parseAutoDeclStmt();
    }
    else if (check(Token::ID)) {
        advance();
        return parseAssignOrExpStmt();
    }
    else if (match(Token::LBRACE)) {
        return parseBlockStmt();
    }
    else if (match(Token::PRINT)){
        match(Token::LPAREN);
        Exp* e = parseCEXP();
        match(Token::RPAREN);
        match(Token::SEMICOL);
        return new PrintStmt(e);
    }
    else if (check(Token::INC) || check(Token::DEC)) {
        advance();
        UnaryOp uop = (previous->type == Token::INC) ? INC_OP : DEC_OP;
        
        if (!match(Token::ID)) {
            throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba variable");
        }
        string name = previous->text;
        
        if (!match(Token::SEMICOL)) {
            throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba ';'");
        }
        return new ExpStmt(new UnaryExp(new IdExp(name), uop, false));
    }
    else {
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": sentencia inválida '" + current->text + "'");
    }
}


Stmt* Parser::parseDeclStmt() {
    Token::Type type = previous->type;

    if (!match(Token::ID)){
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba nombre de variable");
    }
    string name = previous->text;
    Exp* init = nullptr;

    if (match(Token::ASSIGN)){
        init = parseCEXP();
    }
    if (!match(Token::SEMICOL)){
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba ';'");
    }
    return new DeclStmt(name,type, init);
}

Stmt* Parser::parseAutoDeclStmt() {
    if (!match(Token::ID)){
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba nombre de variable");
    }
    string name = previous->text;

    if (!match(Token::ASSIGN)){
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": 'auto' requiere inicialización con '='");
    }
    Exp* init = parseCEXP();

    if (!match(Token::SEMICOL)){
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba ';'");
    }
    return new AutoDeclStmt(name, init);
}

Stmt* Parser::parseAssignOrExpStmt() {
    string name = previous->text;

    if (check(Token::ASSIGN)||check(Token::PLUS_ASSIGN)||check(Token::MINUS_ASSIGN)||check(Token::MUL_ASSIGN)||check(Token::DIV_ASSIGN)||check(Token::MOD_ASSIGN)){
        advance();
        Token::Type op = previous->type;
        Exp* exp = parseCEXP();

        if (!match(Token::SEMICOL)){
            throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba ';'");
        }
        return new AssignStmt(name, op, exp);
    }
    else if (check(Token::INC) || check(Token::DEC)) {
        advance();
        UnaryOp uop = (previous->type == Token::INC) ? INC_OP : DEC_OP;

        if (!match(Token::SEMICOL)){
            throw runtime_error("...");
        }
        return new ExpStmt(new UnaryExp(new IdExp(name), uop, true));
    }
    else {
        throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba '=' o operador de asignación");
    }
}

Stmt* Parser::parseBlockStmt() {
    list<Stmt*> stmts;

    while (!check(Token::RBRACE)) {
        if (isAtEnd())
            throw runtime_error("Error sintáctico: bloque sin cerrar, se esperaba '}'");
        stmts.push_back(parseStmt());
    }
    advance(); 

    return new BlockStmt(stmts);
}

Exp* Parser::parseCEXP() {
    Exp* left = parseE();

    while (check(Token::AND) || check(Token::OR)) {
        advance();
        BinaryOp op = (previous->type == Token::AND) ? AND_OP : OR_OP;
        Exp* right = parseE();
        left = new BinaryExp(left, right, op);
    }

    return left;
}

Exp* Parser::parseE() {
    Exp* left = parseT();

    while (check(Token::EQ)  || check(Token::NEQ) ||
           check(Token::LT)  || check(Token::GT)  ||
           check(Token::LTE) || check(Token::GTE)) {
        advance();
        BinaryOp op;
        switch (previous->type) {
            case Token::EQ:  op = EQ_OP;  break;
            case Token::NEQ: op = NEQ_OP; break;
            case Token::LT:  op = LT_OP;  break;
            case Token::GT:  op = GT_OP;  break;
            case Token::LTE: op = LTE_OP; break;
            case Token::GTE: op = GTE_OP; break;
            default:         op = EQ_OP;  break;
        }
        Exp* right = parseT();
        left = new BinaryExp(left, right, op);
    }

    return left;
}

Exp* Parser::parseT() {
    Exp* left = parseF();

    while (check(Token::PLUS) || check(Token::MINUS)) {
        advance();
        BinaryOp op = (previous->type == Token::PLUS) ? PLUS_OP : MINUS_OP;
        Exp* right = parseF();
        left = new BinaryExp(left, right, op);
    }

    return left;
}

Exp* Parser::parseF() {
    Exp* left = parseU();

    while (check(Token::MUL) || check(Token::DIV) || check(Token::MOD)) {
        advance();
        BinaryOp op;
        switch (previous->type) {
            case Token::MUL: op = MUL_OP; break;
            case Token::DIV: op = DIV_OP; break;
            case Token::MOD: op = MOD_OP; break;
            default:         op = MUL_OP; break;
        }
        Exp* right = parseU();
        left = new BinaryExp(left, right, op);
    }

    return left;
}

Exp* Parser::parseU() {
    if (check(Token::MINUS)) {
        advance();
        Exp* exp = parseU();
        return new UnaryExp(exp, NEG_OP);
    }
    if (check(Token::NOT)) {
        advance();
        Exp* exp = parseU();
        return new UnaryExp(exp, NOT_OP);
    }
    if (check(Token::INC)) {
        advance();
        Exp* exp = parseU();
        return new UnaryExp(exp, INC_OP);
    }
    if (check(Token::DEC)) {
        advance();
        Exp* exp = parseU();
        return new UnaryExp(exp, DEC_OP);
    }

    return parsePrimary();
}

Exp* Parser::parsePrimary() {

    if (match(Token::NUM)) {
        string text = previous->text;
        if (text.find('.') != string::npos){
            return new FloatExp(stof(text));
        }
        else{
            return new IntExp(stoi(text));
        }
    }

    if (match(Token::BOOL_LIT)) {
        return new BoolExp(previous->text == "true");
    }

    if (match(Token::STRING_LIT)) {
        string text = previous->text;
        return new StringExp(text.substr(1, text.size() - 2));
    }

    if (match(Token::ID)) {
        return new IdExp(previous->text);
    }

    if (match(Token::LPAREN)) {
        Exp* exp = parseCEXP();
        if (!match(Token::RPAREN))
            throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": se esperaba ')'");
        return exp;
    }

    throw runtime_error("Error sintáctico línea " + to_string(current->line) + ": expresión inválida '" + current->text + "'");
}
*/