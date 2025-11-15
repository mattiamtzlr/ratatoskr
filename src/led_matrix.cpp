#include "led_matrix.hpp"
#include <cstdint>

#define LED_ON 1
#define LED_OFF 0

LEDMatrix::LEDMatrix(uint8_t addr) : m_addr(addr) {
    m_matrix.begin(m_addr);
}


/* NOTE: WIP id system:
 * 0 -> side-left
 * 1 -> top-left
 * 2 -> top-right
 * 3 -> side-right */
void LEDMatrix::drawWall(uint8_t id) {
    switch (id) {
        case 0:
            m_matrix.drawLine(0,2, 0,6, LED_ON);
            break;
        case 1:
            m_matrix.drawLine(0,0, 3,0, LED_ON);
            break;
        case 2:
            m_matrix.drawLine(4,0, 7,0, LED_ON);
            break;
        case 3:
            m_matrix.drawLine(7,2, 7,6, LED_ON);
            break;
    }
    m_matrix.writeDisplay();
}


void LEDMatrix::clearWall(uint8_t id) {
    switch (id) {
        case 0:
            m_matrix.drawLine(0,2, 0,6, LED_OFF);
            break;
        case 1:
            m_matrix.drawLine(0,0, 3,0, LED_OFF);
            break;
        case 2:
            m_matrix.drawLine(4,0, 7,0, LED_OFF);
            break;
        case 3:
            m_matrix.drawLine(7,2, 7,6, LED_OFF);
            break;
    }
    m_matrix.writeDisplay();
}


void LEDMatrix::clearAll() {
    m_matrix.clear();
    m_matrix.writeDisplay();
}
