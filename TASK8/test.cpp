#include <iostream>
#include <memory>

void test() noexcept {
    throw 5;  // ❌
    std::cout <<"hello";
}

int main() {
    test();
}
