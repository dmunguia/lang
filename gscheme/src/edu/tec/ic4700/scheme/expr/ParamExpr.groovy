package edu.tec.ic4700.scheme.expr

import edu.tec.ic4700.scheme.Stack
import edu.tec.ic4700.scheme.expr.literal.Literal

class ParamExpr implements Expr {

    String id

    @Override
    Literal eval() { Stack.top().locals[id].eval() }
}
