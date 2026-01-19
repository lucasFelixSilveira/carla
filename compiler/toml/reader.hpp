#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <filesystem>
#include <vector>
#include <tuple>
#include <sstream>

#include "../compiler_outputs.hpp"

struct TOMLReader {
    struct Expr {
        std::string nspace;
        std::string field;
        Expr(std::string nspace, std::string field) : nspace(std::move(nspace)), field(std::move(field)) {}
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
    std::string path;
    std::string filename;
    std::string ctx;
    Error block = { std::string(), 0 };

    TOMLReader(std::string iam, const std::string& filename) : iam(std::move(iam)), filename(filename) {
        std::filesystem::path cwd = std::filesystem::current_path();
        this->path = (cwd / filename).string();

        auto [err, code] = this->getctx();
        if( code != 0 ) this->block = { err, code };
    }

    TOMLReader::Values get(TOMLReader::Expr expr);


    std::string getrealtype(TOMLReader::Values field) {
        std::string err = "error when parse";
        if( std::holds_alternative<std::string>(field) ) return "string";
        if( std::holds_alternative<int>(field) ) return "integer";
        if( std::holds_alternative<bool>(field) ) return "boolean";
        if( std::holds_alternative<std::vector<std::string>>(field) ) return "array of strings";
        if( std::holds_alternative<Error>(field) ) return Colorizer::BOLD_RED + err + Colorizer::RESET;
        return "unknown";
    }

    template<typename T>
    T check(std::string field, TOMLReader::Values value) {
        if( std::holds_alternative<T>(value) ) return std::get<T>(value);

        if constexpr(std::is_same_v<T, std::string>) {
            CompilerOutputs::Fatal("Has been expected string in " + this->filename + " ("+ field + ") but has received " + getrealtype(value));
        } else if constexpr(std::is_same_v<T, int>) {
            CompilerOutputs::Fatal("Has been expected integer in " + this->filename + " ("+ field + ") but has received " + getrealtype(value));
        } else if constexpr(std::is_same_v<T, bool>) {
            CompilerOutputs::Fatal("Has been expected boolean in " + this->filename + " ("+ field + ") but has received " + getrealtype(value));
        } else if constexpr(std::is_same_v<T, std::vector<std::string>>) {
            CompilerOutputs::Fatal("Has been expected array of strings in " + this->filename + " ("+ field + ") but has received " + getrealtype(value));
        }

        return T();
    }

private:
    Error getctx() {
        std::ifstream file(this->path, std::ios::binary | std::ios::ate);
        if(! file.is_open() ) return { "File can't be opened", -1 };

        std::streamsize size = file.tellg();
        if( size <= 0 ) return { "Empty file", -2 };

        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size + 1);
        if(! file.read(buffer.data(), size) ) return { "Failed to read file", -3 };
        buffer[size] = '\0';

        this->ctx = std::string(buffer.data(), size);
        return { std::string(), 0 };
    }

    std::tuple<bool, int> is_namespace(size_t i) const {
        if( i >= ctx.length() || ctx[i] != '[' ) return { false, 0 };

        size_t start = i;
        i++;

        while( i < ctx.length() && ctx[i] != ']' && ctx[i] != '\n' ) i++;
        if( i < ctx.length() && ctx[i] == ']' ) return { true, static_cast<int>(i - start + 1) };
        return { false, 0 };
    }

    size_t skip_comment(size_t i) const {
        if(ctx[i] == '#')
        /* -> */ while (i < ctx.length() && ctx[i] != '\n') i++;
        return i;
    }

