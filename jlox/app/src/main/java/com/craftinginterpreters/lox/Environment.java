package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.Map;

public class Environment {
    final Environment parent;
    private final Map<String, Object> values = new HashMap<>();

    public Environment() {
        this.parent = null;
    }

    public Environment(final Environment parent) {
        this.parent = parent;
    }

    void define(String identifier, Object value) {
        values.put(identifier, value);
    }

    Object get(Token identifier) {
        if (values.containsKey(identifier.lexeme())) {
            return values.get(identifier.lexeme());
        } else if (parent != null) {
            return parent.get(identifier);
        } else {
            throw new RuntimeError(identifier,
                    "Undefined variable '" + identifier.lexeme() + "'.");
        }
    }

    void assign(final Token identifier, final Object value) {
        if (values.containsKey(identifier.lexeme())) {
            values.put(identifier.lexeme(), value);
        } else if (parent != null) {
            parent.assign(identifier, value);
        } else {
            throw new RuntimeError(identifier,
                    "Undefined variable '" + identifier.lexeme() + "'.");
        }
    }
}
