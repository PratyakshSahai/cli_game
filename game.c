#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "raylib.h"
#define MAX 51

typedef struct {
  int x, y, score, mapsCompleted;
  bool hasTreasure;
  char sprite;
} Player;

typedef struct {
    int visited;
    int walls[4];
} Cell;

int ROWS=0;
int COLS=0;
// Random Maze Generation
void initMaze();
void shuffle(int *arr, int n);
void generateMaze(int x, int y);
void buildGrid();
void placeSpecialPoints();
void saveMap(const char *filename);
void setupLevel(int level);
void generateMazeFile(const char *filename,int level);

// Start and End Menus
int startMenu();
void rulesMenu();
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

char map[MAX][MAX]={0};

int main() {

  InitWindow(800, 600, "Game");
  SetTargetFPS(60);
  SetExitKey(0);

  while (!WindowShouldClose()) {

    // [CHANGE 1] Toggle fullscreen with F key (works from any screen)
    if (IsKeyPressed(KEY_F)) ToggleFullscreen();

    int restart = runGame();

    while (IsKeyDown(KEY_R) || IsKeyDown(KEY_Q)) {
      BeginDrawing();
      ClearBackground(BLACK);
      EndDrawing();
    }

    if (!restart) break;
  }

  CloseWindow();
  return 0;
}

int runGame() {
  Player player;

  player.sprite='@';
  player.score=0;
  player.hasTreasure=false;
  player.mapsCompleted = 0;
  
  // Start Menu
  int choice;

  while (1) {
    choice = startMenu();

    if (choice == 0) return 0;   // quit
    if (choice == 2) {
      rulesMenu();
      continue;                 // back to menu
    }
    break;                      // start game
  }

  // Gameplay
  loadMap(&player);
  double startTime = GetTime();
  int illegal = 0;
  bool exitGame = false;

  while (!WindowShouldClose() && !exitGame) {

    if (IsKeyPressed(KEY_Q)) {
      exitGame = true;
    }

    // [CHANGE 1] Toggle fullscreen with F key during gameplay
    if (IsKeyPressed(KEY_F)) ToggleFullscreen();

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) illegal = movePlayer(&player, 'w');
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) illegal = movePlayer(&player, 'a');
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) illegal = movePlayer(&player, 's');
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) illegal = movePlayer(&player, 'd');

    BeginDrawing();
    ClearBackground(BLACK);

    renderMap(&player);
    
    if (illegal) {
      DrawText("COLLECT AT LEAST ONE ", 200, 500, 25, YELLOW);
      DrawRectangle(520, 495, 30, 30, YELLOW);
    }

    EndDrawing();
  }

  double endTime = GetTime();

  // End Menu
  return endMenu(&player, endTime - startTime); // endMenu returns 1 when player clicks R to restart

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
    if (IsKeyPressed(KEY_R)) return 2;
    if (IsKeyPressed(KEY_Q)) return 0;
    // [CHANGE 1] Toggle fullscreen with F key in start menu
    if (IsKeyPressed(KEY_F)) ToggleFullscreen();

    BeginDrawing();
    ClearBackground((Color){25, 30, 35, 255});

    int h = GetScreenHeight();

    DrawCenteredText("MAZE GAME", h/4, 60, YELLOW);

    DrawCenteredText("Press ENTER to Start", h - 280, 20, LIGHTGRAY);
    DrawCenteredText("Press R for Rules", h - 250, 20, LIGHTGRAY);
    DrawCenteredText("Press Q to Quit", h - 220, 20, GRAY);
    // [CHANGE 1] Hint for fullscreen toggle
    DrawCenteredText("Press F to toggle Fullscreen", h - 185, 16, (Color){80, 80, 80, 255});

    EndDrawing();
  }
  return 0;
}

