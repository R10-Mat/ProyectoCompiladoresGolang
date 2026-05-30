#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERROR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERROR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

// change this
Programa* Parser::parseProgram() {
    Programa* prog = new Programa();
    return prog;
}

Type* Parser::parseType() {
    // Typename [TypeArgs]
    if (check(Token::ID)) {
        TypeName_TypeArgs* tname_targs = parseTypeName_TypeArgs();
        return tname_targs;
    }
    // TypeLit
    if (check(Token::LCORCHETE) || check(Token::STRUCT) || check(Token::MUL)
            || check(Token::ARROW) || check(Token::CHAN) || check (Token::MAP)) {
        return parseTypeLiteral();
            }
    // "(" Type ")"
    if (check(Token::LPAREN)) {
        match(Token::LPAREN);
        Type* innertype = parseType();
        if (check(Token::RPAREN)) {
            match(Token::RPAREN);
            return innertype;
        }
        throw runtime_error("Falta el ')'");
    }
    throw runtime_error("Tipo de dato no reconocido");
}

TypeName_TypeArgs* Parser::parseTypeName_TypeArgs() {
    auto tname_targs = new TypeName_TypeArgs();
    tname_targs->nombres = parseTypeName();
    if (check(Token::LCORCHETE)){
        tname_targs->argumentos = parseTypeArgs();
        return tname_targs;
    } else {
        tname_targs->argumentos = nullptr;
        return tname_targs;
    }
}

TypeName* Parser::parseTypeName() {
    auto tname = new TypeName();
    if (check(Token::ID)) {
        match(Token::ID);
        string primerId = previous->text;
        if (check(Token::PUNTO)) {
            match(Token::PUNTO);
            if (check(Token::ID)) {
                match(Token::ID);
                tname->prefijo_paquete = primerId;
                tname->nombre = previous->text;
                return tname;
            } throw runtime_error("Necesitas otro id");
        }
        tname->nombre = primerId;
        tname->prefijo_paquete = "";
        return tname;
    } throw runtime_error("Necesita un id");
}

TypeArgs *Parser::parseTypeArgs() {
    auto argumentostipos = new TypeArgs();
    if (check(Token::LCORCHETE)) {
        match(Token::LCORCHETE);
        argumentostipos->lista = parseTypeList();
        if (check(Token::COMMA)) {
            match(Token::COMMA);
        }
        if (check(Token::RCORCHETE)) {
            match(Token::RCORCHETE);
            return argumentostipos;
        }
        throw runtime_error("Falta un ']' ");
    } throw runtime_error("Falta un '[' ");
}

TypeList* Parser::parseTypeList() {
    auto lista = new TypeList();
    lista->lista_tipos.push_back(parseType());
    while (match(Token::COMMA)) {
        if (check(Token::RCORCHETE)) {
            break;
        }
        lista->lista_tipos.push_back(parseType());
    }
    return lista;
}

Type* Parser::parseTypeLiteral() {

    if (check(Token::LCORCHETE)) {
        return parseArrayorSliceType();
    }

    if (check(Token::STRUCT)) {
        return parseStructType();
    }

    if (check(Token::MUL)) {
        return parsePointerType();
    }

    if (check(Token::FUNC)) {
        return parseFunctionType();
    }

    if (check(Token::MAP)) {
        return parseMapType();
    }

    if (check(Token::ARROW) || check(Token::CHAN)) {
        return parseChannelType();
    }

    throw runtime_error("Literal de tipo no soportado o invalido");
}


TypeLiteral *Parser::parseArrayorSliceType() {
    match(Token::LCORCHETE);
    if (check(Token::RCORCHETE)) {
        auto slice = parseSliceType();
        return slice;
    }
    auto array = parseArrayType();
    return array;
}

SliceType *Parser::parseSliceType() {
    if (check(Token::RCORCHETE)) {
        match(Token::RCORCHETE);
        auto slice = new SliceType();
        slice->elementtype = parseType();
    } throw runtime_error ("Declaracion de slice invalida: falta ']' ");
}

ArrayType *Parser::parseArrayType() {
    auto array = new ArrayType();
    array->length = parseExpresion();

    if (check(Token::RCORCHETE)){
        match(Token::RCORCHETE);
        array->elementtype = parseType();
        return array;
    } throw runtime_error("Falta un ']'");
}


