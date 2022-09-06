/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Memory.hpp"

#include "cxxendian/endian.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

StackMachine::stack_t MemoryLocal::load(std::size_t cell, Memory::dtype_t, std::size_t) const {
    if (cell >= mem.size()) throw std::out_of_range("memory cell out of range");
    return mem[cell];
}

void MemoryLocal::store(StackMachine::stack_t data, std::size_t cell, Memory::dtype_t data_type, std::size_t) {
    if (cell >= mem.size()) throw std::out_of_range("memory cell out of range");

    StackMachine::stack_t mask;
    switch (data_type) {
        case Memory::dtype_t::le1:
        case Memory::dtype_t::be1: mask = 0x1; break;
        case Memory::dtype_t::byte: mask = 0xFF; break;
        case Memory::dtype_t::le16:
        case Memory::dtype_t::be16: mask = 0xFFFF; break;
        case Memory::dtype_t::le32:
        case Memory::dtype_t::be32:
        case Memory::dtype_t::le32r:
        case Memory::dtype_t::be32r: mask = 0xFFFFFFFF; break;
        case Memory::dtype_t::le64:
        case Memory::dtype_t::be64:
        case Memory::dtype_t::le64r:
        case Memory::dtype_t::be64r:
        case Memory::dtype_t::le64r4:
        case Memory::dtype_t::be64r4: mask = 0xFFFFFFFFFFFFFFFF; break;
    }

    mem[cell] = data & mask;
}

MemoryReal::MemoryReal(std::size_t cell_size) : Memory(), cell_size(cell_size) {
    switch (cell_size) {
        case 1:
        case 2:
        case 4:
        case 8: break;
        default: throw std::invalid_argument("invalid cell size");
    }
}

StackMachine::stack_t MemoryReal::load(std::size_t cell, Memory::dtype_t data_type, std::size_t index) const {
    switch (data_type) {
        case Memory::dtype_t::le1: return load_bit(cell, index, true);
        case Memory::dtype_t::be1: return load_bit(cell, index, false);
        case Memory::dtype_t::byte: return load_byte(cell);
        case Memory::dtype_t::le16: return load_16(cell, true);
        case Memory::dtype_t::be16: return load_16(cell, false);
        case Memory::dtype_t::le32: return load_32(cell, true, false);
        case Memory::dtype_t::be32: return load_32(cell, false, false);
        case Memory::dtype_t::le32r: return load_32(cell, true, true);
        case Memory::dtype_t::be32r: return load_32(cell, false, true);
        case Memory::dtype_t::le64: return load_64(cell, true, 0);
        case Memory::dtype_t::be64: return load_64(cell, false, 0);
        case Memory::dtype_t::le64r: return load_64(cell, true, 2);
        case Memory::dtype_t::be64r: return load_64(cell, false, 2);
        case Memory::dtype_t::le64r4: return load_64(cell, true, 4);
        case Memory::dtype_t::be64r4: return load_64(cell, false, 4);
    }
}

void MemoryReal::store(StackMachine::stack_t data, std::size_t cell, Memory::dtype_t data_type, std::size_t index) {
    switch (data_type) {
        case Memory::dtype_t::le1: return store_bit(data, cell, index, true);
        case Memory::dtype_t::be1: return store_bit(data, cell, index, false);
        case Memory::dtype_t::byte: return store_byte(data, cell);
        case Memory::dtype_t::le16: return store_16(data, cell, true);
        case Memory::dtype_t::be16: return store_16(data, cell, false);
        case Memory::dtype_t::le32: return store_32(data, cell, true, false);
        case Memory::dtype_t::be32: return store_32(data, cell, false, false);
        case Memory::dtype_t::le32r: return store_32(data, cell, true, true);
        case Memory::dtype_t::be32r: return store_32(data, cell, false, true);
        case Memory::dtype_t::le64: return store_64(data, cell, true, 0);
        case Memory::dtype_t::be64: return store_64(data, cell, false, 0);
        case Memory::dtype_t::le64r: return store_64(data, cell, true, 2);
        case Memory::dtype_t::be64r: return store_64(data, cell, false, 2);
        case Memory::dtype_t::le64r4: return store_64(data, cell, true, 4);
        case Memory::dtype_t::be64r4: return store_64(data, cell, false, 4);
    }
}

