#include "RTTI.hpp"

#include <iostream>

using namespace RTTI;

int main() {
    auto& type = Type::Get<const volatile void* const&>();
    std::cout << type << std::endl;
    return 0;
}
