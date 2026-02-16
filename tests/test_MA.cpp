#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <random>
#include <string>

#include "Assembler.h"
#include "Cpu.h"
#include "ISA.h"
#include "test_helpers.h"

std::string codePath;

TEST(ProduitScalaire, Exemple1) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    setArray(cpu, program, "v", {1, 2, 3});
    setArray(cpu, program, "w", {4, 5, 6});
    setVariable(cpu, program, "n", 3);

    cpu.runProgram();
    EXPECT_EQ(cpu.ACC, 32);
    EXPECT_LE(cpu.nCycles, 150) << "Your program is not efficient enough (more than 150 cycles for 3 elements)";
}

TEST(ProduitScalaire, Single) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    program.labels["w"] = program.labels["v"] + 1;  // w starts right after v

    CPU<ACC_MA> cpu;
    cpu.loadProgramLayout(program);

    setArray(cpu, program, "v", {5});
    setArray(cpu, program, "w", {4});
    setVariable(cpu, program, "n", 1);

    cpu.runProgram();
    EXPECT_EQ(cpu.ACC, 20);
}

TEST(ProduitScalaire, RandomArray) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    constexpr size_t SIZE = 10;
    constexpr uint16_t MAX_VAL = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> dist(0, MAX_VAL);

    program.labels["w"] = program.labels["v"] + SIZE;  // w starts right after v

    int success_count = 0;

    for (int i = 0; i < 100; ++i) {
        std::vector<uint16_t> values(SIZE);
        for (auto& v : values) v = dist(gen);

        std::vector<uint16_t> w(SIZE);
        for (auto& v : w) v = dist(gen);

        int expected_count = 0;
        for (size_t i = 0; i < SIZE; ++i) {
            expected_count += values[i] * w[i];
        }

        CPU<ACC_MA> cpu;
        cpu.loadProgramLayout(program);

        setVariable(cpu, program, "n", SIZE);
        setArray(cpu, program, "v", values);
        setArray(cpu, program, "w", w);

        cpu.runProgram();

        success_count += static_cast<int>(cpu.ACC == expected_count);
    }

    EXPECT_EQ(success_count, 100) << "Produit scalaire failed on " << (100 - success_count) << "/100 random tests.";
}

static void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  " << prog << " --path <path>\n\n"
              << "Example:\n"
              << "  " << prog << " --path ../code_examples/code.s\n";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--path" && i + 1 < argc) {
            codePath = argv[i + 1];
            break;
        }
    }


    if (codePath.empty()) {
        codePath = "./produit_scalaire.txt";  // Default path for testing
        // printUsage(argv[0]);
    }

    else if (!std::filesystem::exists(codePath)) {
        std::cerr << "Error: file does not exist: " << codePath << "\n";
    }

    else if (!std::filesystem::is_regular_file(codePath)) {
        std::cerr << "Error: not a regular file: " << codePath << "\n";
    }

    return RUN_ALL_TESTS();
}
