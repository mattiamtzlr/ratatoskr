#include "../include/sd_card.hpp"
#include <cstdint>

/* TODO: find some way to have useful file names, maybe with an increasing ID
 * or a timestamp (how to get it tho?) */

#define FILENAME "log.txt"

SDCard::SDCard(const uint8_t chip_select) : m_chip_select(chip_select) {
    if (!SD.begin(m_chip_select)) {
        Serial.println("SD card initialization failed. Things to check:");
        Serial.println("  1. is a card inserted?");
        Serial.println("  2. is your wiring correct?");
        Serial.println("  3. did you change the chipSelect pin to match your shield or module?");
        Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    }
}

/* returns 0 on error and 1 on success */
int SDCard::init_log(void) {
    m_file = SD.open(FILENAME, FILE_WRITE);
    if (!m_file) {
        Serial.print("SD card: error opening ");
        Serial.println(FILENAME);
        return 0;
    }
    return 1;
}


void SDCard::log_msg(LogLevel lvl, String msg) {
    if (!m_file) {
        m_file.print(lvl == LVL_LOW        ? "[LOW]"
                     : lvl == LVL_NORMAL   ? "[NORMAL]"
                     : lvl == LVL_CRITICAL ? "[CRITICAL]"
                                           : "[?]");
        m_file.println(msg);
    }
}


void SDCard::end_log(void) {
    if (!m_file)
        m_file.close();
}
