# Battleship [README DRAFT IN PROGRESS]

A command-line implementation of the Battleship board game written in C++. Completed as a group assignment for CSC 255: Objects and Algorithms at Oakton College.

---

## Build

Requires a C++14-compatible compiler (`g++` or `clang++`) and `make`.

```
make
```

To recompile from scratch:

```
make clean && make
```

To build and run the unit tests:

```
make test
```

---

## How to Play

```
./battleship
```

### Fleet Setup

At the start of the game you place all five ships on your 10x10 grid. For each ship enter a starting coordinate and an orientation on the same line:

```
Place Carrier (size 5): B3 H
```

- The **coordinate** is a column letter (`A`–`J`) followed by a row number (`1`–`10`).
- The **orientation** is `H` (horizontal, extends right) or `V` (vertical, extends down).

The computer places its fleet automatically.

### Taking Turns

On your turn, enter the coordinate you want to attack:

```
Brendan, enter attack coordinate (B1, E7, etc.): G7
```

The result is printed after each shot. You and the computer alternate until one side's entire fleet is sunk.

### Board Symbols

| Symbol | Meaning |
|--------|---------|
| `~` | Untouched water |
| `S` | Your ship (only visible on your own board) |
| `X` | Hit |
| `O` | Miss |

### The Fleet

| Ship | Size |
|------|------|
| Carrier | 5 |
| Battleship | 4 |
| Cruiser | 3 |
| Submarine | 3 |
| Destroyer | 2 |

---

## Project Structure

```
CSC-255-Group-Assignment/
├── include/                  # Class declarations
│   ├── Ship.h                # Ship — position, hit tracking, sunk state
│   ├── Board.h               # Board — 10x10 grid, placement, attack logic
│   ├── Player.h              # Player base class, HumanPlayer, AIPlayer
│   └── Game.h                # Game — setup phase and turn loop
├── src/                      # Implementations
│   ├── Board.cpp
│   ├── HumanPlayer.cpp       # Also contains Player base class definitions
│   ├── AIPlayer.cpp
│   ├── Game.cpp
│   └── main.cpp
├── tests/
│   └── tests.cpp             # Unit tests for Ship and Board
├── resources/                # Assignment documents
└── Makefile
```

### Class Responsibilities

| Class | Responsibility |
|-------|---------------|
| `Ship` | Stores a ship's name, size, and occupied coordinates. Tracks hits and exposes `isSunk()`. |
| `Board` | Owns the 10x10 grid and a list of ships placed on it. Handles placement validation, random placement, incoming attacks, and display. |
| `Player` | Abstract base class. Holds a `Board` and the five standard ships. Declares `placeFleet()` and `takeTurn()` as pure virtual methods. |
| `HumanPlayer` | Prompts the user for ship placement and attack coordinates via stdin. Validates and re-prompts on bad input. |
| `AIPlayer` | Places ships randomly and attacks using a hunt/target strategy (see Technical Overview). |
| `Game` | Owns two `Player` instances. Runs the setup phase and turn loop, displays boards, and announces the winner. |

---

## Technical Overview

### Topics Covered

This project was built around two of the course topics: **Games** and **Random Number Generation**.

### Object-Oriented Design

The project is structured around an inheritance hierarchy that reflects the core principle of the course:

- **Encapsulation** — each class owns its data and exposes only what other classes need. `Board` manages its own grid and ship list internally; callers only interact through `placeShip()`, `receiveAttack()`, and `display()`.
- **Inheritance** — `HumanPlayer` and `AIPlayer` both extend the abstract `Player` base class, sharing a common interface (`placeFleet()`, `takeTurn()`) while implementing different behavior.
- **Polymorphism** — `Game` holds two `std::unique_ptr<Player>` pointers and drives the entire game loop through the base class interface, with no knowledge of whether a player is human or AI.

### Random Number Generation

The project uses the C++ `<random>` library rather than the older `rand()` function. Two components are involved:

- **`std::random_device`** — a non-deterministic entropy source used to generate the initial seed in `main.cpp`. This ensures each game produces a different result.
- **`std::mt19937`** — the Mersenne Twister engine, seeded once and stored in `AIPlayer`. It is passed by reference into `Board::placeShipRandomly()` so the same engine drives both placement and attacks, keeping the RNG state consistent. `std::uniform_int_distribution<int>` is used to map raw engine output to valid grid coordinates.

### Algorithms and Data Structures

**Ship placement** (`Board::placeShipRandomly`) uses a retry loop: generate a random origin and orientation, attempt to place, and repeat until a valid position is found. Placement validation does a two-pass check — first confirming all cells are free, then committing — so the grid is never left in a partial state.

**Coordinate system** — internally all positions are stored as zero-indexed `(row, col)` integer pairs. The `Board` class provides static helpers `colToLetter()` and `letterToCol()` to translate between the internal representation and the `A1`–`J10` format shown to the player.

**Attack results** — `Board::receiveAttack()` returns an `AttackResult` enum (`MISS`, `HIT`, `SUNK`, `ALREADY_ATTACKED`). This lets `Game` react to outcomes through a single return value without needing to inspect board state directly.

**AI search/target strategy** — `AIPlayer::takeTurn()` operates in two modes:

- *Search mode* — selects a random un-attacked cell using the Mersenne Twister.
- *Target mode* — after a hit, the four orthogonal neighbours of the hit cell are pushed onto a `std::queue`. The AI drains this queue on subsequent turns before returning to random selection. When a ship is sunk the queue is cleared, ending target mode.

This mimics the strategy an real human player would likely use and produces noticeably more engaging play than fully random AI.

### Testing

A self-contained test suite lives in `tests/tests.cpp` and requires no external framework. Run it with:

```
make test
```

62 tests across 6 sections cover the two core classes:

| Section | What is tested |
|---------|---------------|
| `Ship` | Constructor, getters, `place()`, `checkHit()` hits and misses, `isSunk()`, hit count reset on re-placement |
| `Board` — static helpers | `inBounds()` at all edges, `colToLetter()`, `letterToCol()` including lowercase and invalid input |
| `Board` — `placeShip()` | Valid horizontal/vertical, overflow on both axes, exact fit, out-of-bounds origin, overlap rejection, two non-overlapping ships |
| `Board` — `receiveAttack()` | Miss, hit, sunk, already-attacked, hit-not-sunk on a multi-cell ship |
| `Board` — `allShipsSunk()` | No ships placed, one ship partially/fully hit, two ships requiring both to be sunk |
| `Board` — `placeShipRandomly()` | Coordinates populated, all in bounds, full five-ship fleet completes without error |

### Development Process

TBA