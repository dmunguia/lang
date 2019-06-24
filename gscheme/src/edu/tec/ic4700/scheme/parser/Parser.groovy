package edu.tec.ic4700.scheme.parser

class Parser {
    static List<String> go(String code) {
        def stack = []
        def current = null
        def tokens = code.replace('(', '( ').replace(')', ' )').split(' ')
        tokens.each { token ->
            switch (token) {
                case '(':
                    stack.push(current)
                    current = []
                    break
                case ')':
                    def top = stack.pop()
                    current = top ? top << current : current
                    break
                default:
                    current << token
            }
        }
        current
    }
}
