#include <iostream>
#include <string>

#include <stronk/can_stream.h>
#include <stronk/stronk.h>

struct FirstName : twig::stronk<FirstName, std::string, twig::can_ostream>
{
    using stronk::stronk;
};
struct LastName : twig::stronk<LastName, std::string>
{
    using stronk::stronk;
};

// Strong types protects you from accidentally passing the wrong argument to the wrong parameter.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    // The twig::can_ostream skill overloads the `operator<<(ostream&)` for your type.
    std::cout << firstname << " ";
    // You can also access the underlying type by using the .unwrap<Type>() function.
    std::cout << lastname.unwrap<LastName>() << std::endl;
}

auto main() -> int
{
    print_name(LastName {"Doe"}, FirstName {"John"});
}
