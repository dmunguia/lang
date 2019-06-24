package edu.tec.ic4700.scheme

class SymbolTable {

    private static Map<String, FuncDef> table = [:]

    static FuncDef get(String symbol) { table[symbol] }
    static void set(String symbol, FuncDef expr) { table[symbol] = expr }
}
