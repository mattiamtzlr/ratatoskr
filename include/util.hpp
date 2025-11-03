#pragma once

enum Direction {
    NONE = 0b0000,
    NORTH = 0b1000,
    EAST  = 0b0100,
    SOUTH = 0b0010,
    WEST  = 0b0001,
};

Direction direction_to_cardinal(char direction);

