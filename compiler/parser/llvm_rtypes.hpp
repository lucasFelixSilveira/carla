#pragma once

enum LLVMRadicalType : int {
  i8 = 0, i16 = 2, i32 = 4, i64 = 8, i128 = 16, i256 = 32,
  i1 = -1, void_t = -2, ptr = -3,
};
