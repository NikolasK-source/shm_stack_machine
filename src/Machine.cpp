/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Machine.hpp"

#include "special_instructions.hpp"
#include "split_string.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_set>

static const std::unordered_set<std::string> const_data_types = {"u", "i", "f"};

static const std::unordered_set<std::string> RESERVED = {"STIME",
                                                         "MTIME",
                                                         "CTIME",
                                                         "TTIME",
                                                         "PID",
                                                         "PPID",
                                                         "UID",
                                                         "EUID",
                                                         "RAND",
                                                         "RAND",
                                                         "RANDF",
                                                         "RANDD",
                                                         "STDOUT",
                                                         "STDOUTS",
                                                         "STDOUTF",
                                                         "STDOUTD"};

static unsigned long long parse_unsigned(const std::string &str) {
    unsigned long long result;
    bool               fail = false;
    std::size_t        idx  = 0;
    try {
        result = std::stoull(str, &idx, 0);
    } catch (const std::exception &) { fail = true; }
    fail = fail || idx != str.size();

    if (fail) throw std::runtime_error("invalid format");

    return result;
}

static long long parse_signed(const std::string &str) {
    long long   result;
    bool        fail = false;
    std::size_t idx  = 0;
    try {
        result = std::stoll(str, &idx, 0);
    } catch (const std::exception &) { fail = true; }
    fail = fail || idx != str.size();

    if (fail) throw std::runtime_error("invalid format");

    return result;
}

static double parse_double(const std::string &str) {
    double      result;
    bool        fail = false;
    std::size_t idx  = 0;
    try {
        result = std::stod(str, &idx);
    } catch (const std::exception &) { fail = true; }
    fail = fail || idx != str.size();

    if (fail) throw std::runtime_error("invalid format");

    return result;
}

void Machine::load_file(const std::string &path) {
    std::ifstream input(path);

    if (!input.is_open()) throw std::runtime_error("failed to open input file");

    std::vector<std::string>  section_mem;
    std::vector<std::string>  section_settings;
    std::vector<std::string>  section_var;
    std::vector<std::string>  section_init;
    std::vector<std::string>  section_program;
    std::vector<std::string> *cur_section = nullptr;

    std::string line;
    while (std::getline(input, line)) {
        line = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");

        if (line.empty()) continue;
        if (line[0] == '#') continue;

        auto split_comment = split_string(line, '#', 1);

        split_comment[0] = std::regex_replace(split_comment[0], std::regex("^ +| +$|( ) +"), "$1");

        if (split_comment[0] == "__MEM") {
            cur_section = &section_mem;
            if (!cur_section->empty()) throw std::runtime_error("duplicate section __MEM");
            continue;
        } else if (split_comment[0] == "__SETTINGS") {
            cur_section = &section_settings;
            if (!cur_section->empty()) throw std::runtime_error("duplicate section __SETTINGS");
            continue;
        } else if (split_comment[0] == "__VAR") {
            cur_section = &section_var;
            if (!cur_section->empty()) throw std::runtime_error("duplicate section __VAR");
            continue;
        } else if (split_comment[0] == "__INIT") {
            cur_section = &section_init;
            if (!cur_section->empty()) throw std::runtime_error("duplicate section __INIT");
            continue;
        } else if (split_comment[0] == "__PROGRAM") {
            cur_section = &section_program;
            if (!cur_section->empty()) throw std::runtime_error("duplicate section __PROGRAM");
            continue;
        }

        if (cur_section == nullptr) throw std::runtime_error("instruction outside section");

        cur_section->push_back(split_comment[0]);
    }

    if (input.bad()) throw std::runtime_error("failed to read input file");

    parse_settings(section_settings);
    parse_mem(section_mem);
    parse_var(section_var);
    parse_init(section_init);
    parse_program(section_program);
}

void Machine::init() {
    for (auto &a : var_map) {
        auto &var = a.second;

        if (var.init) {
            instr::POP_var pop(stack_machine, var);
            stack_machine.push(var.init_value);
            pop.exec();
        }
    }
}

void Machine::run() {
    ip = 0;
    while (true) {
        auto instr = instructions.at(ip).get();
        if (!instr->exec()) break;
        ++ip;
    };
    return;
}

