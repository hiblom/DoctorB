# DoctorB

DoctorB is a UCI compatible chess engine, written in C++.

DoctorB is named after a character in *Schachnovelle* by Stefan Zweig. Dr B plays chess against himself by seperating his mind in two personas: Black and White.

## Current features

* Classical bitboard board representation and move generation
* Legal move generator
* Note: DoctorB can't play chess yet!

## Next up
* Simple board evaluation
* Minimax search strategy with iterative deepening

## Down the road
* Advanced board evaluation
* Alpha beta search strategy
* Support opening books
* Hashing
...

## Additional commands
Next to the traditional UCI commands, DoctorB also supports the following commands
* `go perft [n]`, to show the number of possible positions after [n] plies
* `d`, to display the current position


