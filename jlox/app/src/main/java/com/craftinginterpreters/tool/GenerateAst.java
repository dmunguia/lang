package com.craftinginterpreters.tool;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;

import static java.lang.String.format;

public class GenerateAst {

    public static final int EX_USAGE = 64;

    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Usage: generate_ast <output directory>");
            System.exit(EX_USAGE);
        }

        String outputDirectory = args[0];
        defineAst(outputDirectory, "Expr", Arrays.asList(
                "Assign   : Token identifier, Expr value",
                "Binary   : Expr left, Token operator, Expr right",
                "Grouping : Expr expression",
                "Literal  : Object value",
                "Logical  : Expr left, Token operator, Expr right",
                "Unary    : Token operator, Expr right",
                "Variable : Token identifier"
        ));

        defineAst(outputDirectory, "Stmt", Arrays.asList(
                "Block      : List<Stmt> statements",
                "If         : Expr condition, Stmt thenBranch, Stmt elseBranch",
                "While      : Expr condition, Stmt body",
                "Expression : Expr expression",
                "Print      : Expr expression",
                "Var        : Token identifier, Expr initializer"
        ));
    }

    private static void defineAst(String outputDirectory, String baseName, List<String> types) {
        String path = outputDirectory + File.separator + baseName + ".java";
        try (PrintWriter writer = new PrintWriter(path, StandardCharsets.UTF_8)) {
            writer.println("package com.craftinginterpreters.lox;");
            writer.println();
            writer.println("import java.util.List;");
            writer.println();
            writer.println(format("abstract class %s {", baseName));

            defineVisitor(writer, baseName, types);

            for (String type : types) {
                String[] typeParts = type.split(":");
                String className = typeParts[0].trim();
                String fields = typeParts[1].trim();
                defineType(writer, baseName, className, fields);
            }

            writer.println();
            writer.println("    abstract <R> R accept(Visitor<R> visitor);");

            writer.println("}");
        } catch (Exception e) {
            throw new RuntimeException(e);
        }

    }

    private static void defineVisitor(PrintWriter writer, String baseName, List<String> types) {
        writer.println("    interface Visitor<R> {");

        for (String type : types) {
            String typeName = type.split(":")[0].trim();
            writer.println(format("        R visit%s%s(%s %s);",
                    typeName, baseName, typeName, baseName.toLowerCase()));
        }

        writer.println("    }");
    }

    private static void defineType(PrintWriter writer, String baseName, String className, String fields) {
        String[] fieldList = fields.split(",");

        writer.println(format("    static class %s extends %s {", className, baseName));

        for (String field : fieldList) {
            writer.println(format("        final %s;", field.trim()));
        }

        writer.println(format("        %s(%s) {", className, fields));

        for (String field : fieldList) {
            String name = field.trim().split(" ")[1];
            writer.println(format("            this.%s = %s;", name, name));
        }

        writer.println("        }");

        writer.println("        @Override");
        writer.println("        <R> R accept(Visitor<R> visitor) {");
        writer.println(format("            return visitor.visit%s%s(this);",
                className, baseName));
        writer.println("        }");

        writer.println("    }");

    }
}
