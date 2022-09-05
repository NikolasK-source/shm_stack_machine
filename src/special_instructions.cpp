/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "special_instructions.hpp"

#include <ctime>
#include <iostream>
#include <random>
#include <system_error>
#include <unistd.h>

static std::random_device         rd;
static std::default_random_engine re(rd());

template <clockid_t CLOCK_ID>
static double get_time() {
    struct timespec tp;
    int             tmp = clock_gettime(CLOCK_ID, &tp);
    if (tmp != 0) throw std::system_error(errno, std::generic_category(), "call of clock_gettime failed");

    return static_cast<double>(tp.tv_sec) + static_cast<double>(tp.tv_nsec) / 1000000000.0;
}

bool instr_special::PUSH_stime::exec() {
    union {
        StackMachine::stack_t st;
        double                time;
    };
    time = get_time<CLOCK_REALTIME>();
    machine.push(st);
    return true;
}

bool instr_special::PUSH_mtime::exec() {
    union {
        StackMachine::stack_t st;
        double                time;
    };
    time = get_time<CLOCK_MONOTONIC>();
    machine.push(st);
    return true;
}

bool instr_special::PUSH_ctime::exec() {
    union {
        StackMachine::stack_t st;
        double                time;
    };
    time = get_time<CLOCK_PROCESS_CPUTIME_ID>();
    machine.push(st);
    return true;
}

bool instr_special::PUSH_ttime::exec() {
    union {
        StackMachine::stack_t st;
        double                time;
    };
    time = get_time<CLOCK_THREAD_CPUTIME_ID>();
    machine.push(st);
    return true;
}

bool instr_special::PUSH_pid::exec() {
    machine.push(getpid());
    return true;
}

bool instr_special::PUSH_ppid::exec() {
    machine.push(getppid());
    return true;
}

bool instr_special::PUSH_uid::exec() {
    machine.push(getuid());
    return true;
}

bool instr_special::PUSH_euid::exec() {
    machine.push(geteuid());
    return true;
}

bool instr_special::PUSH_rand::exec() {
    auto r_val = re();
    static_assert(sizeof(r_val) >= sizeof(StackMachine::stack_t));
    static_assert(std::is_integral<decltype(r_val)>::value);
    machine.push(r_val);
    return true;
}

bool instr_special::PUSH_randf::exec() {
    static std::uniform_real_distribution<float> dist(0.0, 1.0);

    union {
        uint32_t i;
        float    f;
    };

    f = dist(re);
    machine.push(i);
    return true;
}

bool instr_special::PUSH_randd::exec() {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    union {
        uint64_t i;
        double   d;
    };

    d = dist(re);
    machine.push(i);
    return true;
}

bool instr_special::POP_stdout::exec() {
    std::cout << machine.pop() << std::endl;
    return true;
}

bool instr_special::POP_stdouts::exec() {
    union {
        uint64_t u;
        int64_t  s;
    };
    u = machine.pop();
    std::cout << s << std::endl;
    return true;
}

bool instr_special::POP_stdoutf::exec() {
    union {
        uint32_t i;
        float    f;
    };
    i = static_cast<decltype(i)>(machine.pop());
    std::cout << f << std::endl;
    return true;
}

bool instr_special::POP_stdoutd::exec() {
    union {
        uint64_t i;
        double   d;
    };
    i = static_cast<decltype(i)>(machine.pop());
    std::cout << d << std::endl;
    return true;
}
