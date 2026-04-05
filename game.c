#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "raylib.h"

#define ROWS 6
#define COLS 10 

typedef struct {
  int x, y, score, mapsCompleted;
  bool hasTreasure;
  char sprite;
} Player;

typedef struct {
    int visited;
    int walls[4];
} Cell;

// Random Maze Generation
void initMaze();
void shuffle(int *arr, int n);
void generateMaze(int x, int y);
void buildGrid();
void placeSpecialPoints();
void saveMap(const char *filename);
void generateMazeFile(const char *filename);

// Start and End Menus
int startMenu();
int endMenu(const Player *player, const double gameTime);
void DrawCenteredText(const char *text, int y, int fontSize, Color color);
void DrawCenteredBox(int y, int w, int h, Color color, const char *text, int fontSize);
void DrawKeyBox(const char *key, int x, int y);
void DrawControls();

// Map Rendering
void loadMap(Player *player);
void renderMap(Player *player);

// Player MovementLogic
int movePlayer(Player *player, char ch);
int checkCollision(Player *player, int i, int j);

// Main Game Function
int runGame();

int m=0;  // index of map_list
char map[13][21]={0};

int main() {

  while (runGame());
  return 0;

}

int runGame() {
  Player player;

  player.sprite='@';
  player.score=0;
  player.hasTreasure=false;
  player.mapsCompleted = 0;

  generateMazeFile("maze.map"); // Generate the maze file

  InitWindow(800, 600, "Game");
  SetTargetFPS(60);
  SetExitKey(0);
  
  // Start Menu
  if (!startMenu()) {
    CloseWindow();
    return 0;
  }

  // Gameplay
  loadMap(&player);
  double startTime = GetTime();
  int illegal;
  bool exitGame = false;

  while (!WindowShouldClose() && !exitGame) {

    if (IsKeyPressed(KEY_Q)) {
      exitGame = true;
    }

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) illegal = movePlayer(&player, 'w');
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) illegal = movePlayer(&player, 'a');
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) illegal = movePlayer(&player, 's');
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) illegal = movePlayer(&player, 'd');

    BeginDrawing();
    ClearBackground(BLACK);

    renderMap(&player);
    
    if (illegal) {
      DrawText("COLLECT ATLEAST ONE ", 200, 500, 25, WHITE);
      DrawRectangle(520, 495, 30, 30, YELLOW);
    }

    EndDrawing();
  }

  double endTime = GetTime();

  // End Menu
  if (endMenu(&player, endTime - startTime)) return 1; // endMenu returns 1 when player clicks R to restart

  CloseWindow();
  
  return 0;
}

void DrawCenteredText(const char *text, int y, int fontSize, Color color) {

  int width = MeasureText(text, fontSize);
  int x = (GetScreenWidth() - width) / 2;
  DrawText(text, x, y, fontSize, color);

}

void DrawCenteredBox(int y, int w, int h, Color color, const char *text, int fontSize) {
  int x = (GetScreenWidth() - w) / 2;

  DrawRectangle(x, y, w, h, color);

  int textWidth = MeasureText(text, fontSize);
  DrawText(text,
    x + (w - textWidth)/2,
    y + (h - fontSize)/2,
    fontSize,
    WHITE
  );
}

void DrawKeyBox(const char *key, int x, int y) {
  int w = 40, h = 40;

  // box
  DrawRectangle(x, y, w, h, (Color){40, 40, 40, 255});
  DrawRectangleLines(x, y, w, h, (Color){80, 80, 80, 255});

  // center text inside box
  int textWidth = MeasureText(key, 20);
  DrawText(key,
      x + (w - textWidth)/2,
      y + (h - 20)/2,
      20,
      WHITE
  );
}

void DrawControls() {
  int centerX = GetScreenWidth() / 2;
  int startY = GetScreenHeight() / 2 - 60;

  int spacing = 10;
  int size = 40;

  DrawCenteredText(" OR",  startY + size + spacing + 10, 25, LIGHTGRAY);
  DrawCenteredText("WASD or Arrow-Keys to move", startY + 120, 20, LIGHTGRAY);

  // WASD (left side)
  int leftX = centerX - 120;

  DrawKeyBox("W", leftX + size + spacing - 50, startY);
  DrawKeyBox("A", leftX - 50, startY + size + spacing);
  DrawKeyBox("S", leftX + size + spacing - 50, startY + size + spacing);
  DrawKeyBox("D", leftX + 2*(size + spacing) - 50, startY + size + spacing);

  // Arrow keys (right side)
  int rightX = centerX + 40;

  DrawKeyBox("^", rightX + size + spacing, startY);
  DrawKeyBox("<", rightX, startY + size + spacing);
  DrawKeyBox("v", rightX + size + spacing, startY + size + spacing);
  DrawKeyBox(">", rightX + 2*(size + spacing), startY + size + spacing);

}

int startMenu() {
  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_ENTER)) return 1;
    if (IsKeyPressed(KEY_Q)) return 0;

    BeginDrawing();
    ClearBackground((Color){25, 30, 35, 255});

    int h = GetScreenHeight();

    DrawCenteredText("MAZE GAME", h/4, 60, YELLOW);

    DrawControls();

    DrawCenteredText("Press ENTER to Start", h - 120, 20, LIGHTGRAY);
    DrawCenteredText("Press Q to Quit", h - 80, 20, GRAY);

    EndDrawing();
  }
  return 0;
}

int endMenu(const Player *player, const double gameTime) {
  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_R)) return 1; // restart
    if (IsKeyPressed(KEY_Q)) return 0; // quit

    BeginDrawing();
    ClearBackground((Color){25, 30, 35, 255});

    int h = GetScreenHeight();

    DrawCenteredText("CONGRATULATIONS!", h/5, 50, YELLOW);

    int minutes = (int)(gameTime / 60);
    int seconds = (int)gameTime % 60;

    char timeText[50];
    sprintf(timeText, "Time: %02d:%02d", minutes, seconds);
    DrawCenteredBox(h/2 - 60, 200, 60, (Color){40,40,40,255}, timeText, 20);

    char mapText[50];
    sprintf(mapText, "Maps: %d", player->mapsCompleted);
    DrawCenteredBox(h/2 + 20, 200, 60, (Color){60,60,60,255}, mapText, 20);

    DrawCenteredText("Press R to Restart", h - 120, 20, LIGHTGRAY);
    DrawCenteredText("Press Q to Quit", h - 80, 20, GRAY);

    EndDrawing();
  }
  return 0;
}

void renderMap(Player *player) {

  int tileSize = 30;

  for (int i=0; i<13; i++) {
    for (int j=0; j<21; j++) {

      int x = j*tileSize + 85;
      int y = i*tileSize + 80;

      switch(map[i][j]) {

        case '#':
          DrawRectangle(x, y, tileSize, tileSize, DARKGRAY);;
          break;

        case '@':
          if (player->hasTreasure == true)
            DrawRectangle(x, y, tileSize, tileSize, GREEN);
          else if (player->hasTreasure == false)
            DrawRectangle(x, y, tileSize, tileSize, BLUE);
          break;

        case 'T':
          DrawRectangle(x, y, tileSize, tileSize, YELLOW);
          break;

        case 'K':
          DrawRectangle(x, y, tileSize, tileSize, RED);
          break;

        default:
          DrawRectangle(x, y, tileSize, tileSize, WHITE);
      }
    }
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
      (player->mapsCompleted)++;
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

void placeSpecialPoints() {
    int rows = ROWS * 2 + 1;
    int cols = COLS * 2 + 1;

    sx = 1;
    sy = 1;

    ex = rows - 2;
    ey = cols - 2;

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