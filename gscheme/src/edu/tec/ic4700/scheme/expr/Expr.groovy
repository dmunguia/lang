package edu.tec.ic4700.scheme.expr

import edu.tec.ic4700.scheme.expr.literal.Literal

interface Expr {
    Literal eval()
}
