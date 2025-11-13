
#pragma once

namespace twig
{
namespace stronk_details
{

#if defined(__SIZEOF_INT128__)
__extension__ using u_biggest_int_t = unsigned __int128;
#else
using u_biggest_int_t = uint64_t;

#endif

constexpr auto gcd(const auto& self, const auto& other) -> auto
{
    if (other == 0) {
        return self;
    }

    auto a = self;
    auto b = other;

    while (b != 0) {
        auto temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Compile-time integer square root using binary search
constexpr auto isqrt(u_biggest_int_t n) -> u_biggest_int_t
{
    if (n == 0 || n == 1) {
        return n;
    }

    u_biggest_int_t left = 1;
    u_biggest_int_t right = n;
    u_biggest_int_t result = 0;

    while (left <= right) {
        auto mid = left + ((right - left) / 2);

        // Check if mid*mid <= n (avoid overflow by using division)
        if (mid <= n / mid) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}

constexpr auto pow(u_biggest_int_t base, unsigned int exp) -> u_biggest_int_t
{
    auto result = base;
    for (auto i = u_biggest_int_t {1}; i < exp; ++i) {
        result *= base;
    }
    return result;
}

}  // namespace stronk_details

template<stronk_details::u_biggest_int_t Num, stronk_details::u_biggest_int_t Den = stronk_details::u_biggest_int_t {1}>
struct ratio
{
    constexpr static stronk_details::u_biggest_int_t num = Num / stronk_details::gcd(Num, Den);
    constexpr static stronk_details::u_biggest_int_t den = Den / stronk_details::gcd(Num, Den);

    using type = ratio<num, den>;
};

template<typename Ratio1, typename Ratio2>
using ratio_multiply =
    typename ratio<Ratio1::type::num * Ratio2::type::num, Ratio1::type::den * Ratio2::type::den>::type;

template<typename Ratio1, typename Ratio2>
using ratio_divide = typename ratio<Ratio1::type::num * Ratio2::type::den, Ratio1::type::den * Ratio2::type::num>::type;

template<typename RatioT>
using ratio_sqrt =
    typename ratio<stronk_details::isqrt(RatioT::type::num), stronk_details::isqrt(RatioT::type::den)>::type;

template<typename RatioT, int PowerV>
using ratio_pow = typename ratio<stronk_details::pow(RatioT::type::num, PowerV),
                                 stronk_details::pow(RatioT::type::den, PowerV)>::type;

using atto = ratio<1, 1000000000000000000>;
using femto = ratio<1, 1000000000000000>;
using pico = ratio<1, 1000000000000>;
using nano = ratio<1, 1000000000>;
using micro = ratio<1, 1000000>;
using milli = ratio<1, 1000>;
using centi = ratio<1, 100>;
using deci = ratio<1, 10>;
using deca = ratio<10, 1>;
using hecto = ratio<100, 1>;
using kilo = ratio<1000, 1>;
using mega = ratio<1000000, 1>;
using giga = ratio<1000000000, 1>;
using tera = ratio<1000000000000, 1>;
using peta = ratio<1000000000000000, 1>;
using exa = ratio<1000000000000000000, 1>;

}  // namespace twig
