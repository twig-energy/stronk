#include <string>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "./benchmark_helpers.hpp"

// declared in benchmarks/src/benchmark_helpers.hpp
auto current_test_name() -> std::string
{
    // doctest::detail::g_cs->currentTest->m_name is only accessible in the
    // compilation unit that has DOCTEST_CONFIG_IMPLEMENT or
    // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN defined.
    return doctest::detail::g_cs->currentTest->m_name;
}
