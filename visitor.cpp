#include <iostream>
#include <fstream>
#include <cmath>
#include "ast.h"
#include "visitor.h"
#include "Semantic_types.h"
using namespace std;
// -------------------------- Metodos Accept ----------------------------------
// Parte rayhan //


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

// -----------------------------------------------------------------------------
