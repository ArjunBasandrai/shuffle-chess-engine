<p align="center"><a href="https://github.com/ArjunBasandrai" target="_blank"><img src="assets/logo.png" width=160 title="ShuffleAI" alt="ShuffleAI"></a>
</p>

<h1 align="center"> Shuffle Chess Engine </h1>
<br/>

> Shuffle is a UCI based chess engine written entirely in C.
> It is a magic bitboards based engine with an ELO of around 2200

---

## Features
- Magic Bitboards to pre-calculate sliding piece attacks quickly and efficiently
- Zobrist Hashing and use of Transposition Table to store evaluated positions and detect 3-fold repetition
- Negamax search with Alpha-Beta Pruning to quickly search for all possible positions
- Advanced move ordering techniques like Principle Variation Search, Null Move Pruning, Late Move Reduction to reduce search times
- Tapered Evaluation for static evaluation of given board state
- Dynamically allocated transposition table to allow allocation of memory from the GUI or UCI command
- Custom Polyglot Opening Book for enhanced opening performance
- Ability to play at various time controls
- UCI compatibility

## Ratings

#### [CCRL Blitz](https://www.computerchess.org.uk/ccrl/404/)
| Version | ELO |
| :-----: | :-: |
| [version 1.2.1](https://github.com/ArjunBasandrai/shuffle-chess-engine/releases/tag/v1.2.1) | [2146](https://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?print=Details&each_game=1&eng=Shuffle%201.2.1%2064-bit#Shuffle_1_2_1_64-bit) |

## Instructions



### Directions to run

- Go to the [Releases](https://github.com/ArjunBasandrai/shuffle-chess-engine/releases) page and install the binaries for your operating system
- Open the binary in any UCI based GUI like [Arena GUI](http://www.playwitharena.de/) or [Cute Chess GUI](https://cutechess.com/)
- Set any time control of choice
- Have fun!

#### Using Polyglot book (optional)

- Install the `shuffle.bin` polyglot book from the [Releases](https://github.com/ArjunBasandrai/shuffle-chess-engine/releases) page or use any polyglot book of your choice and rename it to `shuffle.bin`
- Keep the book in the same directory as the binary to use it for openings

### Directions to build

Type the following command in the terminal to create a fresh build for your system - 

```
git clone https://github.com/ArjunBasandrai/shuffle-chess-engine.git
```
#### Windows
```
make windows v=<version-suffix>
```
#### MacOS (only ARM CPUs)
```
make apple v=<version-suffix>
```
#### Linux
```
make linux v=<version-suffix>
```
Once compilation is completed, the executable can be found in the root directory as `shuffle_<version_suffix>`

<br>

## Contributors

<p align="center">Arjun Basandrai</p>
<p align="center">
  <img src = "https://avatars.githubusercontent.com/u/64721050?v=4" width="150" alt="Arjun Basandrai">
</p>
  <p align="center">
    <a href = "https://github.com/ArjunBasandrai">
      <img src = "http://www.iconninja.com/files/241/825/211/round-collaboration-social-github-code-circle-network-icon.svg" width="36" height = "36" alt="GitHub"/>
    </a>
  </p>

## References

[Bitboard Chess Engine in C Series](https://www.youtube.com/playlist?list=PLmN0neTso3Jxh8ZIylk74JpwfiWNI76Cs) by Chess Programming <br>
[VICE Chess Engine in C](https://www.youtube.com/watch?v=bGAfaepBco4&list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg) by Bluefever Software <br>
[Coding Adventures: Chess Bot](https://www.youtube.com/watch?v=U4ogK0MIzqk&t=1007s&pp=ygUWY2hlc3MgZW5naW5lIHNlYmFzdGlhbg%3D%3D) by Sebastian Lague

## License
[![License](http://img.shields.io/:license-gpl3-blue.svg?style=flat-square)](https://www.gnu.org/licenses/gpl-3.0.en.html#license-text)

## Note
- If you're in India and using the Jio Network, you may experience difficulties viewing the images in this file. Please consider switching to a different network provider or utilizing a VPN to access the images

