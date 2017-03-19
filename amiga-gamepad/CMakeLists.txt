cmake_minimum_required(VERSION 3.4.0)
project(amiga-gamepad C)

add_ft90x_executable(amiga-gamepad
    amiga-gamepad.c
    tinyprintf/tinyprintf.c
)