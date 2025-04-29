#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    // Seed the random number generator with the current time
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Generate and print 10 random numbers
    std::cout << "Random Numbers:" << std::endl;
    for (int i = 0; i < 10; ++i) {
        int randomNumber = std::rand(); // Generate a random number
        std::cout << randomNumber << std::endl;
    }

    return 0;
}
