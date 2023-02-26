#include <iostream>  
#include <stdio.h>  
#include <windows.h>  
#include <string>  
#include <vector>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <stack>
#include <random>
#include <time.h>

#define pacmanIcon "\u15E6"
#define ghostIcon "\u15E3"


using namespace std;  

//-----------------------------------------------------------------------------------------------------------------------------
//function prototypes

void printMap(int w , int h);
void gotoxy(int x, int y);
void hidecursor();
void adjacentCell1(int x, int y, int wc, int back);
void PathFinding(int sx, int sy, int x, int y);
void deathScreen();
void difficultyMenu(int &difficulty);
void input(int w , int h , int &x , int &y , int &ghost1Color , int &ghost2Color , int &ghost3Color);
void mainMenu();
void instructions();
void createfile();
void showScoreBoard();
void updateScoreBoard(string, int);
void loadFile(int &w, int &h);
void credits();
void vintagePrint(string);
void deathBlinking();
void generateMaze(int w , int h);
void victory();
void easterEgg();
void pauseMenu(int w , int h);
void save(int, int);
void createcoordinatesFile();
void createMazeFile();

//-------------------------------------------------------------------------------------------------------------------------------------------
// declaring global variables
int pts = 0;
int difficulty;
enum eDirection {LEFT , RIGHT , UP , DOWN};
eDirection dir;

const string FILENAME = "scoreBoard.txt";
const string CREATURE = "creature.txt";
const string MAZE = "maze.txt";

int maxPoints=0;
bool frightenedMod = false;
bool isNewGame = false;
int frameCount = 0;
int initialFrame = 0;
int deathCount = 0; 

int WIDTH;
int HEIGHT;

int mapColor;
int PlaceHolderMapColor;
int PacmanColor;
int primaryColor;
int secondaryColor;

int G1X;                                 
int G1Y;
   
int G2X;
int G2Y;

int G3X;
int G3Y;

int x, y;

vector<char> keepTrack;
//----------------------------------------------------------------------------------------------------------------------------

char tmp_map[50][50];                            // used for bfs algorithm
char map[50][50];                               // a global array to store map

//--------------------------------------------------------maze stuff----------------------------------------------------------
enum Orientation{ Left, Right, Up, Down };

Orientation operator!(const Orientation& dir){
    if(dir==Left || dir==Up)
        return Orientation(dir+1);
    else
        return Orientation(dir-1);

}

struct Coords{
    int x, y;
    static const Coords directionOffset[]; //static const va const dar in tafavot darand ke static const faghat
                                           // yekbar initialized mishe va baraye tamam dafaat baadi ke tabe seda mishe yeki hast
};
const Coords Coords::directionOffset[] = {
    Coords{-1, 0 }, // left
    Coords{ 1, 0 }, // right
    Coords{ 0,-1 }, // up
    Coords{ 0, 1 }  // down
};
Coords operator+(const Coords& left, const Coords& right){
    return Coords{left.x + right.x, left.y + right.y };
}

struct MazeCell{
    char connections = 0;

    bool isConnected(Orientation dir);
    void connect(Orientation dir);
    bool visited();
    static const unsigned char connectionMask[];
};

bool MazeCell::isConnected(Orientation dir){
    return connections & connectionMask[dir];
}
void MazeCell::connect(Orientation dir){
    connections |= connectionMask[dir];
}
bool MazeCell::visited(){
    return connections;
}
const unsigned char MazeCell::connectionMask[] = {
    (1), // left
    (2), // right
    (4), // up
    (16)  // down
};

struct MazePosition;                                                 //class forward declaration
class Maze{
    friend MazePosition;                                             //can access private and protected part if this class
    unsigned int width,height;
    MazeCell* cells;                                                 //pointer to structure

public:
    Maze(unsigned int width, unsigned int height);
    MazePosition pinPoint(unsigned int x, unsigned int y);

private:
    bool succsesfullyConnectedToNeighbour(Coords& cellCoords);
    void connect(Coords& from, Coords& to, Orientation dir);
    bool areCoordsValid(const Coords& coords) const;
    MazeCell& cellAt(const Coords& coords) const;
};
void Maze::connect(Coords& from, Coords& to, Orientation dir){
    cellAt(from).connect(dir);
    cellAt(to).connect(!dir);
}
bool Maze::areCoordsValid(const Coords& coords) const {
    return coords.x >= 0  && coords.y >= 0 && coords.x <  (int)width && coords.y <  (int)height;        // checks if we are in the boundries of the maze
}
MazeCell& Maze::cellAt(const Coords& coords) const {
    if(!areCoordsValid(coords)){cout << "hit bad";}
    return cells[coords.x + coords.y*width];
}
Maze::Maze(unsigned int width, unsigned int height) : width(width), height(height) {
    cells = new MazeCell[width*height];
    Coords currentCellCoords{0,0};
    stack<Coords> visitedCellsStack;
    visitedCellsStack.push(currentCellCoords);

    while(true){
        if(succsesfullyConnectedToNeighbour(currentCellCoords)){
            visitedCellsStack.push(currentCellCoords);
        }else{
            currentCellCoords = visitedCellsStack.top();
            visitedCellsStack.pop();
            if(visitedCellsStack.empty()) break;
        }
    }
}
bool Maze::succsesfullyConnectedToNeighbour(Coords& cellCoords){
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<mt19937::result_type> random(1,4);
    Orientation random_direction = Orientation(random(eng));

    for(int i = 0; i < 4; i++){
        Orientation next_rand_direction = Orientation( (random_direction + i)%4 );
        Coords neighbour_coords = Coords::directionOffset[next_rand_direction] + cellCoords;

        if( areCoordsValid(neighbour_coords) && !cellAt(neighbour_coords).visited() ){
            connect(cellCoords, neighbour_coords, next_rand_direction);
            cellCoords = neighbour_coords;
            return true;
        }
    }
    return false;
}

struct MazePosition{
    const Maze& maze;
    Coords currentCoords;
    MazePosition(const Maze& maze, Coords coords) : maze(maze), currentCoords(coords){}
    
    bool canGo(Orientation dir){
        return maze.cellAt(currentCoords).isConnected(dir);
    }

    void go(Orientation dir){
        if(canGo(dir))
            currentCoords = currentCoords + Coords::directionOffset[dir];
    }
};

MazePosition Maze::pinPoint( unsigned int x, unsigned int y){
    return MazePosition(*this,Coords{(int)x, (int)y}); 
}

    //------------------------------------------------end of maze stuff------------------------------------------------
void clearScreen(){

    HANDLE hOut;
    COORD Position;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    Position.X = 0;
    Position.Y = 0;

    SetConsoleCursorPosition(hOut, Position);
}
//***********************************************************************************************************************************************
void printMap(int w , int h){                                   // translating ascii characters from the array map to unicode
    clearScreen();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, mapColor); //9

        for(int i=0 ; i<2*h+1 ; i++){
            for(int j=0 ; j<2*w+1 ; j++){
                if(map[i][j]=='|'){
                    cout<<"█";
                }
                else if(map[i][j]=='_'){
                    cout<<"■";
                }
                else if(map[i][j]=='.'){
                    cout<<"•";
                }
                else if(map[i][j]=='H'){
                    cout<<"▄";
                }
                else if(map[i][j]=='C'){
                    SetConsoleTextAttribute(hConsole, 4);
                    cout<<"꘧";
                    SetConsoleTextAttribute(hConsole, mapColor);
                }
                else{
                    cout<<' ';
                }
            }
            cout<<endl;
        }

    SetConsoleTextAttribute(hConsole, 15);
}  
//***********************************************************************************************************************************************
void gotoxy(int x, int y){  
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);  
    COORD position = {x, y};  
  
    SetConsoleCursorPosition(hStdout, position);  
}  
//***********************************************************************************************************************************************
void hidecursor(){
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}
//***********************************************************************************************************************************************
struct walk{ 

