## TicTacToe+ (SFML C++ Game)

**Classic Tic Tac Toe game**, built in **C++** using **SFML** for graphics, audio, and input. It features **Player vs Player** and Player vs Algorithm modes, multiple Algorithm difficulty levels, animated menus, sound effects. 

---
## Gameplay GIF


<img width="480" height="270" alt="output(1)" src="https://github.com/user-attachments/assets/768089c3-ba91-4d9c-b56e-af16d700e4aa" />


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
