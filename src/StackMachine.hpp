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

private:
    static constexpr std::size_t DEFAULT_MAX_STACK = 4096 / sizeof(stack_t);

    const std::size_t   MAX_STACK;
    std::stack<stack_t> stack;

public:
    /**
     * @brief create stack machine
     * @param max_stack max stack size
     * @exception std::invalid_argument max stack size to small
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
     */
    void dup();

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
     * @brief integer multiplication
     * @exception std::runtime_error to few elements on stack
     */
    void mul();

    /**
     * @brief integer division
     * @exception std::runtime_error to few elements on stack
     */
    void div();

    /**
     * @brief integer modulo
     * @exception std::runtime_error to few elements on stack
     */
    void mod();

    /**
     * @brief integer exponentiation
     * @exception std::runtime_error to few elements on stack
     */
    void pow();

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
