/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include "instruction.hpp"

namespace instr_special {

class PUSH_special : public instr::PUSH {
protected:
    explicit PUSH_special(StackMachine &machine) : instr::PUSH(machine) {}
};

class PUSH_stime : public PUSH_special {
public:
    explicit PUSH_stime(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_mtime : public PUSH_special {
public:
    explicit PUSH_mtime(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};


class PUSH_ctime : public PUSH_special {
public:
    explicit PUSH_ctime(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_ttime : public PUSH_special {
public:
    explicit PUSH_ttime(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_pid : public PUSH_special {
public:
    explicit PUSH_pid(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_ppid : public PUSH_special {
public:
    explicit PUSH_ppid(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_uid : public PUSH_special {
public:
    explicit PUSH_uid(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_euid : public PUSH_special {
public:
    explicit PUSH_euid(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_rand : public PUSH_special {
public:
    explicit PUSH_rand(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_randf : public PUSH_special {
public:
    explicit PUSH_randf(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class PUSH_randd : public PUSH_special {
public:
    explicit PUSH_randd(StackMachine &machine) : PUSH_special(machine) {}
    bool exec() override;
};

class POP_NULL : public instr::POP {
public:
    explicit POP_NULL(StackMachine &machine) : POP(machine) {}
    bool exec() override {
        machine.pop();
        return true;
    };
};

class POP_stdout : public instr::POP {
public:
    explicit POP_stdout(StackMachine &machine) : POP(machine) {}
    bool exec() override;
};

class POP_stdouts : public instr::POP {
public:
    explicit POP_stdouts(StackMachine &machine) : POP(machine) {}
    bool exec() override;
};

class POP_stdoutf : public instr::POP {
public:
    explicit POP_stdoutf(StackMachine &machine) : POP(machine) {}
    bool exec() override;
};

class POP_stdoutd : public instr::POP {
public:
    explicit POP_stdoutd(StackMachine &machine) : POP(machine) {}
    bool exec() override;
};

}  // namespace instr_special