void Machine::parse_mem(const std::vector<std::string> &data) {
    for (auto &instr : data) {
        const auto split_instr = split_string(instr, ' ');

        // should never happen
        if (split_instr.empty()) throw std::runtime_error("internal error: instruction empty");

        if (split_instr[0] == "local") {
            if (split_instr.size() != 3) {
                std::ostringstream sstr;
                sstr << "invalid memory configuration: " << instr;
                throw std::runtime_error(sstr.str());
            }

            const auto &name = split_instr[1];
            const auto &size = split_instr[2];

            if (mem_map.count(name) != 0) {
                std::ostringstream sstr;
                sstr << "duplicate memory name '" << name << "'";
                throw std::runtime_error(sstr.str());
            }

            unsigned long long mem_size;
            try {
                mem_size = parse_unsigned(size);
            } catch (const std::exception &e) {
                std::ostringstream sstr;
                sstr << "failed to parse '" << size << "' as memory size: " << e.what();
                throw std::runtime_error(sstr.str());
            }

            mem_map[name] = std::make_unique<MemoryLocal>(mem_size);

        } else if (split_instr[0] == "shm") {
            if (split_instr.size() != 4) {
                std::ostringstream sstr;
                sstr << "invalid memory configuration: " << instr;
                throw std::runtime_error(sstr.str());
            }

            const auto &shm_name  = split_instr[1];
            const auto &name      = split_instr[2];
            const auto &cell_size = split_instr[3];

            if (mem_map.count(name) != 0) {
                std::ostringstream sstr;
                sstr << "duplicate memory name '" << name << "'";
                throw std::runtime_error(sstr.str());
            }

            unsigned long long mem_cell_size;
            try {
                mem_cell_size = parse_unsigned(cell_size);
            } catch (const std::exception &e) {
                std::ostringstream sstr;
                sstr << "failed to parse '" << cell_size << "' as memory size: " << e.what();
                throw std::runtime_error(sstr.str());
            }

            mem_map[name] = std::make_unique<MemorySHM>(shm_name, mem_cell_size);
        } else {
            std::ostringstream sstr;
            sstr << "invalid memory configuration: " << instr;
            sstr << " (unknown memory type '" << split_instr[0] << "')";
            throw std::runtime_error(sstr.str());
        }
    }
}

void Machine::parse_settings(const std::vector<std::string> &data) {
    std::unordered_set<std::string> applied_settings;

    for (auto &instr : data) {
        const auto split_instr = split_string(instr, ' ');

        // should never happen
        if (split_instr.empty()) throw std::runtime_error("internal error: instruction empty");

        if (applied_settings.find(split_instr[0]) != applied_settings.end()) {
            std::ostringstream sstr;
            sstr << "duplicate setting '" << split_instr[0] << "'";
            throw std::runtime_error(sstr.str());
        }

        if (split_instr[0] == "CYCLE_MS") {
            if (split_instr.size() != 2) {
                std::ostringstream sstr;
                sstr << "invalid setting: " << instr;
                throw std::runtime_error(sstr.str());
            }

            const auto &value = split_instr[1];
            try {
                this->cycle_time_ms = parse_unsigned(value);
            } catch (const std::exception &e) {
                std::ostringstream sstr;
                sstr << "failed to parse '" << value << "' as cycle time: " << e.what();
                throw std::runtime_error(sstr.str());
            }

            applied_settings.insert(split_instr[0]);
        } else if (split_instr[0] == "CYCLES") {
            const auto &value = split_instr[1];
            try {
                this->cycles = parse_unsigned(value);
            } catch (const std::exception &e) {
                std::ostringstream sstr;
                sstr << "failed to parse '" << value << "' as number of cycles: " << e.what();
                throw std::runtime_error(sstr.str());
            }
        } else {
            std::ostringstream sstr;
            sstr << "invalid setting: " << instr;
            sstr << " (unknown key '" << split_instr[0] << "')";
            throw std::runtime_error(sstr.str());
        }
    }
}

