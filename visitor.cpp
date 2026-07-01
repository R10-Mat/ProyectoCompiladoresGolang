#include <iostream>
#include "ast.h"
#include "visitor.h"
#include "Semantic_types.h"
using namespace std;
// -------------------------- Metodos Accept ----------------------------------
// Parte rayhan //
void Programa::accept(Visitor *visitor) { visitor->visit(this); }
void FunctionDecl::accept(Visitor *visitor) { visitor->visit(this); }
void MethodDecl::accept(Visitor *visitor) { visitor->visit(this); }
void ConstDecl::accept(Visitor *visitor) { visitor->visit(this); }
void ConstSpec::accept(Visitor *visitor) { visitor->visit(this); }
void TypeDecl::accept(Visitor *visitor) { visitor->visit(this); }
void TypeSpec::accept(Visitor *visitor) { visitor->visit(this); }
void VarDecl::accept(Visitor *visitor) { visitor->visit(this); }
void VarSpec::accept(Visitor *visitor) { visitor->visit(this); }
void FieldDecl::accept(Visitor *visitor) { visitor->visit(this); }
void ArrayType::accept(Visitor *visitor) { visitor->visit(this); }
void StructType::accept(Visitor *visitor) { visitor->visit(this); }
void PointerType::accept(Visitor *visitor) { visitor->visit(this); }
void ParameterList::accept(Visitor *visitor) { visitor->visit(this); }
void ParameterDecl::accept(Visitor *visitor) { visitor->visit(this); }

// BasicType e IdentifierList no tienen visit() propio en Visitor: BasicType
// representa un nombre de tipo (builtin o definido por el usuario) cuya
// resolucion aun no esta conectada al TypeChecker, e IdentifierList es una
// lista auxiliar de nombres sin un "tipo" que calcular. Quedan sin operacion
// hasta que se agregue su soporte en visitor.h.
void BasicType::accept(Visitor *visitor) { (void)visitor; }
void IdentifierList::accept(Visitor *visitor) { (void)visitor; }

// ExpList tampoco tiene visit() propio: el "tipo" de una lista de expresiones
// no es un concepto unico, asi que se resuelve visitando cada expresion y
// devolviendo el tipo de la ultima (usado por ReturnStmt con un solo valor).
Semantic_types ExpList::accept(Visitor *visitor) {
    Semantic_types tipo;
    for (auto exp : lista_exp) tipo = exp->accept(visitor);
    return tipo;
}

// Parte bruno //
Semantic_types Block::accept(Visitor *visitor)    {return visitor->visit(this);}
Semantic_types StmtList::accept(Visitor *visitor) { return visitor->visit(this);}
Semantic_types DeclarationStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types BlockStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types ExpresionStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types IncDecStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types Assigment::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types ReturnStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types BreakStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types ContinueStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types IfStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types SwitchStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types ExpCaseClause::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types ForStmt::accept(Visitor *visitor) {return visitor->visit(this);}
Semantic_types ForClause::accept(Visitor *visitor) {return visitor->visit(this);}
// Parte Nico //
// TODO: visitor.h todavia no declara visit(...) para las expresiones de
// Nico (BinaryExp, UnaryExprExp, OperandNameExp, BasicLitExp, etc). Estos
// cuerpos son stubs temporales solo para que el proyecto linkee; cuando se
// agreguen los visit() correspondientes en visitor.h/TypeCheker.cpp hay que
// reemplazarlos por "return visitor->visit(this);" como en Parte bruno.
Semantic_types BinaryExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types UnaryExprExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types ParenExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types OperandNameExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types BasicLitExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types CompositeLitExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types FunctionLit::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types SelectorExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types IndexExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types SliceExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types TypeAssertionExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }
Semantic_types ArgumentsExp::accept(Visitor *visitor) { (void)visitor; return Semantic_types(); }

// -----------------------------------------------------------------------------
