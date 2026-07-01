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

void Parser::error(const std::string &expected) {
  std::string found;
  if (isAtEnd()) {
    found = "fin de entrada";
  } else {
    found = Token::typeName(current->type);
    if (!current->text.empty())
      found += " '" + current->text + "'";
  }
  throw std::runtime_error("Error sintáctico: se esperaba " + expected +
                           ", pero se encontró " + found);
}

void Parser::expect(Token::Type ttype) {
  if (!match(ttype))
    error(Token::typeName(ttype));
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
    // Program = { TopLevelDecl ";" }  -> cero o mas, cada uno terminado en ";"
    auto* program = new Programa();
    while (!isAtEnd()){
        program->listatopleveldecl.push_back(parseTopLevelDecl());
        expect(Token::PCOMMA);
    }
    cout << "Parseo exitoso" << endl;
    return program;
}

TopLevelDecl* Parser::parseTopLevelDecl(){
    if (check(Token::CONST) || check(Token::TYPE) || check(Token::VAR)){
        return parseDeclaration();
    }
    if (match(Token::FUNC)){
        if (match(Token::LPAREN)){
            return parseMethodDecl();
        }
        return parseFunctionDecl();
    }
    error("'const', 'type', 'var' o 'func' para iniciar una declaracion de nivel superior");
    return nullptr; // inalcanzable: error() siempre lanza una excepcion
}

Declaration* Parser::parseDeclaration(){
    if (match(Token::CONST)){
        return parseConstDecl();
    }
    if (match(Token::TYPE)){
        return parseTypeDecl();
    }
    if (match(Token::VAR)){
        return parseVarDecl();
    }
    error("'const', 'type' o 'var'");
    return nullptr; // inalcanzable: error() siempre lanza una excepcion
}

ConstDecl* Parser::parseConstDecl(){
    // "const" ( ConstSpec | "(" { ConstSpec ";" } ")" )
    auto* decl = new ConstDecl();
    if (match(Token::LPAREN)){
        while (!check(Token::RPAREN) && !isAtEnd()) {
            decl->constspecList.push_back(parseConstSpec());
            expect(Token::PCOMMA);
        }
        expect(Token::RPAREN);
    }else{
        decl->constspecList.push_back(parseConstSpec());
    }
    return decl;
}
ConstSpec* Parser::parseConstSpec(){
    // ConstSpec = IdentifierList [ Type ] "=" ExpressionList  ("=" es obligatorio)
    auto constspec = new ConstSpec();
    constspec->identifierList = parseIdentifierList();
    constspec->tipo = parseType();
    expect(Token::ASSIGN);
    constspec->expresionlist = parseExpList();
    return constspec;
}

TypeDecl* Parser::parseTypeDecl(){
    // "type" ( TypeSpec | "(" { TypeSpec ";" } ")" )
    auto typedecl = new TypeDecl();
    if (match(Token::LPAREN)){
        while (!check(Token::RPAREN) && !isAtEnd()) {
            typedecl->typespecList.push_back(parseTypeSpec());
            expect(Token::PCOMMA);
        }
        expect(Token::RPAREN);
    }else{
        typedecl->typespecList.push_back(parseTypeSpec());
    }
    return typedecl;
}

TypeSpec* Parser::parseTypeSpec(){
    // TypeSpec = id Type  (ambos son obligatorios)
    TypeSpec* typespec = new TypeSpec();
    if (!match(Token::ID)){
        error("un identificador para el nombre del tipo");
    }
    typespec->id = previous->text;
    typespec->tipo = parseType();
    if (!typespec->tipo){
        error("un tipo despues del identificador en la declaracion de tipo");
    }
    return typespec;
}
VarDecl* Parser::parseVarDecl(){
    // "var" ( VarSpec | "(" { VarSpec ";" } ")" )
    auto varDecl = new VarDecl();
    if (match(Token::LPAREN)){
        while (!check(Token::RPAREN) && !isAtEnd()) {
            varDecl->varspecList.push_back(parseVarSpec());
            expect(Token::PCOMMA);
        }
        expect(Token::RPAREN);
    }else {
        varDecl->varspecList.push_back(parseVarSpec());
    }
    return varDecl;
}

