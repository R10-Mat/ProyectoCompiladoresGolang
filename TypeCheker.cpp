//
// Created by Usuario on 21/6/2026.
//

#include "TypeCheker.h"
#include "ast.h"

// =============================================================================
// TypeCheckerVisitor — Análisis semántico
// =============================================================================
// Responsabilidades:
//   1. Registrar las funciones y su aridad antes de analizar los cuerpos.
//   2. Verificar que cada variable usada haya sido declarada en el scope.
//   3. Verificar que cada función llamada exista y reciba el número correcto
//      de argumentos.
//   4. Contar las variables locales de cada función para que GenCodeVisitor
//      pueda reservar el espacio correcto en el stack frame.
//   5. Inferencia y chequeo de Tipos en Expresiones:
//   6. Validación de Tipos Compuestos y Structs
//   7. Validación de Punteros y Arreglos
//   8. Registro de métodos
// =============================================================================

// -----------------------------------------------------------------------------
// TypeChecker — punto de entrada
// -----------------------------------------------------------------------------

// Parte Rayhan //

//  Parte Bruno //
Semantic_types TypeChekerVisitor::visit(Block* block) {
    entorno.add_level();

    if (block->lista_statements != nullptr) {
        block->lista_statements->accept(this);
    }
    entorno.remove_level();
    return Semantic_types(Semantic_types::VOID);
}
Semantic_types TypeChekerVisitor::visit(StmtList* stmt_list) {
    for (auto stmt : stmt_list->statements) {
        if (stmt != nullptr) {
            stmt->accept(this);
        }
    }
    return Semantic_types(Semantic_types::VOID);
}
Semantic_types TypeChekerVisitor::visit(DeclarationStmt* declaration_stmt) {
    if (declaration_stmt->declaration != nullptr) {
        declaration_stmt->declaration->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(BlockStmt* block_stmt) {
    if (block_stmt->block != nullptr) {
        block_stmt->block->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ExpresionStmt* expresion_stmt) {
    if (expresion_stmt->expresion != nullptr) {
        expresion_stmt->expresion->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(IncDecStmt *inc_dec_stmt) {
    if (inc_dec_stmt->expresion != nullptr) {
        Semantic_types tipo_izq = inc_dec_stmt->expresion->accept(this);

        if (tipo_izq.getKind() != Semantic_types::INT) {
            cerr << "Error semantico: Los operadores '++' y '--' "
                 << "solo se pueden a aplciar a int. Se encontro: "
                 << tipo_izq.getKind() << "\n";
            exit(1);
        }
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(Assigment *assigment_stmt) {
    // 1. Validación de seguridad básica
    if (assigment_stmt->expresion_list_id == nullptr || assigment_stmt->expresion_list_values == nullptr) {
        return Semantic_types(Semantic_types::VOID);
    }

    auto& lista_izq = assigment_stmt->expresion_list_id->lista_exp;
    auto& lista_der = assigment_stmt->expresion_list_values->lista_exp;

    // 2. Regla 1: Validar que los tamaños coincidan (ej: x, y = 1, 2)
    if (lista_izq.size() != lista_der.size()) {
        cerr <<  "Desajuste en la cantidad de variables y valores de asignación";
        exit(1);
    }

    // 3. Regla 2: Si es un operador compuesto (+=, -=, *=, /=), Go prohíbe la asignación múltiple
    // Es decir, solo se permite 'a += 1', no 'a, b += 1, 2'
    if (assigment_stmt->op != AssignOp::ASSIGN && lista_izq.size() > 1) {
        cerr << "Operadores compuestos no permiten asignación múltiple";
        exit(1);
    }

    // 4. Iterar y chequear tipos par por par
    for (size_t i = 0; i < lista_izq.size(); ++i) {
        Semantic_types tipo_izq = lista_izq[i]->accept(this);
        Semantic_types tipo_der = lista_der[i]->accept(this);

        // Si el lado izquierdo es un identificador que no existe en el entorno, tipo_izq será un tipo de error
        if (tipo_izq.getKind() == Semantic_types::NOTYPE) {
            cerr << "La variable no ha sido declarada";
            exit(1);
            continue;
        }

        // Caso A: Asignación simple ( = )
        if (assigment_stmt->op == AssignOp::ASSIGN) {
            if (tipo_izq.getKind() != tipo_der.getKind()) {
                cerr << "No se puede asignar un tipo 'X' a una variable de tipo 'Y'";
                exit(1);
            }
        }
        // Caso B: Asignación compuesta ( += , -= , *= , /= )
        else {
            // Validar que ambos lados sean estrictamente numéricos y del mismo tipo
            if ((tipo_izq.getKind() != Semantic_types::INT && tipo_izq.getKind() != Semantic_types::FLOAT64) ||
                (tipo_der.getKind() != Semantic_types::INT && tipo_der.getKind() != Semantic_types::FLOAT64)) {
                cerr << "Los operadores compuestos solo se permiten en tipos numéricos";
            }
            else if (tipo_izq.getKind() != tipo_der.getKind()) {
                cerr << "Tipos no coinciden en la operación compuesta (Go no tiene casteo implícito)";
            }
        }
    }

    // Como es un Statement, su evaluación semántica siempre devuelve VOID
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ReturnStmt* return_stmt) {
    if (tipo_retorno_actual.getKind() == Semantic_types::VOID) {
        if (return_stmt->expresion_list != nullptr) {
            cerr << "Error semantico en la función '" << nombre_funcion_actual
                 << "': Una function que retorna void no debe devolver ninguna valor \n ";
            exit(1);
        }
        return Semantic_types(Semantic_types::VOID);
    }
    if (return_stmt->expresion_list == nullptr) {
        cerr << "Error semantico en funcion '" << nombre_funcion_actual
                << "': La funcion no es void, debe devolver un valor de tipo "
                << tipo_retorno_actual.to_string() << ".\n";
        exit(1);
    }
    Semantic_types tipo_expresion = return_stmt->expresion_list->accept(this);

    if (!tipo_retorno_actual.match(&tipo_expresion)) {
        cerr << "Error semantico en funcion '" << nombre_funcion_actual
             << "': El tipo de retorno (" << tipo_expresion.to_string()
             << ") no coincide con el tipo declarado ("
             << tipo_retorno_actual.to_string() << ").\n";
        exit(1);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(BreakStmt* break_stmt) {
    if (this->dentro_de_bucle <= 0) {
        cerr << "Error semantico: La sentencia break solo puede usarse dentro de un bucle";
        exit(1);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ContinueStmt* continue_stmt) {
    if (this->dentro_de_bucle <= 0) {
        cerr << "Error semantico: La sentencia continue solo puede usarse dentro de un bucle";
        exit(1);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(IfStmt *if_stmt) {
    if (if_stmt->expresion != nullptr) {
        Semantic_types tipo_cond = if_stmt->expresion->accept(this);

        if (tipo_cond.getKind() != Semantic_types::BOOL) {
            cerr << "Error semantico: La condifición del if debe ser de tipo bool"
                 << "Se encontro: " << tipo_cond.getKind() << endl;
            exit(1);
        }
    }
    if (if_stmt->cuerpo_if != nullptr) {
        if_stmt->cuerpo_if->accept(this);
    }
    if (if_stmt->if_anidado != nullptr) {
        if_stmt->if_anidado->accept(this);
    }
    if (if_stmt->cuerpo_else != nullptr) {
        if_stmt->cuerpo_else->accept(this);
    }
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(SwitchStmt *switch_stmt) {
    Semantic_types tipo_base_switch(Semantic_types::BOOL);
    if (switch_stmt->expresion != nullptr) {
        tipo_base_switch = switch_stmt->expresion->accept(this);
    }
    Semantic_types tipo_anterior = this->tipo_switch_actual;
    this->tipo_switch_actual = tipo_base_switch;

    entorno.add_level();
    for (auto un_case : switch_stmt->exp_case_clause) {
        if (un_case != nullptr) {
            un_case->accept(this);
        }
    }
    entorno.remove_level();
    this->tipo_switch_actual = tipo_anterior;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ExpCaseClause *exp_case_clause) {
    if (exp_case_clause->expresion_list != nullptr) {
        for (auto exp: exp_case_clause->expresion_list->lista_exp) {
            Semantic_types tipo_exp = exp->accept(this);

            if (tipo_exp.getKind() != this->tipo_switch_actual.getKind()) {
                cerr  << "Error semantico: Tipo del case no coincide con el tipo del switch";
                exit(1);
            }
        }
    }

    if (exp_case_clause->statement_list != nullptr) {
        exp_case_clause->statement_list->accept(this);
    }

    return Semantic_types(Semantic_types::VOID);
}


Semantic_types TypeChekerVisitor::visit(ForStmt *for_stmt) {
    // Activamos la bandera para permitir 'break' y 'continue' dentro del bloque
    this->dentro_de_bucle++;

    // --- CASO 1: For con Cláusula (for init; cond; post { ... }) ---
    if (for_stmt->for_clause != nullptr) {
        // Abrimos el entorno ANTES de la cláusula porque el 'init' (asignacion1)
        // puede declarar variables locales (ej: i := 0) que solo viven en el bucle.
        entorno.add_level();

        for_stmt->for_clause->accept(this);

        if (for_stmt->block != nullptr) {
            for_stmt->block->accept(this);
        }

        entorno.remove_level();
    }
    else {
        if (for_stmt->expresion != nullptr) {
            // La condición se evalúa en el entorno actual (el del padre)
            Semantic_types tipo_condicion = for_stmt->expresion->accept(this);
            if (tipo_condicion.getKind() != Semantic_types::BOOL) {
                cerr << "La condición del bucle for debe ser de tipo bool";
                exit(1);
            }
        }

        // El bloque del 'for' condicional sí abre su propio nivel de variables
        if (for_stmt->block != nullptr) {
            entorno.add_level();
            for_stmt->block->accept(this);
            entorno.remove_level();
        }
    }
    this->dentro_de_bucle--;

    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ForClause *for_clause) {
    // 1. Validar la inicialización (ej: i := 0 o i = 0)
    // Las variables nuevas (si usa :=) se registrarán en el entorno actual
    if (for_clause->asignacion1 != nullptr) {
        for_clause->asignacion1->accept(this);
    }

    // 2. Validar la condición central (debe ser estrictamente un BOOL)
    if (for_clause->expresion != nullptr) {
        Semantic_types tipo_cond = for_clause->expresion->accept(this);
        if (tipo_cond.getKind() != Semantic_types::BOOL) {
            cerr << "La condicion central del for debe ser de tipo bool";
            exit(1);
        }
    }

    // 3. Validar el paso de post-ejecución (ej: i++ o i += 1)
    // Nota: Aquí se evalúa en el mismo entorno, por lo que modificará las variables del init
    if (for_clause->inc_dec_stmt != nullptr) {
        for_clause->inc_dec_stmt->accept(this);
    } else if (for_clause->asignacion2 != nullptr) {
        for_clause->asignacion2->accept(this);
    }

    return Semantic_types(Semantic_types::VOID);
}
// Parte Nico //