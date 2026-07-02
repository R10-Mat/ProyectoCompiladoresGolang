#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"
#include "token.h"
#include "GenCode.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Verificar número de argumentos
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // ---- Fase 1: lexica (dos scanners: uno para el volcado de tokens de
    // depuracion, otro para alimentar al parser) ----
    Scanner scanner1(input.c_str());
    Scanner scanner2(input.c_str());

    ejecutar_scanner(&scanner1, argv[1]);
    cout << "Fase lexica: OK" << endl;

    // ---- Fase 2: sintactica ----
    Parser parser(&scanner2);
    Programa* ast = nullptr;
    try {
        ast = parser.parseProgram();
    } catch (const std::exception& e) {
        cerr << "Fase sintactica: ERROR: " << e.what() << endl;
        return 1;
    }
    cout << "Fase sintactica: OK" << endl;

    // ---- Fase 3: semantica (el chequeo aborta el proceso con exit(1) y su
    // propio mensaje "Error semantico: ..." si algo no es valido) ----
    // Se genera primero en un buffer en memoria (no en el archivo real)
    // para que, si el chequeo semantico falla, no quede un output.s vacio
    // y engañoso -- solo se escribe a disco tras un codegen exitoso.
    ostringstream asmBuffer;
    GenCode gen(asmBuffer);
    gen.tipos.TypeCheker(ast);
    cout << "Fase semantica: OK" << endl;

    // ---- Fase 4: generacion de codigo x86-64 (incluye el plegado de
    // constantes inline como unica optimizacion de esta version) ----
    gen.generar(ast);

    ofstream asmOut("output.s");
    if (!asmOut.is_open()) {
        cerr << "No se pudo crear el archivo de salida output.s" << endl;
        delete ast;
        return 1;
    }
    asmOut << asmBuffer.str();
    asmOut.close();
    cout << "Fase de generacion de codigo: OK" << endl;

    delete ast;
    return 0;
}
