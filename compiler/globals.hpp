#pragma once

#include <string>
#include <vector>

struct GlobalArgs {
    static GlobalArgs& instance() {
        static GlobalArgs inst;
        return inst;
    }

    void set(int argc, char** argv) {
        this->argc = argc;
        args.clear();
        for(int i = 0; i < argc; ++i)
            args.emplace_back(argv[i]);
    }

    int argc = 0;
    std::vector<std::string> args;

private:
    GlobalArgs() = default;
    ~GlobalArgs() = default;
    GlobalArgs(const GlobalArgs&) = delete;
    GlobalArgs& operator=(const GlobalArgs&) = delete;
};
