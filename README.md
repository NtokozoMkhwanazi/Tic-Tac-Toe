# TicTacToe+ (SFML C++ Game)

TicTacToe+ is a modern, visually appealing version of the classic Tic Tac Toe game, built in **C++** using **SFML** for graphics, audio, and input. It features **Player vs Player** and **Player vs AI** modes, multiple AI difficulty levels, animated menus, sound effects, and score tracking.

---

## Features

- **Game Modes**
  - Player vs Player (Local)
  - Player vs AI (PvE)
- **AI Difficulty Levels**
  - Easy (random moves)
  - Medium (moderate strategy, depth-limited minimax)
  - Hard (full-depth minimax with alpha-beta pruning)
- **Visual Features**
  - Animated main menu and buttons
  - Glowing board effects during gameplay
  - Highlighted winning line
  - Day/Night background blending
- **Audio**
  - Background music
  - Click, move, and win sound effects
- **Score Tracking**
  - Tracks wins for X, O, and draws

---

## Screenshots

<img width="1366" height="768" alt="START" src="https://github.com/user-attachments/assets/9aad3a4a-5523-4562-a59b-1b540eab4772" />
<img width="1366" height="768" alt="VERSUS" src="https://github.com/user-attachments/assets/bdf749f3-a38a-43c6-8912-cffd332ede59" />
<img width="1366" height="768" alt="GAMEPLAY" src="https://github.com/user-attachments/assets/294c1385-f5eb-45ac-ba5c-dfae51b9d999" />

---

## Requirements

- **C++ Compiler** supporting C++11 or higher
- **SFML** (Graphics, Window, System, Audio)
- Assets folder containing:
  - Textures (`.png`)
  - Audio (`.wav` and `.ogg`)
  - Fonts (`.ttf`)

---

## Installation / Compilation

Make sure SFML is installed on your system. Compile using:

```bash
g++ TicTacToe.cpp -o tic_tac_toe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
