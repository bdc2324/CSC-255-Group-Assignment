#pragma once
#include "Ship.h"
#include <vector>
#include <string>
#include <random>

// Represents one player's 10x10 grid.
// Person A is responsible for implementing Board.cpp.

// Result returned by receiveAttack() so Game can react appropriately.
enum class AttackResult {
    MISS,
    HIT,
    SUNK,
    ALREADY_ATTACKED
};

class Board {
public:
    static const int SIZE = 10; // Grid is SIZE x SIZE

    Board();

    // --- Setup ---

    // Attempt to place a ship manually (human player).
    // `startRow`, `startCol`: top-left origin of the ship.
    // `horizontal`: true = place left-to-right, false = place top-to-bottom.
    // Returns false if placement is out of bounds or overlaps another ship.
    bool placeShip(Ship& ship, int startRow, int startCol, bool horizontal);

    // Place all ships randomly using the provided RNG engine.
    // Uses std::mt19937 passed in from AIPlayer (or for human auto-place).
    // Call once per ship; retries internally until a valid position is found.
    void placeShipRandomly(Ship& ship, std::mt19937& rng);

    // --- Gameplay ---

    // Process an incoming attack at (row, col).
    // Updates the display grid and delegates hit-checking to ships.
    // Returns an AttackResult so the caller knows what happened.
    AttackResult receiveAttack(int row, int col);

    // Returns true when every ship on this board has been sunk.
    bool allShipsSunk() const;

    // --- Display ---

    // Print the board to stdout.
    // If hideShips is true, ship cells appear as water (used for opponent's board view).
    void display(bool hideShips) const;

    // --- Helpers ---

    // Returns true if (row, col) is within bounds.
    static bool inBounds(int row, int col);

    // Convert a column index (0–9) to a letter ('A'–'J') for display.
    static char colToLetter(int col);

    // Convert a letter ('A'–'J', case-insensitive) to a column index (0–9).
    // Returns -1 if the character is invalid.
    static int letterToCol(char letter);

private:
    // Grid symbols:
    //   '~' = untouched water
    //   'S' = ship (only shown when hideShips == false)
    //   'X' = hit
    //   'O' = miss
    char grid_[SIZE][SIZE];

    // Tracks which cells have already been attacked (prevents re-attacking).
    bool attacked_[SIZE][SIZE];

    // All ships placed on this board.
    std::vector<Ship*> ships_;
};
