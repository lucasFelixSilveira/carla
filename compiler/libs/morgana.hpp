#pragma once

#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "morgana/storage.hpp"

namespace morgana {

    using dynamic = std::monostate;
    using non_size = std::variant<dynamic, int>;

    enum radical { Integer = 0, Alias = 1, Unknown = 2 };

    /*
     * This class represents a type in the IR.
     * Where you can build your own types, such as arrays, structs, etc.
     *
     * Example:
     *
     *     type int32 = type::integer(32);
     *     type int32_ptr = int32.ptr();
     *     type int32_array = int32.vec(10);
     *     type int32_array_ptr = int32_array.ptr();
     */
    struct type {
    private:
        enum radical radical;
        int addr;
        non_size length;
        int bits;
        bool pointer;
        bool vector;

        static std::unordered_map<std::string, int> addrs;
    public:
        type(enum radical radical, int bits): radical(radical), bits(bits), pointer(false), vector(false) {}

        /*
         * Constructor for integer type.
         * - You only need to specify the number of bits.
         */
        static type integer(int bits) {
            type t(radical::Integer, bits);
            return t;
        }

        static type unknown() {
            type t(radical::Unknown, (sizeof(char*) * 8));
            return t;
        }

        /*
         *
         */
        static type clone(Storage& storage, type& t) {
            type clone = t;
            clone.radical = radical::Alias;
            clone.addr = storage.addr++;
            clone.pointer = false;
            clone.vector = false;

            storage.aliases.push_back({ clone.addr, t.string() });
            return clone;
        }

        enum radical get_radical() const {
            return radical;
        }

        int bytes() const {
            return bits / 8;
        }

        /*
         * Constructor for pointer type.
         * - Used for create a simple pointer
         */
        type& ptr() {
            pointer = true;
            return *this;
        }

        /*
         * Constructor for vector type.
         * - You can create a simple array type (vector) with a given size.
         *   But, the size is not required. If not specified, the vector will be dynamic.
         * ================================================================
         * - WARNING!:
         * Keep in mind: The "dynamic size" is not dynamic. It means that the size will
         * be determined at compile time.
         */
        type& vec(non_size size) {
            length = size;
            vector = true;
            return *this;
        }

        /*
         * Make a Shared pointer type without a large code
         */
        std::shared_ptr<type> shared() {
            return std::make_shared<type>(*this);
        }

        /*
         * Convert the type class to the string representation
         * of the type in Morgana IR language.
         */
        std::string string() {
            std::stringstream ss;
            if( vector ) ss << "[";
            if( vector && std::holds_alternative<dynamic>(length) ) ss << "*:";
            if( vector && std::holds_alternative<int>(length) ) ss << std::get<int>(length) << ":";

            if( radical == Integer) {
                ss << "i" << bits;
            } else if( radical == Alias ) {
                ss << "a" << addr;
            }

            if( vector ) ss << "]";
            if( pointer ) ss << "*";
            return ss.str();
        }
    };

    /*
     * Convert the function class to the string representation
     * of the function and their body in Morgana IR language.
     */
    struct function {
        std::string name;
        std::shared_ptr<type> return_type;

        std::vector<std::shared_ptr<type>> arguments;

        std::string body;

        using args = std::vector<std::shared_ptr<type>>;

        function(std::string name, std::shared_ptr<type> return_type, args arguments, std::string body) : name(name), return_type(return_type), arguments(arguments), body(body) {}

        /*
         * Make a Shared pointer type without a large code
         */
        std::shared_ptr<function> shared() {
            return std::make_shared<function>(*this);
        }

        /*
         * Convert the type class to the string representation
         * of the function and their body in Morgana IR language.
         */
        std::string string() {
            std::stringstream ss;
            ss << "\n" << return_type->string() << " " << name << " ";

            for( auto argument : arguments ) {
                ss << argument->string() << " ";
            }

            ss << "{\n" << body << "}\n";
            return ss.str();
        }
    };

    /*
     * That is the options of miscellaneous in
     * Morgana IR language.
     */
    enum mics {
        that
    };


    /*
     * Util class for symbol table of your langugage
     */
    struct variable {
        std::string name;
        std::shared_ptr<morgana::type> type;
        bool mut;

        variable(std::string name, std::shared_ptr<morgana::type> type, bool mut) : name(name), type(type), mut(mut) {}

        /*
         * Make a Shared pointer type without a large code
         */
        std::shared_ptr<variable> shared() {
            return std::make_shared<variable>(*this);
        }
    };

