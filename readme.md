# BrainF*ck

[Read in the wiki](https://en.wikipedia.org/wiki/Brainfuck)

This implementation is inspired by [John Crickett's Challenge](https://codingchallenges.substack.com/p/coding-challenge-81-brainfck-interpreter).

There's 2 different execution mechanism:
- BFExecutor interprets the BF code and executes it, opcode by opcode.
- BFCompiler transpiles the BF code to an arbitrary bytecode, hoping to get some efficiency during compile-time (eg. identifying loop jumps and repeating opcodes). Then, the BFVM executes the bytecode.

### Compiling and executing a simple test

g++ -std=c++20 -O2 -Wall -Wextra bf.cpp -o bf && taskset -c 0 ./bf ./test.bf
