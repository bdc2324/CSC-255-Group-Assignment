#include "Player.h"
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
    std::cout << name_ << " has deployed their fleet.\n";
}


/* Computer AI will target random cells, unless it has hit and not sunk a ship, 
    in which case it will attack nearby cells first. */
AttackResult AIPlayer::takeTurn(Board& opponentBoard) {
    int row = -1, col = -1;

    // Target mode: use the queue of follow-up cells from a previous hit.
    while (!targetQueue_.empty()) {
        std::pair<int,int> cell = targetQueue_.front();
        targetQueue_.pop();
        if (!alreadyAttacked_[cell.first][cell.second]) {
            row = cell.first;
            col = cell.second;
            break;
        }
    }

    // Search mode: pick a random un-attacked cell.
    if (row == -1) {
        std::uniform_int_distribution<int> dist(0, Board::SIZE - 1);
        do {
            row = dist(rng_);
            col = dist(rng_);
        } while (alreadyAttacked_[row][col]);
    }

    alreadyAttacked_[row][col] = true;
    std::cout << name_ << " attacks "
              << Board::colToLetter(col) << (row + 1) << ".\n";

    AttackResult result = opponentBoard.receiveAttack(row, col);

    if (result == AttackResult::HIT) {
        // Queue all valid, un-attacked neighbours for follow-up attacks.
        const int dr[] = {-1, 1,  0, 0};
        const int dc[] = { 0, 0, -1, 1};
        for (int i = 0; i < 4; i++) {
            int nr = row + dr[i];
            int nc = col + dc[i];
            if (Board::inBounds(nr, nc) && !alreadyAttacked_[nr][nc])
                targetQueue_.push({nr, nc});
        }
    } else if (result == AttackResult::SUNK) {
        // Discard leftover targets and return to search mode.
        while (!targetQueue_.empty()) targetQueue_.pop();
    }

    return result;
}
