#include "Ship.h"
#include "Board.h"
#include <iostream>
#include <string>
#include <random>

// ---------------------------------------------------------------------------
// Minimal test runner
// ---------------------------------------------------------------------------

static int passed = 0;
static int failed = 0;

void check(bool condition, const std::string& label) {
    if (condition) {
        std::cout << "  PASS  " << label << "\n";
        ++passed;
    } else {
        std::cout << "  FAIL  " << label << "\n";
        ++failed;
    }
}

void section(const std::string& title) {
    std::cout << "\n-- " << title << " --\n";
}

// ---------------------------------------------------------------------------
// Ship tests
// ---------------------------------------------------------------------------

void testShip() {
    section("Ship");

    Ship s("Destroyer", 2);

    check(s.getName() == "Destroyer",   "getName() returns correct name");
    check(s.getSize() == 2,             "getSize() returns correct size");
    
    check(s.getHits() == 0,             "getHits() starts at 0");
    check(!s.isSunk(),                  "isSunk() false before any hits");

    // Place ship at (3,4) and (3,5)
    s.place({{3, 4}, {3, 5}});

    check(s.getCoordinates().size() == 2, "getCoordinates() has 2 entries after place()");
    check(s.getCoordinates()[0].row == 3, "first coordinate row == 3");
    check(s.getCoordinates()[0].col == 4, "first coordinate col == 4");

    // Miss
    check(!s.checkHit(0, 0),  "checkHit() returns false on a miss");
    check(s.getHits() == 0,   "getHits() unchanged after a miss");
    check(!s.isSunk(),        "isSunk() still false after a miss");

    // First hit
    check(s.checkHit(3, 4),   "checkHit() returns true on a hit");
    check(s.getHits() == 1,   "getHits() == 1 after first hit");
    check(!s.isSunk(),        "isSunk() false after one of two cells hit");

    // Second hit — ship sinks
    check(s.checkHit(3, 5),   "checkHit() returns true on second hit");
    check(s.getHits() == 2,   "getHits() == 2 after second hit");
    check(s.isSunk(),         "isSunk() true when all cells hit");

    // place() resets hit count
    s.place({{1, 1}, {1, 2}});
    check(s.getHits() == 0,   "place() resets hit count");
    check(!s.isSunk(),        "isSunk() false after re-placement");
}

// ---------------------------------------------------------------------------
// Board helper tests
// ---------------------------------------------------------------------------

void testBoardHelpers() {
    section("Board — static helpers");

    check(Board::inBounds(0, 0),            "inBounds(0,0) true");
    check(Board::inBounds(9, 9),            "inBounds(9,9) true");
    check(Board::inBounds(5, 5),            "inBounds(5,5) true");
    check(!Board::inBounds(-1, 0),          "inBounds(-1,0) false");
    check(!Board::inBounds(0, -1),          "inBounds(0,-1) false");
    check(!Board::inBounds(10, 0),          "inBounds(10,0) false");
    check(!Board::inBounds(0, 10),          "inBounds(0,10) false");

    check(Board::colToLetter(0) == 'A',     "colToLetter(0) == 'A'");
    check(Board::colToLetter(9) == 'J',     "colToLetter(9) == 'J'");
    check(Board::colToLetter(4) == 'E',     "colToLetter(4) == 'E'");

    check(Board::letterToCol('A') == 0,     "letterToCol('A') == 0");
    check(Board::letterToCol('J') == 9,     "letterToCol('J') == 9");
    check(Board::letterToCol('a') == 0,     "letterToCol('a') lowercase == 0");
    check(Board::letterToCol('j') == 9,     "letterToCol('j') lowercase == 9");
    check(Board::letterToCol('E') == 4,     "letterToCol('E') == 4");
    check(Board::letterToCol('K') == -1,    "letterToCol('K') returns -1");
    check(Board::letterToCol('Z') == -1,    "letterToCol('Z') returns -1");
    check(Board::letterToCol('1') == -1,    "letterToCol('1') returns -1");
}

// ---------------------------------------------------------------------------
// Board placement tests
// ---------------------------------------------------------------------------

void testBoardPlacement() {
    section("Board — placeShip()");

    {
        Board b;
        Ship s("Cruiser", 3);
        check(b.placeShip(s, 0, 0, true),   "horizontal placement at (0,0) succeeds");
        check(s.getCoordinates().size() == 3, "ship has 3 coordinates after placement");
    }
    {
        Board b;
        Ship s("Cruiser", 3);
        check(b.placeShip(s, 0, 0, false),  "vertical placement at (0,0) succeeds");
    }
    {
        // Horizontal overflow: size-5 ship starting at col 8 goes out of bounds
        Board b;
        Ship s("Carrier", 5);
        check(!b.placeShip(s, 0, 8, true),  "horizontal placement rejected when it overflows right edge");
    }
    {
        // Vertical overflow
        Board b;
        Ship s("Carrier", 5);
        check(!b.placeShip(s, 8, 0, false), "vertical placement rejected when it overflows bottom edge");
    }
    {
        // Exactly fits horizontally (size 5 at col 5 → cols 5-9)
        Board b;
        Ship s("Carrier", 5);
        check(b.placeShip(s, 0, 5, true),   "horizontal placement accepted when it exactly fits");
    }
    {
        // Out-of-bounds origin
        Board b;
        Ship s("Destroyer", 2);
        check(!b.placeShip(s, -1, 0, true), "placement rejected for negative row");
        check(!b.placeShip(s, 0, -1, true), "placement rejected for negative col");
        check(!b.placeShip(s, 10, 0, true), "placement rejected for row == SIZE");
    }
    {
        // Overlap rejection
        Board b;
        Ship a("Destroyer", 2);
        Ship c("Cruiser",   3);
        b.placeShip(a, 2, 2, true); // occupies (2,2) and (2,3)
        check(!b.placeShip(c, 2, 3, false), "overlapping placement rejected");
    }
    {
        // Non-overlapping ships both succeed
        Board b;
        Ship a("Destroyer", 2);
        Ship c("Cruiser",   3);
        b.placeShip(a, 0, 0, true);
        check(b.placeShip(c, 1, 0, true),   "non-overlapping second ship accepted");
    }
}