void Machine::parse_var(const std::vector<std::string> &data) {
    for (auto &instr : data) {
        const auto split_instr = split_string(instr, ' ');

        // should never happen
        if (split_instr.empty()) throw std::runtime_error("internal error: instruction empty");

        if (split_instr.size() != 3) {
            std::ostringstream sstr;
            sstr << "failed to create variable: invalid declaration: '" << instr << "'";
            throw std::runtime_error(sstr.str());
        }

        const auto &addr_str = split_instr[0];

        // check if addr is const
        if (addr_str == "const") {
            const auto &data_type_str = split_instr[1];
            const auto &name_str      = split_instr[2];

            // check data type
            if (const_data_types.find(data_type_str) == const_data_types.end()) {
                std::ostringstream sstr;
                sstr << "failed to create variable: invalid data type for const: '" << data_type_str << "'";
                throw std::runtime_error(sstr.str());
            }

            // check reserved names
            if (RESERVED.find(name_str) != RESERVED.end()) {
                std::ostringstream sstr;
                sstr << "failed to create variable '" << name_str << "': name is a internal variable name.";
                throw std::runtime_error(sstr.str());
            }

            // check if var/const with this name already exists
            if (const_map.count(name_str)) {
                std::ostringstream sstr;
                sstr << "failed to create variable '" << name_str << "': constant with this name already exists.";
                throw std::runtime_error(sstr.str());
            } else if (var_map.count(name_str)) {
                std::ostringstream sstr;
                sstr << "failed to create variable '" << name_str << "': variable with this name already exists.";
                throw std::runtime_error(sstr.str());
            }

            // create constant
            const_map.emplace(std::make_pair(name_str, const_t(data_type_str)));

            continue;
        }

        const auto split_addr = split_string(addr_str, '@');

        if (split_addr.size() != 2) {
            std::ostringstream sstr;
            sstr << "failed to create variable: invalid declaration: '" << instr << "'";
            throw std::runtime_error(sstr.str());
        }

        const auto &mem_name_str  = split_addr[0];
        const auto &cell_str      = split_addr[1];
        const auto &data_type_str = split_instr[1];
        const auto &name_str      = split_instr[2];
        const auto  split_cell    = split_string(cell_str, '.');

        // check if name is available
        if (RESERVED.find(name_str) != RESERVED.end()) {
            std::ostringstream sstr;
            sstr << "failed to create variable '" << name_str << "': name is a internal variable name.";
            throw std::runtime_error(sstr.str());
        } else if (const_map.count(name_str)) {
            std::ostringstream sstr;
            sstr << "failed to create variable '" << name_str << "': constant with this name already exists.";
            throw std::runtime_error(sstr.str());
        } else if (var_map.count(name_str)) {
            std::ostringstream sstr;
            sstr << "failed to create variable '" << name_str << "': variable with this name already exists.";
            throw std::runtime_error(sstr.str());
        }

        // check if memory exists
        if (!mem_map.count(mem_name_str)) {
            std::ostringstream sstr;
            sstr << "failed to create variable: memory '" << mem_name_str << "' does not exist";
            throw std::runtime_error(sstr.str());
        }

        unsigned long long cell;
        try {
            cell = parse_unsigned(split_cell[0]);
        } catch (const std::exception &e) {
            std::ostringstream sstr;
            sstr << "failed to create variable: invalid memory address: '" << cell_str << "'";
            throw std::runtime_error(sstr.str());
        }
        auto check_cell_str = [&](bool index_required) {
            if ((index_required && split_cell.size() != 2) || (!index_required && split_cell.size() != 1)) {
                std::ostringstream sstr;
                sstr << "failed to create variable: invalid memory address: '" << cell_str << "'";
                throw std::runtime_error(sstr.str());
            }
        };

        Memory *mem = mem_map.at(mem_name_str).get();
        if (dynamic_cast<MemoryLocal *>(mem)) {
            check_cell_str(false);
            var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le64, cell)));
        } else {
            if (data_type_str == "le1") {
                check_cell_str(true);
                unsigned long long index;
                try {
                    index = parse_unsigned(split_cell[1]);
                } catch (const std::exception &e) {
                    std::ostringstream sstr;
                    sstr << "failed to create variable: invalid memory address: '" << cell_str << "'";
                    throw std::runtime_error(sstr.str());
                }
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le1, cell, index)));
            } else if (data_type_str == "be1") {
                check_cell_str(true);
                unsigned long long index;
                try {
                    index = parse_unsigned(split_cell[1]);
                } catch (const std::exception &e) {
                    std::ostringstream sstr;
                    sstr << "failed to create variable: invalid memory address: '" << cell_str << "'";
                    throw std::runtime_error(sstr.str());
                }
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le1, cell, index)));
            } else if (data_type_str == "byte") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::byte, cell)));
            } else if (data_type_str == "le16") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le16, cell)));
            } else if (data_type_str == "be16") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::be16, cell)));
            } else if (data_type_str == "le32") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le32, cell)));
            } else if (data_type_str == "be32") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::be32, cell)));
            } else if (data_type_str == "le32r") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le32r, cell)));
            } else if (data_type_str == "be32r") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::be32r, cell)));
            } else if (data_type_str == "le64") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le64, cell)));
            } else if (data_type_str == "be64") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::be64, cell)));
            } else if (data_type_str == "le64r") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le64r, cell)));
            } else if (data_type_str == "be64r") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::be64r, cell)));
            } else if (data_type_str == "le64r4") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::le64r4, cell)));
            } else if (data_type_str == "be64r4") {
                check_cell_str(false);
                var_map.emplace(std::make_pair(name_str, var_t(*mem, Memory::dtype_t::be64r4, cell)));
            } else {
                std::ostringstream sstr;
                sstr << "failed to create variable '" << name_str << "': unknown data type '" << data_type_str << "'";
                throw std::runtime_error(sstr.str());
            }
        }
    }
}

