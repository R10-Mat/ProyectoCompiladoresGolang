#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    enum Type {
        NUM,         // numero
        STRING_LIT,  // string
        BOOL_LIT,    // bool

        ID,          // id

        INT,         // int
        FLOAT,       // float
        DOUBLE,      // double
        CHAR,        // char
        BOOL,        // bool
        AUTO,        // auto
        VOID,        // void 

        PLUS,        // +
        MINUS,       // -
        MUL,         // *
        DIV,         // /
        MOD,         // %
        POW,         // ** 

        
        ASSIGN,      // =
        PLUS_ASSIGN, // +=
        MINUS_ASSIGN,// -=
        MUL_ASSIGN,  // *=
        DIV_ASSIGN,  // /=
        MOD_ASSIGN,  // %=

        
        EQ,          // ==
        NEQ,         // !=
        LT,          // <
        GT,          // >
        LTE,         // <=
        GTE,         // >=

        AND,         // &&
        OR,          // ||
        NOT,         // !

        INC,         // ++
        DEC,         // --

        LPAREN,      // (
        RPAREN,      // )
        LBRACE,      // {
        RBRACE,      // }
        LBRACKET,    // [
        RBRACKET,    // ]
        SEMICOL,     // ;
        COMMA,       // ,
        DOT,         // .
        COLON,       // :

        IF,          // if
        ELSE,        // else
        WHILE,       // while
        FOR,         // for
        RETURN,      // return
        PRINT,       // print

        ERR,         // token inválido
        END          // fin de entrada
    };

    Type type;
    string text;
    int line;   

    Token(Type type, int line = 0);
    Token(Type type, char c, int line = 0);
    Token(Type type, const string& source, int first, int last, int line = 0);

    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H