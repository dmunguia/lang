package edu.tec.ic4700.scheme.expr.literal

class IntLiteral extends Literal {
    int intValue

    IntLiteral(int intValue) {
        this.type = DataType.INT
        this.intValue = intValue
    }

    @Override
    def value() { intValue }

    @Override
    Literal eval() { this }

    @Override
    String toString() { "int: ${intValue}" }
}
