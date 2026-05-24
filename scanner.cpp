#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
Scanner::Scanner(const char* s): input(s), first(0), current(0), line(1) { }

// -----------------------------
// Función auxiliar
// -----------------------------
bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// -----------------------------
// nextToken: obtiene el siguiente token
// -----------------------------
Token* Scanner::nextToken() {
    Token* token;

    while (current < input.length() && is_white_space(input[current])) {
        if (input[current] == '\n') line++;
        current++;
    }

    if (current >= input.length()){
        return new Token(Token::END, line);
    }
    char c = input[current];
    first = current;

    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        if (current < input.length() && input[current] == '.') {
            current++;
            while (current < input.length() && isdigit(input[current]))
                current++;
        }
        token = new Token(Token::NUM, input, first, current, line);
    }
    else if (c == '"') {
        current++;
        while (current < input.length() && input[current] != '"') {
            if (input[current] == '\n') line++;
            current++;
        }
        if (current < input.length()) current++;
        token = new Token(Token::STRING_LIT, input, first, current, line);
    }
    else if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_'))
            current++;
        string lexema = input.substr(first, current - first);

        if      (lexema == "int")    token = new Token(Token::INT,input, first, current, line);
        else if (lexema == "float")  token = new Token(Token::FLOAT,input, first, current, line);
        else if (lexema == "double") token = new Token(Token::DOUBLE,input, first, current, line);
        else if (lexema == "char")   token = new Token(Token::CHAR,input, first, current, line);
        else if (lexema == "bool")   token = new Token(Token::BOOL,input, first, current, line);
        else if (lexema == "auto")   token = new Token(Token::AUTO,input, first, current, line);
        else if (lexema == "void")   token = new Token(Token::VOID,input, first, current, line);
        else if (lexema == "if")     token = new Token(Token::IF,input, first, current, line);
        else if (lexema == "else")   token = new Token(Token::ELSE,input, first, current, line);
        else if (lexema == "while")  token = new Token(Token::WHILE,input, first, current, line);
        else if (lexema == "for")    token = new Token(Token::FOR,input, first, current, line);
        else if (lexema == "return") token = new Token(Token::RETURN,input, first, current, line);
        else if (lexema == "true" || lexema == "false") token = new Token(Token::BOOL_LIT, input, first, current, line);
        else if (lexema == "print")  token = new Token(Token::PRINT,    input, first, current, line);
        else                         token = new Token(Token::ID,       input, first, current, line);
    }
    else {
        switch (c) {
            case '+':
                if (input[current+1] == '+') { current++; token = new Token(Token::INC, input, first, current+1, line); }
                else if (input[current+1] == '=') { current++; token = new Token(Token::PLUS_ASSIGN, input, first, current+1, line); }
                else token = new Token(Token::PLUS, c, line);
                break;
            case '-':
                if (input[current+1] == '-') { current++; token = new Token(Token::DEC, input, first, current+1, line); }
                else if (input[current+1] == '=') { current++; token = new Token(Token::MINUS_ASSIGN, input, first, current+1, line); }
                else token = new Token(Token::MINUS, c, line);
                break;
            case '*':
                if (input[current+1] == '*') { current++; token = new Token(Token::POW, input, first, current+1, line); }
                else if (input[current+1] == '=') { current++; token = new Token(Token::MUL_ASSIGN, input, first, current+1, line); }
                else token = new Token(Token::MUL, c, line);
                break;
            case '/':
                // Comentarios // 
                if (input[current+1] == '/') {
                    while (current < input.length() && input[current] != '\n')
                        current++;
                    return nextToken();
                }
                else if (input[current+1] == '=') { current++; token = new Token(Token::DIV_ASSIGN, input, first, current+1, line); }
                else token = new Token(Token::DIV, c, line);
                break;
            case '%':
                if (input[current+1] == '=') { current++; token = new Token(Token::MOD_ASSIGN, input, first, current+1, line); }
                else token = new Token(Token::MOD, c, line);
                break;
            case '=':
                if (input[current+1] == '=') { current++; token = new Token(Token::EQ, input, first, current+1, line); }
                else token = new Token(Token::ASSIGN, c, line);
                break;
            case '!':
                if (input[current+1] == '=') { current++; token = new Token(Token::NEQ, input, first, current+1, line); }
                else token = new Token(Token::NOT, c, line);
                break;
            case '<':
                if (input[current+1] == '=') { current++; token = new Token(Token::LTE, input, first, current+1, line); }
                else token = new Token(Token::LT, c, line);
                break;
            case '>':
                if (input[current+1] == '=') { current++; token = new Token(Token::GTE, input, first, current+1, line); }
                else token = new Token(Token::GT, c, line);
                break;
            case '&':
                if (input[current+1] == '&') { current++; token = new Token(Token::AND, input, first, current+1, line); }
                else token = new Token(Token::ERR, c, line);
                break;
            case '|':
                if (input[current+1] == '|') { current++; token = new Token(Token::OR, input, first, current+1, line); }
                else token = new Token(Token::ERR, c, line);
                break;
            case '(': token = new Token(Token::LPAREN,   c, line); break;
            case ')': token = new Token(Token::RPAREN,   c, line); break;
            case '{': token = new Token(Token::LBRACE,   c, line); break;
            case '}': token = new Token(Token::RBRACE,   c, line); break;
            case '[': token = new Token(Token::LBRACKET, c, line); break;
            case ']': token = new Token(Token::RBRACKET, c, line); break;
            case ';': token = new Token(Token::SEMICOL,  c, line); break;
            case ',': token = new Token(Token::COMMA,    c, line); break;
            case '.': token = new Token(Token::DOT,      c, line); break;
            case ':': token = new Token(Token::COLON,    c, line); break;
            default:  token = new Token(Token::ERR,      c, line); break;
        }
        current++;
    }

    return token;
}

// -----------------------------
// Destructor
// -----------------------------
Scanner::~Scanner() { }

// -----------------------------
// Función de prueba
// -----------------------------

void ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    string OutputFileName = InputFile;
    size_t pos = OutputFileName.find_last_of(".");
    if (pos != string::npos)
        OutputFileName = OutputFileName.substr(0, pos);
    OutputFileName += "_tokens.txt";

    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
        return;
    }

    outFile << "Scanner\n" << endl;

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << endl;
            delete tok;
            outFile << "\nScanner exitoso" << endl << endl;
            outFile.close();
            return;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido" << endl << endl;
            outFile << "Scanner no exitoso" << endl << endl;
            outFile.close();
            return;
        }

        outFile << *tok << endl;
        delete tok;
    }
}