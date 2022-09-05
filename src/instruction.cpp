/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "instruction.hpp"

std::size_t instr::J::dummy = 0;

bool instr::PUSH_const::exec() {
    machine.push(src.value);
    return true;
}

bool instr::PUSH_var::exec() {
    machine.push(src.mem.load(src.cell, src.data_type, src.index));
    return true;
}

bool instr::POP_var::exec() {
    dst.mem.store(machine.pop(), dst.cell, dst.data_type, dst.index);
    return true;
}
