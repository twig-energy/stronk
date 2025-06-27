#include <sstream>

#include "stronk/skills/can_stream.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_ostreamable_type : stronk<an_ostreamable_type, int, can_ostream>
{
    using stronk::stronk;
};

TEST_SUITE("can_ostream")
{
    TEST_CASE("streaming_to_ostream_prints_the_value")
    {
        auto formattable = an_ostreamable_type {5};
        auto sstream = std::stringstream();
        sstream << formattable;

        CHECK_EQ(sstream.str(), "5");
    }

    struct an_istreamable_type : stronk<an_istreamable_type, int, can_istream>
    {
        using stronk::stronk;
    };

    TEST_CASE("streaming_from_istream_overrides_the_value")
    {
        auto val = an_istreamable_type {5};
        auto sstream = std::stringstream("7");
        sstream >> val;

        CHECK_EQ(val.unwrap<an_istreamable_type>(), 7);
    }
}

}  // namespace twig
