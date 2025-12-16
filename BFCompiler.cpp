#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string.h>

#include "FileHandle.cpp"
#include "BFAbstract.cpp"

// Encoded opcodes:
//   Each opcode is encoded in an int32:
//   The last 4 bits encode the instruction
//   The first 28 bits is optional data
//    each opcode defines how it uses this op_data, if at all.
//    eg. amount of times the opcode repeats, or offset to jump to.
// 
//       start op_data                      start opcodes
//       v                                  v   
// >: 1 (0000 0000 0000 0000 0000 0000 0000 0001)
// <: 2 (0000 0000 0000 0000 0000 0000 0000 0010)
// +: 3 (0000 0000 0000 0000 0000 0000 0000 0011)
// -: 4 (0000 0000 0000 0000 0000 0000 0000 0100)
// [: 5 (0000 0000 0000 0000 0000 0000 0000 0101)
// ]: 6 (0000 0000 0000 0000 0000 0000 0000 0110)
// ,: 7 (0000 0000 0000 0000 0000 0000 0000 0111)
// .: 0 (0000 0000 0000 0000 0000 0000 0000 0000)
//
//   We reserve the other 8 opcodes (those starting with 1) for patterns

class BFCompiler : public BFAbstract {
public:
    BFCompiler(std::string tape) : BFAbstract(tape) {}
    BFCompiler(const FileHandle&) = delete;
    BFCompiler& operator=(const FileHandle&) = delete;

    void Compile() {
        char op;
        int32_t inst, op_d;
        tape_ptr_ = 0;
        while (tape_ptr_ < int32_t(tape_.size())) {
            op = tape_[tape_ptr_];

            switch (op) {
            case '>':
            inst = 1;
            op_d = find_next_not('>');
            inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            tape_ptr_ += op_d-1;
            break;

            case '<':
            inst = 2;
            op_d = find_next_not('<');
            inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            tape_ptr_ += op_d-1;
            break;

            case '+':
            inst = 3;
            op_d = find_next_not('+');
            inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            tape_ptr_ += op_d-1;
            break;

            case '-':
            inst = 4;
            op_d = find_next_not('-');
            inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            tape_ptr_ += op_d-1;
            break;

            case '[':
            inst = 5;
            op_d = find_next(']');
            if (op_d == -1) {
                throw std::runtime_error("BFCompiler: found no matching `]` after `[` at " + tape_ptr_);
            }
            // inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            break;

            case ']':
            inst = 6;
            op_d = find_prev('[');
            if (op_d == -1) {
                throw std::runtime_error("BFCompiler: found no matching `[` before `]` at " + tape_ptr_);
            }
            // inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            break;

            case ',':
            inst = 7;
            // inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            break;

            case '.':
            inst = 0;
            // inst = op_d<<OPLEN | inst;
            bytecode_.push_back(inst);
            break;

            default:  // Ignore other chars
            break;
            }

            tape_ptr_++;
        }

        while (tape_ptr_ < int32_t(bytecode_.size())) {
            op = bytecode_[tape_ptr_];
            inst = OPMASK & op;

            if (inst == 5) {
                op_d = find_next(6);
                if (op_d == -1) {
                    throw std::runtime_error("BFCompiler: found no matching `]` after `[` at " + tape_ptr_);
                }
            } else if (inst == 6) {
                op_d = find_prev(5);
            } else {
                continue;
            }

            inst = op_d<<OPLEN | inst;
            bytecode_[tape_ptr_] = inst;
            tape_ptr_++;
        }
    }

    int32_t find_next_not(const char& c) {
        int32_t i(0);
        while (i < int32_t(tape_.size()-tape_ptr_)) {
            if (tape_[tape_ptr_+i] != c) {
                return i;
            }
            i++;
        }
        return i;
        // std::printf("BFCompiler: found no %c at pointer %i", c, tape_ptr_);
        // throw std::runtime_error("BFCompiler::find_next_not(const char&)");
    }

    void PrintBytecode() {
        for (int i: bytecode_) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
    }

    void PrintState() {
        std::cout << "bytecode_" << std::endl;
        PrintBytecode();
        std::cout << std::endl;
        std::cout << "tape_" << std::endl;
        std::cout << tape_ << std::endl << std::endl;
        std::cout << "tape_ptr_" << std::endl;
        std::cout << tape_ptr_ << std::endl << std::endl;
    }

    std::vector<int32_t> GetBytecode() {
        return bytecode_;
    }
};

class BFVM {
public:
    BFVM(std::vector<int32_t> bytecode) : bytecode_(bytecode), bc_ptr_(0), data_ptr_(0) { }
    BFVM(const FileHandle&) = delete;
    BFVM& operator=(const FileHandle&) = delete;

    void Run() {
        int32_t op, inst, op_d;
        while (bc_ptr_ < int32_t(bytecode_.size())) {
            op = bytecode_[bc_ptr_];
            inst = OPMASK & op;
            op_d = op>>OPLEN;

            // std::printf("op(%i: %i | %i) (@%i)\n", op, inst, op_d, bc_ptr_);

            switch (inst) {
            case 1:
            data_ptr_ += op_d;
            break;

            case 2:
            data_ptr_ -= op_d;
            if (data_ptr_ < 0) {
                throw std::runtime_error("BFVM: data_ptr_ out of bounds at " + bc_ptr_);
            }
            break;

            case 3:
            data_[data_ptr_] += op_d;
            break;

            case 4:
            data_[data_ptr_] -= op_d;
            break;

            case 5:
            if (data_[data_ptr_] == 0) {
                bc_ptr_ = op_d;
            }
            break;

            case 6:
            if (data_[data_ptr_] != 0) {
                bc_ptr_ = op_d;
            }
            break;

            case 7:
            std::cin >> data_[data_ptr_];
            break;

            case 0:
            output_.push_back(data_[data_ptr_]);
            break;

            default:  // Ignore other ops
            break;
            }

            bc_ptr_++;
        }
    }

    void PrintOutput() {
        for (char i: output_) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
        for (int i: output_) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
    }

    void PrintDataFull() {
        for (int i: data_) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
    }

    void PrintState() {
        std::cout << "data_" << std::endl;
        PrintDataFull();
        std::cout << std::endl;
        std::cout << "data_ptr_" << std::endl;
        std::cout << data_ptr_ << std::endl << std::endl;
        std::cout << "bc_ptr_" << std::endl;
        std::cout << bc_ptr_ << std::endl << std::endl;
        std::cout << "output" << std::endl;
        PrintOutput();
    }

private:
    std::vector<int32_t> bytecode_;
    int32_t bc_ptr_;

    int32_t data_[99] = {0};
    int32_t data_ptr_;

    std::vector<int32_t> output_;
};
