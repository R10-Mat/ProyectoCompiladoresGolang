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


// ----------------------------------------------------------------------
// Parte Rayhan: global y types
// ----------------------------------------------------------------------
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

ExpList *Parser::parseExpList() {

}
::


// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
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
    while (match(Token::PCOMMA)) {
        lista_statements->statements.push_back(parseStmt());
    }
    return lista_statements;
}

Stmt *Parser::parseStmt() {
    if (check(Token::VAR) || check(Token::CONST) || check(Token::TYPE)) {
        return parseDeclarationStmt();
    }
    if (check(Token::LLLAVE)) {
        return parseBlockStmt();
    }
    if (check(Token::RETURN)) {
        return parseReturnStmt();
    }
    if (check(Token::BREAK)) {
        return parseBreakStmt();
    }
    if (check(Token::CONTINUE)) {
        return parseReturnStmt();
    }
    if (check(Token::IF)) {
        return parseIfStmt();
    }
    if (check(Token::FOR)) {
        return parseForStmt();
    }
    if (check(Token::SWITCH)) {
        return parseSwitchStmt();
    }

    auto expresion_list = parseExpList();
    if (check(Token::INC) || check(Token::DEC)) {
        return parseIncDecStmt(expresion_list->expressions[0]);
    }

    if (check(Token::PLUSASSIGN) || check(Token::NEGASSIGN) ||
        check(Token::MULASSIGN)  || check(Token::DIVASSIGN)) {
        return parseAssigment(expresion_list);
    }

    return parseExpresionStmt(expresion_list->expressions[0]);

}

DeclarationStmt *Parser::parseDeclarationStmt() {
    auto declaracion = new DeclarationStmt();
    declaracion->declaration = parseDeclaration();
    return declaracion;
}

BlockStmt *Parser::parseBlockStmt() {
    auto bloque = new BlockStmt();
    bloque->block = parseBlock();
    return bloque;
}


ExpresionStmt *Parser::parseExpresionStmt(Exp* e) {
    auto expresion_stmt = new ExpresionStmt();
    expresion_stmt->expresion = e;
    return expresion_stmt;
}

IncDecStmt *Parser::parseIncDecStmt(Exp* e) {
    auto inc_dec_stmt = new IncDecStmt();
    inc_dec_stmt->expresion = e;
    UnaryOp ops;
    if (check(Token::INC)) {
        match(Token::INC);
        ops = INC_OP;
    } else {
        match(Token::DEC);
        ops = DEC_OP;
    }
    inc_dec_stmt->op = ops;
    return inc_dec_stmt;
}


Assigment *Parser::parseAssigment(ExpList* el) {
    auto assignment = new Assigment();
    assignment->expresion_list_id = el;
    AssignOp ops;
    if (check(Token::PLUSASSIGN)) {
        match(Token::PLUSASSIGN);
        ops = PLUS_ASSIGN;
    } else if (check(Token::NEGASSIGN)) {
        match(Token::NEGASSIGN);
        ops = NEG_ASSIGN;
    } else if (check(Token::MULASSIGN)) {
        match(Token::MULASSIGN);
        ops = MUL_ASSIGN;
    } else {
        match(Token::DIVASSIGN);
        ops = DIV_ASSIGN;
    }
    assignment->op = ops;
    assignment->expresion_list_values = parseExpList();
    return assignment;
}

ReturnStmt *Parser::parseReturnStmt() {
    match(Token::RETURN);
    auto return_stmt = new ReturnStmt();
    if (!check(Token::PCOMMA) && !check(Token::RLLAVE)) {
        return_stmt->expresion_list = parseExpList();
    } else {
        return_stmt->expresion_list = nullptr;
    }
    return return_stmt;
}

BreakStmt *Parser::parseBreakStmt() {
    match(Token::BREAK);
    return new BreakStmt();
}

ContinueStmt *Parser::parseContinueStmt() {
    match(Token::CONTINUE);
    return new ContinueStmt();
}

IfStmt *Parser::parseIfStmt() {
    match(Token::IF);
    auto if_stmt = new IfStmt();
    if_stmt->expresion = parseExp();
    if_stmt->cuerpo_if = parseBlock();
    if (check(Token::ELSE)) {
        match(Token::ELSE);
        if (check(Token::LLLAVE)) {
            if_stmt->cuerpo_else = parseBlock();
            if_stmt->cuerpo_if = nullptr;
        } else {
            if_stmt->cuerpo_else = nullptr;
            if_stmt->if_anidado = parseIfStmt();
        }
    } else {
        if_stmt->cuerpo_else = nullptr;
        if_stmt->if_anidado  = nullptr;
    }
    return if_stmt;
}

SwitchStmt *Parser::parseSwitchStmt() {
    match(Token::SWITCH);
    auto switch_stmt =  new SwitchStmt();
    if (!check(Token::LLLAVE)) {
        switch_stmt->expresion = parseExp();
    } else {
        switch_stmt->expresion = nullptr;
    }
    match(Token::LLLAVE);
    while (!check(Token::RLLAVE)) {
        switch_stmt->exp_case_clause.push_back(parseExpCaseClause());
    }
    match(Token::RLLAVE);
    return switch_stmt;
}

ExpCaseClause *Parser::parseExpCaseClause() {
    auto exp_case_clause = new ExpCaseClause();
    if (check(Token::CASE)) {
        match(Token::CASE);
        exp_case_clause->expresion_list = parseExpList();
        match(Token::DOS_PUNTOS);
        exp_case_clause->statement_list = parseStmtList();
    } else {
        match(Token::DEFAULT);
        exp_case_clause->expresion_list = nullptr;
        exp_case_clause->statement_list = parseStmtList();
    }
    return exp_case_clause;
}

ForStmt *Parser::parseForStmt() {
    match(Token::FOR);
    auto for_stmt = new ForStmt();
    if (check(Token::LLLAVE)) {
        for_stmt->block = parseBlock();
        return for_stmt;
    }

    // falta el or ese

    for_stmt->block = parseBlock();
    return for_stmt;
}

ForClause *Parser::parseForClause() {
    // falta coso
}

// ----------------------------------------------------------------------
// Parte Nico: Expresion
// ----------------------------------------------------------------------

Exp *Parser::parseExp() {

}
