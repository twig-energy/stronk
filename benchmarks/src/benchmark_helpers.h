#pragma once
#include <cstdint>
#include <sstream>
#include <string>

#include <stronk/prefabs.h>

struct int8_t_wrapping_type : twig::stronk_default_unit<int8_t_wrapping_type, int8_t>
{
    using stronk_default_unit::stronk_default_unit;
};

struct int64_t_wrapping_type : twig::stronk_default_unit<int64_t_wrapping_type, int64_t>
{
    using stronk_default_unit::stronk_default_unit;
};

struct double_wrapping_type : twig::stronk_default_unit<double_wrapping_type, double>
{
    using stronk_default_unit::stronk_default_unit;
};

struct string_wrapping_type : twig::stronk_default_unit<string_wrapping_type, std::string>
{
    using stronk_default_unit::stronk_default_unit;
};

template<typename T>
struct generate_randomish
{
    auto operator()() -> T
    {
        return static_cast<T>(std::rand());  // NOLINT
    }
};

template<>
struct generate_randomish<std::string>
{
    auto operator()() -> std::string
    {
        auto ss = std::stringstream();
        for (auto i = 0; i < 16; i++) {
            ss << static_cast<char>(std::rand());  // NOLINT
        }
        return ss.str();
    }
};

template<std::floating_point T>
struct generate_randomish<T>
{
    auto operator()() -> T
    {
        return static_cast<T>(std::rand()) / static_cast<T>(RAND_MAX);  // NOLINT
    }
};

template<twig::stronk_like T>
struct generate_randomish<T>
{
    auto operator()() -> T
    {
        return T(generate_randomish<typename T::underlying_type> {}());
    }
};