    /*
     * Class to make a desconstructor of something
     */
    struct desconstruct {
        using data = std::variant<std::string>;
        using values = std::vector<morgana::desconstruct::data>;
        values contents;
        mics option;

        desconstruct(mics option, std::vector<data> contents) : option(option), contents(contents) {};

        /*
         * Convert the deconstructor class to the string representation
         * of some deconstructor in Morgana IR language.
         */
        std::string string() {
            std::stringstream ss;

            switch(option) {
                case mics::that: {
                    ss << "(";
                    for( auto value : contents ) {
                        ss << std::get<std::string>(value) << ", ";
                    }
                    ss << ") @_\n";
                } break;
            }

            return ss.str();
        }
    };

    /*
     * Convert the alloc class to the string representation
     * of some alloc instruction in Morgana IR language.
     */
    struct alloc {
        std::shared_ptr<type> info;
        long long addr = 0;

        alloc(Storage& storage, std::shared_ptr<type> info) : info(info) {
            addr = storage.local++;
        }

        /*
         * Make a Shared pointer type without a large code
         */
        std::shared_ptr<alloc> shared() {
            return std::make_shared<alloc>(*this);
        }

        /*
         * Save the address of the alloc instruction
         */
        alloc& save(int* copy) {
            *copy = addr;
            return *this;
        }

        /*
         * Convert the alloc instruction to the string
         * representation of the alloc instruction
         * on Morgana IR
         */
        std::string string() {
            std::stringstream ss;
            ss << '_' << addr << " = alloc " << info->string() << '\n';
            return ss.str();
        }
    };

    /*
     * Convert the store instruction to the string
     * representation of the store instruction
     * on Morgana IR
     */
    struct store {
        int addr;
        long long int value;

        store(int addr, long long int value) : addr(addr), value(value) {}
        store(std::shared_ptr<alloc> allocation, long long int value) : addr(allocation->addr), value(value) {}

        /*
         * Make a Shared pointer type without a large code
         */
        std::shared_ptr<store> shared() {
            return std::make_shared<store>(*this);
        }

        /*
         * Convert the store instruction to the string
         * representation of the store instruction
         * on Morgana IR
         */
        std::string string() {
            std::stringstream ss;
            ss << "store _" << addr << ' ' << value << '\n';
            return ss.str();
        }
    };

    /*
     * Convert the load instruction to the string
     * representation of the load instruction
     * on Morgana IR
     */
    struct load {
        int allocation_addr;
        int addr;

        load(Storage& storage, int addr) : allocation_addr(addr) {
            addr = storage.local++;
        }

        load(Storage& storage, std::shared_ptr<alloc> allocation) : allocation_addr(allocation->addr) {
            addr = storage.local++;
        }

        /*
         * Make a Shared pointer type without a large code
         */
        std::shared_ptr<load> shared() {
            return std::make_shared<load>(*this);
        }

        /*
         * Save the address of the load instruction
         */
        load& save(int* copy) {
            *copy = addr;
            return *this;
        }

        /*
         * Convert the load instruction to the string
         * representation of the load instruction
         * on Morgana IR
         */
        std::string string() {
            std::stringstream ss;
            ss << "_" << addr << " = load " << '_' << allocation_addr << '\n';
            return ss.str();
        }
    };

    struct operations {
        enum operation { ADD, SUB, MUL, DIV, MOD };
        int addr;
        operation op;
        std::shared_ptr<alloc> dst;
        std::string value;

        operations(Storage& storage, operation op, std::shared_ptr<alloc> dst, std::string value) : op(op), dst(dst), value(value) {
            addr = storage.local++;
        }

        operations(Storage& storage, operation op, std::string value) : op(op), dst(nullptr), value(value) {
            addr = storage.local++;
        }

        /*
            * Convert the op instruction to the string
            * representation of the load instruction
            * on Morgana IR
            */
        std::string string() {
            std::stringstream ss;
            ss << "_" << addr << " = ";

            switch (op) {
                case ADD: ss << "add"; break;
                case SUB: ss << "sub"; break;
                case MUL: ss << "mul"; break;
                case DIV: ss << "div"; break;
                case MOD: ss << "mod"; break;
            }

            ss << " ";
            if( dst == nullptr )
                 ss << "_" << (addr - 1);
            else ss << "_" << dst->addr;
            ss << " " << value << "\n";

            return ss.str();
        }
    };
};
