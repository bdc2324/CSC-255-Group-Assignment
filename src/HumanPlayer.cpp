#include "Player.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <limits>


Player::Player(const std::string& name)
    : name_(name),
      carrier_("Carrier", 5),
      battleship_("Battleship", 4),
      cruiser_("Cruiser", 3),
      submarine_("Submarine", 3),
      destroyer_("Destroyer", 2) {}

std::string Player::getName() const { return name_; }

Board& Player::getBoard() { return board_; }

const Board& Player::getBoard() const { return board_; }


HumanPlayer::HumanPlayer(const std::string& name) : Player(name) {}

void HumanPlayer::placeFleet() {
    std::cout << "\n" << name_ << ", place your fleet.\n";
    std::cout << "Format: <column A-J><row 1-10> <H or V>   (e.g. \"B3 H\")\n\n";

    // Flush any newline left in the buffer from the previous cin >> 
    // Necessary incase of typos that fill the buffer and confuse the player
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Ship* ships[] = { &carrier_, &battleship_, &cruiser_, &submarine_, &destroyer_ };

    for (Ship* ship : ships) {
        board_.display(false);
        bool placed = false;
        while (!placed) {
            std::cout << "Place " << ship->getName()
                      << " (size " << ship->getSize() << "): ";

            // Read the entire line so a missing space can't corrupt following reads.
            std::string line;
            if (!std::getline(std::cin, line)) return;

            std::istringstream iss(line);
            std::string coord, orient;
            if (!(iss >> coord >> orient)) {
                std::cout << "  Enter coordinate and orientation on the same line (e.g. B3 H).\n";
                continue;
            }

            int row, col;
            if (!parseCoordinate(coord, row, col)) {
                std::cout << "  Invalid coordinate. Use a letter (A-J) and a number (1-10).\n";
                continue;
            }

            bool horizontal = (toupper(static_cast<unsigned char>(orient[0])) == 'H');
            if (!board_.placeShip(*ship, row, col, horizontal)) {
                std::cout << "Cannot place ship here (out of bounds or overlap). Select again.\n";
                continue;
            }
            placed = true;
        }
        std::cout << '\n';
    }

    board_.display(false);
    std::cout << name_ << ", your fleet has been deployed.\n";
}

AttackResult HumanPlayer::takeTurn(Board& opponentBoard) {
    while (true) {
        std::cout << name_ << ", enter attack coordinate (B1, E7, etc.): ";
        std::string input;
        if (!(std::cin >> input)) return AttackResult::MISS;
        // Flush the rest of the line so Game's "Press Enter" pause works correctly.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        int row, col;
        if (!parseCoordinate(input, row, col)) {
            std::cout << "Invalid coordinate. Use a letter (A-J) and a number (1-10).\n";
            continue;
        }

        AttackResult result = opponentBoard.receiveAttack(row, col);
        if (result == AttackResult::ALREADY_ATTACKED) {
            std::cout << "Already attacked there. Select a different cell.\n";
            continue;
        }
        return result;
    }
}

bool HumanPlayer::parseCoordinate(const std::string& input,
                                   int& row, int& col) const {
    if (input.size() < 2) return false;

    col = Board::letterToCol(input[0]);
    if (col == -1) return false;

    try {
        int r = std::stoi(input.substr(1));
        if (r < 1 || r > Board::SIZE) return false;
        row = r - 1;
    } catch (...) {
        return false;
    }

    return true;
}
