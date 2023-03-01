# ![Pac-Man logo.png](https://upload.wikimedia.org/wikipedia/en/thumb/f/fc/Pac-Man_logo.png/250px-Pac-Man_logo.png)

## introduction

Pac-Man is a Japanese video game franchise published in 1980s ,Pac-Man is one of the longest-running, best-selling, and highest-grossing video game franchises in history and I intended to build a simple and functioning version of Pac-Man.

## Project Plans

### Project Declaration :

As formerly mentioned I intend to build a simpler version of Pac-Man in c++ and implementing the graphics with the Unicode and Ascii characters

### Project Goals :

this projects main goal was to implement all the features of the original game and add a little bit of creativity while still maintaining the speed and functionality of the program. We also had to meet all the requirements of the judges to get the highest Score between other projects. 

### additional Features :

- Randomly Generated Map
- Custom Colors
- Leader Board
- Easter Egg

## Maze generation

the map is randomly generated base on the dimensions that the user enters using DFS algorithm. it has been implemented using a class named : `maze` 

for the map to be random I used c++ `random` library:

```c++
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<mt19937::result_type> random(1,4);
    Orientation random_direction = Orientation(random(eng));
```

## Ghosts behavior 

In order for the ghost to be able to track the player I used a simple BFS algorithm that finds the path to Pac-Man or a block near it , this is done for sake of randomness in ghost behavior otherwise they would all chose the same path and stick together , only Blinky the red ghost tracks the exact position of the player and is the most aggressive.

the algorithm uses a vector to queue / (push_back) and dequeue / (pop_back) the cells of the map to search for the Pac-Man location. since every time Pac-Man moves the coordination of Pac-Man only changes by one cell updating the BFS algorithm doesn't really matter because the  ghost will be in the general direction of the Pac-Man.

## Graphics

All the visuals are either ascii art that I build myself or copied from google and changed a bit to suit the rest of the game.

and the Unicode are from the font Noto Sans Canadian Aboriginal.

## Saving Data

In order to save your progress mid game and keep track of the scores in leader board and compete with people who played before you I had to write everything in a **.txt** file and read it when needed to construct the leader board or map.

## Future plans

This game has a lots of potential to grow and become better. For instance adding a GUI or making the chase and escape algorithms better , adding a difficulty and time based leader board for more precision would be great.

## resources

[BFS algorithm](https://www.youtube.com/watch?v=KiCBXu4P-2Y)

[Maze generation](https://www.youtube.com/watch?v=Y37-gB83HKE)

[Understanding Ghosts behavior](https://gameinternals.com/understanding-pac-man-ghost-behavior)

[Some inspirations](https://www.youtube.com/watch?v=vC0d1rDmPBs)

[Noto Sans Canadian Aboriginal Font](https://fonts.google.com/noto/specimen/Noto+Sans+Canadian+Aboriginal)

## Credits

Designer and Implementer: Farid Karimi

Professor: PhD. Kheradpisheh

Teacher Assistant: Mobin Nesari

----
#### Winter  1401-1402



