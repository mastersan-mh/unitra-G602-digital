/**
 * @file fake.cpp
 */

#include "config.hpp"

#include <Arduino.h>

void attachInterrupt(uint8_t, void (*)(void), int mode)
{

}

void digitalWrite(uint8_t pin, uint8_t)
{

}

int digitalRead(uint8_t pin)
{
    return 0;
}

void analogReference(uint8_t mode)
{

}

int analogRead(uint8_t pin)
{
    return 0;
}

void analogWrite(uint8_t pin, int value)
{

}

void pinMode(uint8_t pin, uint8_t)
{

}

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh)
{
    return 0;
}

unsigned long millis(void)
{
    return 0;
}
