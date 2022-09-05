/*
 * Copyright (C) 2020-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Machine.hpp"

#include <iostream>
#include <unistd.h>


int main() {
    Machine machine(32);
    machine.load_file("../examples/toggle_ff.statem");

    machine.init();

    while (true) {
        machine.run();
        sleep(1);
    }
    return 0;
}
