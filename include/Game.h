#pragma once
#include "Player.h"
#include <memory>

// Orchestrates the full game: setup, turn loop, win detection, and end screen.
// Person B is responsible for implementing Game.cpp.

class Game {
public:
    // Takes ownership of two players (human and AI, or any two Player subclasses).
    // player1 always goes first.
    Game(std::unique_ptr<Player> player1, std::unique_ptr<Player> player2);

    // Run the complete game from fleet placement through to declaring a winner.
    // Call this once from main() after constructing the Game.
    void run();

private:
    std::unique_ptr<Player> player1_;
    std::unique_ptr<Player> player2_;

    // --- Private helpers ---

    // Prompt both players to place their fleets (calls placeFleet() on each).
    void setupPhase();

    // Alternate turns between players until one fleet is fully sunk.
    void playPhase();

    // Print the current state of both boards:
    //   - Player's own board (ships visible)
    //   - Opponent's board (ships hidden)
    void displayBoards(const Player& current, const Player& opponent) const;

    // Print a message announcing the winner and any end-of-game stats.
    void announceWinner(const Player& winner) const;
};
