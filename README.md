# Tidal

C Chess engine

![Build](https://github.com/Bungogood/tidal/actions/workflows/make.yml/badge.svg)

For chess, b≈35, m ≈ 100 (average ≈ 40) for "reasonable" games
- exact solution completely infeasible!
- would like to eliminate (large) parts of game tree  
$$35^{40}=5.791*10^{61}$$
$$35^{100}=2.552*10^{154}$$
$b \approx 35$
number of moves
$m \approx 100$
$average \approx 40$


## Installation
requires gcc and make
```bash
git clone git@github.com:Bungogood/C-Chess.git
cd C-Chess
mkdir .obj
make && ./chess
```

## Standards

- FEN (Forsyth–Edwards Notation)
- SAN (Standard / Short Algebraic notation)
- LAN (Long Algebraic notation)
- PGN (Portable Game Notation)
- UCI (Universal Chess Interface)

## Techniques

Currently implemented:
- bitboards
- negamax
- alpha beta pruning
- quiescence search

To be added:
- transposition table

## API format
```json
{
    "fen": "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "history": [ // include pgn?

    ],
    "possible": [

    ]
}
```

## Stockfish

```
position fen {fen}
go perft {depth}
```

## Ideas

remap



encode capture type
re write gererate move
store attacked squares
change sort

```
             binary move bits             information      hexidecimal masks
    
    0000 0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0000 1111 0000 0000 0000 0000 0000    captured piece      0xf00000
    0001 0000 0000 0000 0000 0000 0000    capture flag        0x1000000
    0010 0000 0000 0000 0000 0000 0000    double push flag    0x2000000
    0100 0000 0000 0000 0000 0000 0000    enpassant flag      0x4000000
    1000 0000 0000 0000 0000 0000 0000    castling flag       0x8000000
```

## References

### General
- Chess Programming youtube [Playlist](https://www.youtube.com/playlist?list=PLmN0neTso3Jxh8ZIylk74JpwfiWNI76Cs)
### Wikipedia:
- [Negamax](https://en.wikipedia.org/wiki/Negamax)
### Chess programming wiki:
- [Move ordering](https://www.chessprogramming.org/Move_Ordering)

http://wbec-ridderkerk.nl/html/UCIProtocol.html