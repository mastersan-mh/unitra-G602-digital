#ifndef CONFIG_H_
#define CONFIG_H_

/*
-D__AVR_ATmega8__ ???
-DARDUINO_ARCH_AVR ???
-DIDE_ECLIPSE
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/cores/arduino/
-I/home/mastersan/progs/arduino-nightly/./hardware/tools/avr/avr/include/

pins_arduino.h:
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/eightanaloginputs/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/circuitplay32u4/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/mega/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/leonardo/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/robot_motor/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/ethernet/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/standard/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/yun/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/gemma/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/robot_control/
-I/home/mastersan/progs/arduino-nightly/./hardware/arduino/avr/variants/micro/

*/


/* To prevent using of delay() */
#ifdef delay
#   undef delay
#endif
#define delay dealy_is_deprecated

#ifdef IDE_ECLIPSE
#   ifndef __OPTIMIZE__
#       define __OPTIMIZE__
#   endif
#   define F_CPU 8
#   define __AVR_ATmega8__
#   include <Arduino.h>
#endif



#define FACTOR 10

#define DEBUG

#define GDInputDebounced_DEBUG

#ifndef DEBUG
#   define DEBUG_PRINT_INIT()
#   define DEBUG_PRINT(x)
#   define DEBUG_PRINTLN(x)
#else
extern bool debug_inited;
#define DEBUG_INIT_GLOBAL() bool debug_inited = false;
#   define DEBUG_PRINT_INIT() do{ Serial.begin(9600); debug_inited = true; }while(0)
#   define DEBUG_PRINT(x) do{ if(debug_inited) Serial.print(x); }while(0)
#   define DEBUG_PRINTLN(x) do{ if(debug_inited) Serial.println(x); }while(0)
#endif

#endif /* CONFIG_H_ */
