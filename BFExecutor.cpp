#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <string.h>

#include "FileHandle.cpp"
#include "BFAbstract.cpp"

class BFExecutor : public BFAbstract {
public:
    BFExecutor(std::string tape) : BFAbstract(tape) { }
    BFExecutor(const FileHandle&) = delete;
    BFExecutor& operator=(const FileHandle&) = delete;

    void Run() {
        char op;
        int32_t op_d;
        while (tape_ptr_ < int32_t(tape_.size())) {
            op = tape_[tape_ptr_];

            switch (op) {
            case '>':
            data_ptr_++;
            break;

            case '<':
            data_ptr_--;
            if (data_ptr_ < 0) {
                throw std::runtime_error("BFExec: data_ptr_ out of bounds at " + tape_ptr_);
            }
            break;

            case '+':
            data_[data_ptr_]++;
            break;

            case '-':
            data_[data_ptr_]--;
            break;

            case '[':
            if (data_[data_ptr_] == 0) {
                op_d = find_next(']');
                if (op_d == -1) {
                    throw std::runtime_error("BFExec: found no matching `]` after `[` at " + tape_ptr_);
                }
                tape_ptr_ += op_d;
            }
            break;

            case ']':
            if (data_[data_ptr_] != 0) {
                op_d = find_prev('[');
                if (op_d == -1) {
                    throw std::runtime_error("BFExec: found no matching `[` before `]` at " + tape_ptr_);
                }
                tape_ptr_ -= op_d;
            }
            break;

            case ',':
            std::cin >> data_[data_ptr_];
            break;

            case '.':
            output_.push_back(data_[data_ptr_]);
            break;

            default:  // Ignore other chars
            break;
            }

            tape_ptr_++;
        }
    }
};
