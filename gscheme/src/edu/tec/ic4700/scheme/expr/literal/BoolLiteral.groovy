package edu.tec.ic4700.scheme.expr.literal

class BoolLiteral extends Literal {
    boolean boolValue

    BoolLiteral(boolean boolValue) {
        this.type = DataType.BOOLEAN
        this.boolValue = boolValue
    }

    @Override
    def value() { boolValue }

    @Override
    Literal eval() { this }

    @Override
    String toString() { "bool: ${boolValue}" }
}