    int walk_count;  
    int x;  
    int y;  
    int back;  
};  
  
struct target{
    int x;  
    int y;  
};  
//***********************************************************************************************************************************************
vector<target> walk1_queue;  
vector<target> walk2_queue;
vector<target> walk3_queue;

vector<walk> BFS1;
vector<walk> BFS2;
vector<walk> BFS3;  
//***********************************************************************************************************************************************
void adjacentCell1(int x, int y, int wc, int back){

    if (tmp_map[y][x] == ' ' || tmp_map[y][x] == '.')  
    {  
        tmp_map[y][x] = '#';  
        walk tmp;  
        tmp.x = x;  
        tmp.y = y;  
        tmp.walk_count = wc;  
        tmp.back = back;  
        BFS1.push_back(tmp);  
    }  
}  
//***********************************************************************************************************************************************
void adjacentCell2(int x, int y, int wc, int back){

    if (tmp_map[y][x] == ' ' || tmp_map[y][x] == '.')  
    {  
        tmp_map[y][x] = '#';  
        walk tmp;  
        tmp.x = x;  
        tmp.y = y;  
        tmp.walk_count = wc;  
        tmp.back = back;  
        BFS2.push_back(tmp);  
    }  
}
//***********************************************************************************************************************************************
void adjacentCell3(int x, int y, int wc, int back){

    if (tmp_map[y][x] == ' ' || tmp_map[y][x] == '.')  
    {  
        tmp_map[y][x] = '#';  
        walk tmp;  
        tmp.x = x;  
        tmp.y = y;  
        tmp.walk_count = wc;  
        tmp.back = back;  
        BFS3.push_back(tmp);  
    }  
}    
//***********************************************************************************************************************************************
void PathFinding1(int sx, int sy, int x, int y){

    memcpy(tmp_map, map, sizeof(map));  
    BFS1.clear();  
    walk tmp;  
    tmp.x = sx;  
    tmp.y = sy;  
    tmp.walk_count = 0;  
    tmp.back = -1;  
    BFS1.push_back(tmp);  
  
    int i = 0;  
    while (i < BFS1.size()){  
        if (BFS1[i].x == x && BFS1[i].y == y){  
            walk1_queue.clear();  
            target tmp2;  
            while (BFS1[i].walk_count != 0){  
                tmp2.x = BFS1[i].x;  
                tmp2.y = BFS1[i].y;  
                walk1_queue.push_back(tmp2);  
  
                i = BFS1[i].back;  
            }  
  
            break;  
        }  
  
        adjacentCell1(BFS1[i].x + 1, BFS1[i].y, BFS1[i].walk_count + 1, i);  
        adjacentCell1(BFS1[i].x - 1, BFS1[i].y, BFS1[i].walk_count + 1, i);  
        adjacentCell1(BFS1[i].x, BFS1[i].y + 1, BFS1[i].walk_count + 1, i);  
        adjacentCell1(BFS1[i].x, BFS1[i].y - 1, BFS1[i].walk_count + 1, i);
        i++;  
    }  
  
    BFS1.clear();  
}
//***********************************************************************************************************************************************
void PathFinding2(int sx, int sy, int x, int y){

    memcpy(tmp_map, map, sizeof(map));  
    BFS2.clear();  
    walk tmp;  
    tmp.x = sx;  
    tmp.y = sy;  
    tmp.walk_count = 0;  
    tmp.back = -1;  
    BFS2.push_back(tmp);  
  
    int i = 0;  
    while (i < BFS2.size()){  
        if (BFS2[i].x == x && BFS2[i].y == y){  
            walk2_queue.clear();  
            target tmp2;  
            while (BFS2[i].walk_count != 0){  
                tmp2.x = BFS2[i].x;  
                tmp2.y = BFS2[i].y;  
                walk2_queue.push_back(tmp2);  
  
                i = BFS2[i].back;  
            }  
  
            break;  
        }  
  
        adjacentCell2(BFS2[i].x + 1, BFS2[i].y, BFS2[i].walk_count + 1, i);  
        adjacentCell2(BFS2[i].x - 1, BFS2[i].y, BFS2[i].walk_count + 1, i);  
        adjacentCell2(BFS2[i].x, BFS2[i].y + 1, BFS2[i].walk_count + 1, i);  
        adjacentCell2(BFS2[i].x, BFS2[i].y - 1, BFS2[i].walk_count + 1, i);
        i++;  
    }  
  
    BFS2.clear();  
}
//***********************************************************************************************************************************************
void PathFinding3(int sx, int sy, int x, int y){

    memcpy(tmp_map, map, sizeof(map));  
    BFS3.clear();  
    walk tmp;  
    tmp.x = sx;  
    tmp.y = sy;  
    tmp.walk_count = 0;  
    tmp.back = -1;  
    BFS3.push_back(tmp);  
  
    int i = 0;  
    while (i < BFS3.size()){  
        if (BFS3[i].x == x && BFS3[i].y == y){  
            walk3_queue.clear();  
            target tmp3;  
            while (BFS3[i].walk_count != 0){  
                tmp3.x = BFS3[i].x;  
                tmp3.y = BFS3[i].y;  
                walk3_queue.push_back(tmp3);  
  
                i = BFS3[i].back;  
            }  
  
            break;  
        }  
  
        adjacentCell3(BFS3[i].x + 1, BFS3[i].y, BFS3[i].walk_count + 1, i);  
        adjacentCell3(BFS3[i].x - 1, BFS3[i].y, BFS3[i].walk_count + 1, i);  
        adjacentCell3(BFS3[i].x, BFS3[i].y + 1, BFS3[i].walk_count + 1, i);  
        adjacentCell3(BFS3[i].x, BFS3[i].y - 1, BFS3[i].walk_count + 1, i);
        i++;  
    }  
  
    BFS3.clear();  
}
//***********************************************************************************************************************************************
void deathScreen(){

    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 4);
    cout<<"════════════════════════════════════════════════════════════════"<<endl; Sleep(50);
    SetConsoleTextAttribute(hConsole, 12);
    cout<<"     __     __ ____   _    _   _____  _____  ______  _____      "<<endl; Sleep(50);
    cout<<"     \\ \\   / // __ \\ | |  | | |  __ \\|_   _||  ____||  __ \\     "<<endl; Sleep(50);
    cout<<"      \\ \\_/ /| |  | || |  | | | |  | | | |  | |__   | |  | |    "<<endl; Sleep(50);
    cout<<"       \\   / | |  | || |  | | | |  | | | |  |  __|  | |  | |    "<<endl; Sleep(50);
    cout<<"        | |  | |__| || |__| | | |__| |_| |_ | |____ | |__| |    "<<endl; Sleep(50);
    cout<<"        |_|   \\____/  \\____/  |_____/|_____||______||_____/     "<<endl; Sleep(50);
    cout<<"                                                                "<<endl; Sleep(50);
    SetConsoleTextAttribute(hConsole, 4);
    cout<<"════════════════════════════════════════════════════════════════"<<endl;

    cout<<"                   ══════════════════════════                   "<<endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout<<"                           Score: "<< pts <<"                   "<<endl;
    SetConsoleTextAttribute(hConsole, 4);
    cout<<"                   ══════════════════════════                   "<<endl;    

    string name;

    cout<<"                   ══════════════════════════                   "<<endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout<<"                           Name:"<<endl;
    SetConsoleTextAttribute(hConsole, 4);
    cout<<"                   ══════════════════════════                   "<<endl;
    gotoxy(32 , 13);
    cin >> name;
    cout<<endl;
    updateScoreBoard(name, pts);

    system("pause");   
}
//***********************************************************************************************************************************************
void difficultyMenu(int &difficulty){
    int BorderColor = primaryColor;
    int InsideColor = secondaryColor;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒════════════════════════════════╕                 "<<endl;

    cout<<"                    │ ";
    cout<<"       ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"CHOOSE DIFFICULTY       ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘════════════════════════════════╛                 "<<endl;

    SetConsoleTextAttribute(hConsole, 10);
    cout<<"                         ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                         │ ";
    SetConsoleTextAttribute(hConsole, 2);
    cout<<"1";
    SetConsoleTextAttribute(hConsole, 10);
    cout<<" │       ";
    SetConsoleTextAttribute(hConsole, 2);    
    cout<<"EASY       ";
    SetConsoleTextAttribute(hConsole, 10);    
    cout<<"│                 "<<endl;

    cout<<"                         ╘═══╧══════════════════╛                 "<<endl;    

    SetConsoleTextAttribute(hConsole, 6);
    cout<<"                         ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                         │ ";
    SetConsoleTextAttribute(hConsole, 6);
    cout<<"2";
    SetConsoleTextAttribute(hConsole, 6);
    cout<<" │      ";
    SetConsoleTextAttribute(hConsole, 6);    
    cout<<"NORMAL      ";
    SetConsoleTextAttribute(hConsole, 6);    
    cout<<"│                 "<<endl;

    cout<<"                         ╘═══╧══════════════════╛                 "<<endl;    

    SetConsoleTextAttribute(hConsole, 4);
    cout<<"                         ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                         │ ";
    SetConsoleTextAttribute(hConsole, 12);
    cout<<"3";
    SetConsoleTextAttribute(hConsole, 4);
    cout<<" │       ";
    SetConsoleTextAttribute(hConsole, 12);    
    cout<<"HARD       ";
    SetConsoleTextAttribute(hConsole, 4);    
    cout<<"│                 "<<endl;

    cout<<"                         ╘═══╧══════════════════╛                 "<<endl; 
        
    switch(getch()){
        case '1':
         difficulty=3;   break;
        case '2':
         difficulty=2;   break;
        case '3':
         difficulty=1;   break;  
    }
        system("cls");
}
//***********************************************************************************************************************************************
void input(int w , int h , int &x , int &y , int &ghost1Color , int &ghost2Color , int &ghost3Color){
    if(kbhit()){
        switch(getch()){
            case 'a':
                dir=LEFT;  
                break;
            case 's':
                dir=DOWN;  
                break;
            case 'd':
                dir=RIGHT;
                break;
            case 'w':
                dir=UP;    
                break;
            case 'x':
                pauseMenu(w,h);
                printMap(w,h);
                    break;
        }
    }
    switch (dir){
        case UP:
            if (map[y - 1][x] == '.'){  
                y--;  
                pts++;
                map[y][x] = ' ';  
            }  

            else if(map[y - 1][x] == ' '){ 
                y--;
            }

            else if(map[y - 1][x] == 'C'){
                y--;
                map[y][x] = ' ';
                
                frightenedMod = true;
                initialFrame = frameCount;
                
                ghost1Color = 15;
                ghost2Color = 15;
                ghost3Color = 15;
                mapColor = 15;
            }
            break;

            case DOWN:
            if (map[y + 1][x] == '.'){  
                y++;  
                pts++;
                map[y][x]=' ';
            }  
            else if(map[y + 1][x] == ' '){ 
                y++;
            }
            else if(map[y + 1][x] == 'C'){
                y++;
                map[y][x] = ' ';
                frightenedMod = true;
                initialFrame = frameCount;

                ghost1Color = 15;
                ghost2Color = 15;
                ghost3Color = 15;
                mapColor = 15;
            }
                break;
        case LEFT:
            if (map[y][x - 1] == '.'){  
                x--;  
                pts++;
                map[y][x]=' '; 
            }  
            else if (map[y][x - 1] == ' '){
                x--;     
            }
            else if(map[y][x - 1] == 'C'){
                x--;
                map[y][x] = ' ';
                frightenedMod = true;
                initialFrame = frameCount;

                ghost1Color = 15;
                ghost2Color = 15;
                ghost3Color = 15;
                mapColor = 15;
            }
                break;
        case RIGHT:
            if (map[y][x + 1] == '.'){  
                x++;  
                pts++;
                map[y][x] = ' ';
            }  
            else if(map[y][x + 1] == ' '){  
                x++;
            }
            else if(map[y][x + 1] == 'C'){
                x++;
                map[y][x] = ' ';
                frightenedMod = true;
                initialFrame = frameCount;

                ghost1Color = 15;
                ghost2Color = 15;
                ghost3Color = 15;
                mapColor = 15;
            }   
                break;

            default:
                break;
        }
}
//***********************************************************************************************************************************************
void pauseMenu(int w , int h){
    system("cls");
    int BorderColor = primaryColor;
    int InsideColor = secondaryColor;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"╒═════════════════════════════════════════════════════════════════╕"<<endl;
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<" _______  _______  _______        __   __  _______  __    _";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"    │"<<endl;  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|       ||   _   ||       |      |  |_|  ||   _   ||  |  | |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|    _  ||  |_|  ||       | ____ |       ||  |_|  ||   |_| |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|   |_| ||       ||       ||____||       ||       ||       |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|    ___||       ||      _|      |       ||       ||  _    |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|   |    |   _   ||     |_       | ||_|| ||   _   || | |   |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|___|    |__| |__||_______|      |_|   |_||__| |__||_|  |__|";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;  
    cout<<"│                                                                 │"<<endl;
    cout<<"╘═════════════════════════════════════════════════════════════════╛"<<endl;

    cout<<endl;
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"1";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │     ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"CONTINUE     ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;    
    
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"2";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │   ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"SAVE AND EXIT  ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;    

    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"3";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │       ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"EXIT       ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;    

    
    switch(getch()){
        case '1':
            printMap(w,h);
            break;
        case '2':
            save(w, h);
            vintagePrint("Adiós Amigo!\n");
            system("pause");
            mainMenu();
            break;
        case '3':
            exit(0);
            break;
    }

    SetConsoleTextAttribute(hConsole, 15);
    
    system("cls");
}
//***********************************************************************************************************************************************
void dimension(int &w , int &h){
    int BorderColor = primaryColor;
    int InsideColor = secondaryColor;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒════════════════════════════════╕                 "<<endl;

    cout<<"                    │ ";
    cout<<"       ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"CHOOSE DIMENSION        ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘════════════════════════════════╛                 "<<endl;
    cout<<endl;
    int height , width;
    cout<<"                           ════════════════════  "<<endl;
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"                              Enter Width:"<<endl;
    Beep(850,100);
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                           ════════════════════  "<<endl;
    gotoxy(42,5);
    cin >> width;

    cout << endl;
    cout << endl;
    cout<<"                           ════════════════════  "<<endl;
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"                              Enter Height:"<<endl;
    Beep(850,100);
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                           ════════════════════  "<<endl;
    gotoxy(43,9);
    cin>>height;
    Sleep(100);

    w=width/2;
    h=height/2;
    system("cls");
}
//--------------------------------------------------------------------------------------------------------
void generateMaze(int w , int h){
    
    Maze maze(w,h);

    //creating a maze which is small, so it is saved in a vector called "keepTrack"

    for(int y = 0; y < h; y++){

        for(int x = 0; x < w; x++){
            if(maze.pinPoint(x,y).canGo(Left)){
                keepTrack.push_back('_');
            }
            else{
                keepTrack.push_back('|');
            }

            if(maze.pinPoint(x,y).canGo(Down)){
                keepTrack.push_back('.');
            }
            else{
                keepTrack.push_back('_');
            }

            if( x==(w-1) && !maze.pinPoint(x,y).canGo(Right) ){                 // the most right cell "must" be a wall to generate a "valid" maze according to h and w
                keepTrack.push_back('|');
            }
        }

        // keepTrack.push_back('\n');               // if the comment is removed, the small maze would be printed!
    }
    // stretching the maze 

    for(int i=1 ; i<2*h+1 ; i++){
        for(int j=0 ; j<2*w+1 ; j++){
            if((i+j)%2==0){
                map[i][j]='.';
            }
            else{
                map[i][j]=' ';
            }
        }
    }

    int k=0;

    for(int i=2 ; i<2*h+1 ; i+=2){
        for(int j=2 ; j<2*w+1 ; j+=2){
            map[i][j]=keepTrack[k];
            k++;
        }
    }

    for(int i=1 ; i<2*h+1 ; i++){
        for(int j=0 ; j<2*w+1 ; j++){
            if(map[i][j]=='|' && map[i-2][j]=='|'){
                map[i-1][j]='|';
            }
        }
    }

    for(int i=0 ; i<2*h+1 ; i++){
        for(int j=0 ; j<2*w+1 ; j++){
            if(map[i][j]==' ' && map[i][j-1]=='_' && map[i][j+1]=='_'){
                map[i][j]='_';
            }
        }
    }

    for(int i=0 ; i<2*h+1 ; i++){
        map[i][0]='|';
        map[i][2*w]='|';
    }

    for(int i=0 ; i<2*w+1 ; i++){
        map[0][i]='H';
        map[2*h][i]='H';
    }

    map[2*h][2*w]='|';
    map[2*h][0]='|';
    //-----------------------cherry cordinating-------------------------
    map[h][1]='C';
    map[h][2*w - 1]='C';
    map[1][w]='C';
    map[2*h - 1][w]='C';
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void chooseColor(){
    
    system("cls");
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // SetConsoleTextAttribute(hConsole, 9);   
    cout<<"                  ╒══════════════════════════════════╕                    "<<endl;
    cout<<"                  │           CHOOSE COLOR           │                    "<<endl;
    cout<<"                  ╘══════════════════════════════════╛                    "<<endl;Sleep(200);
    
    cout<<"                          ════════════════════                            "<<endl;
    cout<<"                               PRIMARY:                                   "<<endl;
    cout<<"                          ════════════════════                            "<<endl;Sleep(200);
    cout<<endl;
    cout<<"                          ════════════════════                            "<<endl;
    cout<<"                              SECONDARY:                                  "<<endl;
    cout<<"                          ════════════════════                            "<<endl;Sleep(200);
    cout<<endl;
    cout<<"                          ════════════════════                            "<<endl;
    cout<<"                               PACMAN:                                    "<<endl;
    cout<<"                          ════════════════════                            "<<endl;Sleep(200);
    cout<<endl;
    cout<<"                          ════════════════════                            "<<endl;
    cout<<"                                 MAP:                                     "<<endl;
    cout<<"                          ════════════════════                            "<<endl;Sleep(200);
    
    cout<<endl;
    
    
    SetConsoleTextAttribute(hConsole, 1);   cout<<"     01 = █  ";    
    SetConsoleTextAttribute(hConsole, 2);   cout<<"  02 = █  ";    
    SetConsoleTextAttribute(hConsole, 3);   cout<<"  03 = █  ";    
    SetConsoleTextAttribute(hConsole, 4);   cout<<"  04 = █  ";    
    SetConsoleTextAttribute(hConsole, 5);   cout<<"  05 = █  ";    
    SetConsoleTextAttribute(hConsole, 6);   cout<<"  06 = █  ";
    SetConsoleTextAttribute(hConsole, 7);   cout<<"  07 = █  "<<endl;
    cout<<endl; Sleep(200);    
    SetConsoleTextAttribute(hConsole, 8);   cout<<"     08 = █  ";    
    SetConsoleTextAttribute(hConsole, 9);   cout<<"  09 = █  ";    
    SetConsoleTextAttribute(hConsole, 10);   cout<<"  10 = █  ";   
    SetConsoleTextAttribute(hConsole, 11);   cout<<"  11 = █  ";  
    SetConsoleTextAttribute(hConsole, 12);   cout<<"  12 = █  ";
    SetConsoleTextAttribute(hConsole, 13);   cout<<"  13 = █  ";
    SetConsoleTextAttribute(hConsole, 14);   cout<<"  14 = █  "<<endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout<<endl;
    cout<<"  Disclaimer : it's resommended to do not choose colors like white for Map   "<<endl;
    cout<<" also if you are loading an already saved game the colors will be replaced!   "<<endl;
    gotoxy(39, 4);
    cin>>primaryColor; 
    gotoxy(40, 8);
    cin>>secondaryColor;
    gotoxy(38, 12);
    cin>>PacmanColor;
    gotoxy(37 16);
    cin>>mapColor;
    PlaceHolderMapColor = mapColor;


}

//-------------------------------------------------------------------------------------------------------------------------------------------

int main(){ 

    chooseColor();

    int  ghost1Color = 4;
    int  ghost2Color = 5;
    int  ghost3Color = 3;
    
    createfile();
    createcoordinatesFile();
    createMazeFile();

    hidecursor();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int w;
    int h;

    mainMenu();
    if(!isNewGame){

        dimension(w,h);
        generateMaze(w, h); // the maze is fully generated
        difficultyMenu(difficulty);

    }

    WIDTH = w;
    HEIGHT = h;

    int deathCount = 0;  
    x = 2 * w - 1; // pacman x                 
    y = 2 * h - 1; // pacman y

    int pacmanSpawnX = 2 * w - 1;
    int pacmanSpawnY = 2 * h - 1;

    int ghost1SpawnX = 2 * w - 1;
    int ghost1SpawnY = 1;

    int ghost2SpawnX = 1;
    int ghost2SpawnY = 1;

    int ghost3SpawnX = 1;
    int ghost3SpawnY = 2 * h - 1;



    int old_x;  
    int old_y;  

    //cordination of spawn points for da ghosts
    G1X = ghost1SpawnX;                                 
    G1Y = ghost1SpawnY;
   
    G2X = ghost2SpawnX;
    G2Y = ghost2SpawnY;

    G3X = ghost3SpawnX;
    G3Y = ghost3SpawnY;
    
    SetConsoleTextAttribute(hConsole, PacmanColor);
    gotoxy(x, y);  
    cout << pacmanIcon;
    SetConsoleTextAttribute(hConsole, 15);

  
    PathFinding1(G1X, G1Y, x, y);
    PathFinding2(G2X, G2Y, x+2, y+2);
    PathFinding3(G3X, G3Y, x-2, y-2);   


    for(int i=0 ; i<2*h+1 ; i++){
        for(int j=0 ; j<2*w+1 ; j++){
            if(map[i][j] == '.'){
                maxPoints++;
            }
        }
    }

    while( (deathCount != 3) && (pts != maxPoints) ){
        
        printMap(w,h);

        gotoxy(x, y);
        cout<<" ";

        old_x = x;  
        old_y = y;

        input(w , h , x , y , ghost1Color , ghost2Color , ghost3Color);
        
        if(frameCount - initialFrame == 30){
            
            frightenedMod = false;
            
            ghost1Color = 4;
            ghost2Color = 5;
            ghost3Color = 3;
            mapColor = PlaceHolderMapColor;
        }

        if ((old_x != x || old_y != y) && !frightenedMod){

            PathFinding1(G1X, G1Y, x, y);
            PathFinding2(G2X, G2Y, x+2, y+2);
            PathFinding3(G3X, G3Y, x-1, y-3);
        }
        if(frightenedMod){
            PathFinding1(G1X, G1Y, ghost1SpawnX, ghost1SpawnY);
            PathFinding2(G2X, G2Y, ghost2SpawnX, ghost2SpawnY);
            PathFinding3(G3X, G3Y, ghost3SpawnX, ghost3SpawnY);
        }  

        SetConsoleTextAttribute(hConsole, PacmanColor);
        gotoxy(x, y);  
        cout << pacmanIcon;
        SetConsoleTextAttribute(hConsole, 15);  
        //following code makes sure that ghost wont eat the cherry nor the dots
        //----------------------ghost 1-----------------------
        if(map[G1Y][G1X]==' '){
            gotoxy(G1X, G1Y);  
            cout << " ";
        }
        if(map[G1Y][G1X]=='.'){
            gotoxy(G1X, G1Y);  
            cout << "•";
        }
        if(map[G1Y][G1X]=='C'){
            gotoxy(G1X, G1Y);  
            cout << "C";
        }     
        //----------------------ghost 2-----------------------
        if(map[G2Y][G2X]==' '){
            gotoxy(G2X, G2Y);  
            cout << " ";
        }
        if(map[G2Y][G2X]=='.'){
            gotoxy(G2X, G2Y);  
            cout << "•";
        }
        if(map[G2Y][G2X]=='C'){
            gotoxy(G2X, G2Y);  
            cout << "C";
        }
        //----------------------ghost 3-----------------------
        if(map[G3Y][G3X]==' '){
            gotoxy(G3X, G3Y);  
            cout << " ";
        }
        if(map[G3Y][G3X]=='.'){
            gotoxy(G3X, G3Y);  
            cout << "•";
        }      
        if(map[G3Y][G3X]=='C'){
            gotoxy(G3X, G3Y);  
            cout << "C";
        }
        //-----------------------------------------------------
        if (frameCount % difficulty == 0 && walk1_queue.size() != 0){

            G1X = walk1_queue.back().x;  
            G1Y = walk1_queue.back().y;  
            walk1_queue.pop_back();  
        }
        if (frameCount % difficulty == 0 && walk2_queue.size() != 0){
         
            G2X = walk2_queue.back().x;  
            G2Y = walk2_queue.back().y;  
            walk2_queue.pop_back();  
        }
        if (frameCount % difficulty == 0 && walk3_queue.size() != 0){

            G3X = walk3_queue.back().x;  
            G3Y = walk3_queue.back().y;  
            walk3_queue.pop_back();  
        }         

        if (((G1X == x && G1Y == y) || (G2X == x && G2Y == y) || (G3X == x && G3Y == y)) && !frightenedMod){ 

            deathBlinking();
            Sleep(1000);


            deathCount++;
            x=pacmanSpawnX;
            y=pacmanSpawnY;
            gotoxy(x, y);
            cout << pacmanIcon;

            G1X=ghost1SpawnX;
            G1Y=ghost1SpawnY;
            SetConsoleTextAttribute(hConsole, 4);
            gotoxy(G1X, G1Y);  
            cout << ghostIcon;

            G2X=ghost2SpawnX;
            G2Y=ghost2SpawnY;
            SetConsoleTextAttribute(hConsole, 5);
            gotoxy(G2X, G2Y);  
            cout << ghostIcon;
            
            G3X=ghost3SpawnX;
            G3Y=ghost3SpawnY;
            SetConsoleTextAttribute(hConsole, 3);
            gotoxy(G3X, G3Y);  
            cout << ghostIcon;
            SetConsoleTextAttribute(hConsole, 15);
        }
        //-----------------eating ghost 1-----------------------------
        if ((G1X == x && G1Y == y) && frightenedMod){ 
            
            Sleep(300);
            

            G1X=ghost1SpawnX;
            G1Y=ghost1SpawnY;
            
            SetConsoleTextAttribute(hConsole, ghost1Color);
            
            gotoxy(G1X, G1Y);  
            cout << ghostIcon;

            SetConsoleTextAttribute(hConsole, 15);
        }
        //-----------------eating ghost 2-----------------------------
        if ((G2X == x && G2Y == y) && frightenedMod){ 

            
            Sleep(300);
            

            G2X=ghost2SpawnX;
            G2Y=ghost2SpawnY;
            
            SetConsoleTextAttribute(hConsole,  ghost2Color);
            
            gotoxy(G2X, G2Y);  
            cout << ghostIcon;

            SetConsoleTextAttribute(hConsole, 15);
        }
        //-----------------eating ghost 3-----------------------------
        if ((G3X == x && G3Y == y) && frightenedMod){ 

            
            Sleep(300);
            

            G3X=ghost3SpawnX;
            G3Y=ghost3SpawnY;
            
            SetConsoleTextAttribute(hConsole,  ghost3Color);
            
            gotoxy(G3X, G3Y);  
            cout << ghostIcon;
            
            SetConsoleTextAttribute(hConsole, 15);
        }

        SetConsoleTextAttribute(hConsole, ghost1Color); //4
        gotoxy(G1X, G1Y);  
        cout << ghostIcon;  
        
        SetConsoleTextAttribute(hConsole, ghost2Color); //5
        gotoxy(G2X, G2Y);  
        cout << ghostIcon;

        SetConsoleTextAttribute(hConsole, ghost3Color); //3
        gotoxy(G3X, G3Y);  
        cout << ghostIcon;


        gotoxy(0, 2 * h + 1);  
        cout << pts;
        SetConsoleTextAttribute(hConsole, 15); 

        
        SetConsoleTextAttribute(hConsole, 12);

        if (deathCount == 0){
            gotoxy(2*w - 2, 2 * h + 1);                     // the most left heart
            cout << "\u2665";
            gotoxy(2*w - 1, 2 * h + 1);                     // the middle heart
            cout << "\u2665";
            gotoxy(2*w, 2 * h + 1);                         // the most right heart
            cout << "\u2665";
        }
        switch(deathCount){

            case 1:
                gotoxy(2*w - 2, 2 * h + 1);                     // the most left heart
                cout << " ";
                break;
            case 2:
                gotoxy(2*w - 1, 2 * h + 1);                     // the middle heart
                cout << " ";
                break;
            case 3:
                gotoxy(2*w, 2 * h + 1);                         // the most right heart
                cout << " ";
                break;
        }

        SetConsoleTextAttribute(hConsole, 15);

        Sleep(100);  
        frameCount++; 
        clearScreen(); 
    }

    if (pts != maxPoints){                      // if gameover
        deathScreen();
        pts = 0;
    }
    else{                                       // if all the dots have been eaten succesfully
        victory();
        pts = 0;
    }
        main();
    return 0;  
}  
//***********************************************************************************************************************************************
void mainMenu(){
    system("cls");
    
    int BorderColor = primaryColor;
    int InsideColor = secondaryColor;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"╒═════════════════════════════════════════════════════════════════╕"<<endl;
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<" _______  _______  _______        __   __  _______  __    _";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"    │"<<endl;       Sleep(150);  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|       ||   _   ||       |      |  |_|  ||   _   ||  |  | |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;    Sleep(150);  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|    _  ||  |_|  ||       | ____ |       ||  |_|  ||   |_| |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;    Sleep(150);  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|   |_| ||       ||       ||____||       ||       ||       |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;    Sleep(150);  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|    ___||       ||      _|      |       ||       ||  _    |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;    Sleep(150);  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|   |    |   _   ||     |_       | ||_|| ||   _   || | |   |";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;    Sleep(150);  
    cout<<"│  ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"|___|    |__| |__||_______|      |_|   |_||__| |__||_|  |__|";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"   │"<<endl;    Sleep(150);  
    cout<<"│                                                                 │"<<endl;     Sleep(150);
    cout<<"╘═════════════════════════════════════════════════════════════════╛"<<endl;     Sleep(150);

    cout<<endl;

    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"1";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │     ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"NEW GAME     ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;	Sleep(200);
	
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"2";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │       ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"LOAD       ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;	Sleep(200);

    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"3";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │   ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"INSTRUCTIONS   ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;	Sleep(200);

    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"4";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │    ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"SCOREBOARD    ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;	Sleep(200);

    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"5";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │      ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"CREDITS     ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;	Sleep(200);

    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<"                    ╒═══╤══════════════════╕                 "<<endl;

    cout<<"                    │ ";
    SetConsoleTextAttribute(hConsole, InsideColor);
    cout<<"6";
    SetConsoleTextAttribute(hConsole, BorderColor);
    cout<<" │       ";
    SetConsoleTextAttribute(hConsole, InsideColor);    
    cout<<"EXIT       ";
    SetConsoleTextAttribute(hConsole, BorderColor);    
    cout<<"│                 "<<endl;

    cout<<"                    ╘═══╧══════════════════╛                 "<<endl;	


	SetConsoleTextAttribute(hConsole, 15);



	switch(char c =getch()){
	 	case '1':{
			Beep(850,100);
		}break;

	 	case '2':{
			Beep(850,100);

			loadFile(WIDTH, HEIGHT);	
		}break;

	 	case '3':{
			Beep(850,100);
	 		instructions();	
		}break;

		case '4':{
			Beep(850,100);
			showScoreBoard();
		}break;

		case '5':{
			Beep(850,100);
			system("cls");
			credits();
            mainMenu();
		}break;
		
		case '6':{
			Beep(850,100);
			system("cls");

            SetConsoleTextAttribute(hConsole, primaryColor);
			vintagePrint("Adiós Amigo!\nTake care ... :)\n");

			Sleep(300);
			exit(0);
		}break;

        case 'e':{ 

            easterEgg();
        }
		
		default:
			Beep(650,100);
			system("cls");

			SetConsoleTextAttribute(hConsole, 4);
			vintagePrint("Invalid input!\nPlease input a number from 1 to 6");
            SetConsoleTextAttribute(hConsole, 15);

			Sleep(1500);
			mainMenu();	
	}
	system("cls");
}
//***********************************************************************************************************************************************
void instructions(){
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, primaryColor);
	vintagePrint("CONTROLS ARE W,A,S,D\nYOU CAN PAUSE THE GAME BY PRESSING X\nDOTS SCORE ONE POINTS EACH\nAFTER EATING A CHERRY PAC-MAN CAN ATTACK GHOSTS\nBEWARE OF THE FRIGHTENED GHOSTS THAT CAN TURN BACK INTO HUNTING MODE\nGOOD LUCK!\n");
    Sleep(500);
	system("pause");
	Beep(850,100);
    SetConsoleTextAttribute(hConsole, 15);
	mainMenu();
}

