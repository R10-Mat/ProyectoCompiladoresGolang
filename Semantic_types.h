//
// Created by Usuario on 22/6/2026.
//

#ifndef SEMANTIC_TYPES_H
#define SEMANTIC_TYPES_H
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>

using namespace std;

// ===========================================================
//  Representación de tipos del lenguaje (para el TypeChecker)
// ===========================================================
// Cubre los tipos basicos (int, float64, bool, string, void) y deja la base
// para tipos compuestos (POINTER, ARRAY, STRUCT), que hacen falta para
// chequear semanticamente PointerType/ArrayType/StructType del AST.
// Los metodos de conversion/promocion (isNumeric, canPromoteTo,
// isConvertibleTo) son utilidades que el TypeChecker puede usar para
// inferencia y conversion de tipos; no imponen ninguna regla por si solos.

class Semantic_types {
public:
    enum Kind {NOTYPE, VOID, INT, BOOL, FLOAT64, STRING, POINTER, ARRAY, STRUCT};
private:
    Kind kind;
    string nombre_struct;                                        // kind == STRUCT
    shared_ptr<Semantic_types> tipo_base;                         // apuntado (POINTER) / elemento (ARRAY)
    long longitud_arreglo = 0;                                    // kind == ARRAY
    vector<pair<string, shared_ptr<Semantic_types>>> campos_struct; // kind == STRUCT

public:
    Semantic_types():kind(NOTYPE){};
    explicit Semantic_types(Kind _kind):kind(_kind){};

    [[nodiscard]] Kind getKind() const {return this->kind;}

    // ---- Fabricas para tipos compuestos ----
    static Semantic_types makePointer(const Semantic_types& apuntado) {
        Semantic_types t(POINTER);
        t.tipo_base = make_shared<Semantic_types>(apuntado);
        return t;
    }

    static Semantic_types makeArray(const Semantic_types& elemento, long longitud) {
        Semantic_types t(ARRAY);
        t.tipo_base = make_shared<Semantic_types>(elemento);
        t.longitud_arreglo = longitud;
        return t;
    }

    static Semantic_types makeStruct(const string& nombre, const vector<pair<string, Semantic_types>>& campos) {
        Semantic_types t(STRUCT);
        t.nombre_struct = nombre;
        for (const auto& campo : campos) {
            t.campos_struct.emplace_back(campo.first, make_shared<Semantic_types>(campo.second));
        }
        return t;
    }

    [[nodiscard]] bool isPointer() const { return kind == POINTER; }
    [[nodiscard]] bool isArray()   const { return kind == ARRAY; }
    [[nodiscard]] bool isStruct()  const { return kind == STRUCT; }
    [[nodiscard]] bool isNumeric() const { return kind == INT || kind == FLOAT64; }

    // Tipo apuntado (POINTER) o de los elementos (ARRAY); nullptr si no aplica.
    [[nodiscard]] const Semantic_types* baseType() const { return tipo_base.get(); }
    [[nodiscard]] long arrayLength() const { return longitud_arreglo; }
    [[nodiscard]] const string& structName() const { return nombre_struct; }

    // Tipo de un campo del struct; NOTYPE si el campo no existe.
    [[nodiscard]] Semantic_types fieldType(const string& nombre_campo) const {
        for (const auto& campo : campos_struct)
            if (campo.first == nombre_campo && campo.second) return *campo.second;
        return Semantic_types(NOTYPE);
    }

    bool match(const Semantic_types* other) const {
        if (!other) return false;
        if (this->kind != other->kind) return false;
        switch (kind) {
            case POINTER:
                return tipo_base && other->tipo_base && tipo_base->match(other->tipo_base.get());
            case ARRAY:
                return longitud_arreglo == other->longitud_arreglo
                    && tipo_base && other->tipo_base && tipo_base->match(other->tipo_base.get());
            case STRUCT:
                // Los structs de Go son tipos con nombre: comparar por nombre alcanza.
                return nombre_struct == other->nombre_struct;
            default:
                return true;
        }
    }

    // Promocion numerica automatica: en Go no existe conversion implicita
    // entre int y float64, pero se deja disponible por si el TypeChecker
    // decide soportarla como extension del lenguaje base.
    [[nodiscard]] bool canPromoteTo(const Semantic_types& destino) const {
        return kind == INT && destino.kind == FLOAT64;
    }

    // Conversion explicita estilo Go: T(x). No implementa la regla completa
    // del lenguaje, solo lo minimo (mismo tipo, o ambos numericos).
    [[nodiscard]] bool isConvertibleTo(const Semantic_types& destino) const {
        if (kind == destino.kind) return true;
        return isNumeric() && destino.isNumeric();
    }

    [[nodiscard]] string to_string() const {
        switch (kind) {
            case INT:     return "int";
            case FLOAT64: return "float64";
            case BOOL:    return "bool";
            case STRING:  return "string";
            case VOID:    return "void";
            case POINTER: return "*" + (tipo_base ? tipo_base->to_string() : string("?"));
            case ARRAY:   return "[" + std::to_string(longitud_arreglo) + "]" + (tipo_base ? tipo_base->to_string() : string("?"));
            case STRUCT:  return "struct " + nombre_struct;
            default: return "invalid type";
        }
    }

    static Kind string_to_kind(const string& cadena) {
        static const unordered_map<string_view,Kind> type_map = {
            {"int",INT},
            {"float64",FLOAT64},
            {"string",STRING},
            {"bool",BOOL},
            {"void",VOID}
        };
        auto it = type_map.find(cadena);
        if (it != type_map.end()) {
            return it->second;
        }
        return NOTYPE;
    }

    bool set_basic_type(const string&s) {
        Kind k = string_to_kind(s);
        if (k == NOTYPE) return false;
        kind = k;
        return true;
    }
};



#endif //SEMANTIC_TYPES_H