void Machine::parse_init(const std::vector<std::string> &data) {
    for (auto &instr : data) {
        const auto split_instr = split_string(instr, ' ');

        // should never happen
        if (split_instr.empty()) throw std::runtime_error("internal error: instruction empty");

        if (split_instr.size() != 2) {
            std::ostringstream sstr;
            sstr << "invalid initialization: " << instr;
            throw std::runtime_error(sstr.str());
        }

        const auto &var_name  = split_instr[0];
        const auto &value_str = split_instr[1];

        if (const_map.count(var_name)) {
            const_t &constant = const_map.at(var_name);
            if (constant.init) {
                std::ostringstream sstr;
                sstr << "constant '" << var_name << "' is initialized twice";
                throw std::runtime_error(sstr.str());
            }

            union {
                StackMachine::stack_t st;
                uint64_t              u;
                int64_t               i;
                double                f;
            };

            try {
                if (constant.d_type == "u") {
                    u = parse_unsigned(value_str);
                } else if (constant.d_type == "i") {
                    i = parse_signed(value_str);
                } else if (constant.d_type == "f") {
                    f = parse_double(value_str);
                }
            } catch (const std::exception &e) {
                std::ostringstream sstr;
                sstr << "failed tp parse '" << value_str << "' as value for '" << var_name << "': " << e.what();
                throw std::runtime_error(sstr.str());
            }

            constant.value = st;
            constant.init  = true;

        } else if (var_map.count(var_name)) {
            union {
                StackMachine::stack_t st;
                unsigned long long    u;
                signed long long      s;
                double                d;
            };

            static_assert(sizeof(st) == sizeof(u));
            static_assert(sizeof(st) == sizeof(s));
            static_assert(sizeof(st) == sizeof(d));

            if (value_str[0] == '-') {
                try {
                    s = parse_signed(value_str);
                } catch (const std::exception &e) {
                    try {
                        d = parse_double(value_str);
                    } catch (const std::exception &e2) {
                        std::ostringstream sstr;
                        sstr << "failed tp parse '" << value_str << "' as value for '" << var_name << "': " << e.what()
                             << " --- " << e2.what();
                        throw std::runtime_error(sstr.str());
                    }
                }
            } else {
                try {
                    u = parse_unsigned(value_str);
                } catch (const std::exception &e) {
                    try {
                        d = parse_double(value_str);
                    } catch (const std::exception &e2) {
                        std::ostringstream sstr;
                        sstr << "failed tp parse '" << value_str << "' as value for '" << var_name << "': " << e.what()
                             << " --- " << e2.what();
                        throw std::runtime_error(sstr.str());
                    }
                }
            }

            auto &var      = var_map.at(var_name);
            var.init_value = st;
            var.init       = true;
        } else {
            std::ostringstream sstr;
            sstr << "failed to inialize '" << var_name << "': unknown variable: ";
            throw std::runtime_error(sstr.str());
        }
    }

    // check if all constants are initialized
    for (const auto &a : const_map) {
        const auto &name     = a.first;
        const auto &constant = a.second;

        if (!constant.init) {
            std::ostringstream sstr;
            sstr << "constant '" << name << "' was not initialized";
            throw std::runtime_error(sstr.str());
        }
    }
}