//***********************************************************************************************************************************************
void showScoreBoard(){

    fstream dataFile;
    dataFile.open(FILENAME, ios::in);										// opening the file for a read

	string players [10];
	int scores [10];
	int counter = 0;

	while (dataFile >> players[counter] >> scores[counter]){				// this while loop stores the records saveds in the text file in an array of size 10 with the highest 
						  													// score being the first element (with index 0)
		counter ++;
	}

	dataFile.close();

	string ranking[10]={"1st","2nd","3rd","4th","5th","6th","7th","8th","9th","10th"};

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, primaryColor);
    system("cls");

	cout<<" _______  __   __  _______      ____   _______      _______  _______  _______  _______    _______  ___      _______  __   __  _______  ______    _______ "<<endl;	Sleep(100);
	cout<<"|       ||  | |  ||       |    |    | |  _ _  |    |  _    ||       ||       ||       |  |       ||   |    |   _   ||  | |  ||       ||    _ |  |       |"<<endl;	Sleep(100);
	cout<<"|_     _||  |_|  ||    ___|     |   | | |   | |    | |_|   ||    ___||  _____||_     _|  |    _  ||   |    |  |_|  ||  |_|  ||    ___||   | ||  |  _____|"<<endl;	Sleep(100);
	cout<<"  |   |  |       ||   |___      |   | | |   | |    |       ||   |___ | |_____   |   |    |   |_| ||   |    |       ||       ||   |___ |   |_||_ | |_____ "<<endl;	Sleep(100);
	cout<<"  |   |  |       ||    ___|     |   | | |   | |    |  _   | |    ___||_____  |  |   |    |    ___||   |___ |       ||_     _||    ___||    __  ||_____  |"<<endl;	Sleep(100);
	cout<<"  |   |  |   _   ||   |___      |   | | |_ _| |    | |_|   ||   |___  _____| |  |   |    |   |    |       ||   _   |  |   |  |   |___ |   |  | | _____| |"<<endl;	Sleep(100);
	cout<<"  |___|  |__| |__||_______|     |___| |_______|    |_______||_______||_______|  |___|    |___|    |_______||__| |__|  |___|  |_______||___|  |_||_______|"<<endl;	Sleep(100);

	cout<<endl;
    cout<<endl;
    cout<<"                                       RANK                              SCORE                               NAME                                      "<<endl;

	for (int i = 0 ; i < 9 ; i++){
		SetConsoleTextAttribute(hConsole, i+1);
        if (scores[i] != 0){
            if(scores[i] < 10){
                dataFile << players[i] << ' ' << scores[i] << endl; 
                cout << "                                       " << ranking[i] << "                               00" << scores[i] << "                                 " << players[i] << "             " << endl;  Sleep(100);
        }
            else if(scores[i]<100){                    
    		dataFile << players[i] << ' ' << scores[i] << endl; 
    		cout << "                                       " << ranking[i] << "                               0" << scores[i] << "                                 " << players[i] << "             " << endl;	Sleep(100);
            }
            else{
                dataFile << players[i] << ' ' << scores[i] << endl; 
                cout << "                                       " << ranking[i] << "                               " << scores[i] << "                                 " << players[i] << "             " << endl; Sleep(100);
            }
        }
	}

    if (scores[9] != 0){
    	SetConsoleTextAttribute(hConsole, 10);
        if(scores[9] < 10){ 
            cout << "                                       " << ranking[9] << "                               00" << scores[9] << "                                 " << players[9] << "             " << endl;  Sleep(100);
        }else if(scores[9]<100){    	
        cout << "                                       " << ranking[9] << "                              0" << scores[9] << "                                 " << players[9] << "             " << endl;	Sleep(100);
        }else{
            cout << "                                       " << ranking[9] << "                              " << scores[9] << "                                 " << players[9] << "             " << endl;  Sleep(100);
        }
    }
	dataFile.close();
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl;
    system("pause");
    Beep(850,100);
    mainMenu();
		
}
//***********************************************************************************************************************************************
void updateScoreBoard(string currrentPlayer, int currentScore){

    fstream dataFile;
    dataFile.open(FILENAME, ios::in);										// opening the file for a read

	string players [10];
	int scores [10];
	int counter = 0;

	while (dataFile >> players[counter] >> scores[counter]){				// this while loop stores the records saveds in the text file in an array of size 10 with the highest 
						  													// score being the first element (with index 0)
		counter ++;
	}

	dataFile.close();

	if (currentScore >= scores[0]){												// cheking weather the score is the highest
			for (int i = 8 ; i >= 0 ; i--){										// if yes, shifting the top 9 elements of the array to 1 lower place and chaning the value of the 0th element
				scores[i+1] = scores[i];
				players[i+1] = players[i];
			}
			scores[0] = currentScore;
			players[0] = currrentPlayer;
            vintagePrint("YOU ARE THE TOP PLAYER NOW\nYOUR NAME IS NOW ON THE TOP OF THE SCOREBOARD\n");
	}

	else if (currentScore > scores[9]){			            // cheking weather currentScore is among the top 10 scores or not
			
		int placeHolder = 9;							    // this variable holds the place the new record must be placed in

		for (int i = 9 ; i > 0 ; i--){

			if (currentScore > scores[i] ){
				placeHolder --;
			}

			else{
				break;
			}
		}
		
		for (int i = 8 ; i > placeHolder ; i--){								// shifting the records by 1 step to make room for the new record we want to add 
			scores[i+1] = scores[i];
			players[i+1] = players[i];
		}

		scores[placeHolder + 1] = currentScore;									// adding the new record to the array
		players[placeHolder + 1] = currrentPlayer;
        if (placeHolder == 1){
            vintagePrint("congrats! You are now 2nd among the top 10 players!\n");
        }
        else if (placeHolder == 2){
            vintagePrint("congrats! You are now 3rd among the top 10 players!\n");
        }
        else{
            vintagePrint("congrats! You are now ");
            cout << placeHolder + 2;
            vintagePrint("th among the top 10 players!\n");
        }
        
	}	

    //----------------now the arrays "scores" & "players" are all ready to be rewritten to the data file---------------------
    // the following code "updates" the file we have (if nessecarry)
    dataFile.open(FILENAME, ios::out);                                         // opening a file for write

    for (int i = 0; i <= 9; i++){
        dataFile << players[i] << " " << scores[i] << endl;
    }
    dataFile.close();

    if(currentScore < scores[9]){
        vintagePrint("You were not among the top 10 players ");
        cout << currrentPlayer; 
        vintagePrint("! Keep trying...!");
    }


}
//***********************************************************************************************************************************************
void credits(){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, primaryColor);
    cout<<"╒══════════════════════════════════════════════╕"<<endl;Sleep(150);
    cout<<"│                                              │"<<endl;Sleep(150);
    cout<<"│              CREDIT FOR PAC-MAN              │"<<endl;Sleep(150);
    cout<<"│                                              │"<<endl;Sleep(150);
    cout<<"│   GAME PROGRAMMERS:                          │"<<endl;Sleep(150);
    cout<<"│                 FARID KARIMI                 │"<<endl;Sleep(150);
    cout<<"│                 SHAYAN SHAHRABI              │"<<endl;Sleep(150);
    cout<<"│                                              │"<<endl;Sleep(150);
    cout<<"│   SPECIAL THANKS TO:                         │"<<endl;Sleep(150);
    cout<<"│                  STACKOVERFLOW               │"<<endl;Sleep(150);
    cout<<"│                  YOUTUBE/GOOGLE              │"<<endl;Sleep(150);
    cout<<"│                  GEEKS FOR GEEKS             │"<<endl;Sleep(150);
    cout<<"│                                              │"<<endl;Sleep(150);
    cout<<"│             MS PAC-MAN TM NAMCO LTD          │"<<endl;Sleep(150);
    cout<<"│                ©1990 TENGEN INC              │"<<endl;Sleep(150);
    cout<<"│               ALL RIGHTS RESERVED            │"<<endl;Sleep(150);
    cout<<"│                                              │"<<endl;Sleep(150);
    cout<<"╘══════════════════════════════════════════════╛"<<endl;
	SetConsoleTextAttribute(hConsole, 15);
    system("pause");
}

