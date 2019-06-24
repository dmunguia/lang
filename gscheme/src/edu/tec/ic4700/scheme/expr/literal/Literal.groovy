package edu.tec.ic4700.scheme.expr.literal

import edu.tec.ic4700.scheme.expr.Expr

enum DataType { INT, BOOLEAN }

abstract class Literal implements Expr {
    protected DataType type
    abstract def value()
}