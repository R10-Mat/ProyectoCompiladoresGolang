//
// Created by Usuario on 22/6/2026.
//

#ifndef SEMANTIC_TYPES_H
#define SEMANTIC_TYPES_H
#include <string>
#include <unordered_map>

using namespace std;

// ===========================================================
//  Representación de tipos básicos del lenguaje
// ===========================================================

class Semantic_types {
public:
    enum Kind {NOTYPE, VOID, INT, BOOL, FLOAT64, STRING};
private:
    Kind kind;
public:
    Semantic_types():kind(NOTYPE){};
    explicit Semantic_types(Kind _kind):kind(_kind){};

    [[nodiscard]] Kind getKind() const {return this->kind;}

    bool match(const Semantic_types* other) const {
        if (!other) return false;
        return this->kind == other->kind;
    }

    [[nodiscard]] string to_string() const {
        switch (kind) {
            case INT:     return "int";
            case FLOAT64: return "float64";
            case BOOL:    return "bool";
            case STRING:  return "string";
            case VOID:    return "void";
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