StructType *Parser::parseStructType() {
    auto struct_ = new StructType();
    match(Token::STRUCT);
    if (check(Token::LLLAVE)) {
        match(Token::LLLAVE);

        while (!check(Token::RLLAVE) && !isAtEnd()) {
            FieldDecl* fd = parseFieldDecl();
            struct_->declaraciones.push_back(fd);

            if (check(Token::PCOMMA)) {
                match(Token::PCOMMA);
            }
        }
        if (check(Token::RLLAVE)) {
            match(Token::RLLAVE);
            return struct_;
        } throw runtime_error ("Falta un '}'");
    } throw runtime_error ("Falta un '{'");
}

FieldDecl *Parser::parseFieldDecl() {
    auto field = new FieldDecl();
    field->tiene_mul = false;

    if (check(Token::MUL)) {
        match(Token::MUL);
        field->tiene_mul = true;
        field->type = parseTypeName_TypeArgs();
    }
    else {
        if (check(Token::ID)) {
            field->identifierlist = parseIdentifierList();
            if (check(Token::PCOMMA) || check(Token::RLLAVE) || check(Token::STRING_LIT)){
                string nombre_tipo = field->identifierlist.front()->name;
                field->identifierlist.clear();

                auto tname = new TypeName();
                tname->nombre = nombre_tipo;
                tname->prefijo_paquete = "";

                if (check(Token::LCORCHETE)) {
                    auto targs = parseTypeArgs();
                    field->type = new TypeName_TypeArgs(tname,targs);
                } else {
                    field->type = tname;
                }
            } else {
                field->type = parseType();
            }
        } else {
            throw runtime_error ("Declaracion invalida. Se esperaba un ID o '*'");
        }
    }
    if (check(Token::STRING_LIT)) {
        match(Token::STRING_LIT);
        field->tag = previous->text;
    }
    return field;
}

PointerType *Parser::parsePointerType() {
    match(Token::MUL);
    auto puntero = new PointerType();
    puntero->basetype = parseType();
    return puntero;

}

FunctionType *Parser::parseFunctionType() {
    match(Token::FUNC);
    auto func = new FunctionType();
    func->signature = parseSignature();
    return func;
}

Signature *Parser::parseSignature() {
    auto signature = new Signature();
    if (check(Token::LPAREN)) {
        match(Token::LPAREN);
        while (!check(Token::RPAREN) && !isAtEnd()) {
            signature->parameterlist.push_back(parseParameterDecl());
            if (check(Token::COMMA)) match(Token::COMMA);
        }
        if (check(Token::RPAREN)) {
            match(Token::RPAREN);
        } else {
            throw runtime_error ("Falta un ')' en la firma de la funcion");
        }
    } else {
        throw runtime_error ("Falta un '(' en la firma de la funcion");
    }

    if (check(Token::LPAREN)) {
        match(Token::LPAREN);
        while (!check(Token::RPAREN) && !isAtEnd()) {
            signature->result_parameters.push_back(parseParameterDecl());
            if (check(Token::COMMA)) match(Token::COMMA);
        }
        if (check(Token::RPAREN)) {
            match(Token::RPAREN);
        } else {
            throw runtime_error ("Se esperaba ')' al cerrar los retornos.");
        }
    } else {
        if (!check(Token::LLLAVE) && !check(Token::PCOMMA) && !check(Token::RLLAVE)
            && !check(Token::COMMA) && !check(Token::RPAREN)) {
            auto ret = new ParameterDecl();
            ret->type = parseType();
            signature->result_parameters.push_back(ret);
        }
    }
    return signature;
}

ParameterDecl *Parser::parseParameterDecl() {
    auto param = new ParameterDecl();

    if (match(Token::TRES_PUNTOS)) {
        param->es_variadico = true;
    }
    if (check(Token::ID)) {
        param->identifierlist = parseIdentifierList();

        if (check(Token::COMMA) || check(Token::RPAREN)) {
            string nombreTipo = param->identifierlist.front()->name;
            param->identifierlist.clear();

            auto tname = new TypeName();
            tname->nombre = nombreTipo;
            param->type = tname;
        }
        else {
            if (match(Token::TRES_PUNTOS)) {
                param->es_variadico = true;
            }
            param->type = parseType();
        }
    }
    else {
        param->type = parseType();
    }
    return param;
}

