package edu.tec.ic4700.scheme

class Test {
    static List<String> code = [
//            '(+ 1 1)',
            '(define (fact n) (if (= n 0) 1 (* n (fact (- n 1)))))',
            '(fact (+ 4 1))'
    ]

    public static void main(String[] args) {
        code.each {
            println Interpreter.go(it)
        }
    }
}
