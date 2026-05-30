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


    //Exp* parseQualifiedIdent(); // opcionales?
    //Exp* parsePackageName(); // opcionales?

    //Block* parseBlock();
    //StmtList* parseStmtList();
    //Stmt* parseStmt();

    //Decl* parseDecl();
    //Decl* parseTopLevelDecl();
    //ConstDecl* parseConstDecl();
    //ConstSpec* parseConstSpec();

    //list<IdExp*> parseIndetifierList();
    //list<Exp*> parseExpresionList();
public:
    Parser(Scanner* scanner);           
    Programa* parseProgram();           
};
#endif // PARSER_H