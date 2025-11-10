#include "util.hpp"
#include <cctype>

Direction rotate_right(Direction dir) {
    return (Direction)((dir + 1) % 4);
}

Direction rotate_left(Direction dir) {
    return (Direction)((dir + 3) % 4);
}



