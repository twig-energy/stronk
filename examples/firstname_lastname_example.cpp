#include <iostream>
#include <string>

#include <stronk/skills/can_stream.hpp>
#include <stronk/stronk.hpp>

struct FirstName : twig::stronk<FirstName, std::string>
{
    using stronk::stronk;
};
struct LastName : twig::stronk<LastName, std::string, twig::can_ostream>
{
    using stronk::stronk;
};

// Strong types protects you from accidentally passing the wrong argument to the wrong parameter.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    // You can access the underlying type by using the .unwrap<Type>() function.
    std::cout << firstname.unwrap<FirstName>() << " ";
    // Adding the twig::can_ostream skill overloads the `operator<<(ostream&)` for your type.
    std::cout << lastname << std::endl;
    // Generally you should prefer skills or custom functions over unwrapping to preserve your strong types
}

auto main() -> int
{
    print_name(LastName {"Doe"}, FirstName {"John"});
}
