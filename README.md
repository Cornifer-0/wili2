# wili2

A chess engine written from scratch in C++, with a playable terminal interface, magic-bitboard move generation, and an opening book. No external chess libraries or AI-generated code — built entirely from memory during a week without internet access.

<!-- screenshot -->
<img width="573" height="261" alt="Image" src="https://github.com/user-attachments/assets/4f3bc28d-c881-40a8-a36b-ccde7c997646" />

## Features

- **Bitboard board representation** for fast move generation and evaluation
- **Magic bitboards** for sliding piece (rook/bishop/queen) attack generation
- **Zobrist hashing** with a **transposition table** to avoid re-searching known positions
- **Opening book** for stronger, faster play in the early game
- **Alpha-beta search** with a configurable think time
- **Terminal UI**: play directly against the engine from the command line, with standard algebraic notation input
- **Doxygen documentation**: every function is documented, with generated HTML and LaTeX reference manuals included in `docs/`
- **Unit tests** (`tests/`) covering core components

## Getting started

### Build

```bash
make
```

### Play

```bash
./bin/wili2
```

You'll be prompted to enter moves in standard algebraic notation, for example:

```
Nf3
e4
O-O
gxf3
```

After each of your moves, the engine thinks for a few seconds (default: ~3s) and responds with its move on the board. Only the terminal version is currently available.

<img width="605" height="311" alt="Image" src="https://github.com/user-attachments/assets/00114245-b7a8-4abd-af10-c6c4264c5f55" />

## Project structure

```
wili2/
├── bin/          # Compiled binary (wili2)
├── build/        # Build artifacts (.o files)
├── docs/         # Generated documentation
│   ├── html/     # Doxygen HTML reference
│   └── latex/    # Doxygen LaTeX reference manual
├── include/      # Header files (bitboard, board, book, eval, magic, search, tt, zobrist, ...)
├── src/          # Implementation files
├── tests/        # Unit tests
└── Makefile
```

Each core system lives in its own module, both in `include/` and `src/`:

| Module | Responsibility |
|---|---|
| `bitboard` | Core bitboard representation and operations |
| `board` / `boardManipulation` | Board state and move application |
| `magic` / `magic_data` | Magic-bitboard attack table generation for sliding pieces |
| `movegen` | Legal move generation |
| `eval` | Position evaluation |
| `search` | Alpha-beta search |
| `tt` / `zobrist` | Transposition table and Zobrist hashing |
| `book` | Opening book lookup |
| `types` / `defs` / `PST_data` | Shared types, constants, and piece-square tables |

## Documentation

Full API documentation is generated with [Doxygen](https://www.doxygen.nl/), covering every function in the codebase. ( /docs/html/index.html )

**[View the full documentation →](https://Cornifer-0.github.io/wili2/)**

To regenerate it locally:

```bash
doxygen doxygen_config
```

A LaTeX reference manual is also available in `docs/latex/` (build with `make` inside that folder to produce a PDF).

## Background

This engine was built independently over about a week, without internet access, as a personal project to understand chess engine architecture, search algorithms, and low-level board representation in depth. It is currently being extended further.

## License

This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.
