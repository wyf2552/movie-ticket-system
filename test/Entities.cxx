import Entities;

#include <iostream>

int main() {
    std::cout << Movie{}.getStatus() << std::endl;
    std::cout << Cinema{}.getStatus() << std::endl;
    std::cout << Hall{}.getStatus() << std::endl;
}