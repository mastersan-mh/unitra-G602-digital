/**
 * @file Arduino.h
 * @brief Fake
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_

extern "C"
{
#include <stdlib.h>
#include <stdint.h>
}

#define LOW 0
#define HIGH 255
#define PIN_A0 0

#define INPUT_PULLUP 0
#define OUTPUT 0
#define RISING 0
#define DEFAULT 0
#define LED_BUILTIN 0
#define PROGMEM

unsigned long millis(void);

void attachInterrupt(uint8_t, void (*)(void), int mode);
void digitalWrite(uint8_t pin, uint8_t);
int digitalRead(uint8_t pin);
void analogReference(uint8_t mode);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int value);
void pinMode(uint8_t pin, uint8_t);
long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);

#define min(a,b) ((a)<(b)?(a):(b))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

class CSerial
{
public:
    CSerial();
    ~CSerial();
    void begin(int);
    int available();
    int read();
    void write(char);
    void write(const char *);
    void println(const char *);
    void println(char);
    void print(const char *);
    void print(char);


};
extern CSerial Serial;

struct EEPROMClass
{
    uint8_t read( int idx );
    void write( int idx, uint8_t val );
};

extern EEPROMClass EEPROM;

#endif /* ARDUINO_H_ */
