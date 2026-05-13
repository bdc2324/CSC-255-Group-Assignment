#include "Game.h"
#include <iostream>
#include <limits>

Game::Game(std::unique_ptr<Player> player1, std::unique_ptr<Player> player2)
    : player1_(std::move(player1)), player2_(std::move(player2)) {}

void Game::run() {
    setupPhase();
    playPhase();
}

void Game::setupPhase() {
    player1_->placeFleet();

    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    player2_->placeFleet();

    std::cout << "\nAll fleets placed. Game is about to begin!\n";
    std::cout << "Press Enter to begin...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Game::playPhase() {
    Player* current  = player1_.get();
    Player* opponent = player2_.get();

    while (true) {
        std::cout << "  " << current->getName() << "'s turn\n";
        displayBoards(*current, *opponent);

        AttackResult result = current->takeTurn(opponent->getBoard());

        switch (result) {
            case AttackResult::MISS: std::cout << "Miss!\n"; break;
            case AttackResult::HIT:  std::cout << "Hit!\n"; break;
            case AttackResult::SUNK: std::cout << "Ship has been sunk!\n"; break;
            default: break;
        }

        if (opponent->getBoard().allShipsSunk()) {
            announceWinner(*current);
            return;
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::swap(current, opponent);
    }
}

void Game::displayBoards(const Player& current, const Player& opponent) const {
    std::cout << "\n--- " << current.getName() << "'s Fleet ---\n";
    current.getBoard().display(false);

    std::cout << "\n--- " << opponent.getName() << "'s Fleet ---\n";
    opponent.getBoard().display(true);
    std::cout << '\n';
}

void Game::announceWinner(const Player& winner) const {
    std::cout << "  " << winner.getName() << " is victorious!\n";
}
