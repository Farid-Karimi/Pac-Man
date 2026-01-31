<img src="https://static.wikia.nocookie.net/logopedia/images/c/ca/Pac-Man_1980_%28Alt%29.svg/revision/latest/scale-to-width-down/1200?cb=20230727130740" style="width: 365px;" />

## Introduction

Pac-Man is a Japanese video game franchise published in the 1980s. It is one of the longest-running, best-selling, and highest-grossing video game franchises in history. For my final project in the *Basic Programming* course, I aimed to build a simple and functional version of Pac-Man.

![pacman](https://github.com/Farid-Karimi/Pac-Man/assets/118434072/47cdcf36-4e38-4127-adfb-030046aaa424)

## Project Plans

### Project Declaration:

As mentioned earlier, my goal was to create a simplified version of Pac-Man using C++ and implementing graphics with Unicode and ASCII characters.

### Project Goals:

The main objective of this project was to implement all the features of the original game while adding a touch of creativity, without compromising the speed and functionality of the program. Additionally, it was crucial to meet all the requirements set by the judges in order to achieve the highest score among the other projects.

### Additional Features:

- Randomly Generated Map
- Custom Colors
- Leader Board
- Easter Egg

## Implementations
### Maze Generation

The map is randomly generated based on the dimensions entered by the user, utilizing the Depth-First Search **(DFS)** algorithm. This functionality has been implemented using a class named `maze`. To achieve randomness in map generation, I utilized the C++ `random` library:

```c++
random_device rd;
mt19937 eng(rd());
uniform_int_distribution<mt19937::result_type> random(1,4);
Orientation random_direction = Orientation(random(eng));
```

### Ghosts Behavior

To enable the ghosts to track the player, I utilized a simple Breadth-First Search **(BFS)** algorithm that finds the path to Pac-Man or a nearby block. This was done to introduce randomness in the ghost behavior, as otherwise, they would all choose the same path and stick together. Notably, the red ghost, known as Blinky, tracks the exact position of the player and exhibits the most aggressive behavior.

The algorithm uses a vector to queue `(push_back)` and dequeue `(pop_back)` the cells of the map to search for Pac-Man's location. Since Pac-Man's coordinates change by only one cell each time it moves, updating the BFS algorithm doesn't significantly affect the ghost's position. Consequently, the ghost will be in the general direction of Pac-Man.

### Graphics

All the visuals in the game consist of ASCII art, which I either created myself or modified from existing sources to suit the game's aesthetic. Additionally, Unicode characters from the *Noto Sans Canadian Aboriginal* font have been used.

### Saving Data

To save game progress, track scores, and compete with other players, I implemented a mechanism to write and read data from a **.txt** file. The data is stored in a text file, which is utilized to construct the leaderboard and map as needed.

### Compilation 

To compile the program, follow these steps:

1. Ensure you have a C++ compiler installed on your system. Popular options include GCC (GNU Compiler Collection) for Linux and MinGW (Minimalist GNU for Windows) for Windows.

1. Open a terminal or command prompt and navigate to the directory containing the source code files.

1. Run the following command to compile the program:

   ```shell
   g++ main.cpp -o game
   ```

   Replace `g++` with the appropriate compiler command if you are using a different compiler.

1. Once the compilation process completes without any errors, an executable file named `game` (or the name you specified after the `-o` option) will be created in the current directory.

   - On Windows, the file will have the `.exe` extension (`game.exe`).
   - On Linux, the file will have no extension (`game`).

1. You can now run the game by executing the generated executable from the terminal or by double-clicking it in the file explorer.

   ```shell
   ./game      # Linux
   game.exe    # Windows
   ```

It's worth noting that the compilation process may vary depending on your operating system and the specific compiler you are using. Make sure to adjust the compilation commands accordingly.

## Future Plans

This game possesses substantial potential for further growth and improvement. Some potential future enhancements include adding a graphical user interface (GUI), refining the chase and escape algorithms, and implementing a difficulty-based and time-based leaderboard for enhanced precision.

## Resources

- [BFS Algorithm](https://www.youtube.com/watch?v=KiCBXu4P-2Y)
- [Maze Generation](https://www.youtube.com/watch?v=Y37-gB83HKE)
- [Understanding Ghosts Behavior](https://gameinternals.com/understanding-pac-man-ghost-behavior)
- [Some Inspirations](https://www.youtube.com/watch?v=vC0d1rDmPBs)
- [Noto Sans Canadian Aboriginal Font](https://fonts.google.com/noto/specimen/Noto+Sans+Canadian+Aboriginal)

## Credits

Course Instructor: [Dr. Kheradpisheh](https://www.linkedin.com/in/saeed-reza-kheradpisheh-7a0b18155/)  
Teammate: [Shayan Shahrabi](https://www.linkedin.com/in/shayanshahrabi/)
