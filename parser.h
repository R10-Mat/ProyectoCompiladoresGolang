#ifndef PARSER_H       
#define PARSER_H

#include "scanner.h"    // Incluye la definición del escáner (provee tokens al parser)
#include "ast.h"        // Incluye las definiciones para construir el Árbol de Sintaxis Abstracta (AST)

class Parser {
private:
    Scanner* scanner;                       // Puntero al escáner, de donde se leen los tokens
    Token *current, *previous;              // Punteros al token actual y al anterior
    bool match(Token::Type ttype); // Verifica si el token actual coincide con un tipo esperado y avanza si es así
    bool check(Token::Type ttype);          // Comprueba si el token actual es de cierto tipo, sin avanzar
    bool advance();                         // Avanza al siguiente token
    bool isAtEnd(); 
    void expect(Token::Type ttype);
    void error(const std::string &expected);
public:
    Parser(Scanner* scanner);           

    // ----------------------------------------------------------------------
    // Parte Rayhan: global y types
    // ----------------------------------------------------------------------
    Programa* parseProgram();
    TopLevelDecl* parseTopLevelDecl();
    Declaration* parseDeclaration();
    FunctionDecl* parseFunctionDecl();
    MethodDecl* parseMethodDecl();
    ConstDecl* parseConstDecl();
    ConstSpec* parseConstSpec();
    TypeDecl* parseTypeDecl();
    VarDecl* parseVarDecl();
    IdentifierList* parseIdentifierList();
    Type* parseType();
    TypeSpec* parseTypeSpec();
    VarSpec* parseVarSpec();
    ExpList* parseExpList();
    ParameterList* parseParameterList();
    ParameterDecl* parseParameterDecl();
    FieldDecl* parseFieldDecl();
    // ----------------------------------------------------------------------
    // Parte Bruno: Blocks y Statements
    // ----------------------------------------------------------------------
    Block* parseBlock();
    StmtList* parseStmtList();
    Stmt* parseStmt();
    DeclarationStmt* parseDeclarationStmt();
    BlockStmt* parseBlockStmt();
    ExpresionStmt* parseExpresionStmt(Exp* e);
    IncDecStmt* parseIncDecStmt(Exp* e);
    Assigment* parseAssigment(ExpList* el);
    ReturnStmt* parseReturnStmt();
    BreakStmt* parseBreakStmt();
    ContinueStmt* parseContinueStmt();
    IfStmt* parseIfStmt();
    SwitchStmt* parseSwitchStmt();
        ExpCaseClause* parseExpCaseClause();
    ForStmt* parseForStmt();
        ForClause* parseForClause(Exp* primer_exp);
    // ----------------------------------------------------------------------
    // Parte Nico: Expresions
    // ----------------------------------------------------------------------
    Exp* parseExp();
    Exp* parseAndExpr();
    Exp* relationParseExp();
    Exp* additiveParseExp();
    Exp* multiplicateParseExp();
    Exp* primaryParseExp();
    Exp* parseUnaryExpr();
    Exp* operandParseExp();
    vector<KeyedElement*> parseLiteralValue();
    Exp* compositeLiteralParseExp();
    FunctionLit* functionLiteralParseExp();
    Exp* basicLiteralParseExp();
};

#endif // PARSER_H
