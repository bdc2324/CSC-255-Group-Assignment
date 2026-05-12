#include "./include/Player.h"
#include <iostream>

AIPlayer::AIPlayer(const std::string& name, unsigned int seed)
    : Player(name), rng_(seed) {
    for (int r = 0; r < Board::SIZE; r++)
        for (int c = 0; c < Board::SIZE; c++)
            alreadyAttacked_[r][c] = false;
}

void AIPlayer::placeFleet() {
    board_.placeShipRandomly(carrier_, rng_);
    board_.placeShipRandomly(battleship_, rng_);
    board_.placeShipRandomly(cruiser_, rng_);
    board_.placeShipRandomly(submarine_, rng_);
    board_.placeShipRandomly(destroyer_, rng_);
    std::cout << name_ << " has placed their fleet.\n";
}

AttackResult AIPlayer::takeTurn(Board& opponentBoard) {
    std::uniform_int_distribution<int> dist(0, Board::SIZE - 1);

    int row, col;
    do {
        row = dist(rng_);
        col = dist(rng_);
    } while (alreadyAttacked_[row][col]);

    alreadyAttacked_[row][col] = true;

    std::cout << name_ << " attacks "
              << Board::colToLetter(col) << (row + 1) << ".\n";

    return opponentBoard.receiveAttack(row, col);
}
