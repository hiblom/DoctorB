# DoctorB

DoctorB is a UCI compatible chess engine, written in C++.

DoctorB is named after a character in *Schachnovelle* by Stefan Zweig. Dr B plays chess against himself by seperating his mind in two personas: Black and White.

## Current features
* Classical bitboard board representation and move generation
* Legal move generator
* Simple board evaluation purely based on piece value
* Minimax search strategy with iterative deepening
* Note: DoctorB can now play a game of chess (although not so good)

## Next up
* Alpha beta search strategy
* Hashing / transposition table
* Advanced board evaluation
* Support opening books
* Threaded search to implement quit and stop commands properly

## Down the road
* Quiesence search
* End game tables
* Multi-threaded search
* ...

## Additional commands
Next to the traditional UCI commands, DoctorB also supports the following commands
* `go perft [n]` : show the number of possible positions after [n] plies
* `d` : display the current position


