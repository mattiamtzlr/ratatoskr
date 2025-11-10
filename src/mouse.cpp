#include "mouse.hpp"

Mouse::Mouse(Maze &maze, GearMotor &motor_left, GearMotor &motor_right,
             ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
             ToF &tof_right, MPU6050 &gyro, LEDMatrix &screen)
    : m_maze(maze),
      m_motor_left(motor_left),
      m_motor_right(motor_right),
      m_tof_left(tof_left),
      m_tof_front_left(tof_front_left),
      m_tof_front_right(tof_front_right),
      m_tof_right(tof_right),
      m_gyro(gyro),
      m_screen(screen) {}

//===============================[ MAZE ]=======================================
void Mouse::setWall(int x, int y, char direction) {
    m_maze.set_wall(x, y, cardinalCharToDir[direction]);
}

void Mouse::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(x, y, cardinalCharToDir[direction]);
}

int Mouse::mazeHeight() { return m_maze.maze_height(); }
int Mouse::mazeWidth() { return m_maze.maze_width(); }

//===============================[ CONTROL ]====================================
/**
 * turn @angle radians in counterclockwise direction
 */
void Mouse::turn(int angle) {}
void Mouse::turnLeft() { turn(90); }
void Mouse::turnRight() { turn(-90); }

/**
 * move @distance [units] forwards
 */
void Mouse::moveForward(int distance) {
	// convert distance to rpm
	m_motor_left.spin_ccw(50);
	m_motor_right.spin_cw(50);
	delay(1000);
	m_motor_left.stop();
	m_motor_right.stop();
}

//===============================[ SENSING ]====================================
bool Mouse::wallFront() {}
bool Mouse::wallRight() {}
bool Mouse::wallLeft() {}
