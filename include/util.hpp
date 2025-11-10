#pragma once
#include <map>
#include "Arduino.h"

enum Direction {
    NONE = 0b0000,
    NORTH = 0b1000,
    EAST = 0b0100,
    SOUTH = 0b0010,
    WEST = 0b0001,
};

std::map<Direction, char> dirToCardinalChar = {
    {NONE, '\0'}, {NORTH, 'n'}, {EAST, 'e'}, {SOUTH, 's'}, {WEST, 'w'}};
std::map<char, Direction> cardinalCharToDir = {
    {'\0', NONE}, {'n', NORTH}, {'e', EAST}, {'s', SOUTH}, {'w', WEST}};
