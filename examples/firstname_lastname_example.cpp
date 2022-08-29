#include <iostream>
#include <string>

#include <stronk/stronk.h>

struct FirstName : twig::stronk<FirstName, std::string, twig::can_stream>
{
    using stronk::stronk;
};
struct LastName : twig::stronk<LastName, std::string, twig::can_stream>
{
    using stronk::stronk;
};

// Strong types protects you from accidentially parsing a wrong arguments to wrong positions.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    std::cout << firstname << " " << lastname << std::endl;
}

auto main() -> int
{
    print_name(LastName {"Doe"}, FirstName {"John"});
}
