#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

#include <unordered_map>

using namespace std;
// -----------------------------
// Unordered Map para keywords
// -----------------------------
const unordered_map<string,Token::Type> Keywords {
    {"break",       Token::BREAK},
    {"case",        Token::CASE},
    {"chan",        Token::CHAN},
    {"const",       Token::CONST},
    {"continue",    Token::CONTINUE},
    {"default",     Token::DEFAULT},
    {"defer",       Token::DEFER},
    {"else",        Token::ELSE},
    {"fallthrough", Token::FALLTHROUGH},
    {"for",         Token::FOR},
    {"func",        Token::FUNC},
    {"go",          Token::GO},
    {"goto",        Token::GOTO},
    {"if",          Token::IF},
    {"import",      Token::IMPORT},
    {"interface",   Token::INTERFACE},
    {"map",         Token::MAP},
    {"package",     Token::PACKAGE},
    {"range",       Token::RANGE},
    {"return",      Token::RETURN},
    {"select",      Token::SELECT},
    {"struct",      Token::STRUCT},
    {"switch",      Token::SWITCH},
    {"type",        Token::TYPE},
    {"var",         Token::VAR}
};

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

    if (isdigit(c) || (c == '.' && current + 1 < input.length() && isdigit(input[current + 1]))) {
        token = readNumberToken();
        return token;
    }
    else if (c == '"' || c == '`') {
        token = readStringToken();
        return token;
    }
    else if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_'))
            current++;
        string lexema = input.substr(first, current - first);

        auto it = Keywords.find(lexema);
        if (it != Keywords.end()) {
            token = new Token(it->second,input,first,current,line);
        } else {
            token = new Token(Token::ID,       input, first, current, line);
        }
    }
    else {
        switch (c) {
            case '~': token = new Token(Token::VIRGULA,    c, line); break;
            case '+':
                if (current + 1  < input.length() && input[current+1] == '+' ) {
                    current++;
                    token = new Token(Token::INC, input, first, current+1, line);
                } else if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::PLUSASSIGN, input, first, current+1, line);
                } else token = new Token(Token::PLUS, c, line);
                break;
            case '-':
                if (current + 1  < input.length() && input[current+1] == '-' ) {
                    current++;
                    token = new Token(Token::DEC, input, first, current+1, line);
                } else if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::NEGASSIGN, input, first, current+1, line);
                } else token = new Token(Token::NEG, c, line);
                break;
            case '*':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::MULASSIGN, input, first, current+1, line);
                } else token = new Token(Token::MUL, c, line);
                break;
            case '/':
                // 1. Comentarios de una sola línea: //
                if (current + 1 < input.length() && input[current + 1] == '/') {
                    while (current < input.length() && input[current] != '\n') {
                        current++;
                    }
                    return nextToken();
                }
                else if (current + 1 < input.length() && input[current + 1] == '*') {
                    current += 2;

                    bool encontradoCierre = false;
                    while (current < input.length()) {
                        if (input[current] == '\n') {
                            line++;
                        }
                        if (input[current] == '*' && current + 1 < input.length() && input[current + 1] == '/') {
                            current += 2;
                            encontradoCierre = true;
                            break;
                        }
                        current++;
                    }
                    if (!encontradoCierre) {
                        return new Token(Token::ERROR, input, first, current, line);
                    }
                    return nextToken();
                }
                else if (current + 1 < input.length() && input[current + 1] == '=') {
                    current++;
                    token = new Token(Token::DIVASSIGN, input, first, current + 1, line);
                } else  token = new Token(Token::DIV, c, line);
                break;
            case '%':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::MODASSIGN, input, first, current+1, line);
                } else token = new Token(Token::MOD, c, line);
                break;
            case '=':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::EQUAL, input, first, current+1, line);
                } else token = new Token(Token::ASSIGN, c, line);
                break;
            case '!':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::DISTINCT, input, first, current+1, line);
                } else token = new Token(Token::NOT, c, line);
                break;
            case '<':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::LEQ, input, first, current+1, line);
                } else if (current + 1 < input.length() && input[current+1] == '-') {
                    current++;
                    token = new Token(Token::ARROW, input,first,current+1,line);
                } else token = new Token(Token::LES, c, line);
                break;
            case '>':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::GEQ, input, first, current+1, line);
                }  else token = new Token(Token::GER, c, line);
                break;
            case ':':
                if (current + 1  < input.length() && input[current+1] == '=' ) {
                    current++;
                    token = new Token(Token::DEC_ASSIGN, input, first, current+1, line);
                }  else token = new Token(Token::DOS_PUNTOS, c, line);
                break;
            case '&':
                if (current + 1  < input.length() && input[current+1] == '&' ) {
                    current++;
                    token = new Token(Token::AND_LOGICAL, input, first, current+1, line);
                }  else if (current + 1  < input.length() && input[current+1] == '^' ) {
                    current++;
                    token = new Token(Token::BITCLEAR, input, first, current+1, line);
                }  else token = new Token(Token::AND, c, line);
                break;
            case '|':
                if (current + 1  < input.length() && input[current+1] == '|' ) {
                    current++;
                    token = new Token(Token::OR_LOGICAL, input, first, current+1, line);
                } else token = new Token(Token::OR, c, line);
                break;
            case '(': token = new Token(Token::LPAREN,    c, line); break;
            case ')': token = new Token(Token::RPAREN,    c, line); break;
            case '{': token = new Token(Token::LLLAVE,    c, line); break;
            case '}': token = new Token(Token::RLLAVE,    c, line); break;
            case '[': token = new Token(Token::LCORCHETE, c, line); break;
            case ']': token = new Token(Token::RCORCHETE, c, line); break;
            case ';': token = new Token(Token::PCOMMA,    c, line); break;
            case ',': token = new Token(Token::COMMA,     c, line); break;
            case '.':
                if (current + 1 < input.length() && input[current+1] == '.' &&
                    current + 2 < input.length() && input[current+2] == '.') {
                    current+=2;
                    token = new Token(Token::TRES_PUNTOS,input,first,current+2, line);
                } else token = new Token(Token::PUNTO,c, line);
                break;
            default:  token = new Token(Token::ERROR,     c, line); break;
        }
        current++;
    }

    return token;
}

