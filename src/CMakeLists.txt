#
# Copyright (C) 2020-2022 Nikolas Koesling <nikolas@koesling.info>.
# This template is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

# ---------------------------------------- source files (*.cpp, *.cc, ...) ---------------------------------------------
# ======================================================================================================================

target_sources(${Target} PRIVATE main.cpp)
target_sources(${Target} PRIVATE StackMachine.cpp)
target_sources(${Target} PRIVATE Memory.cpp)
target_sources(${Target} PRIVATE Machine.cpp)
target_sources(${Target} PRIVATE instruction.cpp)
target_sources(${Target} PRIVATE special_instructions.cpp)
target_sources(${Target} PRIVATE time_str.cpp)


# ---------------------------------------- header files (*.jpp, *.h, ...) ----------------------------------------------
# ======================================================================================================================

target_sources(${Target} PRIVATE StackMachine.hpp)
target_sources(${Target} PRIVATE Memory.hpp)
target_sources(${Target} PRIVATE Machine.hpp)
target_sources(${Target} PRIVATE instruction.hpp)
target_sources(${Target} PRIVATE special_instructions.hpp)
target_sources(${Target} PRIVATE time_str.hpp)


# ---------------------------------------- subdirectories --------------------------------------------------------------
# ======================================================================================================================