StackMachine::stack_t MemoryReal::load_bit(std::size_t cell, std::size_t index, bool little_endian) const {
    if (cell >= get_size() / cell_size) throw std::out_of_range("memory cell out of range");
    if (index >= cell * 8) throw std::out_of_range("bit index out of range");

    switch (cell_size) {
        case 1: {
            uint8_t data = *reinterpret_cast<const uint8_t *>(get_data(cell * cell_size));
            uint8_t ret  = (data >> index) & 0x1;
            return ret;
        }
        case 2: {
            uint16_t data = *reinterpret_cast<const uint8_t *>(get_data(cell * cell_size));
            if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);
            uint16_t ret = (data >> index) & 0x1;
            return ret;
        }
        case 4: {
            uint32_t data = *reinterpret_cast<const uint32_t *>(get_data(cell * cell_size));
            if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);
            uint32_t ret = (data >> index) & 0x1;
            return ret;
        }

        case 8: {
            uint64_t data = *reinterpret_cast<const uint64_t *>(get_data(cell * cell_size));
            if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);
            uint64_t ret = (data >> index) & 0x1;
            return ret;
        }
    }

    throw std::runtime_error("internal program error: cell size is invalid");
}

StackMachine::stack_t MemoryReal::load_byte(std::size_t cell) const {
    if (cell_size != 1) throw std::runtime_error("Memory cell size is to large to load 8 bit.");
    if (cell >= get_size()) throw std::out_of_range("memory cell out of range");
    return *reinterpret_cast<const uint8_t *>(get_data(cell * cell_size));
}

StackMachine::stack_t MemoryReal::load_16(std::size_t cell, bool little_endian) const {
    if (cell_size > 2) throw std::runtime_error("Memory cell size is to large to load 16 bit.");
    if ((cell * cell_size + 1) >= get_size()) throw std::out_of_range("memory cell out of range");

    auto data = *reinterpret_cast<const uint16_t *>(get_data(cell * cell_size));
    if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);
    return data;
}

StackMachine::stack_t MemoryReal::load_32(std::size_t cell, bool little_endian, bool reg_swap) const {
    if (cell_size > 4) throw std::runtime_error("Memory cell size is to large to load 16 bit.");
    if ((cell * cell_size + 3) >= get_size()) throw std::out_of_range("memory cell out of range");

    union {
        uint32_t data;
        uint16_t reg16[2];
    };
    data = *reinterpret_cast<const uint32_t *>(get_data(cell * cell_size));
    if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);
    if (reg_swap) std::swap(reg16[0], reg16[1]);
    return data;
}

