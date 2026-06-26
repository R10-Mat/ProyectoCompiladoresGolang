//
// Created by Usuario on 21/6/2026.
//

#include "TypeCheker.h"

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
    if (assigment_stmt->expresion_list_id == nullptr || assigment_stmt->expresion_list_values == nullptr) {
        return Semantic_types(Semantic_types::VOID);
    }
     // refinar esta parte
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

    entorno.add_level();
    for (auto un_case : switch_stmt->exp_case_clause) {
        if (un_case != nullptr) {
            un_case->accept(this);
        }
    }
    entorno.remove_level();
    this->tipo_retorno_actual;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ExpCaseClause *exp_case_clause) {

}


Semantic_types TypeChekerVisitor::visit(ForStmt *for_stmt) {
    // falta Validar la condición del for si existe
    this -> dentro_de_bucle++;
    if (for_stmt->block!=nullptr) {
        for_stmt->block->accept(this);
    }
    this -> dentro_de_bucle--;
    return Semantic_types(Semantic_types::VOID);
}

Semantic_types TypeChekerVisitor::visit(ForClause *for_clause) {

}
// Parte Nico //