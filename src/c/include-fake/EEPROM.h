/*
 * EEPROM.h
 *
 */

#ifndef INCLUDE_FAKE_EEPROM_H_
#define INCLUDE_FAKE_EEPROM_H_

#include <stdint.h>

struct EEPROMClass
{
    uint8_t read( int idx );
    void write( int idx, uint8_t val );
};

extern EEPROMClass EEPROM;

#endif /* INCLUDE_FAKE_EEPROM_H_ */
