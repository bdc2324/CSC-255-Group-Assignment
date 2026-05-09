#pragma once
#include <string>
#include <vector>

// Represents a single ship on the board.
// Person A is responsible for implementing Ship.cpp.

struct Coordinate {
    int row; // 0–9
    int col; // 0–9
};

class Ship {
public:
    // Constructor: provide display name and size (number of cells)
    Ship(const std::string& name, int size);

    // Getters
    std::string getName() const;
    int getSize() const;
    int getHits() const;
    bool isSunk() const;

    // Place the ship on the board by providing all occupied coordinates.
    // Call this once during setup (manual or random placement).
    void place(const std::vector<Coordinate>& coords);

    // Returns true if this coordinate is a hit on this ship (and records it).
    // Returns false if the coordinate misses or was already hit.
    bool checkHit(int row, int col);

    // Returns all coordinates this ship occupies (used by Board for collision checks).
    const std::vector<Coordinate>& getCoordinates() const;

private:
    std::string name_;
    int size_;
    int hits_;
    std::vector<Coordinate> coordinates_;
};
