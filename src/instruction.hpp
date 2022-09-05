/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include <utility>

#include "Memory.hpp"
#include "StackMachine.hpp"

struct var_t {
    Memory               &mem;
    Memory::dtype_t       data_type;
    std::size_t           cell;
    std::size_t           index;
    bool                  init       = false;
    StackMachine::stack_t init_value = 0;

    var_t(Memory &mem, Memory::dtype_t data_type, std::size_t cell, std::size_t index = 0)
        : mem(mem), data_type(data_type), cell(cell), index(index) {}
};

struct const_t {
    StackMachine::stack_t value = 0;
    std::string           d_type;
    bool                  init = false;
    explicit const_t(std::string d_type) : d_type(std::move(d_type)) {}
};

class Instruction {
protected:
    StackMachine &machine;

    explicit Instruction(StackMachine &machine) : machine(machine) {}

public:
    virtual ~Instruction() = default;

    virtual bool exec() = 0;
};

namespace instr {

class OnStack : public Instruction {
protected:
    explicit OnStack(StackMachine &machine) : Instruction(machine) {}
};

class Jump : public Instruction {
protected:
    std::size_t  target = 0;
    std::size_t &ip;

    Jump(StackMachine &machine, std::size_t &ip) : Instruction(machine), ip(ip) {}

    virtual bool cond() = 0;

public:
    void set_target(std::size_t new_target) { target = new_target; }
    bool exec() override {
        if (cond()) ip = target;
        return true;
    }
};

class PUSH : public Instruction {
protected:
    explicit PUSH(StackMachine &machine) : Instruction(machine) {}
};

class PUSH_const : public PUSH {
private:
    const_t &src;

public:
    explicit PUSH_const(StackMachine &machine, const_t &src) : PUSH(machine), src(src) {}
    bool exec() override;
};

class PUSH_var : public PUSH {
private:
    var_t &src;

public:
    explicit PUSH_var(StackMachine &machine, var_t &src) : PUSH(machine), src(src) {}
    bool exec() override;
};

class POP : public Instruction {
protected:
    explicit POP(StackMachine &machine) : Instruction(machine) {}

public:
};

class POP_var : public POP {
private:
    var_t &dst;

public:
    explicit POP_var(StackMachine &machine, var_t &dst) : POP(machine), dst(dst) {}
    bool exec() override;
};

class ADD : public OnStack {
public:
    explicit ADD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.add();
        return true;
    }
};

class SUB : public OnStack {
public:
    explicit SUB(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.sub();
        return true;
    }
};

class MUL : public OnStack {
public:
    explicit MUL(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.mul();
        return true;
    }
};

class MULS : public OnStack {
public:
    explicit MULS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.muls();
        return true;
    }
};

class DIV : public OnStack {
public:
    explicit DIV(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.div();
        return true;
    }
};

class DIVS : public OnStack {
public:
    explicit DIVS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.divs();
        return true;
    }
};

class MOD : public OnStack {
public:
    explicit MOD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.mod();
        return true;
    }
};

class MODS : public OnStack {
public:
    explicit MODS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.mods();
        return true;
    }
};

class POW : public OnStack {
public:
    explicit POW(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.pow();
        return true;
    }
};

class POWS : public OnStack {
public:
    explicit POWS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.pows();
        return true;
    }
};

class ADDF : public OnStack {
public:
    explicit ADDF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.addf();
        return true;
    }
};

class SUBF : public OnStack {
public:
    explicit SUBF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.subf();
        return true;
    }
};

class MULF : public OnStack {
public:
    explicit MULF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.mulf();
        return true;
    }
};

class DIVF : public OnStack {
public:
    explicit DIVF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.divf();
        return true;
    }
};

class POWF : public OnStack {
public:
    explicit POWF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.powf();
        return true;
    }
};

class ADDD : public OnStack {
public:
    explicit ADDD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.addd();
        return true;
    }
};

class SUBD : public OnStack {
public:
    explicit SUBD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.subd();
        return true;
    }
};

class MULD : public OnStack {
public:
    explicit MULD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.muld();
        return true;
    }
};

class DIVD : public OnStack {
public:
    explicit DIVD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.divd();
        return true;
    }
};

class POWD : public OnStack {
public:
    explicit POWD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.powd();
        return true;
    }
};

class NOT : public OnStack {
public:
    explicit NOT(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.linv();
        return true;
    }
};

class AND : public OnStack {
public:
    explicit AND(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.land();
        return true;
    }
};

class OR : public OnStack {
public:
    explicit OR(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.lor();
        return true;
    }
};

class XOR : public OnStack {
public:
    explicit XOR(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.lxor();
        return true;
    }
};

