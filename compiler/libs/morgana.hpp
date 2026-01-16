#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include "morgana/context.hpp"
#include "morgana/storage.hpp"

#define OPERATION_ORDER_FIXER(mul)                                   \
    for( (dl = dlayers.at(i++)); i < dlayers.size(); )               \
    if( (dl.op % mul) == 0 ) {                                       \
        cp.push_back(operations {                                    \
            .layer = dl.layer,                                       \
            .lhs = dl.data,                                          \
            .rhs = dlayers.at(i++).data,                             \
            .op = dl.op                                              \
        });                                                          \
        if( (i + 1) < dlayers.size() )                               \
            dl = dlayers.at(i++);                                    \
    } else dl = dlayers.at(i++);

#define TEMP_CHECK_SIDE(side)                                        \
    if( std::regex_match(data.side, digit) ) {                       \
        ss << "temp " << strside().side << " " << data.side << " \n";   \
    }                                                                \
    else if( std::regex_match(data.side, id) ) {                     \
        ss << "temp " << strside().side << " " << data.side << " \n";   \
    }

class strside {
public:
    std::string lhs = "lhs";
    std::string rhs = "rhs";

    strside() = default;
};

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

    struct expr {
        Storage& storage;

        enum operand {
            add = 1, sub = 2, mul = 3, div = 4, mod = 5
        };

        std::array<std::tuple<std::string, operand>, 5> op_names = {
            std::tuple<std::string, operand> {"+", operand::add},
            std::tuple<std::string, operand> {"-", operand::sub},
            std::tuple<std::string, operand> {"*", operand::mul},
            std::tuple<std::string, operand> {"/", operand::div},
            std::tuple<std::string, operand> {"%", operand::mod}
        };

        struct nodes;

        using values = std::variant<std::shared_ptr<nodes>, std::string>;

        struct nodes {
        public:
            operand op;
            values lhs;
            values rhs;

            nodes() = default;

            std::shared_ptr<nodes> shared() {
                return std::make_shared<nodes>(*this);
            }
        };

        struct data_layer {
            operand op;
            int layer;
            std::string data;
            std::shared_ptr<nodes> node;
        };

        std::vector<data_layer> dlayers;
        expr(Storage& storage) : storage(storage) {};

    private:
        void recursivemakecall(std::shared_ptr<nodes> node, int layer = 0) {
            // Coleta primeiro os nós filhos recursivamente
            if (std::holds_alternative<std::shared_ptr<nodes>>(node->lhs)) {
                recursivemakecall(std::get<std::shared_ptr<nodes>>(node->lhs), layer + 1);
            }

            if (std::holds_alternative<std::shared_ptr<nodes>>(node->rhs)) {
                recursivemakecall(std::get<std::shared_ptr<nodes>>(node->rhs), layer + 1);
            }

            // Depois coleta o nó atual
            std::string lhs_data, rhs_data;

            if (std::holds_alternative<std::string>(node->lhs)) {
                lhs_data = std::get<std::string>(node->lhs);
            } else {
                lhs_data = "temp"; // Resultado de operação anterior
            }

            if (std::holds_alternative<std::string>(node->rhs)) {
                rhs_data = std::get<std::string>(node->rhs);
            } else {
                rhs_data = "temp"; // Resultado de operação anterior
            }

            dlayers.push_back(data_layer {
                .op = node->op,
                .layer = layer,
                .data = lhs_data, // lhs do nó atual
                .node = node
            });

            dlayers.push_back(data_layer {
                .op = node->op,
                .layer = layer,
                .data = rhs_data, // rhs do nó atual
                .node = node
            });
        }

        int getPriority(operand op) {
            switch(op) {
                case operand::mul:
                case operand::div:
                case operand::mod:
                    return 2;
                case operand::add:
                case operand::sub:
                    return 1;
                default:
                    return 0;
            }
        }

        std::string opToString(operand op) {
            switch(op) {
                case operand::add: return "add";
                case operand::sub: return "sub";
                case operand::mul: return "mul";
                case operand::div: return "div";
                case operand::mod: return "mod";
                default: return "unknown";
            }
        }

    public:
        struct operations {
            int layer;
            std::string lhs;
            std::string rhs;
            operand op;
            std::shared_ptr<nodes> node;
        };

        std::string make(std::shared_ptr<nodes> root_node) {
            std::stringstream ss;
            dlayers.clear();

            // Coleta todas as operações mantendo a estrutura hierárquica
            std::vector<operations> all_ops;
            collectOperations(root_node, all_ops, 0);

            // Ordena por prioridade (maior primeiro) e depois por camada (mais profunda primeiro)
            std::sort(all_ops.begin(), all_ops.end(),
                [this](const operations& a, const operations& b) {
                    int prioA = getPriority(a.op);
                    int prioB = getPriority(b.op);

                    if (prioA != prioB) {
                        return prioA > prioB; // Prioridade maior primeiro
                    }
                    return a.layer > b.layer; // Camada mais profunda primeiro
                });

            // Processa as operações na ordem correta
            std::unordered_map<std::shared_ptr<nodes>, std::string> node_to_temp;
            int temp_counter = storage.exprcount;

            for (const auto& op : all_ops) {
                std::string lhs_val = op.lhs;
                std::string rhs_val = op.rhs;

                // Se lhs é um nó, pega o resultado temporário dele
                if (std::holds_alternative<std::shared_ptr<nodes>>(op.node->lhs)) {
                    auto child_node = std::get<std::shared_ptr<nodes>>(op.node->lhs);
                    auto it = node_to_temp.find(child_node);
                    if (it != node_to_temp.end()) {
                        lhs_val = it->second;
                    }
                }

                // Se rhs é um nó, pega o resultado temporário dele
                if (std::holds_alternative<std::shared_ptr<nodes>>(op.node->rhs)) {
                    auto child_node = std::get<std::shared_ptr<nodes>>(op.node->rhs);
                    auto it = node_to_temp.find(child_node);
                    if (it != node_to_temp.end()) {
                        rhs_val = it->second;
                    }
                }

                std::string temp_name = "e" + std::to_string(temp_counter++);
                node_to_temp[op.node] = temp_name;

                ss << "temp lhs " << lhs_val << '\n';
                ss << "temp rhs " << rhs_val << '\n';
                ss << "do " << opToString(op.op) << ", " << temp_name << '\n';
            }

            storage.exprcount = temp_counter;
            return ss.str();
        }

    private:
        void collectOperations(std::shared_ptr<nodes> node, std::vector<operations>& ops, int layer) {
            // Coleta operações dos filhos primeiro
            if (std::holds_alternative<std::shared_ptr<nodes>>(node->lhs)) {
                collectOperations(std::get<std::shared_ptr<nodes>>(node->lhs), ops, layer + 1);
            }

            if (std::holds_alternative<std::shared_ptr<nodes>>(node->rhs)) {
                collectOperations(std::get<std::shared_ptr<nodes>>(node->rhs), ops, layer + 1);
            }

            // Coleta a operação atual
            std::string lhs_str = std::holds_alternative<std::string>(node->lhs)
                ? std::get<std::string>(node->lhs)
                : "node_result";

            std::string rhs_str = std::holds_alternative<std::string>(node->rhs)
                ? std::get<std::string>(node->rhs)
                : "node_result";

            ops.push_back(operations {
                .layer = layer,
                .lhs = lhs_str,
                .rhs = rhs_str,
                .op = node->op,
                .node = node
            });
        }
    };
};

/*
x = 1 + (2 + (3 + (4 + 5)))

    1 + ()
        2 ()
           3 ()
              4 5

x = (1 + 2) * (3*4) + 5
    ()  *  ()
  1+2   () + ()
        3*4  5+0

*/
