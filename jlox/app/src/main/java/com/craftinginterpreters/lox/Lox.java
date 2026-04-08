package com.craftinginterpreters.lox;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.MessageFormat;
import java.util.List;
import java.util.Scanner;

public class Lox {

    private static final int EX_USAGE = 64;
    private static final int EX_DATAERR = 65;
    private static final int EX_SOFTWARE = 70;

    private static final Interpreter INTERPRETER = new Interpreter();

    private static boolean hadError = false;
    private static boolean hadRuntimeError = false;

    public static void main(String[] args) {
        try {
            if (args.length > 1) {
                System.out.println("Usage: jlox [script]");
                System.exit(EX_USAGE);
            }

            Lox lox = new Lox();
            if (args.length == 1) {
                lox.runFile(args[0]);
            } else {
                lox.runPrompt();
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(EX_SOFTWARE);
        }
    }

    static void runtimeError(RuntimeError error) {
        System.err.println(error.getMessage() +
                "\n[line " + error.token.line() + "]");
        hadRuntimeError = true;
    }

    private void runPrompt() {
        Scanner scanner = new Scanner(System.in);
        while (scanner.hasNextLine()) {
            System.out.print("> ");
            String line = scanner.nextLine();
            run(line);
            hadError = false;
        }
    }

    private void runFile(String path) {
        try {
            String source = Files.readString(Paths.get(path));
            run(source);
            if (hadError) {
                System.exit(EX_DATAERR);
            }

            if (hadRuntimeError) {
                System.exit(EX_SOFTWARE);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private void run(String source) {
        LoxScanner scanner = new LoxScanner(source);
        List<Token> tokens = scanner.tokens();
        Parser parser = new Parser(tokens);
        final List<Stmt> statements = parser.parse();

        if (hadError) {
            return;
        }

        // System.out.println(new AstPrinter().print(expression));
        INTERPRETER.interpret(statements);
    }

    static void error(int line, String message) {
        report(line, "", message);
    }

    private static void report(int line, String where, String message) {
        System.err.println(MessageFormat.format("[line {0}] Error {1}: {2}",
                line, where, message));

        hadError = true;
    }

    static void error(Token token, String message) {
        if (token.type() == TokenType.EOF) {
            report(token.line(), " at end", message);
        } else {
            report(token.line(), " at '" + token.lexeme() + "'", message);
        }
    }
}