void rulesMenu() {
  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_B)) return;   // go back
    // [CHANGE 1] Toggle fullscreen with F key in rules menu
    if (IsKeyPressed(KEY_F)) ToggleFullscreen();

    BeginDrawing();
    ClearBackground((Color){25, 30, 35, 255});

    int h = GetScreenHeight();

    DrawCenteredText("RULES", h/5, 50, YELLOW);

    DrawControls();

    const char *text1 = "Collect at least one treasure";
    int y1 = h/3 + 230;

    int textWidth1 = MeasureText(text1, 20);
    int centerX = GetScreenWidth() / 2;

    int textX1 = centerX - textWidth1/2 + 20;

    DrawText(text1, textX1, y1, 20, LIGHTGRAY);

    DrawRectangle(textX1 - 45, y1 - 5, 30, 30, YELLOW);

    const char *text2 = "Reach the key to go to next map";
    int y2 = h/3 + 280;

    int textWidth2 = MeasureText(text2, 20);
    int textX2 = centerX - textWidth2/2 + 20;

    DrawText(text2, textX2, y2, 20, LIGHTGRAY);

    // icon
    DrawRectangle(textX2 - 45, y2 - 5, 30, 30, RED);

    DrawCenteredText("Press B to go back", h - 70, 20, GRAY);

    EndDrawing();
  }
}

