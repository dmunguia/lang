package edu.tec.ic4700.scheme

import edu.tec.ic4700.scheme.expr.Expr

class StackFrame {
    Map<String, Expr> locals = [:]

    @Override
    String toString() {
        return locals.toString()
    }
}

