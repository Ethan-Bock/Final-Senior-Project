# 🎲 Dice Game (C++ / SDL2)

A turn-based dice game built with C++ and SDL2, featuring both human and AI players, a fully interactive UI, achievements, and game statistics.
The game includes a menu system, tutorial mode, and supports game resets, and tracking of player progress.

## 🛠 Features

- 🎮 Turn-based gameplay with AI opponents
- 🧠 AI decision-making using timed logic (via `SDL_Delay`)
- 🖱 Interactive UI with clickable buttons for rolling, banking, and toggling dice
- 🏆 Achievements system with persistent progress tracking
- 📊 Player statistics saved and displayed
- 📋 Menu and tutorial navigation
- 🎨 Custom button rendering with highlighting and selection states

---

## 🔧 Built With
- **Language:** C++
- **Graphics & Input:** [SDL2](https://www.libsdl.org/)
  - Version Used is: SDL2-devel-2.26.5-VC  
- **Fonts:** SDL2_ttf
- **Images:** SDL2_image
---

## 🚀 Getting Started

### 1. Clone the Repository

```
git clone https://github.com/Ethan-Bock/dice-game-sdl2.git
cd dice-game-sdl2
```

### 2. Install SDL2 Libraries
```
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
```

### 3. Running the Project locally
The following must be called when opening any Linux Machine
```
export LIBGL_ALWAYS_SOFTWARE=1
export DISPLAY=:0
```
To run the project simply call
```
./main
```
and to update using the Makefile just call
```
make
```

![Zilch_project_photo](https://github.com/user-attachments/assets/84a99212-daf8-4d68-a687-6ac84f536d81)
