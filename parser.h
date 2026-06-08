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
    bool isAtEnd();                         // Comprueba si ya se llegó al final de la entrada


    // ----------------------------------------------------------------------
    // Parte Rayhan: global y types
    // ----------------------------------------------------------------------
    Type* parseType();
        TypeName_TypeArgs *parseTypeName_TypeArgs();
            TypeName* parseTypeName();
            TypeArgs* parseTypeArgs();
            TypeList* parseTypeList();

        Type* parseTypeLiteral();
            TypeLiteral* parseArrayorSliceType();
                ArrayType* parseArrayType();
                SliceType* parseSliceType();
            StructType* parseStructType();
                FieldDecl* parseFieldDecl();
            PointerType* parsePointerType();
            FunctionType* parseFunctionType();
                Signature* parseSignature();
                    ParameterDecl* parseParameterDecl();
            ChannelType* parseChannelType();
            MapType* parseMapType();

    ExpList* parseExpList();
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
            ForClause* parseForClause();
    // ----------------------------------------------------------------------
    // Parte Nico: Expresions
    // ----------------------------------------------------------------------
    Exp* parseExp();


public:
    Parser(Scanner* scanner);           
    Programa* parseProgram();           
};
#endif // PARSER_H