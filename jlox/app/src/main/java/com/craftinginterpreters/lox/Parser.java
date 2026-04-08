package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;

import static com.craftinginterpreters.lox.TokenType.*;

public class Parser {

    private static class ParserError extends RuntimeException {
    }

    private final List<Token> tokens;
    private int current = 0;

    public Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    private boolean match(TokenType... tokenTypes) {
        for (TokenType tokenType : tokenTypes) {
            if (check(tokenType)) {
                advance();
                return true;
            }
        }

        return false;
    }

    private boolean check(TokenType tokenType) {
        if (done()) {
            return false;
        }
        return peek().type() == tokenType;
    }

    private Token advance() {
        if (!done()) {
            this.current++;
        }

        return previous();
    }

    private boolean done() {
        return peek().type() == EOF;
    }

    private Token peek() {
        return this.tokens.get(this.current);
    }

    private Token previous() {
        return this.tokens.get(this.current - 1);
    }

    private Token consume(TokenType tokenType, String message) {
        if (check(tokenType)) {
            return advance();
        }

        throw error(peek(), message);
    }

    private ParserError error(Token token, String message) {
        Lox.error(token, message);
        return new ParserError();
    }

    private void synchronize() {
        advance();
        while (!done()) {
            if (previous().type() == SEMICOLON) {
                return;
            }

            switch (peek().type()) {
                case CLASS:
                case FOR:
                case FUN:
                case IF:
                case PRINT:
                case RETURN:
                case VAR:
                case WHILE:
                    return;
            }

            advance();
        }
    }

    /**
     * program -> declaration* EOF ;
     *
     * @return
     */
    List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();
        while (!done()) {
            statements.add(declaration());
        }

