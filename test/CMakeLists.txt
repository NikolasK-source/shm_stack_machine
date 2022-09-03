#
# Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

#
# general test target
#

add_executable(test_${Target} test.cpp)
add_dependencies(test_${Target} ${Target})
add_test(test_${Target} test_${Target})

enable_warnings(test_${Target})
set_definitions(test_${Target})
set_options(test_${Target} FALSE)

if(CLANG_FORMAT)
    target_clangformat_setup(test_${Target})
endif()

#
# statemachine test target
#

add_executable(test_state_machine test_stack_machine.cpp ../src/StackMachine.cpp ../src/StackMachine.hpp)
target_include_directories(test_state_machine PUBLIC ../src)
add_test(test_state_machine test_state_machine)
enable_warnings(test_state_machine)
set_definitions(test_state_machine)
set_options(test_state_machine FALSE)
if(CLANG_FORMAT)
    target_clangformat_setup(test_state_machine)
endif()
