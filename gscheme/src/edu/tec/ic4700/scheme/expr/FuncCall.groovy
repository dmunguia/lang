package edu.tec.ic4700.scheme.expr

import edu.tec.ic4700.scheme.FuncDef
import edu.tec.ic4700.scheme.Stack
import edu.tec.ic4700.scheme.StackFrame
import edu.tec.ic4700.scheme.SymbolTable
import edu.tec.ic4700.scheme.expr.literal.Literal

class FuncCall implements Expr {

    String name
    List<Expr> params

    @Override
    Literal eval() {
        FuncDef funcDef = SymbolTable.get(name)
        StackFrame frame = new StackFrame()
        funcDef.params.eachWithIndex { paramName, index ->
            frame.locals[paramName] = params[index].eval()
        }

        Stack.push(frame)
        Literal val = funcDef.expr.eval()
        Stack.pop()

        return val
    }
}
