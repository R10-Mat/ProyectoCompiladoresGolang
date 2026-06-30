#include <iostream>
#include "token.h"

using namespace std;


Token::Token(Type type, int line) : type(type), text(""), line(line) { }

Token::Token(Type type, char c, int line) : type(type), text(string(1, c)), line(line) { }

Token::Token(Type type, const string& source, int first, int last, int line) : type(type), text(source.substr(first, last - first)), line(line) { }


static const char* typeName(Token::Type t) {
    switch (t) {
        // identifiers
        case Token::ID:                 return "ID";
        // keywords
        case Token::BREAK:              return "BREAK";
        case Token::CASE:               return "CASE";
        case Token::CHAN:               return "CHAN";
        case Token::CONST:              return "CONST";
        case Token::CONTINUE:           return "CONTINUE";
        case Token::DEFAULT:            return "DEFAULT";
        case Token::DEFER:              return "DEFER";
        case Token::ELSE:               return "ELSE";
        case Token::FALLTHROUGH:        return "FALLTHROUGH";
        case Token::FOR:                return "FOR";
        case Token::FUNC:               return "FUNC";
        case Token::GO:                 return "GO";
        case Token::GOTO:               return "GOTO";
        case Token::IF:                 return "IF";
        case Token::IMPORT:             return "IMPORT";
        case Token::INTERFACE:          return "INTERFACE";
        case Token::MAP:                return "MAP";
        case Token::PACKAGE:            return "PACKAGE";
        case Token::RANGE:              return "RANGE";
        case Token::RETURN:             return "RETURN";
        case Token::SELECT:             return "SELECT";
        case Token::STRUCT:             return "STRUCT";
        case Token::SWITCH:             return "SWITCH";
        case Token::TYPE:               return "TYPE";
        case Token::VAR:                return "VAR";

        // Operators and Punctuation
        case Token::PLUS:               return "PLUS";           // +
        case Token::NEG:                return "NEG";            // -
        case Token::MUL:                return "MUL";            // *
        case Token::DIV:                return "DIV";            // /
        case Token::MOD:                return "MOD";            // %
        case Token::AND:                return "AND";            // &
        case Token::OR:                 return "OR";             // |
        case Token::CARET:              return "CARET";          // ^
        case Token::LSHIFT:             return "LSHIFT";         // <<
        case Token::RSHIFT:             return "RSHIFT";         // >>
        case Token::BITCLEAR:           return "BITCLEAR";       // &^
        case Token::PLUSASSIGN:         return "PLUSASSIGN";     // +=
        case Token::NEGASSIGN:          return "NEGASSIGN";      // -=
        case Token::MULASSIGN:          return "MULASSIGN";      // *=
        case Token::DIVASSIGN:          return "DIVASSIGN";      // /=
        case Token::MODASSIGN:          return "MODASSIGN";      // %=
        case Token::ANDASSIGN:          return "ANDASSIGN";      // &=
        case Token::ORASSIGN:           return "ORASSIGN";       // |=
        case Token::CARETASSIGN:        return "CARETASSIGN";    // ^=
        case Token::LSHIFTASSIGN:       return "LSHIFTASSIGN";   // <<=
        case Token::RSHIFTASSIGN:       return "RSHIFTASSIGN";   // >>=
        case Token::BITCLEARASSIGN:     return "BITCLEARASSIGN"; // &^=
        case Token::AND_LOGICAL:        return "AND_LOGICAL";    // &&
        case Token::OR_LOGICAL:         return "OR_LOGICAL";     // ||
        case Token::ARROW:              return "ARROW";          // <-
        case Token::INC:                return "INC";            // ++
        case Token::DEC:                return "DEC";            // --
        case Token::EQUAL:              return "EQUAL";          // ==
        case Token::LES:                return "LES";            // <
        case Token::GER:                return "GER";            // >
        case Token::ASSIGN:             return "ASSIGN";         // =
        case Token::NOT:                return "NOT";            // !
        case Token::VIRGULA:            return "VIRGULA";        // ~
        case Token::DISTINCT:           return "DISTINCT";       // !=
        case Token::LEQ:                return "LEQ";            // <=
        case Token::GEQ:                return "GEQ";            // >=
        case Token::DEC_ASSIGN:         return "DEC_ASSIGN";     // :=
        case Token::LPAREN:             return "LPAREN";         // (
        case Token::RPAREN:             return "RPAREN";         // )
        case Token::LCORCHETE:          return "LCORCHETE";      // [
        case Token::RCORCHETE:          return "RCORCHETE";      // ]
        case Token::LLLAVE:             return "LLLAVE";         // {
        case Token::RLLAVE:             return "RLLAVE";         // }
        case Token::COMMA:              return "COMMA";          // ,
        case Token::PCOMMA:             return "PCOMMA";         // ;
        case Token::PUNTO:              return "PUNTO";          // .
        case Token::DOS_PUNTOS:         return "DOS_PUNTOS";     // :
        case Token::TRES_PUNTOS:        return "TRES_PUNTOS";    // ...

        // Literals
        case Token::INT_LIT:            return "INT_LIT";
        case Token::FLOAT_LIT:          return "FLOAT_LIT";
        case Token::IMAGINARY_LIT:      return "IMAGINARY_LIT";
        case Token::RUNE_LIT:           return "RUNE_LIT";
        case Token::STRING_LIT:         return "STRING_LIT";


        // System / Errors
        case Token::ERROR:              return "ERROR";
        case Token::END:                return "END";
        default:                        return "ERROR";
    }
}


ostream& operator<<(ostream& outs, const Token& tok) {
    if (tok.type == Token::END) {
        outs << "TOKEN(END)";
    } else if (tok.text.empty()) {
        outs << "TOKEN(" << typeName(tok.type) << ")";
    } else {
        outs << "TOKEN(" << typeName(tok.type) << ", \"" << tok.text << "\")";
    }
    if (tok.line > 0){
        outs << " [line " << tok.line << "]";
    }
    return outs;
}

ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;
}