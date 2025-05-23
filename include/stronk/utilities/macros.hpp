#pragma once

#if defined(_MSC_VER)
#    define STRONK_FORCEINLINE __forceinline  // NOLINT
#elif defined(__clang__)
#    define STRONK_FORCEINLINE __attribute__((always_inline))  // NOLINT
#elif defined(__GNUC__)
#    define STRONK_FORCEINLINE __attribute__((always_inline))  // NOLINT
#else
#    define STRONK_FORCEINLINE  // NOLINT
#endif
