#pragma once

#include <exception>

#include "Combine.h"

#define ToStringFunc(x) #x
#define ToString(x) ToStringFunc(x)

#define Line ToString(__LINE__)

#define ThrowEx(...) throw std::runtime_error(Combine( __FILE__ ": " Line ":\n", __VA_ARGS__))