// ---------------------------------------------------------------------------
// Board attack tests
// ---------------------------------------------------------------------------

void testBoardAttacks() {
    section("Board — receiveAttack()");

    {
        // Miss on empty water
        Board b;
        check(b.receiveAttack(5, 5) == AttackResult::MISS,
              "attack on water returns MISS");
    }
    {
        // Hit
        Board b;
        Ship s("Destroyer", 2);
        b.placeShip(s, 3, 3, true); // (3,3) and (3,4)
        check(b.receiveAttack(3, 3) == AttackResult::HIT,
              "attack on ship cell returns HIT");
    }
    {
        // Sunk — destroyer has size 2
        Board b;
        Ship s("Destroyer", 2);
        b.placeShip(s, 3, 3, true);
        b.receiveAttack(3, 3);
        check(b.receiveAttack(3, 4) == AttackResult::SUNK,
              "second hit on a size-2 ship returns SUNK");
    }
    {
        // ALREADY_ATTACKED
        Board b;
        b.receiveAttack(0, 0);
        check(b.receiveAttack(0, 0) == AttackResult::ALREADY_ATTACKED,
              "repeated attack returns ALREADY_ATTACKED");
    }
    {
        // Hit does not return SUNK for a larger ship
        Board b;
        Ship s("Carrier", 5);
        b.placeShip(s, 0, 0, true);
        check(b.receiveAttack(0, 0) == AttackResult::HIT,
              "first hit on carrier returns HIT not SUNK");
    }
    {
        // Miss does not affect allShipsSunk
        Board b;
        Ship s("Destroyer", 2);
        b.placeShip(s, 0, 0, true);
        b.receiveAttack(9, 9); // miss
        check(!b.allShipsSunk(), "allShipsSunk() false after only a miss");
    }
}

// ---------------------------------------------------------------------------
// Board allShipsSunk tests
// ---------------------------------------------------------------------------

void testAllShipsSunk() {
    section("Board — allShipsSunk()");

    {
        // No ships placed — vacuously true
        Board b;
        check(b.allShipsSunk(), "allShipsSunk() true when no ships placed");
    }
    {
        Board b;
        Ship s("Destroyer", 2);
        b.placeShip(s, 0, 0, true);
        check(!b.allShipsSunk(), "allShipsSunk() false before any attacks");

        b.receiveAttack(0, 0);
        check(!b.allShipsSunk(), "allShipsSunk() false after partial hits");

        b.receiveAttack(0, 1);
        check(b.allShipsSunk(),  "allShipsSunk() true after all cells hit");
    }
    {
        // Two ships — both must be sunk
        Board b;
        Ship a("Destroyer", 2);
        Ship c("Cruiser",   3);
        b.placeShip(a, 0, 0, true); // (0,0),(0,1)
        b.placeShip(c, 2, 0, true); // (2,0),(2,1),(2,2)

        b.receiveAttack(0, 0);
        b.receiveAttack(0, 1); // Destroyer sunk
        check(!b.allShipsSunk(), "allShipsSunk() false when one of two ships remains");

        b.receiveAttack(2, 0);
        b.receiveAttack(2, 1);
        b.receiveAttack(2, 2); // Cruiser sunk
        check(b.allShipsSunk(),  "allShipsSunk() true when both ships sunk");
    }
}

// ---------------------------------------------------------------------------
// Board random placement test
// ---------------------------------------------------------------------------

void testRandomPlacement() {
    section("Board — placeShipRandomly()");

    std::mt19937 rng(42);
    Board b;
    Ship s("Carrier", 5);
    b.placeShipRandomly(s, rng);

    check(s.getCoordinates().size() == 5, "randomly placed ship has 5 coordinates");

    // All coordinates should be in bounds
    bool allInBounds = true;
    for (const auto& c : s.getCoordinates())
        if (!Board::inBounds(c.row, c.col)) allInBounds = false;
    check(allInBounds, "all randomly placed coordinates are in bounds");

    // Place all five standard ships — should never hang or fail
    Ship carrier("Carrier", 5), battleship("Battleship", 4),
         cruiser("Cruiser", 3), submarine("Submarine", 3),
         destroyer("Destroyer", 2);
    Board full;
    full.placeShipRandomly(carrier,    rng);
    full.placeShipRandomly(battleship, rng);
    full.placeShipRandomly(cruiser,    rng);
    full.placeShipRandomly(submarine,  rng);
    full.placeShipRandomly(destroyer,  rng);
    check(true, "placing all five ships randomly completes without error");
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int main() {
    std::cout << "Running Battleship unit tests\n";
    std::cout << "==============================\n";

    testShip();
    testBoardHelpers();
    testBoardPlacement();
    testBoardAttacks();
    testAllShipsSunk();
    testRandomPlacement();

    std::cout << "\n==============================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
