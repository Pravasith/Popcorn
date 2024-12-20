#pragma once
#include "GlobalMacros.h"
#include <cassert>

ENGINE_NAMESPACE_BEGIN
#define PC_ASSERT(expression, message) assert((expression) && message)
#define PC_STATIC_ASSERT(expression, message) static_assert(expression, message)
ENGINE_NAMESPACE_END