StackMachine::stack_t MemoryReal::load_64(std::size_t cell, bool little_endian, std::size_t reg_swap) const {
    if (cell_size > 4) throw std::runtime_error("Memory cell size is to large to load 16 bit.");
    if ((cell * cell_size + 7) >= get_size()) throw std::out_of_range("memory cell out of range");

    union {
        uint64_t data;
        uint16_t reg16[4];
        uint32_t reg32[2];
    };
    data = *reinterpret_cast<const uint32_t *>(get_data(cell * cell_size));
    if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);
    if (reg_swap == 4) std::swap(reg32[0], reg32[1]);
    else if (reg_swap == 2) {
        std::swap(reg16[0], reg16[3]);
        std::swap(reg16[1], reg16[2]);
    }
    return data;
}
void MemoryReal::store_bit(StackMachine::stack_t data, std::size_t cell, std::size_t index, bool little_endian) {
    if (cell >= get_size() / cell_size) throw std::out_of_range("memory cell out of range");
    if (index >= cell * 8) throw std::out_of_range("bit index out of range");

    bool store_bit = data != 0;

    switch (cell_size) {
        case 1: {
            typedef uint8_t mem_t;
            mem_t           data = *reinterpret_cast<mem_t *>(get_data(cell * cell_size));  // read data
            data &= ~(0x1 << index);                                                        // mask bit
            if (store_bit) data |= 0x1 << index;                                            // set bit
            *reinterpret_cast<mem_t *>(get_data(cell * cell_size)) = data;                  // write back
            break;
        }
        case 2: {
            typedef uint16_t mem_t;
            mem_t            data = *reinterpret_cast<mem_t *>(get_data(cell * cell_size));  // read data
            if (endian::HostEndianness.isLittle() != little_endian)
                data = endian::swap(data);        // change endianness to host
            data &= ~(0x1 << index);              // mask bit
            if (store_bit) data |= 0x1 << index;  // set bit
            if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);  // revert endianness
            *reinterpret_cast<mem_t *>(get_data(cell * cell_size)) = data;                      // write back
            break;
        }
        case 4: {
            typedef uint32_t mem_t;
            mem_t            data = *reinterpret_cast<mem_t *>(get_data(cell * cell_size));  // read data
            if (endian::HostEndianness.isLittle() != little_endian)
                data = endian::swap(data);        // change endianness to host
            data &= ~(0x1 << index);              // mask bit
            if (store_bit) data |= 0x1 << index;  // set bit
            if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);  // revert endianness
            *reinterpret_cast<mem_t *>(get_data(cell * cell_size)) = data;                      // write back
            break;
        }

        case 8: {
            typedef uint64_t mem_t;
            mem_t            data = *reinterpret_cast<mem_t *>(get_data(cell * cell_size));  // read data
            if (endian::HostEndianness.isLittle() != little_endian)
                data = endian::swap(data);        // change endianness to host
            data &= ~(0x1 << index);              // mask bit
            if (store_bit) data |= 0x1 << index;  // set bit
            if (endian::HostEndianness.isLittle() != little_endian) data = endian::swap(data);  // revert endianness
            *reinterpret_cast<mem_t *>(get_data(cell * cell_size)) = data;                      // write back
            break;
        }

        default: throw std::runtime_error("internal program error: cell size is invalid");
    }
}

void MemoryReal::store_byte(StackMachine::stack_t data, std::size_t cell) {
    if (cell_size != 1) throw std::runtime_error("Memory cell size is to large to store 8 bit.");
    if (cell >= get_size()) throw std::out_of_range("memory cell out of range");
    *reinterpret_cast<uint8_t *>(get_data(cell * cell_size)) = static_cast<uint8_t>(data);
}

void MemoryReal::store_16(StackMachine::stack_t data, std::size_t cell, bool little_endian) {
    if (cell_size > 2) throw std::runtime_error("Memory cell size is to large to store 16 bit.");
    if ((cell * cell_size + 1) >= get_size()) throw std::out_of_range("memory cell out of range");

    auto d = static_cast<uint16_t>(data);
    if (endian::HostEndianness.isLittle() != little_endian) d = endian::swap(d);
    *reinterpret_cast<uint16_t *>(get_data(cell * cell_size)) = d;
}

void MemoryReal::store_32(StackMachine::stack_t data, std::size_t cell, bool little_endian, bool reg_swap) {
    if (cell_size > 4) throw std::runtime_error("Memory cell size is to large to store 16 bit.");
    if ((cell * cell_size + 3) >= get_size()) throw std::out_of_range("memory cell out of range");

    union {
        uint32_t d;
        uint16_t reg16[2];
    };

    d = static_cast<uint32_t>(data);
    if (endian::HostEndianness.isLittle() != little_endian) d = endian::swap(d);
    if (reg_swap) std::swap(reg16[0], reg16[1]);
    *reinterpret_cast<uint32_t *>(get_data(cell * cell_size)) = d;
}

void MemoryReal::store_64(StackMachine::stack_t data, std::size_t cell, bool little_endian, std::size_t reg_swap) {
    if (cell_size > 4) throw std::runtime_error("Memory cell size is to large to store 16 bit.");
    if ((cell * cell_size + 7) >= get_size()) throw std::out_of_range("memory cell out of range");

    union {
        uint64_t d;
        uint16_t reg16[4];
        uint16_t reg32[2];
    };

    d = static_cast<uint64_t>(data);
    if (endian::HostEndianness.isLittle() != little_endian) d = endian::swap(d);
    if (reg_swap == 4) std::swap(reg32[0], reg32[1]);
    else if (reg_swap == 2) {
        std::swap(reg16[0], reg16[3]);
        std::swap(reg16[1], reg16[2]);
    }
    *reinterpret_cast<uint64_t *>(get_data(cell * cell_size)) = d;
}