class INV : public OnStack {
public:
    explicit INV(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.binv();
        return true;
    }
};

class BAND : public OnStack {
public:
    explicit BAND(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.band();
        return true;
    }
};

class BOR : public OnStack {
public:
    explicit BOR(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.bor();
        return true;
    }
};

class BXOR : public OnStack {
public:
    explicit BXOR(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.bxor();
        return true;
    }
};

class ITOF : public OnStack {
public:
    explicit ITOF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.itof();
        return true;
    }
};

class ITOD : public OnStack {
public:
    explicit ITOD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.itod();
        return true;
    }
};

class FTOI : public OnStack {
public:
    explicit FTOI(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ftoi();
        return true;
    }
};

class DTOI : public OnStack {
public:
    explicit DTOI(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.dtoi();
        return true;
    }
};

class FTOD : public OnStack {
public:
    explicit FTOD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ftod();
        return true;
    }
};

class DTOF : public OnStack {
public:
    explicit DTOF(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.dtof();
        return true;
    }
};

class EQ : public OnStack {
public:
    explicit EQ(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.eq();
        return true;
    }
};

class NE : public OnStack {
public:
    explicit NE(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ne();
        return true;
    }
};

class LT : public OnStack {
public:
    explicit LT(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.lt();
        return true;
    }
};

class GT : public OnStack {
public:
    explicit GT(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.gt();
        return true;
    }
};

class LE : public OnStack {
public:
    explicit LE(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.le();
        return true;
    }
};

class GE : public OnStack {
public:
    explicit GE(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ge();
        return true;
    }
};

class LTS : public OnStack {
public:
    explicit LTS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.lts();
        return true;
    }
};

class GTS : public OnStack {
public:
    explicit GTS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.gts();
        return true;
    }
};

class LES : public OnStack {
public:
    explicit LES(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.les();
        return true;
    }
};

class GES : public OnStack {
public:
    explicit GES(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ges();
        return true;
    }
};

class LTD : public OnStack {
public:
    explicit LTD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ltd();
        return true;
    }
};

class GTD : public OnStack {
public:
    explicit GTD(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.gtd();
        return true;
    }
};

class LED : public OnStack {
public:
    explicit LED(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.led();
        return true;
    }
};

class GED : public OnStack {
public:
    explicit GED(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ged();
        return true;
    }
};

class ABS : public OnStack {
public:
    explicit ABS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.abs();
        return true;
    }
};

class SQRT : public OnStack {
public:
    explicit SQRT(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.sqrt();
        return true;
    }
};

class CBRT : public OnStack {
public:
    explicit CBRT(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.cbrt();
        return true;
    }
};

class LN : public OnStack {
public:
    explicit LN(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.ln();
        return true;
    }
};

class LOG : public OnStack {
public:
    explicit LOG(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.log();
        return true;
    }
};

class LG : public OnStack {
public:
    explicit LG(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.lg();
        return true;
    }
};

class SIN : public OnStack {
public:
    explicit SIN(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.sin();
        return true;
    }
};

class COS : public OnStack {
public:
    explicit COS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.cos();
        return true;
    }
};

class ASIN : public OnStack {
public:
    explicit ASIN(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.asin();
        return true;
    }
};

class ACOS : public OnStack {
public:
    explicit ACOS(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.acos();
        return true;
    }
};

class ATAN : public OnStack {
public:
    explicit ATAN(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.atan();
        return true;
    }
};

class ATANXY : public OnStack {
public:
    explicit ATANXY(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.atanxy();
        return true;
    }
};

class J : public Jump {
private:
    static std::size_t dummy;

public:
    explicit J(StackMachine &machine) : Jump(machine, dummy) {}

protected:
    bool cond() override { return true; }
};

class JZ : public Jump {
public:
    explicit JZ(StackMachine &machine, std::size_t &ip) : Jump(machine, ip) {}

protected:
    bool cond() override { return machine.pop() == 0; }
};

class JNZ : public Jump {
public:
    explicit JNZ(StackMachine &machine, std::size_t &ip) : Jump(machine, ip) {}

protected:
    bool cond() override { return machine.pop() != 0; }
};

class LABEL : public Instruction {
private:
    std::string name;

public:
    explicit LABEL(StackMachine &machine, std::string name) : Instruction(machine), name(std::move(name)) {}
    bool exec() override { return true; }
};

class DUP : public OnStack {
public:
    explicit DUP(StackMachine &machine) : OnStack(machine) {}
    bool exec() override {
        machine.dup();
        return true;
    }
};

class END : public Instruction {
public:
    explicit END(StackMachine &machine) : Instruction(machine) {}
    bool exec() override { return false; }
};

}  // namespace instr