MapType *Parser::parseMapType() {
    match(Token::MAP);
    if (check(Token::LCORCHETE)) {
        match(Token::LCORCHETE);
        auto map = new MapType();
        map->keytype = parseType();
        if (check(Token::RCORCHETE)) {
            match(Token::RCORCHETE);
            map->elementtype = parseType();
            return map;
        } throw runtime_error ("Declaracion de map invalida: falta ']' ");
    } throw runtime_error ("Declaracion de map invalida: falta '[' ");
}

ChannelType *Parser::parseChannelType() {
    if (check(Token::CHAN)) {
        match(Token::CHAN);
        if (check(Token::ARROW)) {
            match(Token::ARROW);
        }
        auto channel = new ChannelType();
        channel->elementtype = parseType();
        return channel;
    } else if (check(Token::ARROW)){
        match(Token::ARROW);
        auto channel = new ChannelType();
        channel->elementtype = parseType();
        return channel;
    } else {
        throw runtime_error(" Declaracion de arrow invalida ");
    }
}



// -------------------------------------------------------
/*
Block *Parser::parseBlock() {
    if (check(Token::LLLAVE)) {
        match(Token::LLLAVE);
        auto bloque = new Block();
        bloque->lista_statements = parseStmtList();
        if (check(Token::RLLAVE)) {
            match(Token::RLLAVE);
            return bloque;
        } throw runtime_error("Un bloque debe terminar con '}'");
    }
    throw runtime_error("Un bloque debe empezar con '{' ");
}

StmtList *Parser::parseStmtList() {
    auto lista_statements = new StmtList();
    lista_statements->statements.push_back(parseStmt());
    while (match(Token::PCOMMA)) {  // error personalizado falta??
        lista_statements->statements.push_back(parseStmt());
    }
    return lista_statements;
}


Decl *Parser::parseDecl() {
    if (check(Token::CONST)) {
        match(Token::CONST);

    } else if (check(Token::TYPE)) {
        match(Token::TYPE);
        // typedeclaration
    } else if (check(Token::VAR)) {
        match(Token::VAR);
        // vardeclaration
    }
    throw runtime_error("Declaración incorrecta");
}

ConstDecl *Parser::parseConstDecl() {
    auto constdecl = new ConstDecl();
    if (check(Token::LPAREN)) {
        match(Token::LPAREN);
        while (!isAtEnd() && !check(Token::RPAREN)) {
            auto spec = parseConstSpec();
            constdecl->specs.push_back(spec);

            if (check(Token::PCOMMA)) {
                match(Token::PCOMMA);
            }
        }
        if (check(Token::RPAREN)) {
            match(Token::RPAREN);
            return constdecl;
        } throw runtime_error ("Cerrar el parentesis");
    } else {
        auto spec = parseConstSpec();
        constdecl->specs.push_back(spec);
        return constdecl;
    }
}

ConstSpec *Parser::parseConstSpec() {
    auto spec = new ConstSpec();
    spec->lista_id = parseIndetifierList();
    if (!check(Token::PCOMMA) && !check(Token::RPAREN)) {
        if (!check(Token::ASSIGN)) {
            spec->tipo = parseType();
        }

        if (check(Token::ASSIGN)) {
            match(Token::ASSIGN);
            spec->lista_expresiones = parseExpresionList();
        } throw runtime_error("Se esperaba '=' en la declarion de la constante");
    }
    return spec;
}
/*

// helpers to delete:
/*
Exp *Parser::parseQualifiedIdent() {
    auto qualifiedident = new QualifiedIdent();
    qualifiedident->prefijo = parsePackageName();
    if (check(Token::PUNTO)) {
        match(Token::PUNTO);
        if (check(Token::ID)) {
            match(Token::ID);
            qualifiedident->subfijo = previous->text;
        } throw runtime_error("Falta un id");
    } throw runtime_error("Falta el punto");
}

Exp *Parser::parsePackageName() {
    auto id = new IdExp();
    if (check(Token::ID)) {
        match(Token::ID);
        id->name = previous->text;
        return id;
    } throw runtime_error("Esto no es un Id");
}
*/