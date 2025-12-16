#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string.h>

#include "FileHandle.cpp"
#include "BFExecutor.cpp"
#include "BFCompiler.cpp"

// TODO make nicer abstract class structure
//   find_next
//   find_next_not
//   find_prev
// TODO output file

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "> Error: must specify a source file: bf <source_file>" << std::endl;
        return 1;
    } 

    std::string file_path(argv[1]);
    FileHandle file_(file_path);
    std::string tape = file_.Print();

    // BFExecutor bfe(tape);
    // bfe.Run();
    // bfe.PrintState();

    std::printf("Compiling...\n\n");

    BFCompiler bfc(tape);
    bfc.Compile();
    bfc.PrintState();

    std::printf("Running...\n\n");

    BFVM bfvm(bfc.GetBytecode());
    bfvm.Run();
    bfvm.PrintState();

    return 0;
}