//***********************************************************************************************************************************************
void vintagePrint(string str){
	for(int i=0 ; i < str.length() ; i++){
		Sleep(36);
		cout << str[i];
	}
}
//***********************************************************************************************************************************************
void deathBlinking(){
    for (int i = 1; i < 5; i++)
    {
        system("Color 0A");
        Sleep (300);
        Beep(400,100);
        system("Color 0C");
        Sleep (300);
    }
}
//***********************************************************************************************************************************************
void victory(){
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole , 12);
    cout<<"                                                                                  "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"                                                 .''.                             "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 10);
    cout<<"                     .''.      .        *''*    :_\\/_:     .                     "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 4);
    cout<<"                    :_\\/_:   _\\(/_  .:.*_\\/_*   : /\\ :  .'.:.'.               "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 12);
    cout<<"                .''.: /\\ :   ./)\\   ':'* /\\ * :  '..'.  -=:o:=-                "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 12);
    cout<<"               :_\\/_:'.:::.    ' *''*    * '.\'/.' _\\(/_'.':'.'                 "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 10);
    cout<<"               : /\\ : :::::     *_\\/_*     -= o =-  /)\\    '  *                "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"                '..'  ':::'     * /\\ *     .'/.\'.   '                           "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 10);
    cout<<"                                 *..*         :                                   "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 10);
    cout<<"           __   __  _______  __   __    _     _  _______  __    _    __           "<<endl;Sleep(200);
    cout<<"          |  | |  ||       ||  | |  |  | | _ | ||       ||  |  | |  |  |          "<<endl;Sleep(200);
    cout<<"          |  |_|  ||   _   ||  | |  |  | || || ||   _   ||   |_| |  |  |          "<<endl;Sleep(200);
    cout<<"          |       ||  | |  ||  |_|  |  |       ||  | |  ||       |  |  |          "<<endl;Sleep(200);
    cout<<"          |_     _||  |_|  ||       |  |       ||  |_|  ||  _    |  |__|          "<<endl;Sleep(200);
    cout<<"            |   |  |       ||       |  |   _   ||       || | |   |   __           "<<endl;Sleep(200);
    cout<<"            |___|  |_______||_______|  |__| |__||_______||_|  |__|  |__|          "<<endl;Sleep(200);
    cout<<"                                                                                  "<<endl;Sleep(200);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"                                                 .''.                             "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 10);
    cout<<"                     .''.      .        *''*    :_\\/_:     .                     "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"                    :_\\/_:   _\\(/_  .:.*_\\/_*   : /\\ :  .'.:.'.               "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 4);
    cout<<"                .''.: /\\ :   ./)\\   ':'* /\\ * :  '..'.  -=:o:=-                "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"               :_\\/_:'.:::.    ' *''*    * '.\'/.' _\\(/_'.':'.'                 "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 12);
    cout<<"               : /\\ : :::::     *_\\/_*     -= o =-  /)\\    '  *                "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 10);
    cout<<"                '..'  ':::'     * /\\ *     .'/.\'.   '                           "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"                                 *..*         :                                   "<<endl;Sleep(50);
    SetConsoleTextAttribute(hConsole , 6);
    cout<<"                                                                                  "<<endl;Sleep(50); 
    cout<<"                            ══════════════════════════                            "<<endl;Sleep(50);
    cout<<"                                    Score: "<< pts <<"                            "<<endl;Sleep(50);
    cout<<"                            ══════════════════════════                            "<<endl;Sleep(50);   

    string name;

    cout<<"                            ══════════════════════════                            "<<endl;Sleep(50);
    cout<<"                                    Name:"<<endl;Sleep(50);
    cout<<"                            ══════════════════════════                            "<<endl;Sleep(50);
    gotoxy(41 , 30);
    cin >> name;
    cout << endl;

    updateScoreBoard(name, pts);
    system("pause");
    SetConsoleTextAttribute(hConsole, 15);
}
//***********************************************************************************************************************************************
void easterEgg(){

    Beep (233,100);Sleep(125);
    Beep (294,100);Sleep(125);
    Beep (349,100);Sleep(125);
    Beep (466,100);Sleep(125);
    Beep (587,100);Sleep(125);
    Beep (698,100);Sleep(125);
    Beep (932,100);Sleep(575);
    Beep (932,100);Sleep(125);
    Beep (932,100);Sleep(125);
    Beep (932,100);Sleep(125);
    Beep (1046,675);
    Beep (1046,675);

    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, primaryColor);
    vintagePrint("Hmmmmmmmm...\nSeems someone has found something mysterious!\nCongrats!\nNow you can leave a mark on the scoreboard!\n");
    vintagePrint("Just give me your name, without any spaces...\nDon't worry, it's just a simple game...!\nName:");
    string name;
    cin >> name;
    vintagePrint("Well ");
    vintagePrint(name);
    cout << "," << endl;
    vintagePrint("Now let's take a look at the scoreboard...\nHere it is...");
    Sleep(200);
    updateScoreBoard(name, 999);
    showScoreBoard();
    system("pause");
    SetConsoleTextAttribute(hConsole, 15);
    mainMenu();
}
//***********************************************************************************************************************************************
void createfile(){
    fstream dataFile;
    dataFile.open(FILENAME, ios::in);                               // opening the file for a read
    if (!dataFile){
        dataFile.open(FILENAME, ios::out);                          // opening a file for write

        for (int i = 1; i <= 10; i++){                              // when the file is created, it is filled with 10 zero records at the start; but these records won't be
                                                                    // shown on the score board
            dataFile << "NoUser" << " " << 0 << endl;
        }
    }
    dataFile.close();
}
//***********************************************************************************************************************************************
void createMazeFile(){
    fstream mazeFile;
    mazeFile.open(MAZE, ios::in);                               // opening the file for a read
    if (!mazeFile){
        mazeFile.open(MAZE, ios::out);                          // opening a file for write
    }
}
//***********************************************************************************************************************************************
void createcoordinatesFile(){
    fstream coordinates;
    coordinates.open(CREATURE, ios::in);                               // opening the file for a read
    if (!coordinates){
        coordinates.open(CREATURE, ios::out);                          // opening a file for write
    }
}
//***********************************************************************************************************************************************
void save(int w, int h){

    fstream mazeFile;
    fstream coordinates;

     mazeFile.open(MAZE, ios::out);                                  // opening a file for write
     coordinates.open(CREATURE, ios::out);                           // opening a file for write

     for (int i = 0; i < 2 * h + 1 ; i++){
        for (int j = 0; j < 2 * w + 1; j++){
            if(map[i][j]==' '){
                mazeFile << ' ';
            }
            else{
                mazeFile << map[i][j];
            }
        }
        mazeFile << endl;
    }

    coordinates << G1X << ' ' << G1Y << endl;                   //first line strores coordinates of the 1st ghost
    coordinates << G2X << ' ' << G2Y << endl;                   //second line strores coordinates of the 2nd ghost
    coordinates << G3X << ' ' << G3Y << endl;                   //third line strores coordinates of the 3rd ghost
    coordinates << x << ' ' << y << endl;                       //fourth line strores coordinates of pacman
    coordinates << pts << endl;                                 // storing points
    coordinates << frameCount << endl;                          //next we have framecount
    coordinates << w << ' ' << h << endl;                       //dimensions of the map
    coordinates << difficulty << endl;                          //difficulty of the game
    coordinates << deathCount << endl;                          //how many times did we die
    coordinates << primaryColor << endl;                        //primary color code
    coordinates << secondaryColor << endl;                      //secondary color code
    coordinates << PacmanColor << endl;                         //pacman icon color
    coordinates << PlaceHolderMapColor << endl;                 //map walls color
     
    vintagePrint("game saved succesfully\n");

    mazeFile.close();
    coordinates.close();
}

