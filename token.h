#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // hay 4 clases de tokens
    enum Type {
        // identifiers
        ID,         // id
        // keywords
        BREAK,
        CASE,
        CHAN,
        CONST,
        CONTINUE,
        DEFAULT,
        DEFER,
        ELSE,
        FALLTHROUGH,
        FOR,
        FUNC,
        GO,
        GOTO,
        IF,
        IMPORT,
        INTERFACE,
        MAP,
        PACKAGE,
        RANGE,
        RETURN,
        SELECT,
        STRUCT,
        SWITCH,
        TYPE,
        VAR,
        // operators and punctuation
        PLUS,           // +
        NEG,            // -
        MUL,            // *
        DIV,            // /
        MOD,            // %
        AND,            // &
        OR,             // |
        CARET,          // ^
        LSHIFT,         // <<
        RSHIFT,         // >>
        BITCLEAR,       // &^
        PLUSASSIGN,     // +=
        NEGASSIGN,      // -=
        MULASSIGN,      // *=
        DIVASSIGN,      // /=
        MODASSIGN,      // %=
        ANDASSIGN,      // &=
        ORASSIGN,       // |=
        CARETASSIGN,    // ^=
        LSHIFTASSIGN,   // <<=
        RSHIFTASSIGN,   // >>=
        BITCLEARASSIGN, // &^=
        AND_LOGICAL,    // &&
        OR_LOGICAL,     // ||
        ARROW,           // <-
        INC,            // ++
        DEC,            // --
        EQUAL,          // ==
        LES,            // <
        GER,            // >
        ASSIGN,         // =
        NOT,            // !
        VIRGULA,        // ~
        DISTINCT,       // !=
        LEQ,            // <=
        GEQ,            // >=
        DEC_ASSIGN,     // :=
        LPAREN,         // (
        RPAREN,         // )
        LCORCHETE,      // [
        RCORCHETE,      // ]
        LLLAVE,         // {
        RLLAVE,         // }
        COMMA,          // ,
        PCOMMA,         // ;
        PUNTO,          // .
        DOS_PUNTOS,     // :
        TRES_PUNTOS,    // ...
        // literals
        INT_LIT,
        FLOAT_LIT,
        IMAGINARY_LIT,
        RUNE_LIT,
        STRING_LIT,

        ERROR,
        END
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