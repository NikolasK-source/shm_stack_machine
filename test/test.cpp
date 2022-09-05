/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This template is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

static std::pair<std::string, int> exec(const char *cmd) {
    std::array<char, 4096> buffer {};
    buffer.fill(0);
    std::string result;

    FILE *pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    int exit_code = pclose(pipe);
    return {result, exit_code};
}

int main() {
    {  // test 1
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "1\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/1.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 1: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 1: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }

    {  // test 2
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "3.141\n3.141\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/2.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 2: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 2: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }

    {  // test 3
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "0.0256\n0.0256\n0.16\n0.294723\n-3.66516\n-1.59176\n-5.28771\n0.0255972\n0."
                                        "999672\n0.0256028\n1.54519\n0.0255944\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/3.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 2: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 2: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }

    {  // test 4
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "1\n1\n1\n1\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/4.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 1: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 1: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }

    {  // test 5
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "18446744073709551615\n-1\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/5.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 1: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 1: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }

    {  // test 6
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "3\n1\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/6.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 1: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 1: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }

    {  // test 7
        const int         EXPECT_EXIT = 0;
        const std::string EXPECT_OUT  = "13\n400\n8\n50\n";

        std::pair<std::string, int> result = exec("../shm-stack-compute ../../test/programs/7.stackm");
        if (result.second != EXPECT_EXIT) {
            std::cerr << "test 1: wrong exit code" << std::endl;
            return EXIT_FAILURE;
        }

        if (result.first != EXPECT_OUT) {
            std::cerr << "test 1: wrong output: >>" << result.first << "<<" << std::endl;
            return EXIT_FAILURE;
        }
    }


    return EXIT_SUCCESS;
}
