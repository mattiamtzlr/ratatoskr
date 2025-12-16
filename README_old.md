# Ratatoskr

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
