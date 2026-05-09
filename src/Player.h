#pragma once
#include "Board.h"
#include <string>
#include <random>

// Abstract base class for both Human and AI players.
// Person B is responsible for implementing HumanPlayer.cpp and AIPlayer.cpp.

class Player {
public:
    Player(const std::string& name);
    virtual ~Player() = default;

    std::string getName() const;
    Board& getBoard();

    // Place this player's fleet on their board.
    // HumanPlayer prompts for coordinates; AIPlayer places randomly.
    virtual void placeFleet() = 0;

    // Take a turn by attacking the opponent's board.
    // Implementations should print feedback (hit/miss/sunk) and return the result.
    virtual AttackResult takeTurn(Board& opponentBoard) = 0;

protected:
    std::string name_;
    Board board_;

    // The five standard Battleship ships — shared setup, initialized in Player().
    // Person A's placeFleet() implementations will use these.
    Ship carrier_;    // size 5
    Ship battleship_; // size 4
    Ship cruiser_;    // size 3
    Ship submarine_;  // size 3
    Ship destroyer_;  // size 2
};


// --- HumanPlayer ---
// Prompts the user for coordinates via stdin.

class HumanPlayer : public Player {
public:
    HumanPlayer(const std::string& name);

    // Prompt the user to place each ship manually (enter coordinate + orientation).
    void placeFleet() override;

    // Prompt for an attack coordinate (e.g., "B7"), validate, and fire.
    AttackResult takeTurn(Board& opponentBoard) override;

private:
    // Parse a coordinate string like "B7" into row/col.
    // Returns false if the input is malformed or out of range.
    bool parseCoordinate(const std::string& input, int& row, int& col) const;
};


// --- AIPlayer ---
// Places ships and attacks using std::mt19937 (Mersenne Twister).

class AIPlayer : public Player {
public:
    // Seed is provided so Game can control reproducibility (or use random_device).
    explicit AIPlayer(const std::string& name, unsigned int seed);

    // Place all ships in random valid positions using rng_.
    void placeFleet() override;

    // Attack a random un-attacked cell.
    // Stretch goal: implement hunt/target mode (attack adjacent cells after a hit).
    AttackResult takeTurn(Board& opponentBoard) override;

private:
    std::mt19937 rng_;

    // Track cells the AI has already attacked to avoid repeats.
    bool alreadyAttacked_[Board::SIZE][Board::SIZE];
};
