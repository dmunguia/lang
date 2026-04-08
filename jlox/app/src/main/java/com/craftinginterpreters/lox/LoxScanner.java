package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.craftinginterpreters.lox.TokenType.*;

public class LoxScanner {
    private static final Map<String, TokenType> keywords;

    static {
        keywords = Map.ofEntries(
                Map.entry("and", AND),
                Map.entry("class", CLASS),
                Map.entry("else", ELSE),
                Map.entry("false", FALSE),
                Map.entry("for", FOR),
                Map.entry("fun", FUN),
                Map.entry("if", IF),
                Map.entry("nil", NIL),
                Map.entry("or", OR),
                Map.entry("print", PRINT),
                Map.entry("return", RETURN),
                Map.entry("super", SUPER),
                Map.entry("this", THIS),
                Map.entry("true", TRUE),
                Map.entry("var", VAR),
                Map.entry("while", WHILE)
        );
    }

    private final String source;
    private final List<Token> tokens = new ArrayList<>();
    private int start;
    private int current;
    private int line;

    public LoxScanner(String source) {
        this.source = source;
        this.start = 0;
        this.current = 0;
        this.line = 1;
    }

    private boolean done() {
        return this.current >= this.source.length();
    }

    public List<Token> tokens() {
        while (!done()) {
            this.start = this.current;
            scan();
        }

        this.tokens.add(new Token(EOF, "", null, line));
        return this.tokens;
    }

    private void scan() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(LEFT_PAREN);
                break;
            case ')':
                addToken(RIGHT_PAREN);
                break;
            case '{':
                addToken(LEFT_BRACE);
                break;
            case '}':
                addToken(RIGHT_BRACE);
                break;
            case ',':
                addToken(COMMA);
                break;
            case '.':
                addToken(DOT);
                break;
            case '-':
                addToken(MINUS);
                break;
            case '+':
                addToken(PLUS);
                break;
            case ';':
                addToken(SEMICOLON);
                break;
            case '*':
                addToken(STAR);
                break;

            case '!':
                addToken(match('=') ? BANG_EQUAL : BANG);
                break;
            case '=':
                addToken(match('=') ? EQUAL_EQUAL : EQUAL);
                break;
            case '<':
                addToken(match('=') ? LESS_EQUAL : LESS);
                break;
            case '>':
                addToken(match('=') ? GREATER_EQUAL : GREATER);
                break;

            case '/':
                if (match('/')) {
                    // consume line comment
                    while (peek() != '\n' && !done()) {
                        advance();
                    }
                } else {
                    addToken(SLASH);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
                // ignore whitespace
                break;

            case '\n':
                this.line++;
                break;

            case '"':
                string();
                break;

            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    Lox.error(this.line, "Unexpected character %s".formatted(c));
                }
                break;

        }
    }

    private void identifier() {
        while (isAlphanumeric(peek())) {
            advance();
        }

        String value = this.source.substring(this.start, this.current);
        TokenType type = LoxScanner.keywords.getOrDefault(value, IDENTIFIER);
        addToken(type, value);
    }

    private boolean isAlphanumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    private void number() {
        while (isDigit(peek())) {
            advance();
        }

        if (peek() == '.' && isDigit(lookahead(1))) {
            advance();
            while (isDigit(peek())) {
                advance();
            }
        }

        addToken(NUMBER, Double.parseDouble(
                this.source.substring(this.start, this.current))
        );
    }

    private char lookahead(int offset) {
        if (this.current + offset >= this.source.length()) {
            return '\0';
        }

        return this.source.charAt(this.current + offset);
    }

    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    private void string() {
        while (peek() != '"' && !done()) {
            if (peek() == '\n') {
                line++;
            }

            advance();
        }

        if (done()) {
            Lox.error(line, "Unterminated string.");
            return;
        }

        advance();

        String value = this.source.substring(this.start + 1, this.current - 1);
        addToken(STRING, value);
    }

    private char peek() {
        return lookahead(0);
    }

    private boolean match(char expected) {
        if (done()) {
            return false;
        }
        if (this.source.charAt(this.current) != expected) {
            return false;
        }

        this.current++;
        return true;
    }

    private char advance() {
        return this.source.charAt(this.current++);
    }

    private void addToken(TokenType type) {
        addToken(type, null);
    }

    private void addToken(TokenType type, Object literal) {
        String text = this.source.substring(this.start, this.current);
        tokens.add(new Token(type, text, literal, line));
    }
}
