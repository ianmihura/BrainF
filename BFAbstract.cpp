#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string.h>

static const int OPLEN = 4;
static const int OPMASK = 0b1111;

class BFAbstract {
public:

    BFAbstract(std::string tape) : tape_ptr_(0), data_ptr_(0) {
        for (char op : tape) {
            if (isValid(op)) {
                tape_.push_back(op);
            }
        }
    }

    bool isValid(char op) {
        return op=='<' || op=='>' || op=='[' || op==']' || op=='.' || op==',' || op=='-' || op=='+';
    }

    std::vector<int32_t> bytecode_;
    std::string tape_;
    int32_t tape_ptr_;

    int32_t data_[99] = {0};
    int32_t data_ptr_;

    std::vector<int32_t> output_;

    int32_t find_next(const char& c) {
        for (int32_t i = 0; i < int32_t(tape_.size()-tape_ptr_); i++) {
            if (tape_[tape_ptr_+i] == c) {
                return i;
            }
        } 
        return -1; // TODO throw
    }

    int32_t find_next(const int inst) {
        for (int32_t i = 0; i < int32_t(bytecode_.size()-tape_ptr_); i++) {
            if ((OPMASK & bytecode_[tape_ptr_+i]) == inst) {
                return i;
            }
        } 
        return -1; // TODO throw
    }

    int32_t find_prev(const char& c) {
        for (int32_t i = 0; i <= tape_ptr_; i++) {
            if (tape_[tape_ptr_-i] == c) {
                return i;
            }
        }
        std::printf("BFCompiler: found no %c at pointer %i", c, tape_ptr_);
        throw std::runtime_error("BFCompiler::find_prev(const char&)");
    }

    int32_t find_prev(const int inst) {
        for (int32_t i = 0; i <= tape_ptr_; i++) {
            if ((OPMASK & bytecode_[tape_ptr_-i]) == inst) {
                return i;
            }
        }
        std::printf("BFCompiler: found no %c at pointer %i", inst, tape_ptr_);
        throw std::runtime_error("BFCompiler::find_prev(const int)");
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
        std::cout << "tape_" << std::endl;
        std::cout << tape_ << std::endl << std::endl;
        std::cout << "tape_ptr_" << std::endl;
        std::cout << tape_ptr_ << std::endl << std::endl;
        std::cout << "output" << std::endl;
        PrintOutput();
    }
};
