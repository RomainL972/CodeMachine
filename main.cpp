#include <string>
#include <unordered_map>
#include <vector>

#include "Cpu.h"
#include "ISA.h"

int main() {
    CPU<ACC_MA_PWN> cpu;
    cpu.loadProgram("code.s");
    cpu.dumpProgram("memory.bin");
    cpu.runProgram();
}
