#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <conio.h>
#define WALL "\u2588"

struct Player {
  int x, y;
  bool hasKey;
  char sprite;
} player;

void loadMap();
void movePlayer(char ch);

int score=0;
int m=0;
char map_list[10][10] = {"map1.map", "map2.map", "map3.map", "map4.map", "map5.map", "map6.map", "map7.map", "map8.map", "map9.map", "map10.map"};
char map[13][21]={0};

int main() {
  loadMap();

  char ch = '\0';

  player.x=0;
  player.y=1;
  
  (ch = tolower(getch())) != 'q';
    movePlayer(ch);

  for (int i=0; i<13; i++) {
    for (int j=0; j<21; j++) {
      printf("%c", map[i][j]);
    }
    printf("\n");
  }

  return 0;
}

void check(int x, int y) {
  if (map[y][x] == 'T') {
    score++;
    map[y][x] = ' ';
  }

  if (map[y][x] == 'K') {
    m++;
    loadMap();
  }
}

void movePlayer(char ch) {

  int i=0,j=0;
  if (ch == 'w') {
    i=-1,j=0;
  } else if (ch == 'a') {
    i=0, j=-1;
  } else if (ch == 's') {
    i=1, j=0;
  } else if (ch == 'd') {
    i=0, j=1;
  }

  check(player.y+i, player.x+j);

  if(map[player.y+i][player.x+j] != '#') {
    map[player.y][player.x]=' ';
    player.x+=j;
    player.y+=i;
    map[player.y][player.x]=player.sprite;
  }

}

void loadMap() {

  printf("MAP %d\n", m+1);

  char ch='\0';
  FILE *file = fopen(map_list[m], "r");

  int i=0, j=0;
  while ((ch = fgetc(file)) != EOF) {
    
    if (ch == '\n') {
      continue;
    } 

    map[i][j]=ch;
    j++;

    if (j==21) {
      i++;
      j=0;
    }

  }

  for (int i=0; i<13; i++) {
    for (int j=0; j<21; j++) {
      printf("%c", map[i][j]);
    }
    printf("\n");
  }

}

// handle player symbol change