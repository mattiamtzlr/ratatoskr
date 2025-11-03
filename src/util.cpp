#include "util.hpp"
#include <cctype>

Direction to_cardinal(char direction) {
    switch (tolower(direction)) {
        case 'n': return NORTH;
        case 'e': return EAST;
        case 's': return SOUTH;
        case 'w': return WEST;
        default: return NONE;
    }
}
