/*
 * Copyright (C) 2020-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Machine.hpp"

#include <iostream>
#include <sysexits.h>

#include "cxxopts.hpp"


int main(int argc, char **argv) {
    cxxopts::Options options(PROJECT_NAME, "Simple stack machine interpreter that can work with shared memory");

    options.add_options()("h,help", "Show usage information", cxxopts::value<bool>()->default_value("false"));
    options.add_options()(
            "v,verbose", "Print what the stack machine executes", cxxopts::value<bool>()->default_value("false"));

    options.add_options()("file", "The file to execute", cxxopts::value<std::string>());

    options.parse_positional({"file"});
    auto opts = options.parse(argc, argv);

    if (!opts.count("file")) {
        std::cerr << "File is mandatory!" << std::endl;
        return EX_USAGE;
    }

    Machine machine(32);
    machine.load_file(opts["file"].as<std::string>());

    machine.init();

    // TODO cycle time

    bool inf = machine.get_cycles() == 0;
    for (std::size_t i = machine.get_cycles(); i || inf; --i) {
        machine.run();
    }

    return 0;
}
