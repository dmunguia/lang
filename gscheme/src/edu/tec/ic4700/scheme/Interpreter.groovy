package edu.tec.ic4700.scheme

import edu.tec.ic4700.scheme.expr.Expr
import edu.tec.ic4700.scheme.expr.FuncCall
import edu.tec.ic4700.scheme.expr.ParamExpr
import edu.tec.ic4700.scheme.expr.builtin.CondExpr
import edu.tec.ic4700.scheme.expr.builtin.arithmetics.*
import edu.tec.ic4700.scheme.expr.builtin.logical.AndExpr
import edu.tec.ic4700.scheme.expr.builtin.logical.NotExpr
import edu.tec.ic4700.scheme.expr.builtin.logical.OrExpr
import edu.tec.ic4700.scheme.expr.builtin.relational.*
import edu.tec.ic4700.scheme.expr.literal.BoolLiteral
import edu.tec.ic4700.scheme.expr.literal.IntLiteral
import edu.tec.ic4700.scheme.parser.Parser

class Interpreter {


    public static final String TOKEN_DEFINE = 'define'
    public static final String TOKEN_TRUE = '#t'
    public static final String TOKEN_FALSE = '#f'
    public static final String TOKEN_ADD = '+'
    public static final String TOKEN_SUB = '-'
    public static final String TOKEN_MULT = '*'
    public static final String TOKEN_QUOT = 'quotient'
    public static final String TOKEN_MOD = 'remainder'
    public static final String TOKEN_EQ = '='
    public static final String TOKEN_GE = '>='
    public static final String TOKEN_GT = '>'
    public static final String TOKEN_LE = '<='
    public static final String TOKEN_LT = '<'
    public static final String TOKEN_AND = 'and'
    public static final String TOKEN_OR = 'or'
    public static final String TOKEN_NOT = 'not'
    public static final String TOKEN_IF = 'if'

    static String go(String code) {
        List<String> tokens = Parser.go(code)

        if (tokens[0] == TOKEN_DEFINE) {
            FuncDef funcDef = processFuncDef(tokens);
            return funcDef.toString()
        } else {
            Expr expr = processExpr(tokens)
            return expr.eval().toString()
        }
    }

    private static Expr processExpr(tokens) {
        String functor = tokens instanceof List ? tokens.head() : tokens
        List<String> params = tokens instanceof List ? tokens.tail() : []

        Expr expr
        switch(functor) {
            case TOKEN_TRUE:
                expr = new BoolLiteral(true)
                break

            case TOKEN_FALSE:
                expr = new BoolLiteral(false)
                break

            case TOKEN_ADD:
                expr = new AddExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_SUB:
                expr = new SubExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_MULT:
                expr = new MultExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_QUOT:
                expr = new QuotExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_MOD:
                expr = new ModExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_EQ:
                expr = new EqExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_GE:
                expr = new GeExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_GT:
                expr = new GtExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_LE:
                expr = new LeExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_LT:
                expr = new LtExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_AND:
                expr = new AndExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_OR:
                expr = new OrExpr(
                        a: processExpr(params[0]),
                        b: processExpr(params[1])
                )
                break

            case TOKEN_NOT:
                expr = new NotExpr(
                        a: processExpr(params[0])
                )
                break

            case TOKEN_IF:
                expr = new CondExpr(
                        cond: processExpr(params[0]),
                        cThen: processExpr(params[1]),
                        cElse: processExpr(params[2])
                )
                break

            default:
                expr = functor.isInteger() ?
                        new IntLiteral(functor.toInteger()) :
                        (SymbolTable.get(functor) ?
                                new FuncCall(
                                        name: functor,
                                        params: params.collect { processExpr(it) }
                                ) :
                                new ParamExpr(id: functor)
                        )
        }

        return expr
    }

    private static FuncDef processFuncDef(List<String> tokens) {
        List<String> spec = tokens[1]
        List<String> expr = tokens[2]

        SymbolTable.set(spec.head(), new FuncDef())

        def funcDef = new FuncDef(
                name: spec.head(),
                params: spec.tail(),
                expr: processExpr(expr)
        )

        SymbolTable.set(spec.head(), funcDef)

        return funcDef

    }
}
