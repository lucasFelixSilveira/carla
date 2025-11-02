#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace morgana {
    using dynamic = std::monostate;
    using non_size = std::variant<dynamic, int>;

    enum radical { Integer = 0 };

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
        non_size length;
        int bits;
        bool pointer;
        bool vector;
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
            }

            if( vector ) ss << "]";
            if( pointer ) ss << "*";
            return ss.str();
        }
    };

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
     *
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
};
