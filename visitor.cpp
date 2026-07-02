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
Semantic_types BasicType::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types ArrayType::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types StructType::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types PointerType::accept(Visitor *visitor) { return visitor->visit(this); }
void ParameterList::accept(Visitor *visitor) { visitor->visit(this); }
void ParameterDecl::accept(Visitor *visitor) { visitor->visit(this); }

// IdentifierList no tiene visit() propio en Visitor: es una lista auxiliar
// de nombres sin un "tipo" propio que calcular (los nodos que la contienen,
// como VarSpec/ParameterDecl/FieldDecl, son quienes le dan tipo a cada id).
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
Semantic_types BinaryExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types UnaryExprExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types ParenExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types OperandNameExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types BasicLitExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types CompositeLitExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types FunctionLit::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types SelectorExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types IndexExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types SliceExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types TypeAssertionExp::accept(Visitor *visitor) { return visitor->visit(this); }
Semantic_types ArgumentsExp::accept(Visitor *visitor) { return visitor->visit(this); }
