#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERROR) {
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

        if (check(Token::ERROR)) {
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


// ----------------------------------------------------------------------
// Parte Rayhan: global y types
// ----------------------------------------------------------------------
Programa* Parser::parseProgram() {
    Programa* program = new Programa();
    program->listatopleveldecl.push_back(parseTopLevelDecl());
    while (match(Token::PCOMMA)){
        program->listatopleveldecl.push_back(parseTopLevelDecl());
    }
    if (!isAtEnd()){
        throw runtime_error("Error sintáctico");
    }
    cout << "Parseo exitoso" << endl;
    return program;
}
TopLevelDecl* Parser::parseTopLevelDecl(){
    TopLevelDecl* d = new TopLevelDecl();
    if (check(Token::CONST)){
        d = parseDeclaration();
    }else if (match(Token::FUNC)){
        if (match(Token::LPAREN)){
            d = parseMethodDecl();
        }else{
            d = parseFunctionDecl();
        }
    }
    return d;
}

Declaration* Parser::parseDeclaration(){
    Declaration* d = new Declaration();
    if (match(Token::CONST)){
        d = parseConstDecl();
    }else if (match(Token::TYPE)){
        d = parseTypeDecl();
    }else if (match(Token::VAR)){
        d = parseVarDecl();
    }
    return d;
}

ConstDecl* Parser::parseConstDecl(){
    ConstDecl* decl = new ConstDecl();
    if (match(Token::LPAREN)){
        decl->constspecList.push_back(parseConstSpec());
        while(Token::PCOMMA){
            decl->constspecList.push_back(parseConstSpec());
        }
        match(Token::RPAREN);
        return decl;
    }else{
        decl->constspecList.push_back(parseConstSpec());
    }
    return decl;
}
ConstSpec* Parser::parseConstSpec(){
    ConstSpec* constspec = new ConstSpec();
    constspec->identifierList = parseIdentifierList();
    constspec->tipo = parseType();
    constspec->expresionlist = parseExpList();
    return constspec;
}

TypeDecl* Parser::parseTypeDecl(){
    TypeDecl* typedecl = new TypeDecl();
    if (match(Token::LPAREN)){
        typedecl->typespecList.push_back(parseTypeSpec());
        while(Token::PCOMMA){
            typedecl->typespecList.push_back(parseTypeSpec());
        }
        match(Token::RPAREN);
        return typedecl;
    }else{
        typedecl->typespecList.push_back(parseTypeSpec());
    }
    return typedecl;
    
}
TypeSpec* Parser::parseTypeSpec(){
    TypeSpec* typespec = new TypeSpec();
    if (match(Token::ID)){
        typespec->id = previous->text;
        typespec->tipo = parseType();// manejar que no haya type
    }else{
        throw runtime_error("Error sintáctico");
    }
    return typespec;
}
VarDecl* Parser::parseVarDecl(){
    VarDecl* varDecl = new VarDecl();
    if (match(Token::LPAREN)){
        varDecl->varspecList.push_back(parseVarSpec());
        while(match(Token::PCOMMA)){
            varDecl->varspecList.push_back(parseVarSpec());
        }
        match(Token::RPAREN);
    }else {
        varDecl->varspecList.push_back(parseVarSpec());
    }
    return varDecl;
}
// Que hago si el tipo tiene que necesariamente poner eso y si no pone debe arrojar error

VarSpec* Parser::parseVarSpec(){
    VarSpec* varSpec = new VarSpec();
    varSpec->identifierlist = parseIdentifierList();
    varSpec->tipo = parseType();
    varSpec->expresionlist = parseExpList();
    return varSpec;
}
MethodDecl* Parser::parseMethodDecl(){
    MethodDecl* methodDecl = new MethodDecl();
    if (match(Token::ID)){
        methodDecl->nombreId = previous->text;
        if (match(Token::MUL)){
            methodDecl->puntero = true;
        }else {
            methodDecl->puntero = false;
        }
        if (match(Token::ID)){
            methodDecl->NombreTipoBase = previous->text;
            if (match(Token::RPAREN)){
                if (match(Token::ID)){
                    methodDecl->nombreMethod = previous->text;
                    if (match(Token::LPAREN)){
                        methodDecl->lista_de_parametros = parseParameterList();
                        if (match(Token::RPAREN)){
                            methodDecl->tipo = parseType();
                            methodDecl->cuerpo = parseBlock();// Es necesario
                        }
                    }else {
                        throw runtime_error("Error sintáctico");
                    }
                }else {
                    throw runtime_error("Error sintáctico");
                }
            }else {
                throw runtime_error("Error sintáctico");
            }
        }else {
            throw runtime_error("Error sintáctico");
        }
    }else{
        throw runtime_error("Error sintáctico");
    }
    return methodDecl;
}
FunctionDecl* Parser::parseFunctionDecl(){
}
Type* Parser::parseType(){
}
IdentifierList* Parser::parseIdentifierList(){

}
ParameterList* Parser::parseParameterList(){

}

// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
Block *Parser::parseBlock() {
    if (check(Token::LLLAVE)) {
        match(Token::LLLAVE);
        auto bloque = new Block();
        bloque->lista_statements = parseStmtList();
        if (check(Token::RLLAVE)) {
            match(Token::RLLAVE);
            return bloque;
        } throw runtime_error("Un bloque debe terminar con '}'");
    }
    throw runtime_error("Un bloque debe empezar con '{' ");
}

StmtList *Parser::parseStmtList() {
    auto lista_statements = new StmtList();
    lista_statements->statements.push_back(parseStmt());
    while (match(Token::PCOMMA)) {
        lista_statements->statements.push_back(parseStmt());
    }
    return lista_statements;
}

Stmt *Parser::parseStmt() {
    if (check(Token::VAR) || check(Token::CONST) || check(Token::TYPE)) {
        return parseDeclarationStmt();
    }
    if (check(Token::LLLAVE)) {
        return parseBlockStmt();
    }
    if (check(Token::RETURN)) {
        return parseReturnStmt();
    }
    if (check(Token::BREAK)) {
        return parseBreakStmt();
    }
    if (check(Token::CONTINUE)) {
        return parseReturnStmt();
    }
    if (check(Token::IF)) {
        return parseIfStmt();
    }
    if (check(Token::FOR)) {
        return parseForStmt();
    }
    if (check(Token::SWITCH)) {
        return parseSwitchStmt();
    }

    auto expresion_list = parseExpList();
    if (check(Token::INC) || check(Token::DEC)) {
        return parseIncDecStmt(expresion_list->expressions[0]);
    }

    if (check(Token::PLUSASSIGN) || check(Token::NEGASSIGN) ||
        check(Token::MULASSIGN)  || check(Token::DIVASSIGN)) {
        return parseAssigment(expresion_list);
    }

    return parseExpresionStmt(expresion_list->expressions[0]);

}

DeclarationStmt *Parser::parseDeclarationStmt() {
    auto declaracion = new DeclarationStmt();
    declaracion->declaration = parseDeclaration();
    return declaracion;
}

BlockStmt *Parser::parseBlockStmt() {
    auto bloque = new BlockStmt();
    bloque->block = parseBlock();
    return bloque;
}


ExpresionStmt *Parser::parseExpresionStmt(Exp* e) {
    auto expresion_stmt = new ExpresionStmt();
    expresion_stmt->expresion = e;
    return expresion_stmt;
}

IncDecStmt *Parser::parseIncDecStmt(Exp* e) {
    auto inc_dec_stmt = new IncDecStmt();
    inc_dec_stmt->expresion = e;
    UnaryOp ops;
    if (check(Token::INC)) {
        match(Token::INC);
        ops = INC_OP;
    } else {
        match(Token::DEC);
        ops = DEC_OP;
    }
    inc_dec_stmt->op = ops;
    return inc_dec_stmt;
}


Assigment *Parser::parseAssigment(ExpList* el) {
    auto assignment = new Assigment();
    assignment->expresion_list_id = el;
    AssignOp ops;
    if (check(Token::PLUSASSIGN)) {
        match(Token::PLUSASSIGN);
        ops = PLUS_ASSIGN;
    } else if (check(Token::NEGASSIGN)) {
        match(Token::NEGASSIGN);
        ops = NEG_ASSIGN;
    } else if (check(Token::MULASSIGN)) {
        match(Token::MULASSIGN);
        ops = MUL_ASSIGN;
    } else {
        match(Token::DIVASSIGN);
        ops = DIV_ASSIGN;
    }
    assignment->op = ops;
    assignment->expresion_list_values = parseExpList();
    return assignment;
}

ReturnStmt *Parser::parseReturnStmt() {
    match(Token::RETURN);
    auto return_stmt = new ReturnStmt();
    if (!check(Token::PCOMMA) && !check(Token::RLLAVE)) {
        return_stmt->expresion_list = parseExpList();
    } else {
        return_stmt->expresion_list = nullptr;
    }
    return return_stmt;
}

BreakStmt *Parser::parseBreakStmt() {
    match(Token::BREAK);
    return new BreakStmt();
}

ContinueStmt *Parser::parseContinueStmt() {
    match(Token::CONTINUE);
    return new ContinueStmt();
}

IfStmt *Parser::parseIfStmt() {
    match(Token::IF);
    auto if_stmt = new IfStmt();
    if_stmt->expresion = parseExp();
    if_stmt->cuerpo_if = parseBlock();
    if (check(Token::ELSE)) {
        match(Token::ELSE);
        if (check(Token::LLLAVE)) {
            if_stmt->cuerpo_else = parseBlock();
            if_stmt->cuerpo_if = nullptr;
        } else {
            if_stmt->cuerpo_else = nullptr;
            if_stmt->if_anidado = parseIfStmt();
        }
    } else {
        if_stmt->cuerpo_else = nullptr;
        if_stmt->if_anidado  = nullptr;
    }
    return if_stmt;
}

SwitchStmt *Parser::parseSwitchStmt() {
    match(Token::SWITCH);
    auto switch_stmt =  new SwitchStmt();
    if (!check(Token::LLLAVE)) {
        switch_stmt->expresion = parseExp();
    } else {
        switch_stmt->expresion = nullptr;
    }
    match(Token::LLLAVE);
    while (!check(Token::RLLAVE)) {
        switch_stmt->exp_case_clause.push_back(parseExpCaseClause());
    }
    match(Token::RLLAVE);
    return switch_stmt;
}

ExpCaseClause *Parser::parseExpCaseClause() {
    auto exp_case_clause = new ExpCaseClause();
    if (check(Token::CASE)) {
        match(Token::CASE);
        exp_case_clause->expresion_list = parseExpList();
        match(Token::DOS_PUNTOS);
        exp_case_clause->statement_list = parseStmtList();
    } else {
        match(Token::DEFAULT);
        exp_case_clause->expresion_list = nullptr;
        exp_case_clause->statement_list = parseStmtList();
    }
    return exp_case_clause;
}

ForStmt *Parser::parseForStmt() {
    match(Token::FOR);
    auto for_stmt = new ForStmt();
    if (check(Token::LLLAVE)) {
        for_stmt->block = parseBlock();
        return for_stmt;
    }

    // falta el or ese

    for_stmt->block = parseBlock();
    return for_stmt;
}

ForClause *Parser::parseForClause() {
    // falta coso
}

// ----------------------------------------------------------------------
// Parte Nico: Expresion
// ----------------------------------------------------------------------

Exp *Parser::parseExp() {

}
