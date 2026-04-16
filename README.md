# Maze Game (C + Raylib)

A simple interactive maze game built in C using the Raylib graphics library. The player navigates through a maze, collects required items, and progresses through maps.

---

## Overview

This project demonstrates basic game development concepts using Raylib, including rendering, input handling, collision detection, and simple game state management.

---

## Game Flow

### Start Screen

When the game launches, the following options are available:

1. Press `Enter` to start the game  
2. Press `Q` to quit  
3. A rules menu is available which explains:
   - Use `W`, `A`, `S`, `D` or arrow keys to move  
   - Collect at least one treasure  
   - Collect the key to unlock and proceed to the next map  
4. Press `F` to toggle fullscreen  

---

### Gameplay Objective

- Navigate through the maze
- Collect at least one treasure
- Find and collect the key
- Progress to the next map

---

### End Screen

After the game ends, the following details are displayed:

- Total time taken (timer)
- Number of maps completed

---

## Features

- Real-time player movement
- Grid-based maze layout
- Collision detection with walls
- Multi-map progression system
- Item collection (treasure and key)
- Start and end screens with game states
- Built using the Raylib graphics library

---

## Technologies Used

- Programming Language: C
- Graphics Library: Raylib
- Compiler: GCC

---

## Installation and Setup Guide

### Step 1: Install Raylib

#### For Windows (MinGW)

1. Install MinGW (GCC compiler)
2. Download Raylib from the official website:  
   https://www.raylib.com/
3. Extract the files to a directory (example):  
   `C:\raylib`
4. Add the following path to Environment Variables:  
   `C:\raylib\w64devkit\bin`

---

### Step 2: Clone the Repository

git clone https://github.com/PratyakshSahai/Maze-Game.git

--- 

How the file structure should look-

```text
C:\
├── raylib\
└── projects\
    ├── Maze-Game\
    ├── Game2\
    └── Game3\
```
---

### Step 3: Compile the Game Open a terminal in the project directory and run: 

1. Go to raylib\w64devkit and run the w64devkit.exe file
2. In the opnened terminal type-

```text
cd C:/.../Maze-Game
```
to make the game directory the present working directory, type the absolute path after 'cd'

```text
gcc game.c -o game.exe -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
```
to compile the game.c file

4. Run the game. Type-

```text
./game.exe
```
to run the executable file
