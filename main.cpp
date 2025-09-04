#include <iostream>
#include <string>

int main() {
    std::string name;

    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    std::cout << "Hello, " << name << "! Welcome to Flip 7 Helper." << std::endl;

    return 0;
}
