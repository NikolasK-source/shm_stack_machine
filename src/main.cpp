/*
 * Copyright (C) 2020-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Machine.hpp"

#include <iostream>
#include <sysexits.h>
#include <unistd.h>

#include "cxxopts.hpp"


int main(int argc, char **argv) {
    cxxopts::Options options(PROJECT_NAME, "Simple stack machine interpreter that can work with shared memory");

    options.add_options()("h,help", "Show usage information", cxxopts::value<bool>()->default_value("false"));
    options.add_options()("d,debug", "Print what the stack machine executes");
    options.add_options()("v,verbose", "Print program status information");

    options.add_options()("file", "The file to execute", cxxopts::value<std::string>());

    options.parse_positional({"file"});
    options.positional_help("PROGRAM_FILE");

    auto opts = options.parse(argc, argv);

    // print usage
    if (opts.count("help")) {
        options.set_width(120);
        std::cout << options.help() << std::endl;
        std::cout << std::endl;
        std::cout << "This application uses the following libraries:" << std::endl;
        std::cout << "  - cxxopts by jarro2783 (https://github.com/jarro2783/cxxopts)" << std::endl;
        std::cout << "  - cxxshm (https://github.com/NikolasK-source/cxxshm)" << std::endl;
        std::cout << "  - cxxendian (https://github.com/NikolasK-source/cxxendian)" << std::endl;
        std::cout << "  - cxxsignal (https://github.com/NikolasK-source/cxxsignal)" << std::endl;
        return EX_OK;
    }

    if (!opts.count("file")) {
        std::cerr << "File is mandatory!" << std::endl;
        return EX_USAGE;
    }

    Machine machine(32, opts.count("verbose"), opts.count("debug"));

    try {
        machine.load_file(opts["file"].as<std::string>());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EX_DATAERR;
    }

    try {
        machine.init();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EX_DATAERR;
    }

    // TODO cycle time

    bool inf = machine.get_cycles() == 0;
    for (std::size_t i = machine.get_cycles(); i || inf; --i) {
        machine.run();
        sleep(1);
    }

    return 0;
}
