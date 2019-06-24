package edu.tec.ic4700.scheme

class Stack {
    private static List<StackFrame> stack = []

    static StackFrame top() { stack.last() }
    static void push(StackFrame frame) { stack.push(frame) }
    static StackFrame pop() { stack.pop() }
}
