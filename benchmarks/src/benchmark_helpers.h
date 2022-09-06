#pragma once
#include <cstdint>
#include <limits>
#include <random>
#include <sstream>
#include <string>

#include <stronk/prefabs.h>
#include <stronk/stronk.h>
#include <stronk/utilities/constexpr_helpers.h>

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
    } else if constexpr (std::is_integral_v<T>) {
        if constexpr (sizeof(T) <= 1) {
            std::uniform_int_distribution<std::conditional_t<std::is_unsigned_v<T>, uint16_t, int16_t>> dist(mi, ma);
            return static_cast<T>(dist(get_random_device()));
        } else {
            std::uniform_int_distribution<T> dist(mi, ma);
            return dist(get_random_device());
        }
    } else {
        // static_assert(twig::stronk_details::not_implemented_type<T> {});
    }
}

template<typename T>
auto rand() -> T
{
    return T();
}

}  // namespace details

template<typename T>
struct generate_randomish
{
    auto operator()() const -> T
    {
        return details::rand<T>();
    }
};
template<>
struct generate_randomish<std::string>
{
    auto operator()() const -> std::string
    {
        auto ss = std::stringstream();
        for (auto i = 0; i < 16; i++) {
            ss << generate_randomish<char> {}();
        }
        return ss.str();
    }
};

template<twig::stronk_like T>
struct generate_randomish<T>
{
    auto operator()() const -> T
    {
        return T {generate_randomish<typename T::underlying_type> {}()};
    }
};

template<typename T>
auto generate_none_zero_randomish() -> T
{
    auto val = generate_randomish<T> {}();
    if (val < static_cast<T>(0)) {
        return val - static_cast<T>(1);
    }
    return val + static_cast<T>(1);
}
