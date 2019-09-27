/**
 * @file fake.cpp
 */

#include "config.hpp"

#include <Arduino.h>

int main()
{
    return EXIT_SUCCESS;
}

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

long map(long, long, long, long, long)
{
    return 0;
}

unsigned long millis(void)
{
    return 0;
}

size_t Print::println()
{
    return 0;
}

size_t Print::print(char const*)
{
    return 0;
}

size_t Print::println(char const*)
{
    return 0;
}

size_t Print::print(int, int)
{
    return 0;
}

size_t Print::println(int, int)
{
    return 0;
}

size_t Print::print(long, int)
{
    return 0;
}

size_t Print::println(long, int)
{
    return 0;
}

size_t Print::print(unsigned int, int)
{
    return 0;
}

size_t Print::println(unsigned int, int)
{
    return 0;
}

size_t Print::println(unsigned long, int)
{
    return 0;
}

/*
inline HardwareSerial::HardwareSerial(
        volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
        volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
        volatile uint8_t *ucsrc, volatile uint8_t *udr
)
{
    *ubrrh = 0;
    *ubrrl = 0;
    *ucsra = 0;
    *ucsrb = 0;
    *ucsrc = 0;
    *udr   = 0;

    *_ubrrh = 0;
    *_ubrrl = 0;
    *_ucsra = 0;
    *_ucsrb = 0;
    *_ucsrc = 0;
    *_udr   = 0;
}
*/
void HardwareSerial::begin(unsigned long, unsigned char)
{

}
