#include <sstream>

#include <gtest/gtest.h>
#include <stronk/can_stream.h>
#include <stronk/stronk.h>

namespace twig
{

struct an_ostreamable_type : stronk<an_ostreamable_type, int, can_ostream>
{
    using stronk::stronk;
};

TEST(can_ostream, streaming_to_ostream_prints_the_value)  // NOLINT
{
    auto formattable = an_ostreamable_type {5};
    auto sstream = std::stringstream();
    sstream << formattable;

    EXPECT_EQ(sstream.str(), "5");
}

struct an_istreamable_type : stronk<an_istreamable_type, int, can_istream>
{
    using stronk::stronk;
};

TEST(can_ostream, streaming_from_istream_overrides_the_value)  // NOLINT
{
    auto val = an_istreamable_type {5};
    auto sstream = std::stringstream("7");
    sstream >> val;

    EXPECT_EQ(val.unwrap<an_istreamable_type>(), 7);
}

}  // namespace twig
