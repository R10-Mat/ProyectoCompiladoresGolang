#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"
using namespace std;

class Scanner {
private:
    string input;
    int first;
    int current;
    int line;
public:
    // Constructor
    explicit Scanner(const char* in_s);

    // Retorna el siguiente token
    Token* nextToken();

    // Destructor
    ~Scanner();

    // INTEGER-LITERAL AND FLOAT-LITERAL
    Token* readNumberToken();

    // STRING-LITERAL
    Token* readStringToken();
};

// Ejecutar scanner
void ejecutar_scanner(Scanner* scanner,const string& InputFile);

#endif // SCANNER_H