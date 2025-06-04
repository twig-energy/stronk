
#pragma once

namespace twig
{
namespace stronk_details
{

#if defined(__SIZEOF_INT128__)
using unsigned_big_num = unsigned __int128;
#else
using unsigned_big_num =uint64_t

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
}  // namespace stronk_details

template<stronk_details::unsigned_big_num Num,
         stronk_details::unsigned_big_num Den = stronk_details::unsigned_big_num {1}>
struct ratio
{
    constexpr static stronk_details::unsigned_big_num num = Num / stronk_details::gcd(Num, Den);
    constexpr static stronk_details::unsigned_big_num den = Den / stronk_details::gcd(Num, Den);

    using type = ratio<num, den>;
};

template<typename Ratio1, typename Ratio2>
using ratio_multiply =
    typename ratio<Ratio1::type::num * Ratio2::type::num, Ratio1::type::den * Ratio2::type::den>::type;

template<typename Ratio1, typename Ratio2>
using ratio_divide = typename ratio<Ratio1::type::num * Ratio2::type::den, Ratio1::type::den * Ratio2::type::num>::type;

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
