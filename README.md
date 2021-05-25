# DoctorB

DoctorB is a UCI compatible chess engine, written in C++.

DoctorB is named after a character in *Schachnovelle* by Stefan Zweig. Dr B plays chess against himself by seperating his mind in two personas: Black and White.

## Current features
* Classical bitboard board representation and move generation
* Alpha beta search strategy with iterative deepening
* Piece square board evaluation
* Hashing / transposition table
* Move ordering
* Static Exchange Evaluation (SEE)
* Repetition detection

## Next up
* Quiesence search
* Support opening books
* Threaded search to implement quit and stop commands properly

## Down the road
* Advanced board evaluation
* End game tables
* Aspiration Windows
* Multi-threaded search
* ...

## Additional commands
Next to the traditional UCI commands, DoctorB also supports the following commands
* `go perft [n]` : show the number of possible positions after [n] plies
* `d` : display the current position

## Acknowledgements
I'm trying to keep things originial. However, this engine would not have been possible without the following sources:
* https://www.chessprogramming.org/Main_Page
* https://www.arasanchess.org/programr.shtml
* https://github.com/AndyGrant/Ethereal
* https://github.com/jdart1/arasan-chess
* http://wbec-ridderkerk.nl/html/UCIProtocol.html
* http://hgm.nubati.net/book_format.html


