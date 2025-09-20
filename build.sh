cd compiler

g++ -g -O0 -fpermissive main.cpp compiler_outputs.hpp params.hpp \
    tokenizer/scanner.hpp tokenizer/token.hpp tokenizer/token_kind.hpp \
    parser/ast.hpp parser/parser.hpp parser/symbols.hpp parser/pattern.hpp parser/llvm_rtypes.hpp \
    parser/patterns/declaration.hpp \
    debug/debug.hpp -o ../build/carla

cd ..