void Machine::parse_program(const std::vector<std::string> &data) {
    std::unordered_map<instr::Jump *, std::string> jump_targets;

    for (auto &instr : data) {
        if (instr == "ADD") {
            instructions.emplace_back(std::make_unique<instr::ADD>(stack_machine));
        } else if (instr == "SUB") {
            instructions.emplace_back(std::make_unique<instr::SUB>(stack_machine));
        } else if (instr == "MUL") {
            instructions.emplace_back(std::make_unique<instr::MUL>(stack_machine));
        } else if (instr == "MULS") {
            instructions.emplace_back(std::make_unique<instr::MULS>(stack_machine));
        } else if (instr == "DIV") {
            instructions.emplace_back(std::make_unique<instr::DIV>(stack_machine));
        } else if (instr == "DIVS") {
            instructions.emplace_back(std::make_unique<instr::DIVS>(stack_machine));
        } else if (instr == "MOD") {
            instructions.emplace_back(std::make_unique<instr::MOD>(stack_machine));
        } else if (instr == "MODS") {
            instructions.emplace_back(std::make_unique<instr::MODS>(stack_machine));
        } else if (instr == "POW") {
            instructions.emplace_back(std::make_unique<instr::POW>(stack_machine));
        } else if (instr == "POWS") {
            instructions.emplace_back(std::make_unique<instr::POWS>(stack_machine));
        } else if (instr == "ADDF") {
            instructions.emplace_back(std::make_unique<instr::ADDF>(stack_machine));
        } else if (instr == "SUBF") {
            instructions.emplace_back(std::make_unique<instr::SUBF>(stack_machine));
        } else if (instr == "MULF") {
            instructions.emplace_back(std::make_unique<instr::MULF>(stack_machine));
        } else if (instr == "POWF") {
            instructions.emplace_back(std::make_unique<instr::POWF>(stack_machine));
        } else if (instr == "DIVF") {
            instructions.emplace_back(std::make_unique<instr::DIVF>(stack_machine));
        } else if (instr == "ADDD") {
            instructions.emplace_back(std::make_unique<instr::ADDD>(stack_machine));
        } else if (instr == "SUBD") {
            instructions.emplace_back(std::make_unique<instr::SUBD>(stack_machine));
        } else if (instr == "MULD") {
            instructions.emplace_back(std::make_unique<instr::MULD>(stack_machine));
        } else if (instr == "DIVD") {
            instructions.emplace_back(std::make_unique<instr::DIVD>(stack_machine));
        } else if (instr == "POWD") {
            instructions.emplace_back(std::make_unique<instr::POWD>(stack_machine));
        } else if (instr == "NOT") {
            instructions.emplace_back(std::make_unique<instr::NOT>(stack_machine));
        } else if (instr == "AND") {
            instructions.emplace_back(std::make_unique<instr::AND>(stack_machine));
        } else if (instr == "OR") {
            instructions.emplace_back(std::make_unique<instr::OR>(stack_machine));
        } else if (instr == "XOR") {
            instructions.emplace_back(std::make_unique<instr::XOR>(stack_machine));
        } else if (instr == "INV") {
            instructions.emplace_back(std::make_unique<instr::INV>(stack_machine));
        } else if (instr == "BAND") {
            instructions.emplace_back(std::make_unique<instr::BAND>(stack_machine));
        } else if (instr == "BOR") {
            instructions.emplace_back(std::make_unique<instr::BOR>(stack_machine));
        } else if (instr == "BXOR") {
            instructions.emplace_back(std::make_unique<instr::BXOR>(stack_machine));
        } else if (instr == "ITOF") {
            instructions.emplace_back(std::make_unique<instr::ITOF>(stack_machine));
        } else if (instr == "ITOD") {
            instructions.emplace_back(std::make_unique<instr::ITOD>(stack_machine));
        } else if (instr == "FTOI") {
            instructions.emplace_back(std::make_unique<instr::FTOI>(stack_machine));
        } else if (instr == "DTOI") {
            instructions.emplace_back(std::make_unique<instr::DTOI>(stack_machine));
        } else if (instr == "FTOD") {
            instructions.emplace_back(std::make_unique<instr::FTOD>(stack_machine));
        } else if (instr == "DTOF") {
            instructions.emplace_back(std::make_unique<instr::DTOF>(stack_machine));
        } else if (instr == "EQ") {
            instructions.emplace_back(std::make_unique<instr::EQ>(stack_machine));
        } else if (instr == "NE") {
            instructions.emplace_back(std::make_unique<instr::NE>(stack_machine));
        } else if (instr == "LT") {
            instructions.emplace_back(std::make_unique<instr::LT>(stack_machine));
        } else if (instr == "GT") {
            instructions.emplace_back(std::make_unique<instr::GT>(stack_machine));
        } else if (instr == "LE") {
            instructions.emplace_back(std::make_unique<instr::LE>(stack_machine));
        } else if (instr == "GE") {
            instructions.emplace_back(std::make_unique<instr::GE>(stack_machine));
        } else if (instr == "LTS") {
            instructions.emplace_back(std::make_unique<instr::LTS>(stack_machine));
        } else if (instr == "GTS") {
            instructions.emplace_back(std::make_unique<instr::GTS>(stack_machine));
        } else if (instr == "LES") {
            instructions.emplace_back(std::make_unique<instr::LES>(stack_machine));
        } else if (instr == "GES") {
            instructions.emplace_back(std::make_unique<instr::GES>(stack_machine));
        } else if (instr == "LTD") {
            instructions.emplace_back(std::make_unique<instr::LTD>(stack_machine));
        } else if (instr == "GTD") {
            instructions.emplace_back(std::make_unique<instr::GTD>(stack_machine));
        } else if (instr == "LED") {
            instructions.emplace_back(std::make_unique<instr::LED>(stack_machine));
        } else if (instr == "GED") {
            instructions.emplace_back(std::make_unique<instr::GED>(stack_machine));
        } else if (instr == "DUP") {
            instructions.emplace_back(std::make_unique<instr::DUP>(stack_machine));
        } else if (instr == "ABS") {
            instructions.emplace_back(std::make_unique<instr::ABS>(stack_machine));
        } else if (instr == "SQRT") {
            instructions.emplace_back(std::make_unique<instr::SQRT>(stack_machine));
        } else if (instr == "CBRT") {
            instructions.emplace_back(std::make_unique<instr::CBRT>(stack_machine));
        } else if (instr == "LN") {
            instructions.emplace_back(std::make_unique<instr::LN>(stack_machine));
        } else if (instr == "LG") {
            instructions.emplace_back(std::make_unique<instr::LG>(stack_machine));
        } else if (instr == "LOG") {
            instructions.emplace_back(std::make_unique<instr::LOG>(stack_machine));
        } else if (instr == "SIN") {
            instructions.emplace_back(std::make_unique<instr::SIN>(stack_machine));
        } else if (instr == "COS") {
            instructions.emplace_back(std::make_unique<instr::COS>(stack_machine));
        } else if (instr == "ASIN") {
            instructions.emplace_back(std::make_unique<instr::ASIN>(stack_machine));
        } else if (instr == "ACOS") {
            instructions.emplace_back(std::make_unique<instr::ACOS>(stack_machine));
        } else if (instr == "ATAN") {
            instructions.emplace_back(std::make_unique<instr::ATAN>(stack_machine));
        } else if (instr == "ATANXY" || instr == "ATAN2") {
            instructions.emplace_back(std::make_unique<instr::ATANXY>(stack_machine));
        } else {
            const auto split_instr = split_string(instr, ' ');

            if (split_instr.size() == 1 && instr[0] == '$') {
                std::string name = instr;
                name.erase(0, 1);

                if (name.empty()) {
                    std::ostringstream sstr;
                    sstr << "empty label name";
                    throw std::runtime_error(sstr.str());
                }

                if (label_pos.count(name)) {
                    std::ostringstream sstr;
                    sstr << "duplicate label '" << name << "'";
                    throw std::runtime_error(sstr.str());
                }

                label_pos[name] = instructions.size();
                instructions.emplace_back(std::make_unique<instr::LABEL>(stack_machine, name));
                continue;
            }

            if (split_instr.size() != 2) {
                std::ostringstream sstr;
                sstr << "invalid instruction: " << instr;
                throw std::runtime_error(sstr.str());
            }

            if (split_instr[0] == "PUSH" || split_instr[0] == "L") {
                const auto &target = split_instr[1];

                if (const_map.count(target)) {
                    instructions.emplace_back(std::make_unique<instr::PUSH_const>(stack_machine, const_map.at(target)));
                } else if (var_map.count(target)) {
                    instructions.emplace_back(std::make_unique<instr::PUSH_var>(stack_machine, var_map.at(target)));
                } else {
                    if (target == "STIME") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_stime>(stack_machine));
                    } else if (target == "MTIME") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_mtime>(stack_machine));
                    } else if (target == "CTIME") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_ctime>(stack_machine));
                    } else if (target == "TTIME") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_ttime>(stack_machine));
                    } else if (target == "PID") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_pid>(stack_machine));
                    } else if (target == "PPID") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_ppid>(stack_machine));
                    } else if (target == "UID") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_uid>(stack_machine));
                    } else if (target == "EUID") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_euid>(stack_machine));
                    } else if (target == "RAND") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_rand>(stack_machine));
                    } else if (target == "RANDF") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_randf>(stack_machine));
                    } else if (target == "RANDD") {
                        instructions.emplace_back(std::make_unique<instr_special::PUSH_randd>(stack_machine));
                    } else {
                        std::ostringstream sstr;
                        sstr << "failed to pares instruction '" << instr << "': unknown variable '" << target << "'";
                        throw std::runtime_error(sstr.str());
                    }
                }

            } else if (split_instr[0] == "POP" || split_instr[0] == "S") {
                const auto &target = split_instr[1];

                if (var_map.count(target)) {
                    instructions.emplace_back(std::make_unique<instr::POP_var>(stack_machine, var_map.at(target)));
                } else {
                    if (target == "STDOUT") {
                        instructions.emplace_back(std::make_unique<instr_special::POP_stdout>(stack_machine));
                    } else if (target == "STDOUTS") {
                        instructions.emplace_back(std::make_unique<instr_special::POP_stdouts>(stack_machine));
                    } else if (target == "STDOUTF") {
                        instructions.emplace_back(std::make_unique<instr_special::POP_stdoutf>(stack_machine));
                    } else if (target == "STDOUTD") {
                        instructions.emplace_back(std::make_unique<instr_special::POP_stdoutd>(stack_machine));
                    } else if (target == "NULL") {
                        instructions.emplace_back(std::make_unique<instr_special::POP_NULL>(stack_machine));
                    } else {
                        std::ostringstream sstr;
                        sstr << "failed to pares instruction '" << instr << "': unknown variable '" << target << "'";
                        throw std::runtime_error(sstr.str());
                    }
                }
            } else if (split_instr[0] == "J") {
                instructions.emplace_back(std::make_unique<instr::J>(stack_machine));
                jump_targets[dynamic_cast<instr::Jump *>(instructions.back().get())] = split_instr[1];
            } else if (split_instr[0] == "JZ") {
                instructions.emplace_back(std::make_unique<instr::JZ>(stack_machine, ip));
                jump_targets[dynamic_cast<instr::Jump *>(instructions.back().get())] = split_instr[1];
            } else if (split_instr[0] == "JNZ") {
                instructions.emplace_back(std::make_unique<instr::JNZ>(stack_machine, ip));
                jump_targets[dynamic_cast<instr::Jump *>(instructions.back().get())] = split_instr[1];
            }
        }
    }

    // assign jump targets
    for (auto &a : jump_targets) {
        auto &jump       = a.first;
        auto &label_name = a.second;

        if (!label_pos.count(label_name)) {
            std::ostringstream sstr;
            sstr << "unknown jump target: " << label_name;
            throw std::runtime_error(sstr.str());
        }

        jump->set_target(label_pos[label_name]);
    }

    // add end instruction
    instructions.emplace_back(std::make_unique<instr::END>(stack_machine));
}