    Values parse_array(const std::string& value_str) {
        std::string trimmed = value_str;

        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

        if( trimmed.length() >= 2 && trimmed[0] == '[' && trimmed.back() == ']' ) {
            std::string inner = trimmed.substr(1, trimmed.length() - 2);

            inner.erase(0, inner.find_first_not_of(" \t"));
            inner.erase(inner.find_last_not_of(" \t") + 1);

            if( inner.empty() ) return std::vector<std::string>();

            size_t first_comma = inner.find(',');
            std::string first_element = inner.substr(0, first_comma);

            first_element.erase(0, first_element.find_first_not_of(" \t"));
            first_element.erase(first_element.find_last_not_of(" \t") + 1);

            bool is_quoted_string = (
                first_element.length() >= 2 && (
                    (
                        first_element[0] == '"' && first_element.back() == '"'
                    ) || (
                        first_element[0] == '\'' && first_element.back() == '\''
                    )
                )
            );

            if( is_quoted_string ) {
                std::vector<std::string> string_array;
                std::stringstream ss(inner);
                std::string item;
                bool inside_quotes = false;
                char quote_char = '\0';
                std::string current_item;

                for( size_t idx = 0; idx < inner.length(); idx++ ) {
                    char c = inner[idx];

                    if(! inside_quotes && (c == '"' || c == '\'') ) {
                        inside_quotes = true;
                        quote_char = c;
                        current_item += c;
                    }
                    else if( inside_quotes && c == quote_char ) {
                        bool escaped = false;
                        if( idx > 0 && inner[idx-1] == '\\' ) {
                            int backslash_count = 0;
                            for( int j = idx-1; j >= 0 && inner[j] == '\\'; j-- ) backslash_count++;
                            escaped = (backslash_count % 2 == 1);
                        }

                        if (!escaped) inside_quotes = false;
                        current_item += c;
                    }
                    else if (! inside_quotes && c == ',' ) {
                        if(! current_item.empty() ) {
                            current_item.erase(0, current_item.find_first_not_of(" \t"));
                            current_item.erase(current_item.find_last_not_of(" \t") + 1);
                            if( current_item.length() >= 2 && (
                                current_item[0] == '"' || current_item[0] == '\''
                            )) {
                                current_item = current_item.substr(1, current_item.length() - 2);
                                std::string processed;
                                for( size_t j = 0; j < current_item.length(); j++ ) {
                                    if (current_item[j] == '\\' && j + 1 < current_item.length())
                                        switch (current_item[++j]) {
                                            case 'n': processed += '\n'; break;
                                            case 't': processed += '\t'; break;
                                            case 'r': processed += '\r'; break;
                                            case '\\': processed += '\\'; break;
                                            case '"': processed += '"'; break;
                                            case '\'': processed += '\''; break;
                                            default: processed += current_item[j]; break;
                                        }
                                    else processed += current_item[j];
                                }
                                string_array.push_back(processed);
                            }
                            current_item.clear();
                        }
                    } else {
                        current_item += c;
                    }
                }

                if(! current_item.empty() ) {
                    current_item.erase(0, current_item.find_first_not_of(" \t"));
                    current_item.erase(current_item.find_last_not_of(" \t") + 1);
                    if( current_item.length() >= 2 && (
                        current_item[0] == '"' || current_item[0] == '\''
                    )) {
                        current_item = current_item.substr(1, current_item.length() - 2);
                        std::string processed;
                        for( size_t j = 0; j < current_item.length(); j++ ) {
                            if( current_item[j] == '\\' && j + 1 < current_item.length() )
                                switch(current_item[++j]) {
                                    case 'n': processed += '\n'; break;
                                    case 't': processed += '\t'; break;
                                    case 'r': processed += '\r'; break;
                                    case '\\': processed += '\\'; break;
                                    case '"': processed += '"'; break;
                                    case '\'': processed += '\''; break;
                                    default: processed += current_item[j]; break;
                                }
                            else processed += current_item[j];
                        }
                        string_array.push_back(processed);
                    }
                }

                return string_array;
            } else {
                try {
                    std::vector<int> int_array;
                    std::stringstream ss(inner);
                    std::string item;

                    while(std::getline(ss, item, ',')) {
                        item.erase(0, item.find_first_not_of(" \t"));
                        item.erase(item.find_last_not_of(" \t") + 1);

                        if( item.empty() ) int_array.push_back(std::stoi(item));
                    }

                    return int_array;
                } catch (...) {
                    std::vector<std::string> string_array;
                    std::stringstream ss(inner);
                    std::string item;

                    while(std::getline(ss, item, ',')) {
                        item.erase(0, item.find_first_not_of(" \t"));
                        item.erase(item.find_last_not_of(" \t") + 1);

                        if( item.empty() ) string_array.push_back(item);
                    }

                    return string_array;
                }
            }
        }

        return value_str;
    }
};

