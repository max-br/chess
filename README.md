### About

This is a UCI-chess engine. It utilizes [bitboards](https://en.wikipedia.org/wiki/Bitboard) for fast move generation. 
The magic bitboard implementation is from BuzzOS by Pradu Kannan. 
The Move generator is tested using test positions from [chessprogramming.wikispaces.com] (https://chessprogramming.wikispaces.com/Perft+Results).

### Todo
At the moment, the game is fully playable, but could be improved by implementing:
  * Quiescence search
  * Iterative deepening
  * Zobrist hashing
  * Multithreading
  * consider game phase in Evaluation
  * Documentation
  * UCI implementation is the bare minimum
