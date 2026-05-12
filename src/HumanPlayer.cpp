#include "./include/Player.h"
#include <iostream>
#include <string>
#include <cctype>


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
    std::cout << "\n" << name_ << ", place ships.\n";
    std::cout << "Format: <column A-J><row 1-10> <H or V>   (e.g. \"B3 H\")\n\n";

    Ship* ships[] = { &carrier_, &battleship_, &cruiser_, &submarine_, &destroyer_ };

    for (Ship* ship : ships) {
        board_.display(false);
        bool placed = false;
        while (!placed) {
            std::cout << "Place " << ship->getName()
                      << " (size " << ship->getSize() << "): ";
            std::string coord, orient;
            if (!(std::cin >> coord >> orient)) return;

            int row, col;
            if (!parseCoordinate(coord, row, col)) {
                std::cout << "  Invalid coordinate. Use a letter (A-J) and a number (1-10).\n";
                continue;
            }

            bool horizontal = (toupper(static_cast<unsigned char>(orient[0])) == 'H');
            if (!board_.placeShip(*ship, row, col, horizontal)) {
                std::cout << "  Cannot place ship here (out of bounds or overlap). Select again.\n";
                continue;
            }
            placed = true;
        }
        std::cout << '\n';
    }

    board_.display(false);
    std::cout << name_ << "'s fleet is ready!\n";
}

AttackResult HumanPlayer::takeTurn(Board& opponentBoard) {
    while (true) {
        std::cout << name_ << ", enter attack coordinate (e.g. B7): ";
        std::string input;
        if (!(std::cin >> input)) return AttackResult::MISS;

        int row, col;
        if (!parseCoordinate(input, row, col)) {
            std::cout << "  Invalid coordinate. Use a letter (A-J) and a number (1-10).\n";
            continue;
        }

        AttackResult result = opponentBoard.receiveAttack(row, col);
        if (result == AttackResult::ALREADY_ATTACKED) {
            std::cout << "  Already attacked there. Choose a different cell.\n";
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
