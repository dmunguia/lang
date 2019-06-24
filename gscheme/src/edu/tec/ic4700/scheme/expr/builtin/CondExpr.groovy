package edu.tec.ic4700.scheme.expr.builtin

import edu.tec.ic4700.scheme.expr.Expr
import edu.tec.ic4700.scheme.expr.literal.Literal

class CondExpr implements Expr {
    Expr cond, cThen, cElse

    @Override
    Literal eval() { cond.eval().value() ? cThen.eval() : cElse.eval() }
}
