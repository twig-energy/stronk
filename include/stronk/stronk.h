#pragma once
#include <algorithm>
#include <concepts>
#include <ostream>
#include <type_traits>
#include <utility>

#include <stronk/utilities/equality.h>
#include <stronk/utilities/macros.h>
#include <stronk/utilities/strings.h>

namespace twig
{

template<typename T>
concept should_be_copy_constructed = std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(T*);

template<typename Tag, typename T, template<typename> typename... Skills>
struct stronk : public Skills<Tag>...
{
    using underlying_type = T;

    // we need the underlying value to have public visibility for stronk types to be usable as non-type template
    // parameters. This is to fulfil the `literal class type` requirement.
    // To discourage direct usage of the underlying value, we have given it a long ugly name.
    T _you_should_not_be_using_this_but_rather_unwrap;

    constexpr stronk() noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    STRONK_FORCEINLINE
    constexpr explicit stronk(underlying_type value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(
        should_be_copy_constructed<T>)
        : _you_should_not_be_using_this_but_rather_unwrap(value)
    {
    }

    STRONK_FORCEINLINE
    constexpr explicit stronk(const underlying_type& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(
        !should_be_copy_constructed<T>)
        : _you_should_not_be_using_this_but_rather_unwrap(value)
    {
    }

    STRONK_FORCEINLINE
    constexpr explicit stronk(underlying_type&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(
        !should_be_copy_constructed<T> && std::is_move_constructible_v<T>)
        : _you_should_not_be_using_this_but_rather_unwrap(std::move(value))
    {
    }

    template<typename Expected>
    [[nodiscard]] constexpr auto unwrap() noexcept -> underlying_type&
    {
        static_assert(std::is_same_v<Expected, Tag>,
                      "To access the underlying type you need to provide the stronk type you expect to be querying. By "
                      "doing so you will be protected from unsafe accesses if you chose to change the type");
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }

    template<typename Expected>
    [[nodiscard]] constexpr auto unwrap() const noexcept -> const underlying_type&
    {
        static_assert(std::is_same_v<Expected, Tag>,
                      "To access the underlying type you need to provide the stronk type you expect to be querying. By "
                      "doing so you will be protected from unsafe accesses if you chose to change the type");
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }

    constexpr friend void swap(stronk& a, stronk& b) noexcept
    {
        using std::swap;
        swap(a._you_should_not_be_using_this_but_rather_unwrap, b._you_should_not_be_using_this_but_rather_unwrap);
    }

  protected:
    [[nodiscard]] constexpr auto val() noexcept -> T&
    {
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }
    [[nodiscard]] constexpr auto val() const noexcept -> const T&
    {
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }
};

template<typename T>
concept stronk_like = requires(T v)
{
    {
        v.template unwrap<T>()
        } -> std::convertible_to<typename T::underlying_type>;
};

template<typename StronkT>
struct can_negate
{
    constexpr friend auto operator-(const StronkT& elem) noexcept -> StronkT
    {
        return StronkT {-elem.template unwrap<StronkT>()};
    }
};

template<typename StronkT>
struct can_add
{
    constexpr friend auto operator+=(StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        lhs.template unwrap<StronkT>() += rhs.template unwrap<StronkT>();
        return lhs;
    }

    constexpr friend auto operator+(const StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        return StronkT(lhs.template unwrap<StronkT>() + rhs.template unwrap<StronkT>());
    }
};

template<typename StronkT>
struct can_subtract
{
    constexpr friend auto operator-=(StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        lhs.template unwrap<StronkT>() -= rhs.template unwrap<StronkT>();
        return lhs;
    }

    constexpr friend auto operator-(const StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        return StronkT(lhs.template unwrap<StronkT>() - rhs.template unwrap<StronkT>());
    }
};

template<typename StronkT>
struct can_multiply
{
    using can_multiply_with_self = std::true_type;

    friend auto operator*=(StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        lhs.template unwrap<StronkT>() *= rhs.template unwrap<StronkT>();
        return lhs;
    }

    friend auto operator*(const StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        return StronkT {lhs.template unwrap<StronkT>() * rhs.template unwrap<StronkT>()};
    }
};

template<typename StronkT>
struct can_divide
{
    using can_divide_with_self = std::true_type;

    constexpr friend auto operator/=(StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        lhs.template unwrap<StronkT>() /= rhs.template unwrap<StronkT>();
        return lhs;
    }

    constexpr friend auto operator/(const StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        return StronkT {lhs.template unwrap<StronkT>() / rhs.template unwrap<StronkT>()};
    }
};

// The skills can_divide and can_multiply are different from the unit system -
// multiplying two A*A with can_multiply results in type 'A', where with units
// it results in an 'A^2' type. Therefore the two systems cannot be mixed. We
// recommend using units.
template<typename StronkT>
concept is_none_unit_behaving =
    (std::same_as<typename StronkT::can_multiply_with_self,
                  std::true_type> || std::same_as<typename StronkT::can_divide_with_self, std::true_type>);

template<typename StronkT>
struct can_equate
{
    constexpr friend auto operator==(const StronkT& lhs, const StronkT& rhs) noexcept -> bool
    {
        static_assert(!std::is_floating_point_v<typename StronkT::underlying_type>);
        return lhs.template unwrap<StronkT>() == rhs.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_less_than_greater_than
{
    constexpr friend auto operator<(const StronkT& lhs, const StronkT& rhs) noexcept -> bool
    {
        return lhs.template unwrap<StronkT>() < rhs.template unwrap<StronkT>();
    }
    constexpr friend auto operator>(const StronkT& lhs, const StronkT& rhs) noexcept -> bool
    {
        return lhs.template unwrap<StronkT>() > rhs.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_less_than_greater_than_or_equal
{
    constexpr friend auto operator<=(const StronkT& lhs, const StronkT& rhs) noexcept -> bool
    {
        return lhs.template unwrap<StronkT>() <= rhs.template unwrap<StronkT>();
    }
    constexpr friend auto operator>=(const StronkT& lhs, const StronkT& rhs) noexcept -> bool
    {
        return lhs.template unwrap<StronkT>() >= rhs.template unwrap<StronkT>();
    }
};

// "Parameter type" class for 'can_equate_with_is_close_base'
struct is_close_params
{
    template<typename T>
    static constexpr auto abs_tol()
    {
        return twig::stronk_details::default_abs_tol<T>();
    }
    template<typename T>
    static constexpr auto rel_tol()
    {
        return twig::stronk_details::default_rel_tol<T>();
    }
    static constexpr bool nan_equals = false;
};

// "Parameter type" class for 'can_equate_with_is_close_base'
struct is_close_with_nan_equals_params
{
    template<typename T>
    static constexpr auto abs_tol()
    {
        return twig::stronk_details::default_abs_tol<T>();
    }
    template<typename T>
    static constexpr auto rel_tol()
    {
        return twig::stronk_details::default_rel_tol<T>();
    }
    static constexpr bool nan_equals = true;
};

// "Parameter type" class for 'can_equate_with_is_close_base'
struct is_close_using_abs_tol_only_params
{
    template<typename T>
    static constexpr auto abs_tol() -> T
    {
        return twig::stronk_details::default_abs_tol<T>();
    }

    template<typename T>
    static constexpr auto rel_tol() -> T
    {
        return T(0);
    }
    static constexpr bool nan_equals = false;
};

template<typename StronkT, typename CloseParamsT>
struct can_equate_with_is_close_base
{
    constexpr friend auto operator==(const StronkT& lhs, const StronkT& rhs) noexcept -> bool
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);
        using type = typename StronkT::underlying_type;
        auto inner_val_1 = lhs.template unwrap<StronkT>();
        auto inner_val_2 = rhs.template unwrap<StronkT>();

        constexpr auto abs_tol = CloseParamsT::template abs_tol<type>();
        constexpr auto rel_tol = CloseParamsT::template rel_tol<type>();
        return twig::stronk_details::is_close(abs_tol, rel_tol, CloseParamsT::nan_equals)(inner_val_1, inner_val_2);
    }
};

template<typename StronkT>
using can_equate_with_is_close = can_equate_with_is_close_base<StronkT, is_close_params>;

template<typename StronkT>
using can_equate_with_is_close_nan_equals = can_equate_with_is_close_base<StronkT, is_close_with_nan_equals_params>;

template<typename StronkT>
using can_equate_with_is_close_abs_tol_only =
    can_equate_with_is_close_base<StronkT, is_close_using_abs_tol_only_params>;

template<typename T>
struct default_can_equate_builder
{
    template<typename StronkT>
    struct skill : can_equate<StronkT>
    {
    };
};

template<std::floating_point T>
struct default_can_equate_builder<T>
{
    template<typename StronkT>
    struct skill : can_equate_with_is_close_abs_tol_only<StronkT>
    {
    };
};

template<typename StronkT>
struct can_order
{
    // note you probably also want the == operator either from can_equate or one
    // of the can_equate_with_is_close

    constexpr friend auto operator<=>(const StronkT& lhs, const StronkT& rhs) noexcept
    {
        return lhs.template unwrap<StronkT>() <=> rhs.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_abs
{
    [[nodiscard]] constexpr auto abs() const noexcept -> StronkT
    {
        return StronkT {std::abs(static_cast<const StronkT&>(*this).template unwrap<StronkT>())};
    }
};

template<typename T>
concept can_abs_like = requires(T v)
{
    stronk_like<T>;
    {
        v.abs()
        } -> std::same_as<T>;
};

[[nodiscard]] constexpr auto abs(can_abs_like auto elem) noexcept
{
    return elem.abs();
}

template<typename StronkT>
struct can_isnan
{
    [[nodiscard]] constexpr auto isnan() const noexcept -> bool
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);
        return std::isnan(static_cast<const StronkT&>(*this).template unwrap<StronkT>());
    }
};

template<typename T>
concept can_isnan_like = requires(T v)
{
    stronk_like<T>;
    {
        v.isnan()
        } -> std::same_as<bool>;
};

[[nodiscard]] constexpr auto isnan(can_isnan_like auto elem) noexcept -> bool
{
    return elem.isnan();
}

template<typename StronkT>
struct can_be_used_as_flag
{
    [[nodiscard]] auto is_on() const noexcept -> bool
    {
        static_assert(std::is_same_v<typename StronkT::underlying_type, bool>);
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>();
    }

    [[nodiscard]] auto is_off() const noexcept -> bool
    {
        return !this->is_on();
    }

    [[nodiscard]] static auto on() noexcept -> StronkT
    {
        static_assert(std::is_same_v<typename StronkT::underlying_type, bool>);
        return StronkT(true);
    }

    [[nodiscard]] static auto off() noexcept -> StronkT
    {
        static_assert(std::is_same_v<typename StronkT::underlying_type, bool>);
        return StronkT(false);
    }
};

template<typename StronkT>
struct can_stream
{
    friend auto operator<<(std::ostream& os, const StronkT& elem) -> std::ostream&
    {
        return os << elem.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_hash
{
    using can_hash_indicator = std::true_type;
};
template<typename StronkT>
concept can_hash_like = std::same_as<typename StronkT::can_hash_indicator, std::true_type>;

template<typename StronkT>
struct can_size
{
    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().size();
    }
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return this->size() == static_cast<std::size_t>(0);
    }
};

template<typename StronkT>
struct can_const_iterate
{
    [[nodiscard]] constexpr auto begin() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]] constexpr auto end() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().end();
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]] constexpr auto cend() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().end();
    }
};

template<typename StronkT>
struct can_iterate : can_const_iterate<StronkT>
{
    [[nodiscard]] constexpr auto begin() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]] constexpr auto end() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().end();
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]] constexpr auto cend() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().end();
    }

    [[nodiscard]] constexpr auto begin() noexcept
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]] constexpr auto end() noexcept
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>().end();
    }
};

template<typename StronkT>
struct can_const_index
{
    [[nodiscard]] constexpr auto operator[](const auto& indexer) const noexcept -> const auto&
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>()[indexer];
    }

    [[nodiscard]] constexpr auto at(const auto& indexer) const -> const auto&
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().at(indexer);
    }
};

template<typename StronkT>
struct can_index
{
    [[nodiscard]] constexpr auto operator[](const auto& indexer) const noexcept -> const auto&
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>()[indexer];
    }

    [[nodiscard]] constexpr auto at(const auto& indexer) const -> const auto&
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().at(indexer);
    }

    [[nodiscard]] constexpr auto operator[](const auto& indexer) noexcept -> auto&
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>()[indexer];
    }

    [[nodiscard]] constexpr auto at(const auto& indexer) -> auto&
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>().at(indexer);
    }
};

}  // namespace twig

template<twig::can_hash_like T>
struct std::hash<T>
{
    [[nodiscard]] auto operator()(T const& s) const noexcept -> std::size_t
    {
        return std::hash<typename T::underlying_type> {}(s.template unwrap<T>());
    }
};
