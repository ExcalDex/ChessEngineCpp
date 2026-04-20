# Chess Engine

A C++ chess engine featuring a fully functional game implementation with AI opponent powered by minimax search with position evaluation.

---

## Features

- **Full Chess Rules:** Pawn promotion, castling, en passant, check/checkmate detection
- **AI Engine:** Minimax search algorithm with piece-square table evaluation
- **Game State Management:** Full move history with undo/revert functionality
- **Clean Architecture:** Separation of concerns (Model, View, Controller, AI)

---

## Building

### Requirements

- C++17 compiler
- CMake 3.10+

### Build Instructions

```bash
git clone https://github.com/agomesdev/ChessEngineCpp.git
cd ChessEngineCpp
cmake -B build
cmake --build build
```

The executable will be at build/chess on Linux/Mac or build/Debug/chess.exe on Windows.

---

## How to Play

Run the executable and follow the on-screen prompts:

```bash
./chess.exe  # or ./chess on Linux/Mac
```

- Enter coordinates in algebraic notation (e.g., `e2` or `h8`)
- White plays first
- The AI (black) will respond with its best move

---

## Architecture

### Core Components

**Board** (`core/Board.hpp`):
- 8x8 piece matrix representation
- Raw move execution without validation

**Game** (`core/Game.hpp`):
- Chess rule enforcement (legality, check, checkmate, stalemate)
- Move history and undo/revert functionality
- Game state tracking (castling rights, en passant, turn)

**Engine** (`core/Engine.hpp`):
- Minimax search to configurable depth (default: 3)
- Position evaluation using material count + piece-square tables
- Endgame detection for king positioning adjustment

**ChessUI** (`view/ChessUI.hpp`):
- Board display with coordinates
- Input parsing and validation

---

## Design Decisions

**Depth 3 Minimax:** Balances move quality with reasonable search time. Deeper search is possible but requires optimization (alpha-beta pruning, transposition tables).

**Piece-Square Tables:** Standard chess evaluation tables reward piece placement (e.g., pawns advancing, knights in center, kings centralizing in endgame).

**No Inheritance:** Kept the design simple with composition over inheritance, avoiding over-abstraction.

**Stack-only allocation:** The project deliberately avoids heap allocations wherever possible. All core game state — the board, piece data, game state structs — lives on the stack. This keeps memory management predictable, avoids fragmentation, and removes the overhead of dynamic allocation in the hot path of the search algorithm. The choice reflects a conscious effort to write C++ that actually takes advantage of the language rather than treating it like a garbage-collected language with manual cleanup.

---

## The Journey

This is my fifth chess implementation, and the first one I'm fully satisfied with architecturally.

It started in 2021 with a ~3000 line implementation in VisuAlg (a Portuguese pseudocode language) during the first year of technical school — built purely because the coursework got too easy. That version is lost.

From there came a JavaScript web implementation during the second year, featuring a partially functional AI called `PeacockBass` — the name has no official explanation. It was abandoned due to JavaScript's performance limits and a bug in the capture evaluation that took too long to track down. The code is still on GitHub if you're curious.

Then a first C++ version, functional but without proper architecture. Then implementations in Java, Rust, and others — each one using chess as a benchmark for learning a new language. The same problem, different constraints, different lessons each time.

This version is the result of all of that. Same problem, finally done right.

---

## Known Limitations

- Search depth limited to 3 (sufficient for reasonable play, but not tournament-level)
- No opening book or endgame tables
- Evaluation considers only material and position, not advanced tactics (pins, forks, etc.)
- No alpha-beta pruning (planned for future versions)

---

## Future Improvements

- Alpha-beta pruning for faster search
- Transposition table to cache evaluated positions
- Move ordering heuristics to improve pruning efficiency
- Opening book
- Endgame tablebases
- Perft testing for move generation verification
- Python test suite for automated position validation

---

## Technical Notes

The engine uses a minimax algorithm with alternating maximization/minimization layers:

- Engine maximizes its own position score
- Opponent minimizes the engine's advantage (plays optimally against you)
- Positions are evaluated at max depth; deeper moves are explored recursively

Position evaluation = material value + positional bonuses from piece-square tables. Checkmate returns ±∞; stalemate returns -50 (discouraged but acceptable).

---

## About the Author

I'm André, a Computer Science student at UFLA (Universidade Federal de Lavras), with a technical degree in Systems Development from CEFET-MG Timóteo.

I tend to learn by building things that genuinely interest me rather than following curricula. Chess has been a recurring benchmark throughout that process — something complex enough to teach you a lot about a language, familiar enough that the logic never gets in the way of the learning.

Most of what I know I taught myself out of curiosity: multiple programming languages, chess engines, procedural dungeon generators, PDF parsers, web scrapers. I'm most comfortable in C++, Python and Java, and most interested in algorithm design and software architecture.

If you want to see more: [github.com/andresgcsdev](https://github.com/andresgcsdev)

---

*Built as a portfolio project to demonstrate C++ fundamentals, algorithm design, and software architecture.*