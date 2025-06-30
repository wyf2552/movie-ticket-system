import entities;

#include <iostream>

int main() {
    User user;
    std::cout << Movie{}.getStatus() << std::endl;
    std::cout << Cinema{}.getStatus() << std::endl;
    std::cout << Hall{}.getStatus() << std::endl;
}