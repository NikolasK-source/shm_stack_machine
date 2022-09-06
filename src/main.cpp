/*
 * Copyright (C) 2020-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Machine.hpp"

#include "cxxitimer.hpp"
#include "cxxopts.hpp"
#include "cxxsignal.hpp"
#include "license.hpp"
#include "time_str.hpp"
#include <filesystem>
#include <iostream>
#include <sysexits.h>
#include <thread>

static volatile bool terminate = false;

class TerminateHandler final : public cxxsignal::SignalHandler {
public:
    explicit TerminateHandler(int signal_number) : cxxsignal::SignalHandler(signal_number) {}

    void handler(int signal_number, siginfo_t *, ucontext_t *) override { terminate = true; }
};

class CycleTimeWarning final : public cxxsignal::SignalHandler {
private:
    bool waiting;

public:
    explicit CycleTimeWarning(int signal_number) : cxxsignal::SignalHandler(signal_number), waiting(false) {}

    void handler(int signal_number, siginfo_t *, ucontext_t *context) override {
        if (!waiting) std::cerr << now_str() << " WARNING: cycle time exceeded" << std::endl;
    }

    bool _wait() {
        waiting  = true;
        auto ret = wait();
        waiting  = false;
        return ret;
    }
};

int main(int argc, char **argv) {
    const std::string exe_name = std::filesystem::path(argv[0]).filename().string();
    cxxopts::Options  options(PROJECT_NAME, "Simple stack machine emulator that can work with shared memory");

    auto exit_usage = [&exe_name]() {
        std::cerr << "Use '" << exe_name << " --help' for more information." << std::endl;
        return EX_USAGE;
    };

    options.add_options()("s,stack-size", "Machine stack size (default: 32)", cxxopts::value<std::size_t>());
    options.add_options()("h,help", "Show usage information");
    options.add_options()("d,debug", "Print what the stack machine executes");
    options.add_options()("v,verbose", "Print program status information");
    options.add_options()("version", "print version information");
    options.add_options()("license", "show licences");

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
        std::cout << "  - cxxitimer (https://github.com/NikolasK-source/cxxitimer)" << std::endl;
        return EX_OK;
    }

    // print usage
    if (opts.count("version")) {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << " (compiled with " << COMPILER_INFO << " on "
                  << SYSTEM_INFO << ')' << std::endl;
        return EX_OK;
    }

    // print licenses
    if (opts.count("license")) {
        print_licenses(std::cout);
        return EX_OK;
    }

    if (!opts.count("file")) {
        std::cerr << "File is mandatory!" << std::endl;
        return EX_USAGE;
    }

    TerminateHandler sigint_handler(SIGINT);
    TerminateHandler sigterm_handler(SIGTERM);
    TerminateHandler sigquit_handler(SIGQUIT);

    try {
        sigint_handler.establish();
        sigterm_handler.establish();
        sigquit_handler.establish();
    } catch (const std::exception &e) {
        std::cerr << now_str() << " ERROR: " << e.what() << std::endl;
        return EX_OSERR;
    }

    std::size_t stack_size = 32;
    if (opts.count("stack-size")) { stack_size = opts["stack-size"].as<std::size_t>(); }

    std::unique_ptr<Machine> machine;
    try {
        machine = std::make_unique<Machine>(stack_size, opts.count("verbose"), opts.count("debug"));
    } catch (const std::exception &e) {
        std::cerr << now_str() << " ERROR: " << e.what() << std::endl;
        return EX_USAGE;
    }

    try {
        machine->load_file(opts["file"].as<std::string>());
    } catch (const std::exception &e) {
        std::cerr << now_str() << " ERROR: " << e.what() << std::endl;
        return EX_DATAERR;
    }

    try {
        machine->init();
    } catch (const std::exception &e) {
        std::cerr << now_str() << " ERROR: " << e.what() << std::endl;
        return EX_DATAERR;
    }

    const auto cycle_ms = machine->get_cycle_time_ms();

    CycleTimeWarning       timer_handler(SIGALRM);
    cxxitimer::ITimer_Real timer(static_cast<double>(cycle_ms) / 1000.0);

    try {
        timer_handler.establish();
        timer.start();
    } catch (const std::exception &e) {
        std::cerr << now_str() << " ERROR: " << e.what() << std::endl;
        return EX_OSERR;
    }

    bool inf = machine->get_cycles() == 0;
    for (std::size_t i = machine->get_cycles(); (i || inf) && !terminate; --i) {
        try {
            machine->run();
        } catch (const std::exception &e) {
            std::cerr << now_str() << " ERROR: execution failed: " << e.what() << std::endl;
            return EX_DATAERR;
        }

        try {
            timer_handler._wait();
        } catch (const std::exception &e) {
            if (terminate) break;
            std::cerr << now_str() << "ERROR: " << e.what() << std::endl;
            return EX_OSERR;
        }
    }

    return 0;
}
