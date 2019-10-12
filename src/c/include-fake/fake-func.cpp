/**
 * @file fake.cpp
 */

#include <Arduino.h>
#include "../config.hpp"

void setup();
void loop();


int main()
{
    setup();
    while(1)
    {
        loop();
    }
    return EXIT_SUCCESS;
}

void attachInterrupt(uint8_t, void (*)(void), UNUSED int mode)
{

}

void digitalWrite(UNUSED uint8_t pin, uint8_t)
{

}

int digitalRead(UNUSED uint8_t pin)
{
    return 0;
}

void analogReference(UNUSED uint8_t mode)
{

}

int analogRead(UNUSED uint8_t pin)
{
    return 0;
}

void analogWrite(UNUSED uint8_t pin, UNUSED int value)
{

}

void pinMode(UNUSED uint8_t pin, uint8_t)
{

}

long map(UNUSED long value, UNUSED long fromLow, UNUSED long fromHigh, UNUSED long toLow, UNUSED long toHigh)
{
    return 0;
}

unsigned long millis(void)
{
    return 0;
}
