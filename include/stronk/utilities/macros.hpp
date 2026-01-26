#pragma once

#if defined(_MSC_VER)
#    define EMPTY_BASES __declspec(empty_bases)
#    define STRONK_FORCEINLINE __forceinline  // NOLINT
#elif defined(__clang__)
#    define EMPTY_BASES
#    define STRONK_FORCEINLINE __attribute__((always_inline))  // NOLINT
#elif defined(__GNUC__)
#    define EMPTY_BASES
#    define STRONK_FORCEINLINE __attribute__((always_inline))  // NOLINT
#else
#    define EMPTY_BASES
#    define STRONK_FORCEINLINE  // NOLINT
#endif
