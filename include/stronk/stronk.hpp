#pragma once
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>

#include "stronk/utilities/equality.hpp"
#include "stronk/utilities/macros.hpp"

namespace twig
{

template<typename Tag, typename T, template<typename> typename... Skills>
struct stronk : public Skills<Tag>...
{
    using self_t = stronk<Tag, T, Skills...>;
    using underlying_type = T;

    // we need the underlying value to have public visibility for stronk types to be usable as non-type template
    // parameters. This is to fulfill the `literal class type` requirement.
    // To discourage direct usage of the underlying value, we have given it a long ugly name.
    T _you_should_not_be_using_this_but_rather_unwrap;  // NOLINT(readability-identifier-naming)

    constexpr stronk() noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    template<typename ConvertConstructibleT>
        requires(std::convertible_to<ConvertConstructibleT, underlying_type>
                 && !std::same_as<ConvertConstructibleT, self_t>)
    STRONK_FORCEINLINE constexpr explicit stronk(ConvertConstructibleT&& value)
        : _you_should_not_be_using_this_but_rather_unwrap(std::forward<ConvertConstructibleT>(value))
    {
    }

    template<typename... ConvertConstructibleTs>
        requires(std::constructible_from<underlying_type, ConvertConstructibleTs...>
                 && sizeof...(ConvertConstructibleTs) >= 2)
    STRONK_FORCEINLINE constexpr explicit stronk(ConvertConstructibleTs&&... values)
        : _you_should_not_be_using_this_but_rather_unwrap(std::forward<ConvertConstructibleTs>(values)...)
    {
    }

    template<typename E>
        requires(std::constructible_from<underlying_type, std::initializer_list<E>>)
    STRONK_FORCEINLINE constexpr stronk(std::initializer_list<E> init_list)
        : _you_should_not_be_using_this_but_rather_unwrap(init_list)
    {
    }

    template<typename ExpectedT>
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto unwrap() noexcept -> underlying_type&
    {
        static_assert(std::same_as<ExpectedT, Tag>,
                      "To access the underlying type you need to provide the stronk type you expect to be querying. By "
                      "doing so you will be protected from unsafe accesses if you chose to change the type");
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }

    template<typename ExpectedT>
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto unwrap() const noexcept -> const underlying_type&
    {
        static_assert(std::same_as<ExpectedT, Tag>,
                      "To access the underlying type you need to provide the stronk type you expect to be querying. By "
                      "doing so you will be protected from unsafe accesses if you chose to change the type");
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }

    // useful for recursively getting an underlying type - in case of stronk types of stronk types, or to ensure that
    // the underlying type is what you expect.
    template<typename ExpectedT, typename ExpectedUnderlyingT>
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto unwrap_as() noexcept -> ExpectedUnderlyingT&
    {
        if constexpr (std::same_as<underlying_type, ExpectedUnderlyingT>) {
            return this->unwrap<ExpectedT>();
        } else {
            return this->unwrap<ExpectedT>().template unwrap_as<underlying_type, ExpectedUnderlyingT>();
        }
    }

    // useful for recursively getting an underlying type - in case of stronk types of stronk types, or to ensure that
    // the underlying type is what you expect.
    template<typename ExpectedT, typename ExpectedUnderlyingT>
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto unwrap_as() const noexcept -> const ExpectedUnderlyingT&
    {
        if constexpr (std::same_as<underlying_type, ExpectedUnderlyingT>) {
            return this->unwrap<ExpectedT>();
        } else {
            return this->unwrap<ExpectedT>().template unwrap_as<underlying_type, ExpectedUnderlyingT>();
        }
    }

    // unwraps the type and returns a new StronkT with the result of the function
    template<typename FunctorT>
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto transform(const FunctorT& functor) const -> Tag
    {
        return Tag {functor(this->val())};
    }

    STRONK_FORCEINLINE constexpr friend void swap(stronk& a, stronk& b) noexcept
    {
        using std::swap;
        swap(a._you_should_not_be_using_this_but_rather_unwrap, b._you_should_not_be_using_this_but_rather_unwrap);
    }

  protected:
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto val() noexcept -> T&
    {
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }
    [[nodiscard]]
    STRONK_FORCEINLINE constexpr auto val() const noexcept -> const T&
    {
        return this->_you_should_not_be_using_this_but_rather_unwrap;
    }
};

template<typename T>
concept stronk_like = requires(T v) {
    {
        v.template unwrap<T>()
    } -> std::convertible_to<typename T::underlying_type>;
};

template<typename StronkT>
struct can_negate
{
    STRONK_FORCEINLINE constexpr friend auto operator-(const StronkT& elem) -> StronkT
    {
        return StronkT {-elem.template unwrap<StronkT>()};
    }
};

template<typename StronkT>
struct can_add
{
    STRONK_FORCEINLINE constexpr friend auto operator+=(StronkT& lhs, const StronkT& rhs) -> StronkT
    {
        lhs.template unwrap<StronkT>() += rhs.template unwrap<StronkT>();
        return lhs;
    }

