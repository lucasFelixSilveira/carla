#pragma once

#include <string>

#define STATEMENTS_FIELDS \
    X(STMT_PUTS, puts)

namespace carla {
    #define X(id, _) id,
    enum Statements { STMT_UNKNOWN = -1, STATEMENTS_FIELDS };
    #undef X

    struct Stmt {
        int data;
        Stmt(std::string data) {
            if( false ) {} // "Wild if"
            #define X(val, id) else if( data == #id ) this->data = val;
            STATEMENTS_FIELDS
            #undef X
            else { this->data = STMT_UNKNOWN; }
        };
    };
}
