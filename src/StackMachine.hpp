/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include <cstdint>
#include <stack>

class StackMachine {
public:
    typedef uint64_t stack_t;
    typedef int64_t  signed_stack_t;

private:
    static constexpr std::size_t DEFAULT_MAX_STACK = 4096 / sizeof(stack_t);

    const std::size_t   MAX_STACK;
    std::stack<stack_t> stack;

public:
    /**
     * @brief create stack machine
     * @param max_stack max stack size
     * @exception std::invalid_argument max stack size to small
     * @exception std::bad_alloc failed to allocate memory for stack
     */
    explicit StackMachine(std::size_t max_stack = DEFAULT_MAX_STACK);

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Stack instructions                                                                                             */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief push tat to the stack
     * @param data data to push
     * @exception std::runtime_error stack is full
     * @exception std::bad_alloc failed to allocate memory for stack
     */
    void push(stack_t data);

    /**
     * @brief pop data from the stack
     * @return popped data
     * @exception std::runtime_error stack is empty
     */
    stack_t pop();

    /**
     * @brief get top of stack
     * @return top of stack
     * @exception std::runtime_error stack is empty
     */
    [[nodiscard]] stack_t get() const;

    /**
     * @brief duplicate top of stack
     * @exception std::runtime_error stack is empty
     * @exception std::runtime_error stack is full
     * @exception std::bad_alloc failed to allocate memory for stack
     */
    void dup();

    /**
     * @brief clear stack
     * @exception std::bad_alloc failed to allocate memory for stack
     */
    void clr();

    /**
     * @brief get current stack size
     * @return stack size
     */
    inline std::size_t size() const { return stack.size(); }

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Arithmetic instructions                                                                                        */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief integer addition
     * @exception std::runtime_error to few elements on stack
     */
    void add();

    /**
     * @brief integer subtraction
     * @exception std::runtime_error to few elements on stack
     */
    void sub();

    /**
     * @brief integer multiplication (unsigned)
     * @exception std::runtime_error to few elements on stack
     */
    void mul();

    /**
     * @brief integer multiplication (signed)
     * @exception std::runtime_error to few elements on stack
     */
    void muls();

    /**
     * @brief integer division (unsigned)
     * @exception std::runtime_error to few elements on stack
     */
    void div();

    /**
     * @brief integer division (signed)
     * @exception std::runtime_error to few elements on stack
     */
    void divs();

    /**
     * @brief integer modulo (unsigned)
     * @exception std::runtime_error to few elements on stack
     */
    void mod();

    /**
     * @brief integer modulo (signed)
     * @exception std::runtime_error to few elements on stack
     */
    void mods();

    /**
     * @brief integer exponentiation (unsigned)
     * @exception std::runtime_error to few elements on stack
     */
    void pow();

    /**
     * @brief integer exponentiation (signed)
     * @exception std::runtime_error to few elements on stack
     */
    void pows();

    /**
     * @brief float addition
     * @exception std::runtime_error to few elements on stack
     */
    void addf();

    /**
     * @brief float subtraction
     * @exception std::runtime_error to few elements on stack
     */
    void subf();

    /**
     * @brief float multiplication
     * @exception std::runtime_error to few elements on stack
     */
    void mulf();

    /**
     * @brief float division
     * @exception std::runtime_error to few elements on stack
     */
    void divf();

    /**
     * @brief float exponentiation
     * @exception std::runtime_error to few elements on stack
     */
    void powf();

    /**
     * @brief double addition
     * @exception std::runtime_error to few elements on stack
     */
    void addd();

    /**
     * @brief double subtraction
     * @exception std::runtime_error to few elements on stack
     */
    void subd();

    /**
     * @brief double multiplication
     * @exception std::runtime_error to few elements on stack
     */
    void muld();

    /**
     * @brief double division
     * @exception std::runtime_error to few elements on stack
     */
    void divd();

    /**
     * @brief float exponentiation
     * @exception std::runtime_error to few elements on stack
     */
    void powd();

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Logic instructions                                                                                             */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief logic inversion (NOT)
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void linv();

    /**
     * @brief logic and
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void land();

    /**
     * @brief logic or
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void lor();

    /**
     * @brief logic exclusive or
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void lxor();

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Bitwise instructions                                                                                           */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief bitwise inversion
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void binv();

