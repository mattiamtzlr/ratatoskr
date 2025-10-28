# CS-358a Project: Ratatoskr Micromouse

## Logistics
- Use `src/` for source code (`.cpp`, `.c`) and for unit tests (`.test.cpp`).
- Use `include/` for headers (`.hpp`, `.h`)
- Use `tests/` for any other form of testing
- the `.clang-format` file specifies the formatting style and can be applied to all files using `make format`

## PlatformIO usage
To compile the project run `pio run` from the command-line in the root directory.  
To upload the project run `pio run --target upload` from the command-line in the root directory.  
To open the serial monitor run `pio device monitor` from the command-line in the root directory.  

> [!WARNING]
> Before doing anything for the first time, run `pio run --target compiledb` from the command-line in the root directory to generate `compile_commands.json` which tells `clangd` how to compile everything.
