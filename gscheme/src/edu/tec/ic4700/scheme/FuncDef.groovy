package edu.tec.ic4700.scheme

import edu.tec.ic4700.scheme.expr.Expr

class FuncDef {
    String name
    List<String> params
    Expr expr

    @Override
    String toString() { "function: ${name}" }
}
