# first-chip8

![C++](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white)

### Description

A simple [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C++.
All the opcodes are implemented and OpenGL is used for graphics, though no sound or system buzzer has been implemented yet. The games are generally functional, but some fixes to the clock should be done.

3 public domain ROMs have been included: Pong2, Invaders and Tetris.

### Dependencies

[FreeGLUT](https://it.wikipedia.org/wiki/FreeGLUT)

### Usage

Navigate to the project directory from a terminal and run the following command

`./chip8 ./games/<game_name>.c8`
