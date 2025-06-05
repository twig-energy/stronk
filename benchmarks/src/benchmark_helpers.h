#pragma once
#include <cstdint>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>

#include <stronk/utilities/constexpr_helpers.hpp>

#include "stronk/stronk.hpp"
#include "stronk/unit.hpp"
#include "stronk/utilities/ratio.hpp"

struct a_unit : twig::stronk_default_unit<a_unit, twig::ratio<1>>
{
};

using int8_t_wrapping_type = a_unit::value<int8_t>;
using int64_t_wrapping_type = a_unit::value<int64_t>;
using double_wrapping_type = a_unit::value<double>;

struct string_wrapping_type : twig::stronk<string_wrapping_type, std::string>
{
    using stronk::stronk;
};

namespace details
{
template<typename T>
auto default_max_for_random() -> T
{
    if constexpr (std::is_floating_point_v<T>) {
        return T {1.0};
    } else {
        return std::numeric_limits<T>::max();
    }
}

inline static auto get_random_device() -> std::mt19937&
{
    static thread_local std::random_device dev;
    static thread_local std::mt19937 rng(dev());
    return rng;
}

template<typename T>
auto rand(T mi, T ma) -> T
{
    if constexpr (std::is_floating_point_v<T>) {
        auto dist = std::uniform_real_distribution<T>(mi, ma);
        return dist(get_random_device());
    } else if constexpr (std::is_integral_v<T> && sizeof(T) <= 1) {
        using random_supported_type = std::conditional_t<std::is_unsigned_v<T>, uint16_t, int16_t>;
        auto dist = std::uniform_int_distribution<random_supported_type>(static_cast<random_supported_type>(mi),
                                                                         static_cast<random_supported_type>(ma));
        return static_cast<T>(dist(get_random_device()));
    } else if constexpr (std::is_integral_v<T>) {
        auto dist = std::uniform_int_distribution<T>(mi, ma);
        return dist(get_random_device());
    } else {
        static_assert(twig::stronk_details::not_implemented_type<T> {});
    }
}

template<typename T>
auto rand() -> T
{
    return rand<T>(T {0}, default_max_for_random<T>());
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
    return generate_randomish<T> {}() + static_cast<T>(1);
}
