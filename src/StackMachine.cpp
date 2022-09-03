/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "StackMachine.hpp"

#include <array>
#include <cmath>
#include <stdexcept>
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Constants                                                                                                          */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

//* Minimum elements on the stack to execute an arithmetic instruction
static constexpr std::size_t MIN_ARITH = 2;

//* Minimum elements on the stack to execute a conversion instruction
static constexpr std::size_t MIN_CONV = 1;

//* Minimum stack size a stack machine needs to operate
static constexpr std::size_t MIN_STACK = std::max(MIN_ARITH, MIN_CONV);

static_assert(std::is_integral<StackMachine::stack_t>::value, "Integral required.");

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Helper types                                                                                                       */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

static_assert(sizeof(float) <= sizeof(StackMachine::stack_t));
static_assert(sizeof(double) <= sizeof(StackMachine::stack_t));

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

static_assert(sizeof(data_d) >= sizeof(StackMachine::stack_t));
static_assert(sizeof(data_f) >= sizeof(StackMachine::stack_t));

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Helper funcitons                                                                                                   */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

static StackMachine::stack_t ipow(StackMachine::stack_t x, StackMachine::stack_t p) {
    if (p == 0) return 1;
    if (p == 1) return x;

    StackMachine::stack_t tmp = ipow(x, p / 2);
    if (p % 2 == 0) return tmp * tmp;
    return x * tmp * tmp;
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Constructor                                                                                                        */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

StackMachine::StackMachine(std::size_t max_stack) : MAX_STACK(max_stack) {
    if (MAX_STACK < MIN_STACK) throw std::invalid_argument("max stack size to small");
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Stack instructions                                                                                                 */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

void StackMachine::push(StackMachine::stack_t data) {
    if (stack.size() >= MAX_STACK) throw std::runtime_error("stack full");
    stack.push(data);
}

StackMachine::stack_t StackMachine::_pop() {
    auto data = stack.top();
    stack.pop();
    return data;
}

StackMachine::stack_t StackMachine::pop() {
    auto data = get();
    stack.pop();
    return data;
}

StackMachine::stack_t StackMachine::get() const {
    if (stack.empty()) throw std::runtime_error("stack empty");
    return stack.top();
}

void StackMachine::dup() { push(get()); }

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Arithmetic instructions                                                                                            */
/**********************************************************************************************************************/
/**********************************************************************************************************************/


void StackMachine::add() {
    check_arith();
    const auto R = _pop();
    const auto L = _pop();
    stack.push(L + R);
}

void StackMachine::sub() {
    check_arith();
    const auto R = _pop();
    const auto L = _pop();
    stack.push(L - R);
}

void StackMachine::mul() {
    check_arith();
    const auto R = _pop();
    const auto L = _pop();
    stack.push(L * R);
}

void StackMachine::div() {
    check_arith();
    const auto R = _pop();
    const auto L = _pop();
    stack.push(L / R);
}

void StackMachine::mod() {
    check_arith();
    const auto R = _pop();
    const auto L = _pop();
    stack.push(L % R);
}

void StackMachine::pow() {
    check_arith();
    const auto R = _pop();
    const auto L = _pop();
    stack.push(ipow(L, R));
}

void StackMachine::addf() {
    check_arith();
    const data_f R   = _pop();
    const data_f L   = _pop();
    const data_f RES = L.f + R.f;
    stack.push(RES.st);
}

void StackMachine::subf() {
    check_arith();
    const data_f R   = _pop();
    const data_f L   = _pop();
    const data_f RES = L.f - R.f;
    stack.push(RES.st);
}

void StackMachine::mulf() {
    check_arith();
    const data_f R   = _pop();
    const data_f L   = _pop();
    const data_f RES = L.f * R.f;
    stack.push(RES.st);
}

void StackMachine::divf() {
    check_arith();
    const data_f R   = _pop();
    const data_f L   = _pop();
    const data_f RES = L.f / R.f;
    stack.push(RES.st);
}

void StackMachine::powf() {
    check_arith();
    const data_f R   = _pop();
    const data_f L   = _pop();
    const data_f RES = std::pow(L.f, R.f);
    stack.push(RES.st);
}

void StackMachine::addd() {
    check_arith();
    const data_d R   = _pop();
    const data_d L   = _pop();
    const data_d RES = L.d + R.d;
    stack.push(RES.st);
}

void StackMachine::subd() {
    check_arith();
    const data_d R   = _pop();
    const data_d L   = _pop();
    const data_d RES = L.d - R.d;
    stack.push(RES.st);
}

void StackMachine::muld() {
    check_arith();
    const data_d R   = _pop();
    const data_d L   = _pop();
    const data_d RES = L.d * R.d;
    stack.push(RES.st);
}

void StackMachine::divd() {
    check_arith();
    const data_d R   = _pop();
    const data_d L   = _pop();
    const data_d RES = L.d / R.d;
    stack.push(RES.st);
}

void StackMachine::powd() {
    check_arith();
    const data_d R   = _pop();
    const data_d L   = _pop();
    const data_d RES = std::pow(L.d, R.d);
    stack.push(RES.st);
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Data conversion instructions                                                                                       */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

void StackMachine::itof() {
    check_conv();
    const auto   SRC = _pop();
    const data_f DST = static_cast<float>(SRC);
    stack.push(DST.st);
}

void StackMachine::itod() {
    check_conv();
    const auto   SRC = _pop();
    const data_d DST = static_cast<double>(SRC);
    stack.push(DST.st);
}

void StackMachine::ftoi() {
    check_conv();
    const data_f SRC = _pop();
    const auto   DST = static_cast<StackMachine::stack_t>(SRC.f);
    stack.push(DST);
}

void StackMachine::dtoi() {
    check_conv();
    const data_d SRC = _pop();
    const auto   DST = static_cast<StackMachine::stack_t>(SRC.d);
    stack.push(DST);
}

void StackMachine::ftod() {
    check_conv();
    const data_f SRC = _pop();
    const data_d DST = static_cast<double>(SRC.f);
    stack.push(DST.st);
}

void StackMachine::dtof() {
    check_conv();
    const data_d SRC = _pop();
    const data_f DST = static_cast<float>(SRC.d);
    stack.push(DST.st);
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* private methods                                                                                                    */
/**********************************************************************************************************************/
/**********************************************************************************************************************/

void StackMachine::check_arith() const {
    if (stack.size() < MIN_ARITH) throw std::runtime_error("to few elements on stack");
}

void StackMachine::check_conv() const {
    if (stack.size() < MIN_CONV) throw std::runtime_error("to few elements on stack");
}
