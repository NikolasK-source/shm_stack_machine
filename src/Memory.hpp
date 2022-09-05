/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include "StackMachine.hpp"

#include "cxxshm.hpp"
#include <cstddef>
#include <string>
#include <vector>

/**
 * @brief abstract class that represents a memory for the stack machine
 */
class Memory {
public:
    /**
     * @brief load/store data types
     */
    enum class dtype_t {
        le1,     //*< bit, cell is little endian
        be1,     //*< bit, cell is little endian
        byte,    //*< byte
        le16,    //*< 16 bit little endian
        le32,    //*< 32 bit little endian
        le32r,   //*< 32 bit little endian (16 bit registers, reversed)
        le64,    //*< 64 bit little endian
        le64r,   //*< 64 bit little endian (16 bit registers, reversed)
        le64r4,  //*< 64 bit little endian (32 bit registers, reversed)
        be16,    //*< 16 bit    big endian
        be32,    //*< 32 bit    big endian
        be32r,   //*< 32 bit    big endian (16 bit registers, reversed)
        be64,    //*< 64 bit    big endian
        be64r,   //*< 64 bit    big endian (16 bit registers, reversed)
        be64r4,  //*< 64 bit    big endian (32 bit registers, reversed)
    };

protected:
    /** protected default constructor */
    Memory() = default;

public:
    /** default destructor */
    virtual ~Memory() = default;

    /**
     * @brief load from memory
     * @param cell memory base cell
     * @param data_type data type to load
     * @return loaded data
     */
    [[nodiscard]] virtual StackMachine::stack_t load(std::size_t cell, dtype_t data_type) const final {
        return load(cell, data_type, 0);
    }

    /**
     * @brief store in memory
     * @param data data to store
     * @param cell memory base cell
     * @param data_type data type to store
     */
    virtual void store(StackMachine::stack_t data, std::size_t cell, dtype_t data_type) final {
        store(data, cell, data_type, 0);
    }

    /**
     * @brief load from memory
     * @param cell memory base cell
     * @param data_type data type to load
     * @param index bit index (only relevant for loading bits)
     * @return loaded data
     */
    [[nodiscard]] virtual StackMachine::stack_t load(std::size_t cell, dtype_t data_type, std::size_t index) const = 0;

    /**
     * @brief store in memory
     * @param data data to store
     * @param cell memory base cell
     * @param data_type data type to store
     * @param index bit index (only relevant for storing bits)
     */
    virtual void store(StackMachine::stack_t data, std::size_t cell, dtype_t data_type, std::size_t index) = 0;
};

/**
 * @brief Class that represents a local memory
 * @details
 *   - cell size is always stack machine word size
 *   - ignores the data type (except mapping)
 */
class MemoryLocal : public Memory {
private:
    std::vector<StackMachine::stack_t> mem; /** memory container*/

public:
    /**
     * @brief create local memory
     * @param size local memory size (in number of cells)
     */
    explicit MemoryLocal(std::size_t size) : mem(size, 0) {}

    [[nodiscard]] StackMachine::stack_t load(std::size_t cell, dtype_t data_type, std::size_t index) const override;
    void store(StackMachine::stack_t data, std::size_t cell, dtype_t data_type, std::size_t index) override;
};

class MemoryReal : public Memory {
private:
    const std::size_t cell_size;  //*< memory cell size

protected:
    /**
     * @brief create real memory
     * @param cell_size memory cell size
     */
    explicit MemoryReal(std::size_t cell_size);

    /**
     * @brief get memory size
     * @return size in bytes
     */
    [[nodiscard]] virtual std::size_t get_size() const = 0;

    /**
     * @brief get pointer to data (const)
     * @param index data byte index
     * @return pointer to data
     */
    [[nodiscard]] virtual const void *get_data(std::size_t index) const = 0;

    /**
     * @brief get pointer to data
     * @param index data byte index
     * @return pointer to data
     */
    [[nodiscard]] virtual void *get_data(std::size_t index) = 0;

public:
    ~MemoryReal() override = default;

    [[nodiscard]] StackMachine::stack_t load(std::size_t cell, dtype_t data_type, std::size_t index) const override;
    void store(StackMachine::stack_t data, std::size_t cell, dtype_t data_type, std::size_t index) override;

private:
    [[nodiscard]] StackMachine::stack_t load_bit(std::size_t cell, std::size_t index, bool little_endian) const;
    [[nodiscard]] StackMachine::stack_t load_byte(std::size_t cell) const;
    [[nodiscard]] StackMachine::stack_t load_16(std::size_t cell, bool little_endian) const;
    [[nodiscard]] StackMachine::stack_t load_32(std::size_t cell, bool little_endian, bool reg_swap) const;
    [[nodiscard]] StackMachine::stack_t load_64(std::size_t cell, bool little_endian, std::size_t reg_swap) const;

    void store_bit(StackMachine::stack_t data, std::size_t cell, std::size_t index, bool little_endian);
    void store_byte(StackMachine::stack_t data, std::size_t cell);
    void store_16(StackMachine::stack_t data, std::size_t cell, bool little_endian);
    void store_32(StackMachine::stack_t data, std::size_t cell, bool little_endian, bool reg_swap);
    void store_64(StackMachine::stack_t data, std::size_t cell, bool little_endian, std::size_t reg_swap);
};

/**
 * @brief class that represents a shared memory
 */
class MemorySHM : public MemoryReal {
private:
    cxxshm::SharedMemory shm;  //*< shared memory instance
public:
    /**
     * @brief create shared memory instance
     * @param name shared memory name
     * @param cell_size memory cell size
     */
    MemorySHM(const std::string &name, std::size_t cell_size) : MemoryReal(cell_size), shm(name) {};

    ~MemorySHM() override = default;

protected:
    [[nodiscard]] size_t      get_size() const override { return shm.get_size(); }
    [[nodiscard]] const void *get_data(std::size_t index) const override {
        return shm.get_addr<const uint8_t *>() + index;
    }
    [[nodiscard]] void *get_data(std::size_t index) override { return shm.get_addr<uint8_t *>() + index; }
};
