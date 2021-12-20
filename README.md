# C Chess

## Installation
```bash
git clone git@github.com:Bungogood/C-Chess.git
cd C-Chess
mkdir .obj
make && ./chess
```

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