VarSpec* Parser::parseVarSpec(){
    // VarSpec = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList )
    auto varSpec = new VarSpec();
    varSpec->identifierlist = parseIdentifierList();
    varSpec->tipo = parseType();
    if (match(Token::ASSIGN)) {
        varSpec->expresionlist = parseExpList();
    } else {
        varSpec->expresionlist = nullptr;
        if (!varSpec->tipo) {
            error("un tipo o '=' seguido de una lista de expresiones en la declaracion de variable");
        }
    }
    return varSpec;
}

IdentifierList* Parser::parseIdentifierList(){
    // IdentifierList = id { "," id }  (siempre debe empezar con un id)
    auto identifierlist = new IdentifierList();
    if (!match(Token::ID)) {
        error("un identificador");
    }
    identifierlist->lista_ids.push_back(previous->text);
    while (match(Token::COMMA)) {
        if (!match(Token::ID)) {
            error("un identificador despues de ','");
        }
        identifierlist->lista_ids.push_back(previous->text);
    }
    return identifierlist;
}

MethodDecl* Parser::parseMethodDecl(){
    // "func" "(" id [ "*" ] id ")" id "(" [ ParameterList ] ")" [ Type ] Block
    // El "(" inicial ya fue consumido por parseTopLevelDecl.
    auto* methodDecl = new MethodDecl();
    expect(Token::ID);
    methodDecl->nombreId = previous->text;
    methodDecl->puntero = match(Token::MUL);
    expect(Token::ID);
    methodDecl->NombreTipoBase = previous->text;
    expect(Token::RPAREN);
    expect(Token::ID);
    methodDecl->nombreMethod = previous->text;
    expect(Token::LPAREN);
    methodDecl->lista_de_parametros = parseParameterList();
    expect(Token::RPAREN);
    methodDecl->tipo = parseType();
    methodDecl->cuerpo = parseBlock();
    return methodDecl;
}

FunctionDecl* Parser::parseFunctionDecl(){
    // "func" id "(" [ ParameterList ] ")" [ Type ] Block
    auto* functionDecl = new FunctionDecl();
    expect(Token::ID);
    functionDecl->name = previous->text;
    expect(Token::LPAREN);
    functionDecl->lista_de_parametros = parseParameterList();
    expect(Token::RPAREN);
    functionDecl->tipo = parseType();
    functionDecl->cuerpo = parseBlock();
    return functionDecl;
}
Type* Parser::parseType(){
    // Type = id | BasicType | ArrayType | StructType | PointerType
    // Devuelve nullptr cuando el token actual no puede iniciar un Type: es
    // valido, ya que Type es opcional en varios contextos (ConstSpec,
    // VarSpec, FunctionDecl, MethodDecl); el que llama decide si es un error.
    if (match(Token::ID)) {
        // "int"/"float64"/"bool"/"string" no son palabras clave: llegan
        // como ID igual que cualquier nombre de tipo definido por el usuario.
        return new BasicType(previous->text);
    }

    if (match(Token::STRUCT)){
        // StructType = "struct" "{" { FieldDecl ";" } "}"  (puede ser vacio)
        StructType* st = new StructType();
        expect(Token::LLLAVE);
        while (!check(Token::RLLAVE) && !isAtEnd()) {
            st->declaraciones.push_back(parseFieldDecl());
            expect(Token::PCOMMA);
        }
        expect(Token::RLLAVE);
        return st;
    }

    if (match(Token::MUL)){
        // PointerType = "*" Type
        PointerType* pointer = new PointerType();
        pointer->basetype = parseType();
        if (!pointer->basetype) {
            error("un tipo despues de '*' en la declaracion de puntero");
        }
        return pointer;
    }

    if (match(Token::LCORCHETE)){
        // ArrayType = "[" int_lit "]" Type  ("]" cierra ANTES del tipo de elemento)
        ArrayType* array = new ArrayType();
        array->length = parseExp();
        expect(Token::RCORCHETE);
        array->elementtype = parseType();
        if (!array->elementtype) {
            error("un tipo para los elementos del arreglo");
        }
        return array;
    }

    return nullptr; // Ningun Type presente en esta posicion
}

