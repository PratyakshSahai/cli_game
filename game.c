#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>

#define ROWS 6
#define COLS 10

#define WALL_BG "\x1b[47m  \x1b[0m"
#define KEY "\x1b[41m  \x1b[0m"
#define TREASURE "\x1b[43m  \x1b[0m"
#define PLAYER_WITH_KEY "\x1b[42m  \x1b[0m"
#define PLAYER_NO_KEY "\x1b[44m  \x1b[0m"

typedef struct {
  int x, y, score;
  bool hasTreasure;
  char sprite;
} Player;

typedef struct {
    int visited;
    int walls[4];
} Cell;

void loadMap(Player *player);
void renderMap(Player *player);
int movePlayer(Player *player, char ch);
int checkCollision(Player *player, int i, int j);
void initMaze();
void shuffle(int *arr, int n);
void generateMaze(int x, int y);
void buildGrid();
void placeSpecialPoints();
void saveMap(const char *filename);
void generateMazeFile(const char *filename);

int m=0;  // index of map_list
char map[13][21]={0};

int main() {
  Player player;

  player.sprite='@';
  player.score=0;
  player.hasTreasure=false;
  generateMazeFile("maze.map");
  char ch = '\0';
  loadMap(&player);

  while ((ch = tolower(getch())) != 'q') {
    if (movePlayer(&player, ch) == 0) {
      renderMap(&player);
      continue;
    }
    renderMap(&player);
    printf("Collect " TREASURE " to proceed\n\t\t\t\t\t  ");
  }

  return 0;
}

void renderMap(Player *player) {

  system("cls");  // clear screen
  printf("\t\t\t\t\t  MAP %d\n\t\t\t\t\t  ", m+1);

  for (int i=0; i<13; i++) {
    for (int j=0; j<21; j++) {
      switch(map[i][j]) {
        case '#':
          printf(WALL_BG);
          break;
        case '@':
          if (player->hasTreasure == true)
            printf(PLAYER_WITH_KEY);
          else if (player->hasTreasure == false)
            printf(PLAYER_NO_KEY);
          else
            printf("@@");        
          break;
        case 'T':
          printf(TREASURE);
          break;
        case 'K':
          printf(KEY);
          break;
        default:
          printf("  ");
      }
    }
    printf("\n\t\t\t\t\t  ");
  }
  
}

int checkCollision(Player *player, int i, int j) {

  int *x = &player->x;
  int *y = &player->y;

  if ((map[*y+i][*x+j] == 'K' && player->hasTreasure == false))
    return 1;
  
  else if (map[*y+i][*x+j] != '#') {

    map[*y][*x]=' ';  // current position set to blank after sprite moves
    *x+=j;
    *y+=i;

    if (map[*y][*x] == 'T') {
      player->hasTreasure=true;
      player->score++;  // treasure obtained
    } 
    
    else if (map[*y][*x] == 'K' && player->hasTreasure == true) {
      m++;  // increment map number (goto next map)
      player->hasTreasure = false;  // reset player state
      loadMap(player);  // load next map
    }

    map[*y][*x]=player->sprite;
  }
  return 0;
}

int movePlayer(Player *player, char ch) {

  int i=0,j=0;  // i is CHANGE in y coordinate, j is CHANGE in x coordinate
  if (ch == 'w') {
    i=-1,j=0; 
  } else if (ch == 'a') {
    i=0, j=-1;
  } else if (ch == 's') {
    i=1, j=0;
  } else if (ch == 'd') {
    i=0, j=1;
  }

  return checkCollision(player, i, j);
}

void loadMap(Player *player) {

  char ch='\0';
  FILE *file = fopen("maze.map", "r"); // filename is map_list element

  int i=0, j=0;
  while ((ch = fgetc(file)) != EOF) {
    
    if (ch == '\n')
      continue;

    map[i][j]=ch; // store each character read from file into 'map' matrix
    j++;

    if (j==21) {
      i++;
      j=0;
    }

  }

  renderMap(player);
  player->x = player->y = 1;

}

