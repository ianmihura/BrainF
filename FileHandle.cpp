#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <string.h>

class FileHandle {
public:
    FileHandle(const std::string& filename) : file_(filename, std::ios::out), filename_(filename) {
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    ~FileHandle() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    std::string Print() {
        std::string tape("");
        if (file_.is_open()) {
            std::string line;
            while (std::getline(file_, line)) {
                tape.append(line);
            }
        }
        return tape;
    }

private:
    std::ifstream file_;
    std::string filename_;
};