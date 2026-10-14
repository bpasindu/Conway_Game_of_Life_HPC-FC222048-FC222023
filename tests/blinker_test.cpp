#include "../src/game_of_life_cpu.hpp"

#include <iostream>

int main() {
    if (!runBlinkerCorrectnessTest()) {
        std::cerr << "Blinker correctness test failed.\n";
        return 1;
    }

    std::cout << "Blinker correctness test passed.\n";
    return 0;
}
