# CS-358a Project: Ratatoskr (Micromouse)
Leoluca Bernardi, Leonardo Bolognese, Ali Gorgani, Mattia Metzler, Anthony Tamberg

_sick animation here_  
_image of finished v1 and v2 mice next to each other here_  

## Introduction
Ratatoskr is a small autonomous robot, called a 'micromouse' which can find the fastest path through a maze.
In two separate `search` and `run` phases, the mouse first finds the most optimal path and then traverses it,
trying to accomplish both in the shortest possible time. In this project we designed a compact micromouse capable
of traversing a maze smoothly and precisely. Additionally we optimised the final `run` phase such that the mouse
can take diagonal paths if it is more efficient.

### Inspiration and References
This project is inspired by the micromice of a previous iteration of the course, namely
[MinosMouse](https://github.com/epfl-cs358/2024fa-minosmouse) and
[PicoMouse](https://github.com/epfl-cs358/2024fa-picomouse). We used their projects to get an idea of the needed
components and the general layout of the software.  
We also used parts of their maze for convenience and to save on resources and waste, as remaking these would
include a lot of material and time. More about this in the maze section.  

<img style="float:right" src="./img/ratatoskr.png" height="150">
Finally, the name 'Ratatoskr' comes from Norse mythology and refers to a squirrel which runs through the world-tree
Yggdrasil, carrying messages. We found this name to be quite fitting for our project, as Ratatoskr is one of the only
creatures capable of traversing the complicated insides of Yggdrasil.  
For your enjoyment, here's an image from a 17<sup>th</sup> icelandic manuscript depicting Ratatoskr, taken from Wikipedia (public domain).


# ↓ OLD VERSION ↓

## Logistics
- Use `./src/` for source code (`.cpp`, `.c`) and for unit tests (`.test.cpp`).
- Use `./include/` for headers (`.hpp`, `.h`)
- Use `./tests/` for any other form of testing
- The `.clang-format` file specifies the formatting style and can be applied to all files using `make format`

### CAD
- Use `./CAD/STL` **only** for `.stl` files, preferably with a well-chosen name
- Use `./CAD/3D_print` **only** for `.3mf` files, preferably with a name related to the STL files it includes.

See below for an example.

#### Wheels
The final wheels `.stl` files are `./CAD/STL/rim_17.5x12.5mm_no_chamfer.stl` and `./CAD/STL/tire_17.5-27x12.5mm_treaded_0.75mm_tol.stl` which correspond to the wheel of width 12.5mm, tire inner diameter 17.5mm and tire outer diameter 27mm.  
The corresponding 3D printing files are:
- `./CAD/3D_print/rims_17.5x12.5mm_no_chamfer.3mf`
- `./CAD/3D_print/tires_17.5-27x12.5mm_treaded_0.75mm_tol.3mf` for the tires with -0.75mm diameter-tolerance

## PlatformIO usage
To compile the project run `pio run` from the command-line in the root directory.  
To upload the project run `pio run --target upload` from the command-line in the root directory.  
To open the serial monitor run `pio device monitor` from the command-line in the root directory.  

> [!WARNING]
> Before doing anything for the first time, run `pio run --target compiledb` from the command-line in the root directory to generate `compile_commands.json` which tells `clangd` how to compile everything.

## Software

### Class Hierarchy
`gear_motor` - Class that controls the N20 Motor. Requires interrupt GPIO and PWM pins.
`gyro` - Class that handles interactions with the 6050 Gyroscope. Requires I2C pins.

`tof` - Class that handles interactions with the ToF Sensors. Requires I2C and GPIO pins.

`led_matrix` - [Deprecated] Class that controls the 8x8 LED Matrix. 

`sd_card` - [Deprecated] Class that handles interactions with the SD Card. 

`maze` - Class representation of the discovered part of the labyrinth. This class contains an array representation to track the walls as well as one for the distances to target. Further it maintains the targets and the visited coordinates.

`mouse` - Abstract class that outlines the interface a mouse (that can interact with the solver) should expose.

`ratatoskr`
`virtual_mouse`
`solver`