// handle player symbol change
// reset player position on new map load DONE

//Maze generator
Cell maze[ROWS][COLS];
char grid[ROWS * 2 + 1][COLS * 2 + 1];

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

int sx, sy; // start
int kx, ky; // key
int ex, ey; // exit
//Treasures
int tx1, ty1;
int tx2, ty2;
int tx3, ty3;

void initMaze() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            maze[i][j].visited = 0;
            for (int k = 0; k < 4; k++)
                maze[i][j].walls[k] = 1;
        }
    }
}

void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

void generateMaze(int x, int y) {
    maze[x][y].visited = 1;

    int dirs[4] = {0, 1, 2, 3};
    shuffle(dirs, 4);

    for (int i = 0; i < 4; i++) {
        int d = dirs[i];
        int nx = x + dx[d];
        int ny = y + dy[d];

        if (nx >= 0 && nx < ROWS && ny >= 0 && ny < COLS && !maze[nx][ny].visited) {
            maze[x][y].walls[d] = 0;
            maze[nx][ny].walls[(d + 2) % 4] = 0;
            generateMaze(nx, ny);
        }
    }
}

void buildGrid() {
    int rows = ROWS * 2 + 1;
    int cols = COLS * 2 + 1;

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            grid[i][j] = '#';

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int gx = i * 2 + 1;
            int gy = j * 2 + 1;

            grid[gx][gy] = ' ';

            if (!maze[i][j].walls[0]) grid[gx - 1][gy] = ' ';
            if (!maze[i][j].walls[1]) grid[gx][gy + 1] = ' ';
            if (!maze[i][j].walls[2]) grid[gx + 1][gy] = ' ';
            if (!maze[i][j].walls[3]) grid[gx][gy - 1] = ' ';
        }
    }
}

// Place S, T, K on valid empty cells
void placeSpecialPoints() {
    int rows = ROWS * 2 + 1;
    int cols = COLS * 2 + 1;

    // Start fixed at top-left open cell
    sx = 1;
    sy = 1;

    // Exit fixed at bottom-right open cell
    ex = rows - 2;
    ey = cols - 2;

    // Place key randomly
    int count = 0;
    // Treasure 1
while (1) {
    int x = rand() % rows;
    int y = rand() % cols;

    if (grid[x][y] == ' ' &&
        !(x == sx && y == sy) &&
        !(x == ex && y == ey)) {
        tx1 = x;
        ty1 = y;
        break;
    }
}

// Treasure 2
while (1) {
    int x = rand() % rows;
    int y = rand() % cols;

    if (grid[x][y] == ' ' &&
        !(x == sx && y == sy) &&
        !(x == ex && y == ey) &&
        !(x == tx1 && y == ty1)) {
        tx2 = x;
        ty2 = y;
        break;
    }
}

// Treasure 3
while (1) {
    int x = rand() % rows;
    int y = rand() % cols;

    if (grid[x][y] == ' ' &&
        !(x == sx && y == sy) &&
        !(x == ex && y == ey) &&
        !(x == tx1 && y == ty1) &&
        !(x == tx2 && y == ty2)) {
        tx3 = x;
        ty3 = y;
        break;
    }
}
}

void saveMap(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    int rows = ROWS * 2 + 1;
    int cols = COLS * 2 + 1;


    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            if (i == sx && j == sy)
              fputc('@', fp);
            else if ((i == tx1 && j == ty1) ||
                     (i == tx2 && j == ty2) ||
                     (i == tx3 && j == ty3))
                        fputc('T', fp);
            else if (i == ex && j == ey)
              fputc('K', fp);
            else
              fputc(grid[i][j], fp);
            }
        fputc('\n', fp);
    }

    fclose(fp);
}

void generateMazeFile(const char *filename) {
    srand(time(NULL));

    initMaze();
    generateMaze(0, 0);
    buildGrid();
    placeSpecialPoints();
    saveMap(filename);
}