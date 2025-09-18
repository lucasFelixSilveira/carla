cd compiler;

g++ main.cpp compiler_outputs.hpp params.hpp \
    tokenizer/scanner.hpp tokenizer/token.hpp tokenizer/token_kind.hpp \
    parser/ast.hpp parser/parser.hpp parser/symbols.hpp \
    debug/debug.hpp -o ../build/carla
cd
