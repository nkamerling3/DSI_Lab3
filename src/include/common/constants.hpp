#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <iostream>

#define UNUSED(p)  ((void)(p))

#define ASSERT_WITH_MESSAGE(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion \033[1;31mFAILED\033[0m: " << message << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::abort(); \
        } \
    } while(0)

// =============================================================================
// Constants - DO NOT MODIFY
// =============================================================================

static constexpr uint16_t INVALID_VALUE = std::numeric_limits<uint16_t>::max();

using PageID = uint16_t;
