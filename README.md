# CS-358a Project: Ratatoskr (Micromouse)

Team members: Leoluca Bernardi, Leonardo Bolognese, Ali Gorgani, Mattia Metzler, Anthony Tamberg

<p align="center">sick animation here</p>
<p align="center">finished photo here</p>

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

<img align="right" src="./img/ratatoskr_myth.png" height="150">
Finally, the name 'Ratatoskr' comes from Norse mythology and refers to a squirrel which runs through the world-tree
Yggdrasil, carrying messages. We found this name to be quite fitting for our project, as Ratatoskr is one of the only
creatures capable of traversing the complicated insides of Yggdrasil.  
For your enjoyment, here's an image from a 17<sup>th</sup> icelandic manuscript depicting Ratatoskr, taken from Wikipedia
(public domain).

<br>

## Design

The design of Ratatoskr is made to be fully 3D-printable for most parts, with the remaining parts being common Lego
pieces.  
The final Micromouse weighs approximately 190g, it's 7cm wide and about 10cm long. Most of the weight comes from the
9V battery used to power it.

### Hardware

Ratatoskr requires the following electronic and mechanical components:
| Component                                  | Quantity | Notes                                                                                        |
|--------------------------------------------|----------|----------------------------------------------------------------------------------------------|
| N20 DC gear motor w/ encoder and connector | 2        | Working RPMs: 500, 750, 300[^1]                                                              |
| MX1508 motor driver                        | 1        | -                                                                                            |
| ESP32 development kit                      | 1        | **30-pin** version, higher should be fine                                                    |
| LM2596 buck converter                      | 1        | -                                                                                            |
| VL53L1X time-of-flight sensor              | 4        | Also known as `TOF400C`                                                                      |
| MPU-6050 3-axis accelerometer w/ gyroscope | 1        | -                                                                                            |
| SSD1306 OLED screen                        | 1        | These come in multiple colors, we used hybrid yellow-blue                                    |
| 9V (PP3) battery w/ connector              | 1        | We recommend using good quality batteries for this, as the current should be somewhat stable |
| LEGO(r) 58090 Tire 30.4 x 14 Solid         | 2        | Other tires of approximately the same size should work                                       |
| LEGO(r) 55982 Wheel 18mm x 14mm Axle Hole  | 2        | Alternatively, these can be 3D-printed: [STL file](./CAD/STL/rim_17.5x12.5mm_no_chamfer.stl) |
| LEGO(r) 3705 Technic Axle 4L               | 2        | -                                                                                            |

[^1]: Not tested, but should work given the torque and speed requirements

#### Schematic
Below is the full electronics schematic of ratatoskr.
<p align="center"><img src="./img/schematic.png"></p>
Note that the exact pinout of the components might differ to the actual parts, as there are generally many different
versions available.

### Firmware and Software

Below follows a very brief overview of how the software for Ratatoskr works. For more details look at the source code
:wink:

#### Firmware

We use the following libraries to simplify the hardware interfaces:

| Library                                                                   | Used for                 |
| ------------------------------------------------------------------------- | ------------------------ |
| [pololu/VL53L1X](https://github.com/pololu/vl53l1x-arduino)               | ToF sensors              |
| [ronny-antoon/NVSDatabase](https://github.com/ronny-antoon/DatabaseAPI)   | Flash storage database   |
| [adafruit/Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) | OLED screen              |
| [robjen/GFX_fonts](https://github.com/robjen/GFX_fonts)                   | Font for the OLED screen |

Additionally, we use [PlatformIO](https://platformio.org/) to compile the source code for the ESP32 and to handle all
the libraries above. See [PIO Setup](#pio-setup) for more details.

All sensors communicate with the ESP over the I<sup>2</sup>C protocol, where the same SDA and SCL pin can be used for
all sensors, as the components are able to use different addresses. An exception to this are the ToF sensors which
additionally require some XSHUT logic. The OLED is also controlled via I<sup>2</sup>C.  
There are C++ classes for most components which handle all their low-level usage and provide a nice high-level interface
to interact with the components.

#### Software and Logic

To make testing easier we use [MMS](https://github.com/mackorone/mms), a micromouse simulator. It allows us to test the
behavior of Ratatoskr without physically needing to run it in a maze.

Ratatoskr uses _principal-integral-derivative_ (PID) controllers to generate precise and stable movement. These
controllers constantly adjust the motor power based on sensor feedback, allowing the micromouse to correct itself on the
fly. There are three PID systems in total:

1. Motor encoders: ensure that both wheels spin at the same, correct rate allowing for controlled acceleration.
2. Side ToF sensors: ensure the mouse stays centered between the walls and generally positions itself correctly in the
   maze.
3. Front ToF sensors: ensure that the mouse stays centered during diagonals and detect the end of diagonals.

To find the best path in the maze, Ratatoskr uses a floodfill algorithm while constantly updating its image of the maze
using the sensors. The best path is then extracted from a Dijkstra graph constructed from this information. The Dijkstra
algorithm is tuned to favor diagonals over turns. This path is finally further optimised to combine multiple forward
movements into one so that Ratatoskr doesn't stop at every cell.

#### PIO Setup

The [`platformio.ini`](./platformio.ini) file specifies the environment of the project, namely the microcontroller and
the libraries needed for the project. To easiest way of interacting with PlatformIO is their
[CLI](https://docs.platformio.org/en/latest/core/index.html) which is used as follows:

- Compile the project
    ```sh
    pio run
    ```
- Upload the project to the ESP32
    ```sh
    pio run --target upload
    ```
- Open the serial monitor
    ```sh
    pio device monitor
    ```

All of these commands need to be run from the root directory of this repository.

> [!WARNING]
> Before doing anything for the first time, run
> ```sh
> pio run --target compiledb
> ```
> from the command-line in the root directory to generate `compile_commands.json` which tells `clangd` how to compile
> everything.

### The Maze

The maze we use for Ratatoskr uses the same walls and mounting posts as the previous iterations of micromice with a newly
designed floor. The posts are 3D-printed with a pocket to put an M2.5 nut. The walls and baseplates are lasercut from MDF.
As a sidenote, it would have probably been better to redesign the posts and walls as well, as they aren't that easy
to assemble and use. However, for the sake of saving on resources and time, we decided to go with the existing ones.  
To mount the posts to the baseplates M2.5 flathead screws are used whose heads can be countersunk in the underside of
the baseplates. The walls can then simply be slid inbetween the posts.

The baseplates have been designed to interlock similarly to puzzle tiles eliminating the need for complex mounting

Below is both a technical drawing of the maze baseplates as well as a picture of 4 fully assembled maze baseplates with
some walls.
<p align="center"><img src="./img/maze_v2_drawing.png" width="50%"><!--img src="" width="50%"--></p>


<br>

## Assembly
To assemble Ratatoskr, the following additional hardware is needed:

| Item | Quantity | Note |
| --- | --- | --- |
| M2.5 threaded inserts | 6 | 4x for ESP32 mount, 2x for chassis |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |

### 3D-printing

### Chassis

### Drivetrain and Housing

### Battery and ESP32 Mount

### OLED Mount

### Wiring
