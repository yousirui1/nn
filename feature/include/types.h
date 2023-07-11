#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <vector>

//typedef double BaseFloat;
typedef float BaseFloat;

typedef int32_t int32;
typedef int64_t int64;

// CPU
template<typename T>
using VectorBase = std::vector<T>;

template<typename T>
using Vector = std::vector<T>;

#endif //  __TYPES_H__
