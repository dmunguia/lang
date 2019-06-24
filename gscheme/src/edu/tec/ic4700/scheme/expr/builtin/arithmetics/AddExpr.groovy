package edu.tec.ic4700.scheme.expr.builtin.arithmetics

import edu.tec.ic4700.scheme.expr.builtin.BinaryExpr
import edu.tec.ic4700.scheme.expr.literal.IntLiteral
import edu.tec.ic4700.scheme.expr.literal.Literal

class AddExpr extends BinaryExpr {
    @Override
    Literal eval() { new IntLiteral(a.eval().value() + b.eval().value()) }
}