ParameterList* Parser::parseParameterList(){
    // ParameterList = ParameterDecl { "," ParameterDecl }
    // Notar que en "(" [ ParameterList ] ")" la lista completa es opcional:
    // una lista vacia es valida (func sin parametros).
    ParameterList* lista_parametros = new ParameterList();
    if (!check(Token::ID)) {
        return lista_parametros;
    }
    lista_parametros->parameterList.push_back(parseParameterDecl());
    while (match(Token::COMMA)) {
        lista_parametros->parameterList.push_back(parseParameterDecl());
    }
    return lista_parametros;
}

ParameterDecl* Parser::parseParameterDecl(){
    // ParameterDecl = IdentifierList Type  (el tipo es obligatorio)
    ParameterDecl* parameterDecl = new ParameterDecl();
    parameterDecl->identifierlist = parseIdentifierList();
    parameterDecl->type = parseType();
    if (!parameterDecl->type) {
        error("un tipo para el parametro");
    }
    return parameterDecl;
}
FieldDecl* Parser::parseFieldDecl(){
    // FieldDecl = IdentifierList Type  (el tipo es obligatorio)
    FieldDecl* fieldDecl = new FieldDecl();
    fieldDecl->identifierlist = parseIdentifierList();
    fieldDecl->type = parseType();
    if (!fieldDecl->type) {
        error("un tipo para el campo del struct");
    }
    return fieldDecl;
}

ExpList *Parser::parseExpList() {
    auto a = new ExpList();
    a->lista_exp.push_back(parseExp());
    while (match(Token::COMMA)) {
        a->lista_exp.push_back(parseExp());
    }
    return a;
}
// ----------------------------------------------------------------------
// Parte Bruno: Blocks y Statements
// ----------------------------------------------------------------------
Block *Parser::parseBlock() {
    // Block = "{" StatementList "}"
    expect(Token::LLLAVE);
    auto bloque = new Block();
    bloque->lista_statements = parseStmtList();
    expect(Token::RLLAVE);
    return bloque;
}

