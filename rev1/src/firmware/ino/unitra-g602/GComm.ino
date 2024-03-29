
#include "GComm.hpp"

#include "utils.hpp"

#include <Arduino.h>
#include <string.h>

#define CRC_SIZE 2 /* CRC size, bytes */

GComm::GComm()
: GCommBase()
{
}

GComm::~GComm()
{

}

unsigned GComm::bytesRawAvailable()
{
    return Serial.available();
}

int GComm::byteReadRaw()
{
    return Serial.read();
}

void GComm::byteWriteRaw(uint8_t value)
{

    Serial.write(value);
    //Serial.println((char)(value));

    //Serial.println((uint8_t)(hi4 + '0'));
    //Serial.println((uint8_t)(lo4 + '0'));

}
