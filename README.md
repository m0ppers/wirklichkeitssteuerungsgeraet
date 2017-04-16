# Wirklichkeitssteuerungsgeraet

(Key) components used to realize "Wirklichkeitssteuerungsgeraet" released at revision 2017 as part of the wild compo (modified game files are not contained)

(Un)license is UNLICENSE

Parts (well the initial code) of `amiga-gamepad` was derived from one of the ft90x examples so their restrictions apply (see code for details).

This is also a wild (heh) language zoo :D

## Components

- amiga-joystick-click-board

    Simple Kicad pcb for joystick2USB click board on top of the ft90x clicker2
    BEWARE: I THINK I found out during soldering that I made a classic of by one error. Be sure to recount the rows and compare it with the clicker if you are really reproducing this.

- amiga-gamepad

    Firmware for a clicker2 ft90x. This will create a full USB Gamepad from an Amiga Joystick connected to a clicker2 ft90x having a `amiga-joystick-click-board`.
    The code can be compiled using the official FTDI 90x toolchain + cmake. This is actually completely useable (and maybe even useful?) standalone. Tested on Windows and Android.

- credits

    Code used to create the credits. SVG+CSS

- font

    Reengineered font from cycles the game. Done using http://www.pentacom.jp/pentacom/bitfontmaker2/ (super awesome tool!)

- firmware

    Arduino code for the BMW <=> Amiga Joystick translation :D Upload to the Arduino micro

- hardware

  Simple fritzing layout of wirklichkeitssteuerungsgeraet. This was quickly put together at the party and is not to be trusted. Also some parts are missing because I fail to create
  proper parts in fritzing. The real MCP2515 also has CAN L and CAN H. These should be connected to the motorcycle connector. You also have to apply a 120 Ohm resistor to the CAN§
  wiring (the speedometer is a terminating node) - the resistor should be available on your board.

- logo

    The logo :S Code is more or less completely stolen from here: https://www.reaktor.com/blog/crash-course-to-amiga-assembly-programming/
    The in game variant had to be slightly adjusted to make it play well with the rest

- png2amiga

    Super hackish (my rust is super bad :() tool to convert an image with a fixed PNG palette to an raw Amiga image

## External components

- brumm case (https://github.com/m0ppers/brumm-case)
- RPck file archiver (https://github.com/m0ppers/rpck)

## Necessary hardware

Please note that the hardware components are not chosen because they were well suited for the job but because I had them at home :D (and I could finally put the otherwise useless clicker2 board to good use :D). From scratch I would probably just  use 1 ExploreEmbedded M3 board (https://www.exploreembedded.com/blog/new-product-announcements/explore-m3-this-board-is-more-about-learning-than-anything-else/) because it has everything on board (except for the bosch sensor) that is needed here. Their support is super awesome as well.

The actual hardware:

- clicker2 ft90x (https://www.mikroe.com/ft90x/) for the Joystick2USB Gamepad part

    Please note that this is good but SUPER proprietary hardware. I bought it because I needed a cheap dev board for the ft90x. They even try to crosssell their (proprietary) compiler (lol :S). Also it is a pain to get the code onboard the chip (I used the proprietary FTDI programmer that costs ~30 EUR).

- Arduino micro (https://www.arduino.cc/en/Main/ArduinoBoardMicro)

    On pictures you will see that I am not using an original one. I bought a iteaduino nano. Do not support these *** (had trouble the second time). Their support is super @^@Q#$@$. Buy genuine. Be cool unlike me.

- MCP2515 board (https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20170317132251&SearchText=MCP2515)
- Adafruit BNO055 breakout board (http://www.exp-tech.de/adafruit-9-dof-absolute-orientation-imu-fusion-breakout-bno055)

## BMW Connector

The information about the BMW connector is licensed under a different license:
```
/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <andreas@mop.koeln> searched the internet. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return Poul-Henning Kamp
* ----------------------------------------------------------------------------
*/
```

The connectors are:

http://www.mouser.de/pdfdocs/ENG_CS_13079993_Micro_Quadlok__1211_13079993.PDF

    Female: 1-967616-1
    Male:  1-967587-3

(Seriously. It took me hours to find that. You DO owe me a beer if you found this info helpful).


## Acknowledgments

- http://mrjester.hapisan.com/04_MC68/ is invaluable for any n00b starting m68k assembly
- https://www.reaktor.com/blog/crash-course-to-amiga-assembly-programming/
- BMW CAN Reversing (https://docs.google.com/spreadsheets/d/1tUrOES5fQZa92Robr6uP8v2dzQDq9ohHjUiTU3isqdc/edit#gid=0)
- Wanted Teams rpck decruncher (http://wt.exotica.org.uk/others.html) made me understand the rpck format. They are still active :D
