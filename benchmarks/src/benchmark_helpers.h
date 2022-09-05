#pragma once
#include <cstdint>
#include <random>
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

namespace details
{
template<typename T>
auto default_max_for_random() -> T
{
    if constexpr (std::is_floating_point_v<T>) {
        return T(1.0);
    } else {
        return std::numeric_limits<T>::max();
    }
}

static inline auto get_random_device() -> std::mt19937&
{
    static thread_local std::random_device dev;
    static thread_local std::mt19937 rng(dev());
    return rng;
}

template<typename T>
auto rand(T mi, T ma) -> T
{
    if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(mi, ma);
        return dist(get_random_device());
    } else {
        std::uniform_int_distribution<T> dist(mi, ma);
        return dist(get_random_device());
    }
}

template<typename T>
auto rand() -> T
{
    T mi = 0;
    T ma = default_max_for_random<T>();
    return rand(mi, ma);
}

}  // namespace details

template<typename T>
struct generate_randomish
{
    auto operator()() -> T
    {
        return details::rand<T>();
    }
};

template<>
struct generate_randomish<std::string>
{
    auto operator()() -> std::string
    {
        auto ss = std::stringstream();
        for (auto i = 0; i < 16; i++) {
            ss << details::rand<char>();
        }
        return ss.str();
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

template<typename T>
auto generate_none_zero_randomish() -> T
{
    // generate_randomish is always positive so adding 1 makes sure its not zero
    return generate_randomish<T> {}() + static_cast<T>(1);
}
