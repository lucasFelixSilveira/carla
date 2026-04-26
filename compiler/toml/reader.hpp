#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include <tuple>
#include <filesystem>

#include "../compiler_outputs.hpp"

struct TOMLReader {
    struct Expr {
        std::string nspace;
        std::string field;
        Expr(std::string nspace, std::string field)
            : nspace(std::move(nspace)), field(std::move(field)) {}
    };

    using Error = std::tuple<std::string, int>;

    using Values = std::variant<
        bool,
        int,
        std::string,
        std::vector<int>,
        std::vector<std::string>,
        Error
    >;

    std::string iam;
    std::string filename;
    std::string ctx;
    Error block = { "", 0 };

    TOMLReader(std::string iam, const std::string& filename)
        : iam(std::move(iam)), filename(filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            block = { "File can't be opened", -1 };
            return;
        }

        std::stringstream ss;
        ss << file.rdbuf();
        ctx = ss.str();

        if (ctx.empty()) block = { "Empty file", -2 };
    }

    Values get(Expr expr) {
        if (std::get<1>(block) != 0) return block;

        std::istringstream stream(ctx);
        std::string line;
        std::string current_ns;

        while (std::getline(stream, line)) {
            trim(line);
            if (line.empty() || line[0] == '#') continue;

            // namespace
            if (line.front() == '[' && line.back() == ']') {
                current_ns = line.substr(1, line.size() - 2);
                trim(current_ns);
                continue;
            }

            if (current_ns != expr.nspace &&
                current_ns != (iam + "." + expr.nspace))
                continue;

            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            trim(key);
            trim(value);

            if (key != expr.field) continue;

            return parse_value(value);
        }

        return Error{ "Field not found", 1 };
    }

    std::string getrealtype(Values field) {
        if (std::holds_alternative<std::string>(field)) return "string";
        if (std::holds_alternative<int>(field)) return "integer";
        if (std::holds_alternative<bool>(field)) return "boolean";
        if (std::holds_alternative<std::vector<std::string>>(field)) return "array of strings";
        if (std::holds_alternative<std::vector<int>>(field)) return "array of integers";
        if (std::holds_alternative<Error>(field)) return "error";
        return "unknown";
    }

    template<typename T>
    T check(std::string field, Values value) {
        if (std::holds_alternative<T>(value))
            return std::get<T>(value);

        CompilerOutputs::Fatal(
            "Expected " + getrealtype(T()) +
            " in " + filename +
            " (" + field + "), got " +
            getrealtype(value)
        );

        return T();
    }

private:
    static void trim(std::string& s) {
        size_t start = s.find_first_not_of(" \t\r");
        size_t end = s.find_last_not_of(" \t\r");

        if (start == std::string::npos) {
            s.clear();
            return;
        }

        s = s.substr(start, end - start + 1);
    }

    Values parse_value(const std::string& v) {
        if (v == "true") return true;
        if (v == "false") return false;

        // string
        if (v.size() >= 2 &&
            ((v.front() == '"' && v.back() == '"') ||
             (v.front() == '\'' && v.back() == '\''))) {
            return v.substr(1, v.size() - 2);
        }

        // array
        if (!v.empty() && v.front() == '[' && v.back() == ']') {
            std::string inner = v.substr(1, v.size() - 2);
            std::vector<std::string> items;
            std::stringstream ss(inner);
            std::string item;

            while (std::getline(ss, item, ',')) {
                trim(item);
                items.push_back(item);
            }

            // tenta converter pra int
            try {
                std::vector<int> ints;
                for (auto& i : items)
                    ints.push_back(std::stoi(i));
                return ints;
            } catch (...) {
                return items;
            }
        }

        // int
        try {
            return std::stoi(v);
        } catch (...) {}

        return v;
    }
};