        return statements;
    }

    /**
     * declaration -> varDecl | statement ;
     *
     * @return
     */
    private Stmt declaration() {
        try {
            if (match(VAR)) {
                return varDeclaration();
            } else {
                return statement();
            }
        } catch (ParserError err) {
            synchronize();
            return null;
        }
    }

    /**
     * varDecl -> 'var' IDENTIFIER ( '=' expression )? ';' ;
     *
     * @return
     */
    private Stmt varDeclaration() {
        Token id = consume(IDENTIFIER, "Expected variable name.");

        Expr initializer = null;
        if (match(EQUAL)) {
            initializer = expression();
        }

        consume(SEMICOLON, "Expected ';' after variable declaration.");
        return new Stmt.Var(id, initializer);
    }


    /**
     * statement -> exprStmt | forStmt | ifStmt | printStmt | whileStmt | block ;
     *
     * @return
     */
    private Stmt statement() {
        if (match(PRINT)) {
            return printStatement();
        } else if (match(LEFT_BRACE)) {
            return block();
        } else if (match(FOR)) {
            return forStatement();
        } else if (match(IF)) {
            return ifStatement();
        } else if (match(WHILE)) {
            return whileStatement();
        } else {
            return expressionStatement();
        }
    }

    private Stmt forStatement() {
        consume(LEFT_PAREN, "Expected '(' after 'for'.");

        Stmt initializer;
        if (match(SEMICOLON)) {
            initializer = null;
        } else if (match(VAR)) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        Expr condition = null;
        if (!check(SEMICOLON)) {
            condition = expression();
        }

        consume(SEMICOLON, "Expected ';' after loop condition.");

        Expr increment = null;
        if (!check(RIGHT_PAREN)) {
            increment = expression();
        }
        consume(RIGHT_PAREN, "Expected ')' after for clauses.");

        Stmt body = statement();

        if (increment != null) {
            body = new Stmt.Block(List.of(body, new Stmt.Expression(increment)));
        }

        if (condition == null) {
            condition = new Expr.Literal(true);
        }

        body = new Stmt.While(condition, body);

        if (initializer != null) {
            body = new Stmt.Block(List.of(initializer, body));
        }

        return body;
    }

    /**
     * exprStmt -> expression ';' ;
     *
     * @return
     */
    private Stmt expressionStatement() {
        Expr expression = expression();
        consume(SEMICOLON, "Expected ';' after expression.");
        return new Stmt.Expression(expression);
    }

    /**
     * ifStmt -> 'if' '(' expression ')' statement ( 'else' statement )? ;
     *
     * @return
     */
    private Stmt ifStatement() {
        consume(LEFT_PAREN, "Expected '(' after 'if'.");
        Expr condition = expression();
        consume(RIGHT_PAREN, "Expected ')' after condition.");

        Stmt thenBranch = statement();
        Stmt elseBranch = null;
        if (match(ELSE)) {
            elseBranch = statement();
        }

        return new Stmt.If(condition, thenBranch, elseBranch);
    }

    /**
     * printStmt ->  'print' expression ';' ;
     *
     * @return
     */
    private Stmt printStatement() {
        Expr value = expression();
        consume(SEMICOLON, "Expected ';' after value.");
        return new Stmt.Print(value);
    }

    /**
     * block -> '{' declaration* '}'
     *
     * @return
     */
    private Stmt block() {
        List<Stmt> statements = new ArrayList<>();

        while (!check(RIGHT_BRACE) && !done()) {
            statements.add(declaration());
        }

        consume(RIGHT_BRACE, "Expected '}' after block.");
        return new Stmt.Block(statements);
    }

    /**
     * whileStmt -> 'while' '(' expression ')' statement ;
     *
     * @return
     */
    private Stmt whileStatement() {
        consume(LEFT_PAREN, "Expected '(' after 'while'.");
        Expr condition = expression();
        consume(RIGHT_PAREN, "Expected ')' after condition.");

        Stmt body = statement();
        return new Stmt.While(condition, body);
    }

    /**
     * expression -> assignment ;
     *
     * @return
     */
    private Expr expression() {
        return assignment();
    }

    /**
     * assignment -> IDENTIFIER '=' assignment | disjunction ;
     *
     * @return
     */
    private Expr assignment() {
        Expr expr = disjunction();

        if (match(EQUAL)) {
            Token equals = previous();
            Expr value = assignment();

            if (expr instanceof Expr.Variable) {
                Token identifier = ((Expr.Variable) expr).identifier;
                return new Expr.Assign(identifier, value);
            }

            throw error(equals, "Invalid assignment target.");
        }

        return expr;
    }

    /**
     * disjunction -> conjunction ( 'or' conjunction )* ;
     *
     * @return
     */
    private Expr disjunction() {
        Expr expr = conjunction();

        while (match(OR)) {
            Token operator = previous();
            Expr right = conjunction();
            expr = new Expr.Logical(expr, operator, right);
        }

        return expr;
    }

    /**
     * conjunction -> equality ( 'and' equality )* ;
     *
     * @return
     */
    private Expr conjunction() {
        Expr expr = equality();

        while (match(AND)) {
            Token operator = previous();
            Expr right = equality();
            expr = new Expr.Logical(expr, operator, right);
        }

        return expr;
    }

    /**
     * equality -> comparison ( ( '!=' | '==' ) comparison )* ;
     *
     * @return
     */
    private Expr equality() {
        Expr expression = comparison();

        while (match(BANG_EQUAL, EQUAL_EQUAL)) {
            Token operator = previous();
            Expr right = comparison();
            expression = new Expr.Binary(expression, operator, right);
        }

        return expression;
    }

    /**
     * comparison -> term ( ( '>' | '>=' | '<' | '<=' ) term )*
     *
     * @return
     */
    private Expr comparison() {
        Expr expression = term();

        while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
            Token operator = previous();
            Expr right = term();
            expression = new Expr.Binary(expression, operator, right);
        }

        return expression;
    }

    /**
     * term -> factor ( ( '-' | '+' ) factor )*
     *
     * @return
     */
    private Expr term() {
        Expr expression = factor();

        while (match(MINUS, PLUS)) {
            Token operator = previous();
            Expr right = factor();
            expression = new Expr.Binary(expression, operator, right);
        }

        return expression;
    }

    /**
     * factor -> unary ( ( '/' | '*' ) unary )*
     *
     * @return
     */
    private Expr factor() {
        Expr expression = unary();

        while (match(SLASH, STAR)) {
            Token operator = previous();
            Expr right = unary();
            expression = new Expr.Binary(expression, operator, right);
        }

        return expression;
    }

    /**
     * unary -> ( '!' | '-' ) unary | primary
     *
     * @return
     */
    private Expr unary() {
        if (match(BANG, MINUS)) {
            Token operator = previous();
            Expr right = unary();
            return new Expr.Unary(operator, right);
        }

        return primary();
    }

    /**
     * primary -> NUMBER | STRING | 'true' | 'false' | 'nil' | IDENTIFIER | '(' expression ')' ;
     *
     * @return
     */
    private Expr primary() {
        if (match(FALSE)) {
            return new Expr.Literal(false);
        }

        if (match(TRUE)) {
            return new Expr.Literal(true);
        }

        if (match(NIL)) {
            return new Expr.Literal(null);
        }

        if (match(NUMBER, STRING)) {
            return new Expr.Literal(previous().literal());
        }

        if (match(IDENTIFIER)) {
            return new Expr.Variable(previous());
        }

        if (match(LEFT_PAREN)) {
            Expr expression = expression();
            consume(RIGHT_PAREN, "Expected ')' after expression.");
            return new Expr.Grouping(expression);
        }

        throw error(peek(), "Expected expression.");
    }

}