    STRONK_FORCEINLINE constexpr friend auto operator+(const StronkT& lhs, const StronkT& rhs) -> StronkT
    {
        return StronkT {lhs.template unwrap<StronkT>() + rhs.template unwrap<StronkT>()};
    }
};

template<typename StronkT>
struct can_subtract
{
    STRONK_FORCEINLINE constexpr friend auto operator-=(StronkT& lhs, const StronkT& rhs) -> StronkT
    {
        lhs.template unwrap<StronkT>() -= rhs.template unwrap<StronkT>();
        return lhs;
    }

    STRONK_FORCEINLINE constexpr friend auto operator-(const StronkT& lhs, const StronkT& rhs) -> StronkT
    {
        return StronkT {lhs.template unwrap<StronkT>() - rhs.template unwrap<StronkT>()};
    }
};

template<typename StronkT>
struct can_equate
{
    STRONK_FORCEINLINE constexpr friend auto operator==(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        static_assert(!std::is_floating_point_v<typename StronkT::underlying_type>);
        return lhs.template unwrap<StronkT>() == rhs.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_less_than_greater_than
{
    constexpr friend auto operator<(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        return lhs.template unwrap<StronkT>() < rhs.template unwrap<StronkT>();
    }
    constexpr friend auto operator>(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        return lhs.template unwrap<StronkT>() > rhs.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_less_than_greater_than_or_equal
{
    constexpr friend auto operator<=(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        return lhs.template unwrap<StronkT>() <= rhs.template unwrap<StronkT>();
    }
    constexpr friend auto operator>=(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        return lhs.template unwrap<StronkT>() >= rhs.template unwrap<StronkT>();
    }
};

// "Parameter type" class for 'can_equate_with_is_close_base'
struct is_close_params
{
    template<typename T>
    constexpr static auto abs_tol()
    {
        return twig::stronk_details::default_abs_tol<T>();
    }
    template<typename T>
    constexpr static auto rel_tol()
    {
        return twig::stronk_details::default_rel_tol<T>();
    }
    constexpr static bool nan_equals = false;
};

// "Parameter type" class for 'can_equate_with_is_close_base'
struct is_close_with_nan_equals_params
{
    template<typename T>
    constexpr static auto abs_tol()
    {
        return twig::stronk_details::default_abs_tol<T>();
    }
    template<typename T>
    constexpr static auto rel_tol()
    {
        return twig::stronk_details::default_rel_tol<T>();
    }
    constexpr static bool nan_equals = true;
};

// "Parameter type" class for 'can_equate_with_is_close_base'
struct is_close_using_abs_tol_only_params
{
    template<typename T>
    constexpr static auto abs_tol() -> T
    {
        return twig::stronk_details::default_abs_tol<T>();
    }

    template<typename T>
    constexpr static auto rel_tol() -> T
    {
        return T {0};
    }
    constexpr static bool nan_equals = false;
};

template<typename StronkT, typename CloseParamsT>
struct can_equate_with_is_close_base
{
    constexpr friend auto operator==(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        return twig::stronk_details::almost_equals<CloseParamsT>(lhs, rhs);
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
struct can_equate_underlying_type_specific
{
    STRONK_FORCEINLINE constexpr friend auto operator==(const StronkT& lhs, const StronkT& rhs) -> bool
    {
        if constexpr (std::is_floating_point_v<typename StronkT::underlying_type>) {
            return twig::stronk_details::almost_equals<is_close_using_abs_tol_only_params>(lhs, rhs);
        } else {
            return lhs.template unwrap<StronkT>() == rhs.template unwrap<StronkT>();
        }
    }
};

template<typename StronkT>
struct can_order
{
    // note you probably also want the == operator either from can_equate or one
    // of the can_equate_with_is_close

    STRONK_FORCEINLINE constexpr friend auto operator<=>(const StronkT& lhs, const StronkT& rhs)
    {
        return lhs.template unwrap<StronkT>() <=> rhs.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_size
{
    [[nodiscard]]
    constexpr auto size() const -> std::size_t
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().size();
    }
    [[nodiscard]]
    constexpr auto empty() const -> bool
    {
        return this->size() == static_cast<std::size_t>(0);
    }
};

}  // namespace twig
