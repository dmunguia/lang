package com.craftinginterpreters.lox;

import java.util.List;

public class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {

    private Environment environment = new Environment();

    void interpret(List<Stmt> statements) {
        try {
            for (Stmt statement : statements) {
                execute(statement);
            }
        } catch (RuntimeError error) {
            Lox.runtimeError(error);
        }
    }

    private void execute(final Stmt statement) {
        statement.accept(this);
    }

    void executeBlock(final List<Stmt> statements, final Environment environment) {
        Environment restore = this.environment;

        try {
            this.environment = environment;

            for (Stmt statement : statements) {
                execute(statement);
            }
        } finally {
            this.environment = restore;
        }
    }

    private String stringify(Object object) {
        if (object == null) {
            return "nil";
        }

        if (object instanceof Double) {
            String text = object.toString();
            if (text.endsWith(".0")) {
                text = text.substring(0, text.length() - 2);
            }

            return text;
        }

        return object.toString();
    }

    private Object evaluate(Expr expression) {
        return expression.accept(this);
    }

    private boolean isTruthy(Object object) {
        if (object == null) {
            return false;
        }

        if (object instanceof Boolean) {
            return (Boolean) object;
        }

        return true;
    }

    private boolean isEqual(Object a, Object b) {
        if (a == null && b == null) {
            return true;
        }
        if (a == null) {
            return false;
        }

        return a.equals(b);
    }

    private void checkNumericOperand(Token operator, Object operand) {
        if (operand instanceof Double) {
            return;
        }

        throw new RuntimeError(operator, "Operand must be a number.");
    }

    private void checkNumericOperands(Token operator,
                                      Object left,
                                      Object right) {
        if (left instanceof Double && right instanceof Double) {
            return;
        }

        throw new RuntimeError(operator, "Operands must be numbers.");
    }

    @Override
    public Object visitAssignExpr(final Expr.Assign expr) {
        Object value = evaluate(expr.value);
        environment.assign(expr.identifier, value);
        return value;
    }

    @Override
    public Object visitBinaryExpr(Expr.Binary expr) {
        Object left = evaluate(expr.left);
        Object right = evaluate(expr.right);

        switch (expr.operator.type()) {
            case BANG_EQUAL:
                return !isEqual(left, right);
            case EQUAL_EQUAL:
                return isEqual(left, right);
            case GREATER:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) > ((Double) right);
            case GREATER_EQUAL:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) >= ((Double) right);
            case LESS:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) < ((Double) right);
            case LESS_EQUAL:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) <= ((Double) right);
            case MINUS:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) - ((Double) right);
            case SLASH:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) / ((Double) right);
            case STAR:
                checkNumericOperands(expr.operator, left, right);
                return ((Double) left) * ((Double) right);
            case PLUS:
                if (left instanceof Double && right instanceof Double) {
                    return ((Double) left) + ((Double) right);
                }
                if (left instanceof String && right instanceof String) {
                    return ((String) left) + ((String) right);
                }

                throw new RuntimeError(expr.operator,
                        "Operands must be two numbers or two strings.");
        }

        return null;
    }

    @Override
    public Object visitGroupingExpr(Expr.Grouping expr) {
        return evaluate(expr.expression);
    }

    @Override
    public Object visitLiteralExpr(Expr.Literal expr) {
        return expr.value;
    }

    @Override
    public Object visitLogicalExpr(final Expr.Logical expr) {
        Object left = evaluate(expr.left);

        if (expr.operator.type() == TokenType.OR) {
            if (isTruthy(left)) {
                return left;
            }
        } else if (expr.operator.type() == TokenType.AND) {
            if (!isTruthy(left)) {
                return left;
            }
        }

        return evaluate(expr.right);
    }

    @Override
    public Object visitUnaryExpr(Expr.Unary expr) {
        Object right = evaluate(expr.right);

        switch (expr.operator.type()) {
            case BANG:
                return !isTruthy(right);
            case MINUS:
                checkNumericOperand(expr.operator, right);
                return -((Double) right);
        }

        return null;
    }

    @Override
    public Object visitVariableExpr(final Expr.Variable expr) {
        return this.environment.get(expr.identifier);
    }

    @Override
    public Void visitBlockStmt(final Stmt.Block stmt) {
        executeBlock(stmt.statements, new Environment(this.environment));
        return null;
    }

    @Override
    public Void visitIfStmt(final Stmt.If stmt) {
        Object condition = evaluate(stmt.condition);
        if (isTruthy(condition)) {
            execute(stmt.thenBranch);
        } else if (stmt.elseBranch != null) {
            execute(stmt.elseBranch);
        }

        return null;
    }

    @Override
    public Void visitWhileStmt(final Stmt.While stmt) {
        while (isTruthy(evaluate(stmt.condition))) {
            execute(stmt.body);
        }

        return null;
    }

    @Override
    public Void visitExpressionStmt(final Stmt.Expression stmt) {
        evaluate(stmt.expression);
        return null;
    }

    @Override
    public Void visitPrintStmt(final Stmt.Print stmt) {
        Object value = evaluate(stmt.expression);
        System.out.println(stringify(value));
        return null;
    }

    @Override
    public Void visitVarStmt(final Stmt.Var stmt) {
        Object value = null;
        if (stmt.initializer != null) {
            value = evaluate(stmt.initializer);
        }

        environment.define(stmt.identifier.lexeme(), value);
        return null;
    }
}
