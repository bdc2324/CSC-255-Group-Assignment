#include "Board.h"
#include <iostream>

Board::Board() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++) {
            grid_[r][c]     = '~';
            attacked_[r][c] = false;
        }
}

bool Board::placeShip(Ship& ship, int startRow, int startCol, bool horizontal) {
    if (!inBounds(startRow, startCol)) return false;

    int size = ship.getSize();
    if ( horizontal && startCol + size > SIZE) return false;
    if (!horizontal && startRow + size > SIZE) return false;

    // check for open cells on the first pass
    for (int i = 0; i < size; i++) {
        int r = horizontal ? startRow     : startRow + i;
        int c = horizontal ? startCol + i : startCol;
        if (grid_[r][c] == 'S') return false;
    }

    // Place ship on second pass
    std::vector<Coordinate> coords;
    for (int i = 0; i < size; i++) {
        int r = horizontal ? startRow     : startRow + i;
        int c = horizontal ? startCol + i : startCol;
        coords.push_back({r, c});
        grid_[r][c] = 'S';
    }

    ship.place(coords);
    ships_.push_back(&ship);
    return true;
}

void Board::placeShipRandomly(Ship& ship, std::mt19937& rng) {
    std::uniform_int_distribution<int> posDist(0, SIZE - 1);
    std::uniform_int_distribution<int> orientDist(0, 1);

    // Retry until a valid spot is found.
    while (true) {
        int  row        = posDist(rng);
        int  col        = posDist(rng);
        bool horizontal = orientDist(rng);
        if (placeShip(ship, row, col, horizontal)) return;
    }
}

AttackResult Board::receiveAttack(int row, int col) {
    if (attacked_[row][col]) return AttackResult::ALREADY_ATTACKED;

    attacked_[row][col] = true;

    for (Ship* ship : ships_) {
        if (ship->checkHit(row, col)) {
            grid_[row][col] = 'X';
            return ship->isSunk() ? AttackResult::SUNK : AttackResult::HIT;
        }
    }

    grid_[row][col] = 'O';
    return AttackResult::MISS;
}

bool Board::allShipsSunk() const {
    for (const Ship* ship : ships_)
        if (!ship->isSunk()) return false;
    return true;
}

void Board::display(bool hideShips) const {
    // Col headers
    std::cout << "   ";
    for (int c = 0; c < SIZE; c++)
        std::cout << colToLetter(c) << ' ';
    std::cout << '\n';

    for (int r = 0; r < SIZE; r++) {
        // row number alignment
        if (r + 1 < 10) std::cout << ' ';
        std::cout << (r + 1) << ' ';

        for (int c = 0; c < SIZE; c++) {
            char cell = grid_[r][c];
            if (hideShips && cell == 'S') cell = '~';
            std::cout << cell << ' ';
        }
        std::cout << '\n';
    }
}

bool Board::inBounds(int row, int col) {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

char Board::colToLetter(int col) {
    return static_cast<char>('A' + col);
}

int Board::letterToCol(char letter) {
    letter = static_cast<char>(toupper(static_cast<unsigned char>(letter)));
    if (letter < 'A' || letter > 'J') return -1;
    return letter - 'A';
}
