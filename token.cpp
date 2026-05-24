#include <iostream>
#include "token.h"

using namespace std;


Token::Token(Type type, int line) : type(type), text(""), line(line) { }

Token::Token(Type type, char c, int line) : type(type), text(string(1, c)), line(line) { }

Token::Token(Type type, const string& source, int first, int last, int line) : type(type), text(source.substr(first, last - first)), line(line) { }


static const char* typeName(Token::Type t) {
    switch (t) {

        case Token::NUM:          return "NUM";
        case Token::STRING_LIT:   return "STRING_LIT";
        case Token::BOOL_LIT:     return "BOOL_LIT";
        case Token::ID:           return "ID";

        case Token::INT:          return "INT";
        case Token::FLOAT:        return "FLOAT";
        case Token::DOUBLE:       return "DOUBLE";
        case Token::CHAR:         return "CHAR";
        case Token::BOOL:         return "BOOL";
        case Token::AUTO:         return "AUTO";
        case Token::VOID:         return "VOID";

        case Token::PLUS:         return "PLUS";
        case Token::MINUS:        return "MINUS";
        case Token::MUL:          return "MUL";
        case Token::DIV:          return "DIV";
        case Token::MOD:          return "MOD";
        case Token::POW:          return "POW";

        case Token::ASSIGN:       return "ASSIGN";
        case Token::PLUS_ASSIGN:  return "PLUS_ASSIGN";
        case Token::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case Token::MUL_ASSIGN:   return "MUL_ASSIGN";
        case Token::DIV_ASSIGN:   return "DIV_ASSIGN";
        case Token::MOD_ASSIGN:   return "MOD_ASSIGN";

        case Token::EQ:           return "EQ";
        case Token::NEQ:          return "NEQ";
        case Token::LT:           return "LT";
        case Token::GT:           return "GT";
        case Token::LTE:          return "LTE";
        case Token::GTE:          return "GTE";

        case Token::AND:          return "AND";
        case Token::OR:           return "OR";
        case Token::NOT:          return "NOT";

        case Token::INC:          return "INC";
        case Token::DEC:          return "DEC";
        
        case Token::LPAREN:       return "LPAREN";
        case Token::RPAREN:       return "RPAREN";
        case Token::LBRACE:       return "LBRACE";
        case Token::RBRACE:       return "RBRACE";
        case Token::LBRACKET:     return "LBRACKET";
        case Token::RBRACKET:     return "RBRACKET";
        case Token::SEMICOL:      return "SEMICOL";
        case Token::COMMA:        return "COMMA";
        case Token::DOT:          return "DOT";
        case Token::COLON:        return "COLON";
        case Token::IF:           return "IF";
        case Token::ELSE:         return "ELSE";
        case Token::WHILE:        return "WHILE";
        case Token::FOR:          return "FOR";
        case Token::RETURN:       return "RETURN";
        case Token::PRINT:        return "PRINT";
        // Especiales
        case Token::ERR:          return "ERR";
        case Token::END:          return "END";
        default:                  return "UNKNOWN";
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