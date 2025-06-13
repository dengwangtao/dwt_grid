#pragma once


using s32 = int;
using u32 = unsigned int;
using u64 = unsigned long long;
using s64 = long long;


#define MAKE64(high, low) (((u64)high) << 32 | (u64)low)

#define HIGH32(x) ((u32)((x) >> 32))
#define LOW32(x) ((u32)(x))