// ----------------------------
// Function to read Numbers and generate Tokens
// ----------------------------
Token *Scanner::readNumberToken() {
    int start = current;
    enum State {
        START,
        STATE_ZERO,   //
        INT_DEC,      // Dígitos decimales comunes
        HEX_START,    // '0x' o '0X'
        HEX_DIGITS,   // Dígitos hexadecimales
        BIN_START,    // '0b'
        BIN_DIGITS,   // Dígitos binarios
        OCT_START,    // '0o'
        OCT_DIGITS,   // Dígitos octales
        FLOAT_DOT,    // '.'
        FLOAT_FRAC,   // Dígitos después del punto
        EXP_START,    // 'e', 'E', 'p' , 'P'
        EXP_SIGN,     // '+' , '-' después del exponente
        EXP_DIGITS    // Dígitos del exponente
    };

    State state = START;
    bool isHexFloat = false;
    char lastChar = '\0';

    while (current<input.length()) {
        char ch = input[current];

        switch (state) {
            case START:
                if (ch == '0') {
                    state = STATE_ZERO;
                } else if (ch == '.') {
                    state = FLOAT_DOT;
                } else if (isdigit(ch)) {
                    state = INT_DEC;
                }
                break;
            case STATE_ZERO:
                if (ch == 'x' || ch == 'X') {
                    state = HEX_START;
                } else if (ch == 'b' || ch == 'B') {
                    state = BIN_START;
                } else if (ch == 'o' || ch == 'O') {
                    state = OCT_START;
                } else if (isdigit(ch) || ch == '_') {
                    state = INT_DEC;
                } else {
                    goto end_loop;
                }
                break;
            case INT_DEC:
                if (ch == '.') {
                    state = FLOAT_FRAC;
                } else if (ch == 'e' || ch == 'E') {
                    state = EXP_START;
                } else if (isdigit(ch) || ch == '_'){
                    state = INT_DEC;
                } else {
                    goto end_loop;
                }
                break;
            case HEX_START:
                if (isxdigit(ch)) {
                    state = HEX_DIGITS;}
                else return new Token(Token::ERROR);
                break;
            case HEX_DIGITS:
                if (ch == '.') {
                    isHexFloat = true;
                    state = FLOAT_FRAC;
                } else if (ch == 'p' || ch == 'P') {
                    isHexFloat = true;
                    state = EXP_START;
                } else if (isxdigit(ch) || ch == '_') {
                    state = HEX_DIGITS;
                } else {
                    goto end_loop;
                }
                break;
            case BIN_START:
                if (ch == '0' || ch == '1') {
                    state = BIN_DIGITS;
                } else return new Token(Token::ERROR);
                break;
            case BIN_DIGITS:
                if (ch == '0' || ch == '1' || ch == '_') {
                    state = BIN_DIGITS;
                } else {
                    goto end_loop;
                }
                break;
            case OCT_START:
                if (ch >= '0' && ch <= '7') {
                    state = OCT_DIGITS;
                } else return new Token(Token::ERROR);
                break;
            case OCT_DIGITS:
                if ((ch >= '0' && ch <= '7')|| ch == '_') {
                    state = OCT_DIGITS;
                } else {
                    goto end_loop;
                }
                break;
            case FLOAT_DOT:
                if (isdigit(ch)) {
                    state = FLOAT_FRAC;
                } else return new Token(Token::ERROR);
                break;
            case FLOAT_FRAC:
                if (ch == 'e' || ch == 'E' || ch == 'p' || ch == 'P') {
                    if (( ch == 'p' || ch == 'P') && ! isHexFloat) {
                        return new Token(Token::ERROR);
                    }
                    state = EXP_START;
                } else if ((isHexFloat? isxdigit(ch):isdigit(ch)) || ch == '_'){
                    state = FLOAT_FRAC;
                } else {
                    goto end_loop;
                }
                break;
            case EXP_START:
                if (ch == '+' || ch == '-') {
                    state = EXP_SIGN;
                } else if (isdigit(ch)) {
                    state = EXP_DIGITS;
                } else return new Token(Token::ERROR);
                break;
            case EXP_SIGN:
                if (isdigit(ch)) {
                    state = EXP_DIGITS;
                } else return new Token(Token::ERROR);
                break;
            case EXP_DIGITS:
                if (isdigit(ch) || ch == '_') {
                    state = EXP_DIGITS;
                } else {
                    goto end_loop;
                }
                break;
        }
        lastChar = ch;
        current++;
    }
    end_loop:
        if (lastChar == '_') {
            return new Token(Token::ERROR);
        }
        string lexema = input.substr(start,current-start);
        if (lexema.find("__") != std::string::npos) {
            return new Token(Token::ERROR);
        }
        if (lexema.size() > 3 && lexema[2] == '_') {
            if (lexema[1] == 'x' || lexema[1] == 'X' || lexema[1] == 'b' || lexema[1] == 'B' || lexema[1] == 'o' || lexema[1] == 'O') {
                return new Token(Token::ERROR);
            }
        }
        if (state == INT_DEC || state == HEX_DIGITS || state == BIN_DIGITS || state == OCT_DIGITS || state == STATE_ZERO) {
            return new Token(Token::INT_LIT, input, start, current, line);
        }
        if (state == FLOAT_FRAC || state == EXP_DIGITS) {
            return new Token(Token::FLOAT_LIT, input, start, current, line);
        }
        return new Token(Token::ERROR);
}
// -----------------------------
// Function to read Strings and generate Tokens
// -----------------------------
Token *Scanner::readStringToken() {
    int start = current;
    char tipo_comillas = input[current];

    enum State {
        STRING_RAW,         //   `.....`
        STRING_INTERPRETED, //   "....."
        STRING_ESCAPE
    };
    State state = (tipo_comillas == '"') ? STRING_INTERPRETED:STRING_RAW;
    current++;

    while (current< input.length()) {
        char ch = input[current];

        switch (state) {
            case STRING_RAW:
                if (ch == '`') {
                    current++;
                    return new Token(Token::STRING_LIT,input,start,current,line);
                }
                else if (ch == '\\') {
                    line++;
                }
                break;
            case STRING_INTERPRETED:
                if (ch == '"') {
                    current++;
                    return new Token(Token::STRING_LIT, input, start, current, line);
                }
                else if (ch == '\\') {
                    state = STRING_ESCAPE;
                }
                else if (ch == '\n') {
                    return new Token(Token::ERROR);
                }
                break;
            case STRING_ESCAPE:
                if (ch == '\n') line++;
                state = STRING_INTERPRETED;
                break;
        }
        current++;
    }
    return new Token(Token::ERROR);
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

        if (tok->type == Token::ERROR) {
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