//***********************************************************************************************************************************************
void loadFile(int &w, int &h){
    int i, j, row, column;

    for (i = 0; i < 50; i++ ){
        for (j = 0; j < 50; j++ ){
            map[i][j] = ' ';
        }
    }
    

    fstream mazeFile;
    fstream coordinates;

    mazeFile.open(MAZE, ios::in);                               // opening the file for a read
    coordinates.open(CREATURE, ios::in);                               // opening the file for a read

    row = 0; 
    column = 0;
    string str;

    while (std::getline(mazeFile, str)){
        for (column = 0; column < str.length(); column++){
            map[row][column] = str[column];
        }

        row ++;
        w = ( str.length() - 1 ) / 2;
    }

    h = (row - 1) / 2;

    int curentNumber;
    int counter = 1;

    while (coordinates >> curentNumber){
        switch(counter){
            case 1:
                G1X = curentNumber;
                break;
            case 2:
                G1Y = curentNumber;
                break;
            case 3:
                G2X = curentNumber;
                break;
            case 4:
                G2Y = curentNumber;
                break;
            case 5:
                G3X = curentNumber;
                break;
            case 6:
                G3Y = curentNumber;
                break;
            case 7:
                x = curentNumber;
                break;
            case 8:
                y = curentNumber;
                break;
            case 9:
                pts = curentNumber;
                break;
            case 10:
                frameCount = curentNumber;
                break;
            case 11:
                w = curentNumber;
                break;
            case 12:
                h = curentNumber;
                break;
            case 13:
                difficulty = curentNumber;
                break;
            case 14:
                deathCount = curentNumber;
                break;
            case 15:
                primaryColor = curentNumber;
                break;
            case 16:
                secondaryColor = curentNumber;
                break;
            case 17:
                PacmanColor = curentNumber;
                break;
            case 18:
                PlaceHolderMapColor = curentNumber;
                break;
        }
        counter++;
    }
    
    mazeFile.close();
    coordinates.close();
    isNewGame = true;

    // for ( i = 0 ; i < 50 ; i ++){
    //     for ( j = 0 ; j < 50; j++){
    //         cout << map[i][j];
    //     }
    //     cout << endl;
    // }
    // system("pause");
}

