//
// Created by Usuario on 21/6/2026.
//

#ifndef TYPECHEKER_H
#define TYPECHEKER_H

#include "Enviroment.h"
#include "visitor.h"

using namespace std;

template <typename T>
class TypeChekerVisitor: public Visitor{
public:
    //          [key] = [value]
    // nombre función = número de variables que la función necesita
    unordered_map<string,int> fun_var_contador;

    //          [key] = [value]
    // nombre funcion = número de parametros que la función necesita
    unordered_map<string, int> fun_par_contador;

    //Entorno de variables
    Environment<T> entorno;   // creo que aca se tiene que cambiar el tipo por un type

    // nombre de la funcipón actual
    string funcion_actual;

    void TypeCheker(Programa* programa);




};



#endif //TYPECHEKER_H