    /**
     * @brief bitwise and
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void band();

    /**
     * @brief bitwise or
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void bor();

    /**
     * @brief bitwise exclusive or
     * @details any nonzero value is treated as true
     * @exception std::runtime_error to few elements on stack
     */
    void bxor();

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Data conversion instructions                                                                                   */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief convert int to float
     * @exception std::runtime_error to few elements on stack
     */
    void itof();

    /**
     * @brief convert int to double
     * @exception std::runtime_error to few elements on stack
     */
    void itod();

    /**
     * @brief convert float to int
     * @exception std::runtime_error to few elements on stack
     */
    void ftoi();

    /**
     * @brief convert double to int
     * @exception std::runtime_error to few elements on stack
     */
    void dtoi();

    /**
     * @brief convert float to double
     * @exception std::runtime_error to few elements on stack
     */
    void ftod();

    /**
     * @brief convert double to float
     * @exception std::runtime_error to few elements on stack
     */
    void dtof();

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Relational instructions                                                                                        */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief check equal
     * @exception std::runtime_error to few elements on stack
     */
    void eq();

    /**
     * @brief check not equal
     * @exception std::runtime_error to few elements on stack
     */
    void ne();

    /**
     * @brief check less than (unsigned integer)
     * @exception std::runtime_error to few elements on stack
     */
    void lt();

    /**
     * @brief check greater than (unsigned integer)
     * @exception std::runtime_error to few elements on stack
     */
    void gt();

    /**
     * @brief check less than or equal (unsigned integer)
     * @exception std::runtime_error to few elements on stack
     */
    void le();

    /**
     * @brief check greater than or equal (unsigned integer)
     * @exception std::runtime_error to few elements on stack
     */
    void ge();

    /**
     * @brief check less than (signed integer)
     * @exception std::runtime_error to few elements on stack
     */
    void lts();

    /**
     * @brief check greater than (signed integer)
     * @exception std::runtime_error to few elements on stack
     */
    void gts();

    /**
     * @brief check less than or equal (signed integer)
     * @exception std::runtime_error to few elements on stack
     */
    void les();

    /**
     * @brief check greater than or equal (signed integer)
     * @exception std::runtime_error to few elements on stack
     */
    void ges();

    /**
     * @brief check less than (64 bit float)
     * @exception std::runtime_error to few elements on stack
     */
    void ltd();

    /**
     * @brief check greater than (64 bit float)
     * @exception std::runtime_error to few elements on stack
     */
    void gtd();

    /**
     * @brief check less than or equal (64 bit float)
     * @exception std::runtime_error to few elements on stack
     */
    void led();

    /**
     * @brief check greater than or equal (64 bit float)
     * @exception std::runtime_error to few elements on stack
     */
    void ged();

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* Floating point math instructions                                                                               */
    /******************************************************************************************************************/
    /******************************************************************************************************************/

    /**
     * @brief get absolute value
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void abs();

    /**
     * @brief get square root
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void sqrt();

    /**
     * @brief get cubic root
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void cbrt();

    /**
     * @brief get the natural (base e) logarithm
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void ln();

    /**
     * @brief get the common (base 10) logarithm
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void log();

    /**
     * @brief get the binary (base 2) logarithm
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void lg();

    /**
     * @brief get sine
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void sin();

    /**
     * @brief get cosines
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void cos();

    /**
     * @brief get tangent
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void tan();

    /**
     * @brief get the principal value of the arc sine
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void asin();

    /**
     * @brief get the principal value of the arc cosine
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void acos();

    /**
     * @brief get the principal value of the arc tangent
     * @details 64 float only
     * @exception std::runtime_error to few elements on stack
     */
    void atan();

    /**
     * @brief Computes the arc tangent of x an y
     * @details 64 float only
     *      X is left operand
     *      Y is right operand (tos)
     * @exception std::runtime_error to few elements on stack
     */
    void atanxy();

private:
    /**
     * @brief checks if the condition for an arithmetic operation is fulfilled (at least two values on the stack)
     * @exception std::runtime_error to few elements on stack
     */
    void check_arith() const;

    /**
     * @brief checks if the condition for an conversion operation is fulfilled (at least one value on the stack)
     * @exception std::runtime_error to few elements on stack
     */
    void check_conv() const;


private:
    /**
     * @brief pop data from the stack
     * @details like pop, but without range check
     * @return popped data
     */
    stack_t _pop();
};