TOMLReader::Values TOMLReader::get(TOMLReader::Expr expr) {
    if( std::get<1>(this->block) != 0 ) return this->block;

    size_t i = 0;
    size_t file_length = ctx.length();
    bool in_target_namespace = false;

    while( i < file_length ) {
        while( i < file_length && (ctx[i] == ' ' || ctx[i] == '\t' || ctx[i] == '\r') ) i++;
        if( i >= file_length ) break;

        if( ctx[i] == '#' ) {
            i = skip_comment(i);
            continue;
        }

        auto [is_ns, ns_length] = is_namespace(i);
        if( is_ns ) {
            std::string ns_name = ctx.substr(i + 1, ns_length - 2);

            size_t start = 0;
            while(start < ns_name.length() && ns_name[start] == ' ') start++;
            size_t end = ns_name.length();
            while(end > 0 && ns_name[end-1] == ' ') end--;

            if( start < end ) ns_name = ns_name.substr(start, end - start);

            in_target_namespace = (expr.nspace == ns_name) || (expr.nspace == (iam + '.' + ns_name));
            i += ns_length;
            continue;
        }

        if( in_target_namespace ) {
            size_t line_start = i;
            while( i < file_length && ctx[i] != '\n' ) i++;

            std::string line = ctx.substr(line_start, i - line_start);

            size_t equals_pos = line.find('=');
            if( equals_pos != std::string::npos ) {
                std::string field_name = line.substr(0, equals_pos);

                size_t field_start = 0;
                while(field_start < field_name.length() && field_name[field_start] == ' ') field_start++;
                size_t field_end = field_name.length();
                while(field_end > 0 && field_name[field_end-1] == ' ') field_end--;

                if( field_start < field_end ) {
                    field_name = field_name.substr(field_start, field_end - field_start);

                    if( field_name == expr.field ) {
                        std::string value_str = line.substr(equals_pos + 1);

                        size_t value_start = 0;
                        while(value_start < value_str.length() && value_str[value_start] == ' ') value_start++;
                        size_t value_end = value_str.length();
                        while(value_end > 0 && value_str[value_end-1] == ' ') value_end--;

                        if( value_start < value_end ) {
                            value_str = value_str.substr(value_start, value_end - value_start);

                            if(! value_str.empty() && value_str[0] == '[' ) return parse_array(value_str);

                            if( value_str == "true" ) return true;
                            if( value_str == "false" ) return false;

                            try {
                                if(!value_str.empty() && (
                                        isdigit(value_str[0]) || value_str[0] == '-'
                                    )
                                ) return std::stoi(value_str);
                            } catch (...) {}

                            if( value_str.length() >= 2 && (
                                    ( value_str[0] == '"' && value_str.back() == '"' ) ||
                                    ( value_str[0] == '\'' && value_str.back() == '\'' )
                                )
                            ) {
                                std::string unquoted = value_str.substr(1, value_str.length() - 2);
                                std::string processed;
                                for( size_t j = 0; j < unquoted.length(); j++ ) {
                                    if( unquoted[j] == '\\' && j + 1 < unquoted.length() ) {
                                        j++;
                                        switch(unquoted[j]) {
                                            case 'n': processed += '\n'; break;
                                            case 't': processed += '\t'; break;
                                            case 'r': processed += '\r'; break;
                                            case '\\': processed += '\\'; break;
                                            case '"': processed += '"'; break;
                                            case '\'': processed += '\''; break;
                                            default: processed += unquoted[j]; break;
                                        }
                                    } else processed += unquoted[j];
                                }
                                return processed;
                            }

                            return value_str;
                        }
                    }
                }
            }

            if( i < file_length && ctx[i] == '\n' ) i++;
        } else {
            while (i < file_length && ctx[i] != '\n') i++;
            if( i < file_length && ctx[i] == '\n' ) i++;
        }
    }

    return Error{ "Field not found", 1 };
}
