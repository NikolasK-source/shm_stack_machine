
/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include "Memory.hpp"
#include "StackMachine.hpp"
#include "instruction.hpp"

#include <memory>
#include <unordered_map>
#include <utility>

class Machine {
private:
    std::size_t                                              cycle_time_ms = 1000;
    StackMachine                                             stack_machine;
    std::unordered_map<std::string, std::unique_ptr<Memory>> mem_map;
    std::unordered_map<std::string, var_t>                   var_map;
    std::unordered_map<std::string, const_t>                 const_map;
    std::vector<std::unique_ptr<Instruction>>                instructions;
    std::unordered_map<std::string, std::size_t>             label_pos;

    std::size_t ip = 0;

    // TODO instruction list
    // TODO instruction pointer
public:
    explicit Machine(std::size_t stack_size) : stack_machine(stack_size) {}

    void load_file(const std::string &path);

    void init();

    void run();

    inline std::size_t get_cycle_time_ms() const { return cycle_time_ms; }

private:
    void parse_mem(const std::vector<std::string> &data);
    void parse_settings(const std::vector<std::string> &data);
    void parse_var(const std::vector<std::string> &data);
    void parse_init(const std::vector<std::string> &data);
    void parse_program(const std::vector<std::string> &data);
};