int endMenu(const Player *player, const double gameTime) {
  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_R)) return 1; // restart
    if (IsKeyPressed(KEY_Q)) return 0; // quit
    // [CHANGE 1] Toggle fullscreen with F key in end menu
    if (IsKeyPressed(KEY_F)) ToggleFullscreen();

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

  // [CHANGE 2] Compute tile size dynamically so the maze fits and stays centered
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();

  int mazeCols = COLS * 2 + 1;
  int mazeRows = ROWS * 2 + 1;

  // Reserve 60px at top for "Map X" label and 20px bottom margin
  int maxTileW = (screenW - 40) / mazeCols;
  int maxTileH = (screenH - 80) / mazeRows;
  int tileSize  = (maxTileW < maxTileH) ? maxTileW : maxTileH;
  if (tileSize < 4) tileSize = 4;

  // Center the maze horizontally; push it down slightly to leave room for the label
  int offsetX = (screenW - mazeCols * tileSize) / 2;
  int offsetY = (screenH - mazeRows * tileSize) / 2 + 20;

  // [CHANGE 3] Draw "Map X" centered above the maze
  char mapLabel[32];
  sprintf(mapLabel, "Map %d", player->mapsCompleted + 1);
  int labelFontSize = (tileSize > 12) ? 24 : 14;
  int labelWidth    = MeasureText(mapLabel, labelFontSize);
  DrawText(mapLabel,
           (screenW - labelWidth) / 2,
           offsetY - labelFontSize - 6,
           labelFontSize,
           LIGHTGRAY);

  for (int i = 0; i < mazeRows; i++) {
    for (int j = 0; j < mazeCols; j++) {

      // [CHANGE 2] Use computed offset instead of hardcoded 85 / 80
      int x = j * tileSize + offsetX;
      int y = i * tileSize + offsetY;

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

  generateMazeFile("maze.map",player->mapsCompleted); // Generate the maze file

  char ch='\0';
  FILE *file = fopen("maze.map", "r");

  int i=0, j=0;
  while ((ch = fgetc(file)) != EOF) {
    
    if (ch == '\n')
      continue;

    map[i][j]=ch; // store each character read from file into 'map' matrix
    j++;

    if (j==COLS*2+1) {
      i++;
      j=0;
    }

  }

  renderMap(player);
  player->x = player->y = 1;

}

//Maze generator
Cell maze[25][25];
char grid[MAX][MAX];

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

int sx, sy; // start
int kx, ky; // key
int ex, ey; // exit

//Treasures (2 total)
int tx1, ty1;
int tx2, ty2;

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
    int row = ROWS * 2 + 1;
    int col = COLS * 2 + 1;

    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
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

// BFS from start (sx,sy) treating the key cell (ex,ey) as impassable.
// Returns 1 if (tx,ty) can be reached without stepping on the key, 0 otherwise.
int isReachableWithoutKey(int tx, int ty) {
    int row = ROWS * 2 + 1;
    int col = COLS * 2 + 1;

    static int visited[MAX][MAX];
    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
            visited[i][j] = 0;

    // Simple BFS with a flat queue
    static int qx[MAX * MAX], qy[MAX * MAX];
    int head = 0, tail = 0;

    qx[tail] = sx; qy[tail] = sy; tail++;
    visited[sx][sy] = 1;

    int ddx[4] = {-1, 1,  0, 0};
    int ddy[4] = { 0, 0, -1, 1};

    while (head < tail) {
        int cx = qx[head], cy = qy[head]; head++;

        if (cx == tx && cy == ty) return 1;

        for (int d = 0; d < 4; d++) {
            int nx = cx + ddx[d];
            int ny = cy + ddy[d];

            if (nx < 0 || nx >= row || ny < 0 || ny >= col) continue;
            if (visited[nx][ny]) continue;
            if (grid[nx][ny] == '#') continue;
            if (nx == ex && ny == ey) continue; // key is treated as a wall

            visited[nx][ny] = 1;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }
    return 0; // not reachable without passing through key
}

// Knocks down random interior walls to create loops / multiple routes.
void addExtraPaths(int numExtra) {
    int added = 0;
    int maxAttempts = numExtra * 60;

    for (int attempt = 0; attempt < maxAttempts && added < numExtra; attempt++) {
        int type = rand() % 2;

        if (type == 0 && ROWS > 1) {
            int i  = rand() % (ROWS - 1);
            int j  = rand() % COLS;
            int gx = i * 2 + 2;
            int gy = j * 2 + 1;
            if (grid[gx][gy] == '#') { grid[gx][gy] = ' '; added++; }
        } else if (COLS > 1) {
            int i  = rand() % ROWS;
            int j  = rand() % (COLS - 1);
            int gx = i * 2 + 1;
            int gy = j * 2 + 2;
            if (grid[gx][gy] == '#') { grid[gx][gy] = ' '; added++; }
        }
    }
}

void placeSpecialPoints() {
    int row = ROWS * 2 + 1;
    int col = COLS * 2 + 1;

    sx = 1;
    sy = 1;

    ex = row - 2;
    ey = col - 2;

    // Treasure 1 — must be reachable from start without crossing the key
    while (1) {
        int x = rand() % row;
        int y = rand() % col;

        if (grid[x][y] == ' ' &&
            !(x == sx && y == sy) &&
            !(x == ex && y == ey) &&
            isReachableWithoutKey(x, y)) {
            tx1 = x;
            ty1 = y;
            break;
        }
    }

    // Treasure 2 — same constraint, and must differ from treasure 1
    while (1) {
        int x = rand() % row;
        int y = rand() % col;

        if (grid[x][y] == ' ' &&
            !(x == sx && y == sy) &&
            !(x == ex && y == ey) &&
            !(x == tx1 && y == ty1) &&
            isReachableWithoutKey(x, y)) {
            tx2 = x;
            ty2 = y;
            break;
        }
    }

}

void saveMap(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    int row = ROWS * 2 + 1;
    int col = COLS * 2 + 1;


    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {

            if (i == sx && j == sy)
              fputc('@', fp);
            else if ((i == tx1 && j == ty1) ||
                     (i == tx2 && j == ty2))
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

void setupLevel(int level) {
    int base = 8;

    ROWS = base +  level/ 2;
    COLS = base + level / 2;
    if (ROWS > (MAX-1)/2)
      ROWS = (MAX-1)/2;
    if (COLS > (MAX-1)/2) 
      COLS = (MAX-1)/2;    
}


void generateMazeFile(const char *filename,int level) {
    srand(time(NULL));
    setupLevel(level);
    initMaze();
    generateMaze(0,0);  
    buildGrid();
    int extraPaths = (ROWS + COLS) / 2;
    addExtraPaths(extraPaths);
    placeSpecialPoints();
    saveMap(filename);
}