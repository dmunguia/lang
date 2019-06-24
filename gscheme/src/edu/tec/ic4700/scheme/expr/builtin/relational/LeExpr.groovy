package edu.tec.ic4700.scheme.expr.builtin.relational

import edu.tec.ic4700.scheme.expr.builtin.BinaryExpr
import edu.tec.ic4700.scheme.expr.literal.BoolLiteral
import edu.tec.ic4700.scheme.expr.literal.Literal

class LeExpr extends BinaryExpr {
    @Override
    Literal eval() { new BoolLiteral(a.eval().value() <= b.eval().value()) }
}