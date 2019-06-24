package edu.tec.ic4700.scheme.expr.builtin.logical

import edu.tec.ic4700.scheme.expr.builtin.UnaryExpr
import edu.tec.ic4700.scheme.expr.literal.BoolLiteral
import edu.tec.ic4700.scheme.expr.literal.Literal

class NotExpr extends UnaryExpr {
    @Override
    Literal eval() { new BoolLiteral(!a.eval().value()) }
}
