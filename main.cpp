#include <string>
#include <unordered_map>
#include <vector>

#include "Cpu.h"
#include "ISA.h"

int main(int argc, char** argv) {
    CPU<ACC_MA_PWN> cpu;

    if (argc < 2) {
        cpu.loadProgram("code.s");
    } else {
        cpu.loadProgram(argv[1]);
    }
    cpu.dumpProgram("memory.bin");
    cpu.runProgram();
}
