import edu.tec.ic4700.scheme.Interpreter

Interpreter interpreter = new Interpreter()
while (true) {
    def code = System.console().readLine('scheme> ')
    println(interpreter.go(code))
    println()
}