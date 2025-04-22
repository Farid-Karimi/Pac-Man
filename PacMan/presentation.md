# Classic PAC-MAN in C++ (Console Version)




---

<img src="https://static.wikia.nocookie.net/logopedia/images/c/ca/Pac-Man_1980_%28Alt%29.svg/revision/latest/scale-to-width-down/1200?cb=20230727130740" style="width: 365px;" />

# 🟡 Pac-Man Clone — Open Source Project Analysis

##  Project Overview

TThis is a terminal-based clone of the classic **Pac-Man** game, written in **C++**, using nothing but standard libraries, ASCII characters, and a bit of creativity. It recreates core mechanics like maze generation, ghost AI, collectibles, and player movement — all within the terminal. 

 this codebase was chosen for its clarity, modular design, and relevance to game logic fundamentals, making it a solid candidate for open-source investigation.

![Gameplay](https://github.com/Farid-Karimi/Pac-Man/assets/118434072/47cdcf36-4e38-4127-adfb-030046aaa424)

---





### **Core Components**
1. **Game Loop**: Handles input, movement, collisions, and rendering.
2. **Maze Generation**: Uses Depth-First Search (DFS) to create random mazes.
3. **Ghost AI**: Implements Breadth-First Search (BFS) for pathfinding.
4. **Data Persistence**: Saves scores, maps, and game states to `.txt` files.
5. **UI**: Terminal-based with Unicode characters for visuals.

---


#### **1. Approaches Taken**


###  Maze Generation (DFS)

When the game starts, it builds a new map using a randomized **Depth-First Search** algorithm. This ensures the maze is always solvable and changes every time you play.

The randomness is powered by:

```cpp
random_device rd;
mt19937 eng(rd());
uniform_int_distribution<mt19937::result_type> random(1,4);
```

So, you’ll never play the same maze twice.

  

- **Ghost Behavior**:

- Ghosts chase you using a simple **Breadth-First Search** algorithm to find the shortest path to your position—or close to it. To avoid all  
-  ghosts clumping together and feeling too robotic, some randomness is added so they don’t always take the same route.

- Red ghost (Blinky) is the aggressive one—he always goes straight for you.

  - **Blinky (Red)**: Aggressively chases Pac-Man using BFS.
  - **Pinky (Pink) & Inky (Cyan)**: Semi-random paths with offset targeting.
  - **Frightened Mode**: Ghosts flee when Pac-Man eats a cherry.



#### **2. Data Structures**
| Structure         | Purpose                          |
|-------------------|----------------------------------|
| `vector<char>`    | Stores maze layout during generation. |
| `stack<Coords>`   | DFS backtracking stack.          |
| `vector<target>`  | BFS queue for ghost pathfinding. |
| 2D `char` arrays  | Map storage (`map[50][50]`).     |

### 3. Tradeoffs made

| Decision                        | Tradeoff / Benefit                                     |
|---------------------------------|--------------------------------------------------------|
| ASCII graphics only             | No dependencies,no external library needed; limited visual appeal                 |
| BFS over A* for AI              | Simpler, but less optimal ghost movement               |
| DFS for maze                    | Ensures solvable map but may generate less complex mazes |
| File I/O for leaderboard        | Simple and portable; not scalable for large data       |

---
 
  - BFS is computationally heavy but ensures optimal ghost paths.
  - DFS for maze generation is slower but guarantees connectivity.

---

##  Visuals 

The entire UI is rendered using:
- **ASCII characters** for walls, players, and ghosts.

- Uses standard Unicode block elements.

- Windows Console API for color control.
---

##  Saving Game State

Game data such as:
- Top scores
- Player names
- Map seeds

...are stored in a plain `.txt` file (`leaderboard.txt`) for easy retrieval and leaderboard display.

---

##  Compilation Instructions

```bash
g++ main.cpp -o game
./game     # On Linux
game.exe   # On Windows
```

> Requires a basic C++ compiler (GCC, MinGW, etc.)

---

##  Future Possibilities

- Add **GUI** using SFML or SDL.
- Smarter **AI with A\*** for ghost pathfinding.
- **Multiplayer mode** over LAN.
- Dynamic **difficulty scaling**.
- Online **leaderboard syncing**.

---

##  External Resources

- [Pac-Man Ghost AI Deep Dive](https://gameinternals.com/understanding-pac-man-ghost-behavior)
- [DFS Maze Generation Video](https://www.youtube.com/watch?v=Y37-gB83HKE)
- [BFS Algorithm Refresher](https://www.youtube.com/watch?v=KiCBXu4P-2Y)
- [Inspiration: Terminal Pac-Man](https://www.youtube.com/watch?v=vC0d1rDmPBs)

---

##  Credits

- **Original Developer**: [Farid Karimi](https://github.com/Farid-Karimi)

Here's a refined version of your presentation with clear Q&A sections based on your code:

### **Pac-Man Technical  Q&A**

#### **1. Maze Generation**
**Q: Why did you choose DFS over other maze algorithms?**  
*A:* DFS was chosen because:
- Guarantees a solvable maze with no isolated sections
- Naturally creates longer corridors (better for Pac-Man gameplay)
- Simple to implement with just a stack data structure

```cpp
stack<Coords> visitedCellsStack; // Core of DFS implementation
```

**Q: How does the randomization work?**  
*A:* We use Mersenne Twister (mt19937) for high-quality randomness:
```cpp
random_device rd;
mt19937 eng(rd());
uniform_int_distribution<mt19937::result_type> random(1,4);
```


**Tradeoff:**  
- BFS is O(n²) while A* could be O(n log n)
- But performance impact is minimal on small mazes

**Q: How do ghosts avoid clustering together?**  
*A:* Each ghost has unique targeting:
```cpp
PathFinding1(G1X, G1Y, x, y);      // Blinky - direct chase
PathFinding2(G2X, G2Y, x+2, y+2);  // Pinky - offset target
PathFinding3(G3X, G3Y, x-2, y-2);  // Inky - mirrored position
```


**Q: How does the maze generation actually work in the code?**
A: The game uses a Depth-First Search algorithm with backtracking. It starts at a random cell, carves out paths in random directions while remembering where it's been using a stack. When it hits a dead end, it backtracks until it can find new paths. This guarantees all areas are connected.

**Q: Why did you choose BFS for ghost pathfinding?**
A: I used Breadth-First Search because:
1. It always finds the shortest path, which makes Blinky properly aggressive
2. It's simpler to implement than A* for this small-scale game
3. The performance impact is negligible for terminal-sized mazes
The different ghost personalities come from how they choose their targets, not the pathfinding itself.

**Q: What are the main technical limitations?**
A: The biggest issues are:
1. Pathfinding gets slow if maze sizes increase significantly
2. The renderer flickers because it redraws the entire screen each frame
3. Windows API calls mean it doesn't work perfectly on other operating systems

**Q: If you had more time, what would you improve?**
A: Top priorities would be:
1. Implement A* pathfinding at least for Blinky
2. Add proper error handling for save files
3. Switch to ANSI escape codes for cross-platform support
4. Add double-buffering to eliminate screen flicker
5. Implement proper game state management instead of global variables

