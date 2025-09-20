#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include "../parser/llvm_rtypes.hpp"

llvm::Type* getLLVMType(LLVMRadicalType radical, llvm::LLVMContext& context) {
    switch(radical) {
        case void_t: return llvm::Type::getVoidTy(context);
        case i8: return llvm::Type::getInt8Ty(context);
        case i16: return llvm::Type::getInt16Ty(context);
        case i32: return llvm::Type::getInt32Ty(context);
        case i64: return llvm::Type::getInt64Ty(context);
        case i128: return llvm::Type::getIntNTy(context, 128);
        case i256: return llvm::Type::getIntNTy(context, 256);
        default: return llvm::Type::getInt32Ty(context);
    }
}