StmtList *Parser::parseStmtList() {
    // StatementList = { Statement ";" }  -> cero o mas, cada uno terminado en ";"
    // Se detiene ante '}' (fin de Block) o 'case'/'default' (fin de un
    // ExprCaseClause dentro de un switch), que son los dos contextos donde
    // se invoca parseStmtList.
    auto lista_statements = new StmtList();
    while (!check(Token::RLLAVE) && !check(Token::CASE) && !check(Token::DEFAULT) && !isAtEnd()) {
        lista_statements->statements.push_back(parseStmt());
        expect(Token::PCOMMA);
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
        return parseContinueStmt();
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
        return parseIncDecStmt(expresion_list->lista_exp[0]);
    }

    if (check(Token::ASSIGN)       || check(Token::PLUSASSIGN) ||
        check(Token::NEGASSIGN)    || check(Token::MULASSIGN)  ||
        check(Token::DIVASSIGN)) {
        return parseAssigment(expresion_list);
    }

    return parseExpresionStmt(expresion_list->lista_exp[0]);

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
    } else if (check(Token::DIVASSIGN)){
        match(Token::DIVASSIGN);
        ops = DIV_ASSIGN;
    } else {
        match(Token::ASSIGN);
        ops = ASSIGN;
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
    bool restaurar = sinLiteralCompuesto;
    sinLiteralCompuesto = true;
    if_stmt->expresion = parseExp();
    sinLiteralCompuesto = restaurar;
    if_stmt->cuerpo_if = parseBlock();
    if (check(Token::ELSE)) {
        match(Token::ELSE);
        if (check(Token::LLLAVE)) {
            if_stmt->cuerpo_else = parseBlock();
            if_stmt->if_anidado = nullptr;
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
        bool restaurar = sinLiteralCompuesto;
        sinLiteralCompuesto = true;
        switch_stmt->expresion = parseExp();
        sinLiteralCompuesto = restaurar;
    } else {
        switch_stmt->expresion = nullptr;
    }
    expect(Token::LLLAVE);
    while (!check(Token::RLLAVE) && !isAtEnd()) {
        switch_stmt->exp_case_clause.push_back(parseExpCaseClause());
    }
    expect(Token::RLLAVE);
    return switch_stmt;
}

ExpCaseClause *Parser::parseExpCaseClause() {
    // ExprCaseClause = "case" ExpressionList ":" StatementList
    //                | "default" ":" StatementList
    auto exp_case_clause = new ExpCaseClause();
    if (match(Token::CASE)) {
        exp_case_clause->expresion_list = parseExpList();
        expect(Token::DOS_PUNTOS);
        exp_case_clause->statement_list = parseStmtList();
    } else {
        expect(Token::DEFAULT);
        expect(Token::DOS_PUNTOS);
        exp_case_clause->expresion_list = nullptr;
        exp_case_clause->statement_list = parseStmtList();
    }
    return exp_case_clause;
}

ForStmt *Parser::parseForStmt() {
    // ForStmt = "for" [ Expression | ForClause ] Block
    match(Token::FOR);
    auto for_stmt = new ForStmt();
    // Caso 1: for  {...}
    if (check(Token::LLLAVE)) {
        for_stmt->block = parseBlock();
        return for_stmt;
    }

    bool restaurar = sinLiteralCompuesto;
    sinLiteralCompuesto = true;

    // Caso 2: "for ; [cond]; [post] {}" con init vacio: no hay expresion que
    // parsear antes del primer ';'.
    if (check(Token::PCOMMA)) {
        for_stmt->for_clause = parseForClause(nullptr);
    } else {
        auto primera_exp = parseExp();
        // Caso 3: es un ForClause si tras la primera expresion aparece ','
        // (mas identificadores antes del '=') o el inicio de un Assignment
        // ('=' o un operador compuesto).
        if (check(Token::PCOMMA)    || check(Token::COMMA) ||
            check(Token::ASSIGN)    || check(Token::PLUSASSIGN) ||
            check(Token::NEGASSIGN) || check(Token::MULASSIGN) ||
            check(Token::DIVASSIGN)) {
            for_stmt->for_clause = parseForClause(primera_exp);
        }
        // Caso 4: Es una expresion condicional
        else {
            for_stmt->expresion = primera_exp;
        }
    }

    sinLiteralCompuesto = restaurar;
    for_stmt->block = parseBlock();
    return for_stmt;
}

ForClause *Parser::parseForClause(Exp* primera_exp) {
    // ForClause = [ Assignment ] ";" [ Expression ] ";" [ Assignment | IncDecStmt ]
    auto clause = new ForClause();
    if (!check(Token::PCOMMA)) {
        auto el1 = new ExpList();
        el1->lista_exp.push_back(primera_exp);
        while (match(Token::COMMA)) {
            el1->lista_exp.push_back(parseExp());
        }
        clause->asignacion1 = parseAssigment(el1);
    }
    expect(Token::PCOMMA);
    if (!check(Token::PCOMMA)) {
        clause->expresion = parseExp();
    }
    expect(Token::PCOMMA);
    if (!check(Token::LLLAVE)) {
        auto tercera_exp = parseExp();
        if (check(Token::INC) || check(Token::DEC)) {
            clause->inc_dec_stmt = parseIncDecStmt(tercera_exp);
        } else {
            auto el2 = new ExpList();
            el2->lista_exp.push_back(tercera_exp);
            clause->asignacion2 = parseAssigment(el2);
        }
    }
    return clause;
}

// ----------------------------------------------------------------------
// Parte Nico: Expresion
// ----------------------------------------------------------------------
Exp* Parser::parseExp() {
    Exp* left = parseAndExpr();
    while (check(Token::OR_LOGICAL)) {
        advance();
        Exp* right = parseAndExpr();
        left = new BinaryExp(left,right,OR_OP);
    }
    return left;
}

Exp* Parser::parseAndExpr(){
    Exp* left = relationParseExp();
    while (check(Token::AND_LOGICAL)) {
        advance();
        Exp* right = relationParseExp();
        left = new BinaryExp(left, right,AND_OP);
    }
    return left;
}

Exp* Parser::relationParseExp() {
    Exp* left = additiveParseExp();
    
    while (check(Token::LES) || check(Token::LEQ) || check(Token::GER) || 
           check(Token::GEQ) || check(Token::EQUAL) || check(Token::DISTINCT)) {
        
        BinaryOp op = tokenToBinaryOp(current->type);
        advance();
        Exp* right = additiveParseExp();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::additiveParseExp() {
    Exp* left = multiplicateParseExp();
    
    while (check(Token::PLUS) || check(Token::NEG) || check(Token::OR)) {
        BinaryOp op = tokenToBinaryOp(current->type);
        advance();
        Exp* right = multiplicateParseExp();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::multiplicateParseExp() {
    Exp* left = parseUnaryExpr();
    
    while (check(Token::MUL) || check(Token::DIV) || check(Token::MOD) || check(Token::AND) ) {
        BinaryOp op = tokenToBinaryOp(current->type);
        advance();
        Exp* right = parseUnaryExpr();
        left = new BinaryExp(left, right,op);
    }
    
    return left;
}


Exp* Parser::primaryParseExp() {
    Exp* base = operandParseExp();
    
    while (true) {
        if (check(Token::PUNTO)) {
            advance();
            
            if (match(Token::LPAREN)) {
                auto ta = new TypeAssertionExp();
                ta->expresion = base;
                ta->tipo = parseType();
                expect(Token::RPAREN);
                base = ta;
            }
            else if (check(Token::ID)) {
                auto sel = new SelectorExp();
                sel->expresion = base;
                sel->campo = current->text;
                advance();
                base = sel;
            }
            else {
                throw runtime_error("Se esperaba identificador o '(' despues de '.'");
            }
        }
        else if (match(Token::LCORCHETE)) {
            // Dentro de "[...]" no hay ambiguedad con el Block de un
            // if/for/switch, asi que se permite literales compuestos.
            bool restaurar = sinLiteralCompuesto;
            sinLiteralCompuesto = false;

            Exp* low = nullptr;
            Exp* high = nullptr;
            Exp* max = nullptr;

            if (!check(Token::DOS_PUNTOS)) {
                low = parseExp();
            }

            if (match(Token::DOS_PUNTOS)) {
                if (!check(Token::DOS_PUNTOS) && !check(Token::RCORCHETE)) {
                    high = parseExp();
                }
                if (match(Token::DOS_PUNTOS)) {
                    max = parseExp();
                }
                match(Token::COMMA);
                expect(Token::RCORCHETE);

                auto sl = new SliceExp(base,low,high,max);
                base = sl;
            }
            else {
                match(Token::COMMA);
                expect(Token::RCORCHETE);

                auto idx = new IndexExp();
                idx->expresion = base;
                idx->indice = low;
                base = idx;
            }
            sinLiteralCompuesto = restaurar;
        }
        else if (match(Token::LPAREN)) {
            // Dentro de "(...)" tampoco hay ambiguedad.
            bool restaurar = sinLiteralCompuesto;
            sinLiteralCompuesto = false;

            auto call = new ArgumentsExp();
            call->funcion = base;

            if (!check(Token::RPAREN)) {
                call->args = parseExpList();
                match(Token::COMMA);
            }

            expect(Token::RPAREN);
            base = call;
            sinLiteralCompuesto = restaurar;
        }
        else {
            break;
        }
    }

    return base;
}

Exp* Parser::parseUnaryExpr() {
    // UnaryExpr = unary_op UnaryExpr | PrimaryExpr
    // unary_op = "+" | "-" | "!" | "*" | "&"  ('*' es dereferencia, '&' es direccion de memoria)
    if (check(Token::PLUS) || check(Token::NEG) || check(Token::NOT) ||
        check(Token::MUL)  || check(Token::AND)) {

        UnaryOp op = tokenToUnaryOp(current->type);
        advance();
        Exp* expresion = parseUnaryExpr();

        return new UnaryExprExp(expresion,op);
    }

    return primaryParseExp();
}


Exp* Parser::operandParseExp() {
    if (match(Token::LPAREN)) {
        // Dentro de "(...)" no hay ambiguedad con el Block de un if/for/switch.
        bool restaurar = sinLiteralCompuesto;
        sinLiteralCompuesto = false;
        Exp* expresionInterna = parseExp();
        sinLiteralCompuesto = restaurar;
        expect(Token::RPAREN);

        auto parenExp = new ParenExp(expresionInterna);
        return parenExp;
    }

    if (check(Token::FUNC)) {
        match(Token::FUNC);
        return functionLiteralParseExp();
    }

    if (check(Token::INT_LIT) || check(Token::STRING_LIT) ||
        check(Token::RUNE_LIT) || check(Token::IMAGINARY_LIT) ||
        check(Token::FLOAT_LIT)) {
        return basicLiteralParseExp();
    }

    if (check(Token::ID)) {
        string nombre = current->text;
        match(Token::ID);

        // Operand = id | CompositeLit ; un id seguido de '{' es un literal
        // compuesto con un nombre de tipo (ej: Point{x: 1, y: 2}), salvo
        // dentro de la cabecera de un if/for/switch, donde esa '{' en
        // realidad abre el Block (misma ambiguedad que existe en Go real:
        // ahi el literal compuesto necesita parentesis extra).
        if (check(Token::LLLAVE) && !sinLiteralCompuesto) {
            auto compLit = new CompositeLitExp();
            compLit->tipo = new BasicType(nombre);
            compLit->elementos = parseLiteralValue();
            return compLit;
        }

        auto operandName = new OperandNameExp();
        operandName->name = nombre;
        return operandName;
    }

    // Operand = ... | CompositeLit  con un Type que no es un id (struct/array/pointer)
    if (check(Token::STRUCT) || check(Token::MUL) || check(Token::LCORCHETE)) {
        return compositeLiteralParseExp();
    }

    error("un operando (identificador, literal, '(', 'func' o un literal compuesto)");
    return nullptr; // inalcanzable: error() siempre lanza una excepcion
}


Exp* Parser::compositeLiteralParseExp() {
    auto compLit = new CompositeLitExp();
    
    compLit->tipo = parseType();
    
    compLit->elementos = parseLiteralValue();
    
    return compLit;
}


vector<KeyedElement*> Parser::parseLiteralValue() {
    expect(Token::LLLAVE);
    vector<KeyedElement*> elementos;

    // Ya estamos dentro de las llaves de un literal compuesto: la '{'
    // ambigua con el Block de un if/for/switch ya se resolvio, asi que los
    // elementos anidados pueden usar literales compuestos con id libremente.
    bool restaurar = sinLiteralCompuesto;
    sinLiteralCompuesto = false;

    while (!check(Token::RLLAVE) && !isAtEnd()) {
        auto elemento = new KeyedElement();
        
        Exp* exp1 = parseExp();
        
        if (match(Token::DOS_PUNTOS)) {
            elemento->key = exp1;
            elemento->value = parseExp();
        } else {
            elemento->key = nullptr;
            elemento->value = exp1;
        }
        
        elementos.push_back(elemento);

        if (!match(Token::COMMA)) {
            break;
        }
    }

    sinLiteralCompuesto = restaurar;
    expect(Token::RLLAVE);
    return elementos;
}


Exp* Parser::basicLiteralParseExp() {
    auto basicLit = new BasicLitExp();
    basicLit->valor = current->text;
    basicLit->tipoLiteral = current->type;
    
    if (match(Token::INT_LIT) || match(Token::STRING_LIT) ||
        match(Token::RUNE_LIT) || match(Token::IMAGINARY_LIT) ||
        match(Token::FLOAT_LIT)) {
        return basicLit;
    }

    delete basicLit;
    error("un literal (entero, flotante, cadena, rune o imaginario)");
    return nullptr; // inalcanzable: error() siempre lanza una excepcion
}


FunctionLit* Parser::functionLiteralParseExp() {
    auto funcLit = new FunctionLit();

    expect(Token::LPAREN);
    funcLit->lista_de_parametros = parseParameterList();
    expect(Token::RPAREN);

    // parseType() ya devuelve nullptr si no hay Type en esta posicion.
    funcLit->tipo = parseType();
    funcLit->cuerpo = parseBlock();

    return funcLit;
}