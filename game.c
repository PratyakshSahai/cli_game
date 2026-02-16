#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>

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

void loadMap(Player *player);
void renderMap(Player *player);
int movePlayer(Player *player, char ch);
int checkCollision(Player *player, int i, int j);

int m=0;  // index of map_list
char map_list[10][10] = {"map1.map", "map2.map", "map3.map", "map4.map", "map5.map", "map6.map", "map7.map", "map8.map", "map9.map", "map10.map"};
char map[13][21]={0};

int main() {
  Player player;

  player.sprite='@';
  player.score=0;
  player.hasTreasure=false;
  
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
  FILE *file = fopen(map_list[m], "r"); // filename is map_list element

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