#include "Game.h"
#include "Player.h"
#include <iostream>
#include <memory>
#include <random>
#include <string>

int main() {
    std::cout << "BATTLESHIP\n";

    while (true) {
        std::cout << "Enter your name to play: ";
        std::string name;
        std::cin >> name;

        unsigned int seed = std::random_device{}();

        Game game(
            std::make_unique<HumanPlayer>(name),
            std::make_unique<AIPlayer>("Computer", seed)
        );
        game.run();
        
        char playAgain;
        std::cout << "Do you want to play again? (y/n): ";
        std::cin >> playAgain;

        if (playAgain == 'n' || playAgain == 'N') {
            std::cout << "Thanks for playing, goodbye!\n";
            break;
        }
        
        std::cout << "\n";
    }

    return 0;
}
