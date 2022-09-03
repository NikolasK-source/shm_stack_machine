/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "StackMachine.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

union data_f {
    data_f() = default;
    data_f(StackMachine::stack_t st) : st(st) {}  // NOLINT
    data_f(float f) : f(f) { dummy.fill(0); }     // NOLINT
    StackMachine::stack_t st;
    struct {
        float                                       f;
        std::array<uint8_t, sizeof(st) - sizeof(f)> dummy;
    };
};

union data_d {
    data_d() = default;
    data_d(StackMachine::stack_t st) : st(st) {}  // NOLINT
    data_d(double d) : d(d) { dummy.fill(0); }    // NOLINT
    StackMachine::stack_t st;
    struct {
        double                                      d;
        std::array<uint8_t, sizeof(st) - sizeof(d)> dummy;
    };
};

static constexpr std::size_t STACK_SIZE = 8;

int main() {
    StackMachine machine(STACK_SIZE);

    machine.push(100);
    machine.push(200);
    machine.add();
    assert(machine.get() == 300);

    try {
        machine.add();
        assert(0);
    } catch (const std::exception &) {}

    machine.push(50);
    machine.sub();
    assert(machine.get() == 250);

    machine.push(4);
    machine.mul();
    assert(machine.get() == 1000);

    machine.push(10);
    assert(machine.size() == 2);
    machine.div();
    assert(machine.get() == 100);
    assert(machine.size() == 1);

    machine.push(1000);
    assert(machine.size() == 2);
    machine.mod();
    assert(machine.pop() == 100);
    assert(machine.size() == 0);

    assert(machine.size() == 0);

    std::size_t sum = 0;
    for (std::size_t i = 0; i < STACK_SIZE; ++i) {
        machine.push(i);
        sum += i;
    }

    try {
        machine.push(0x42);
        assert(0);
    } catch (const std::exception &) {}

    for (std::size_t i = 1; i < STACK_SIZE; ++i)
        machine.add();
    assert(machine.get() == sum);

    data_f f1 = 42.3f;
    data_f f2 = 3.141f;
    data_f f3 = f1.f + f2.f;
    data_f f4 = f3.f * f3.f;
    machine.push(f1.st);
    machine.push(f2.st);
    machine.addf();
    assert(machine.get() == f3.st);
    machine.dup();
    machine.mulf();
    assert(machine.get() == f4.st);

    data_d d1 = static_cast<double>(f4.f);
    data_d d2 = -1.45640541651;
    data_d d3 = std::pow(d1.d, d2.d);
    machine.ftod();
    machine.push(d2.st);
    machine.powd();
    assert(machine.get() == d3.st);

    machine.push(1000000);
    machine.itod();
    machine.muld();
    machine.dtoi();
    assert(machine.pop() == 14);
    assert(machine.size() == 1);
    assert(machine.pop() == sum);
    assert(machine.size() == 0);
}
