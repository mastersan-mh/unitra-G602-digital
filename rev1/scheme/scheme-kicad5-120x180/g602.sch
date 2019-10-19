EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A2 23386 16535
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 1250 1600 0    60   ~ 0
Блок питания
Text Notes 8650 8500 0    60   ~ 0
Arduino nano
Text Notes 10500 14100 0    60   ~ 0
Драйвер микролифта
Text Notes 10250 12350 0    60   ~ 0
Драйвер двигателя
Text Notes 6700 4400 0    60   ~ 0
Блок\nобратной связи\nдвигателя
Text Notes 11750 1050 0    60   ~ 0
Панель управления
$Comp
L g602:arduino-nano X3
U 1 1 5BA13A93
P 9600 9400
F 0 "X3" H 9600 9450 60  0000 C CNN
F 1 "arduino-nano" H 9600 9300 60  0000 C CNN
F 2 "Module:Arduino_Nano" H 9600 9550 60  0001 C CNN
F 3 "" H 9600 9550 60  0000 C CNN
	1    9600 9400
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5BA13F58
P 12100 1450
F 0 "SW1" H 12250 1560 50  0000 C CNN
F 1 "SW_PUSH" H 12100 1370 50  0000 C CNN
F 2 "" H 12100 1450 50  0001 C CNN
F 3 "" H 12100 1450 50  0000 C CNN
	1    12100 1450
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5BA14279
P 12700 1450
F 0 "SW2" H 12850 1560 50  0000 C CNN
F 1 "SW_PUSH" H 12700 1370 50  0000 C CNN
F 2 "" H 12700 1450 50  0001 C CNN
F 3 "" H 12700 1450 50  0000 C CNN
	1    12700 1450
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5BA142EE
P 13300 1450
F 0 "SW3" H 13450 1560 50  0000 C CNN
F 1 "SW_PUSH" H 13300 1370 50  0000 C CNN
F 2 "" H 13300 1450 50  0001 C CNN
F 3 "" H 13300 1450 50  0000 C CNN
	1    13300 1450
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5BA1435A
P 13900 1450
F 0 "SW4" H 14050 1560 50  0000 C CNN
F 1 "SW_PUSH" H 13900 1370 50  0000 C CNN
F 2 "" H 13900 1450 50  0001 C CNN
F 3 "" H 13900 1450 50  0000 C CNN
	1    13900 1450
	1    0    0    -1  
$EndComp
Text Notes 12050 1200 0    60   ~ 0
45/33
Text Notes 12550 1200 0    60   ~ 0
АВТОСТОП
Text Notes 13250 1200 0    60   ~ 0
ПУСК
Text Notes 13850 1200 0    60   ~ 0
СТОП
$Comp
L Connector:Conn_01x05_Female XS5
U 1 1 5BA1CC17
P 12100 2500
F 0 "XS5" H 12100 2800 50  0000 C CNN
F 1 "Buttons" H 12100 2200 50  0000 C CNN
F 2 "" H 12100 2500 50  0001 C CNN
F 3 "" H 12100 2500 50  0001 C CNN
	1    12100 2500
	0    -1   1    0   
$EndComp
$Comp
L Connector:Conn_01x05_Male XP5
U 1 1 5BA1D282
P 12100 4500
F 0 "XP5" H 12100 4800 50  0000 C CNN
F 1 "Buttons" H 12100 4200 50  0000 C CNN
F 2 "g602:Connector_01x05" H 12100 4500 50  0001 C CNN
F 3 "" H 12100 4500 50  0001 C CNN
	1    12100 4500
	0    -1   1    0   
$EndComp
Text Notes 12300 2300 1    60   ~ 0
+5V\n45/33\nAS\nSTART\nSTOP
Text Notes 12300 4500 1    60   ~ 0
+5V\n45/33\nAS\nSTART\nSTOP
$Comp
L power:GNDREF #PWR06
U 1 1 5BA220B1
P 11900 6550
F 0 "#PWR06" H 11900 6300 50  0001 C CNN
F 1 "GNDREF" H 11900 6400 50  0000 C CNN
F 2 "" H 11900 6550 50  0001 C CNN
F 3 "" H 11900 6550 50  0001 C CNN
	1    11900 6550
	1    0    0    -1  
$EndComp
Text Notes 9000 9100 2    50   ~ 0
IN_DRIVE_ROTATE_PULSE
$Comp
L power:GNDREF #PWR02
U 1 1 5BA306D1
P 6600 9150
F 0 "#PWR02" H 6600 8900 50  0001 C CNN
F 1 "GNDREF" H 6600 9000 50  0000 C CNN
F 2 "" H 6600 9150 50  0001 C CNN
F 3 "" H 6600 9150 50  0001 C CNN
	1    6600 9150
	1    0    0    -1  
$EndComp
Text Notes 9000 9800 2    50   ~ 0
OUT_ENGINE
$Comp
L Device:CP C2
U 1 1 5BA34755
P 11500 12200
F 0 "C2" H 11510 12270 50  0000 L CNN
F 1 "2200uF" H 11510 12120 50  0000 L CNN
F 2 "Capacitor_THT:C_Radial_D10.0mm_H12.5mm_P5.00mm" H 11500 12200 50  0001 C CNN
F 3 "" H 11500 12200 50  0001 C CNN
	1    11500 12200
	-1   0    0    1   
$EndComp
$Comp
L g602:IRL2203NPBF Q2
U 1 1 5BA3609D
P 10700 13700
F 0 "Q2" V 10950 13650 50  0000 L CNN
F 1 "IRL2203NPBF" V 10850 13500 50  0000 L CNN
F 2 "g602:IRL2203NPBF" H 10950 13625 50  0001 L CIN
F 3 "" H 10700 13700 50  0001 L CNN
	1    10700 13700
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R14
U 1 1 5BA37B3A
P 10700 11250
F 0 "R14" H 10750 11200 50  0000 L CNN
F 1 "10" H 10750 11300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" H 10700 11250 50  0001 C CNN
F 3 "" H 10700 11250 50  0001 C CNN
	1    10700 11250
	-1   0    0    1   
$EndComp
Text Notes 10150 9800 0    50   ~ 0
MANUAL SPEED ADJUSTING
$Comp
L Device:D D4
U 1 1 5BA41E09
P 11800 13950
F 0 "D4" H 11800 14050 50  0000 C CNN
F 1 "D" H 11800 13850 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P10.16mm_Horizontal" H 11800 13950 50  0001 C CNN
F 3 "" H 11800 13950 50  0001 C CNN
	1    11800 13950
	0    -1   -1   0   
$EndComp
Text Notes 9000 9400 2    50   ~ 0
IN_BTN_45_33
Text Notes 9000 9600 2    50   ~ 0
IN_BTN_START
Text Notes 13050 14950 0    60   ~ 0
Стробоскоп
$Comp
L Device:LED_Small D5
U 1 1 5BA359DE
P 13300 15200
F 0 "D5" H 13250 15325 50  0000 L CNN
F 1 "LED_Small" H 13125 15100 50  0000 L CNN
F 2 "" V 13300 15200 50  0001 C CNN
F 3 "" V 13300 15200 50  0001 C CNN
	1    13300 15200
	-1   0    0    1   
$EndComp
Text Notes 9850 14750 0    60   ~ 0
DC 23-25V, 100Hz pulses 
$Comp
L Device:R_Small R10
U 1 1 5BA3844D
P 10000 15000
F 0 "R10" V 10080 15000 50  0000 C CNN
F 1 "2k" V 10000 15000 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 9930 15000 50  0001 C CNN
F 3 "" H 10000 15000 50  0001 C CNN
	1    10000 15000
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R7
U 1 1 5BA3904E
P 8750 14800
F 0 "R7" V 8830 14800 50  0000 C CNN
F 1 "510" V 8750 14800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 8680 14800 50  0001 C CNN
F 3 "" H 8750 14800 50  0001 C CNN
	1    8750 14800
	0    1    1    0   
$EndComp
Text Notes 4800 3450 0    60   ~ 0
GND
Text Notes 6500 4100 0    60   ~ 0
GND
Text Notes 6200 4100 0    60   ~ 0
+5V
$Comp
L Device:R R3
U 1 1 5BA3E288
P 7200 5450
F 0 "R3" V 7280 5450 50  0000 C CNN
F 1 "1k" V 7200 5450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 7130 5450 50  0001 C CNN
F 3 "" H 7200 5450 50  0001 C CNN
	1    7200 5450
	-1   0    0    1   
$EndComp
Text Notes 7250 5700 0    60   ~ 0
OUT
$Comp
L g602:TCST1103 X1
U 1 1 5BA59B68
P 7850 1350
F 0 "X1" H 7850 1450 60  0000 C CNN
F 1 "TCST1103" H 7850 1000 60  0000 C CNN
F 2 "" H 7850 1350 60  0001 C CNN
F 3 "" H 7850 1350 60  0001 C CNN
	1    7850 1350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female XS2
U 1 1 5BA5A0E6
P 7700 2350
F 0 "XS2" H 7700 2550 50  0000 C CNN
F 1 "Feedback_drive" H 7700 2050 50  0000 C CNN
F 2 "" H 7700 2350 50  0001 C CNN
F 3 "" H 7700 2350 50  0001 C CNN
	1    7700 2350
	0    -1   1    0   
$EndComp
$Comp
L Connector:Conn_01x03_Male XP2
U 1 1 5BA5BB29
P 7100 4600
F 0 "XP2" H 7100 4800 50  0000 C CNN
F 1 "Feedback_drive" H 7200 4450 50  0000 C CNN
F 2 "g602:Connector_01x03" H 7100 4600 50  0001 C CNN
F 3 "" H 7100 4600 50  0001 C CNN
	1    7100 4600
	0    -1   1    0   
$EndComp
Text Notes 7850 2350 3    60   ~ 0
GND\nE+\nD+
Text Notes 7200 4600 1    60   ~ 0
GND\nE+\nD+
$Comp
L g602:TCST1103 X2
U 1 1 5BA70D71
P 9100 1350
F 0 "X2" H 9100 1450 60  0000 C CNN
F 1 "TCST1103" H 9100 1000 60  0000 C CNN
F 2 "" H 9100 1350 60  0001 C CNN
F 3 "" H 9100 1350 60  0001 C CNN
	1    9100 1350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female XS3
U 1 1 5BA70D77
P 8950 2350
F 0 "XS3" H 8950 2550 50  0000 C CNN
F 1 "Feedback_table" H 8950 2050 50  0000 C CNN
F 2 "" H 8950 2350 50  0001 C CNN
F 3 "" H 8950 2350 50  0001 C CNN
	1    8950 2350
	0    -1   1    0   
$EndComp
Text Notes 9100 2350 3    60   ~ 0
GND\nE+\nD+
Text Notes 8200 4400 0    60   ~ 0
Блок\nобратной связи\nвертушки
Text Notes 8000 4100 0    60   ~ 0
GND
Text Notes 7700 4100 0    60   ~ 0
+5V
$Comp
L Device:R R4
U 1 1 5BA713F9
P 8250 5050
F 0 "R4" V 8330 5050 50  0000 C CNN
F 1 "510" V 8250 5050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 8180 5050 50  0001 C CNN
F 3 "" H 8250 5050 50  0001 C CNN
	1    8250 5050
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 5BA713FF
P 8700 5450
F 0 "R6" V 8780 5450 50  0000 C CNN
F 1 "1k" V 8700 5450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 8630 5450 50  0001 C CNN
F 3 "" H 8700 5450 50  0001 C CNN
	1    8700 5450
	-1   0    0    1   
$EndComp
Text Notes 8750 5700 0    60   ~ 0
OUT
$Comp
L Connector:Conn_01x03_Male XP3
U 1 1 5BA7140C
P 8600 4600
F 0 "XP3" H 8600 4800 50  0000 C CNN
F 1 "Feedback_table" H 8700 4450 50  0000 C CNN
F 2 "g602:Connector_01x03" H 8600 4600 50  0001 C CNN
F 3 "" H 8600 4600 50  0001 C CNN
	1    8600 4600
	0    -1   1    0   
$EndComp
Text Notes 8700 4600 1    60   ~ 0
GND\nE+\nD+
Text Notes 9550 4400 0    60   ~ 0
Блок определения \nостанова (автостоп)
Text Notes 9500 4100 0    60   ~ 0
GND
Text Notes 9200 4100 0    60   ~ 0
+5V
$Comp
L Device:R R8
U 1 1 5BA7289A
P 9750 5050
F 0 "R8" V 9830 5050 50  0000 C CNN
F 1 "510" V 9750 5050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 9680 5050 50  0001 C CNN
F 3 "" H 9750 5050 50  0001 C CNN
	1    9750 5050
	0    1    1    0   
$EndComp
$Comp
L Device:R R11
U 1 1 5BA728A0
P 10200 5400
F 0 "R11" V 10280 5400 50  0000 C CNN
F 1 "1k" V 10200 5400 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 10130 5400 50  0001 C CNN
F 3 "" H 10200 5400 50  0001 C CNN
	1    10200 5400
	-1   0    0    1   
$EndComp
Text Notes 10250 5700 0    60   ~ 0
OUT
Text Notes 10200 4600 1    60   ~ 0
GND\nE+\nD+
$Comp
L g602:TCST1103 X4
U 1 1 5BA7480A
P 10250 1350
F 0 "X4" H 10250 1450 60  0000 C CNN
F 1 "TCST1103" H 10250 1000 60  0000 C CNN
F 2 "g602:TCST1103-veritcal" H 10250 1350 60  0001 C CNN
F 3 "" H 10250 1350 60  0001 C CNN
	1    10250 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 1100 8100 1100
Wire Wire Line
	8250 1950 8250 1100
Wire Wire Line
	7400 1100 7600 1100
Wire Wire Line
	7400 1100 7400 2050
Wire Wire Line
	5250 3650 6400 3650
Wire Notes Line
	7450 5700 6150 5700
Wire Notes Line
	7450 4100 6150 4100
Wire Notes Line
	7450 4100 7450 5700
Wire Wire Line
	6500 4900 7000 4900
Wire Wire Line
	7000 4800 7000 4900
Wire Notes Line
	6150 4100 6150 5700
Wire Wire Line
	7500 9400 9050 9400
Wire Wire Line
	7400 9500 9050 9500
Wire Wire Line
	7300 9600 9050 9600
Wire Wire Line
	7200 9700 9050 9700
Wire Wire Line
	6600 9000 6600 9150
Wire Wire Line
	9050 9000 6600 9000
Wire Wire Line
	7300 7900 15150 7900
Wire Wire Line
	11800 14200 11800 14100
Wire Wire Line
	11800 13700 11800 13800
Wire Wire Line
	11800 12450 11800 12300
Wire Wire Line
	7500 7700 12950 7700
Wire Wire Line
	7400 7800 14050 7800
Wire Wire Line
	12000 5200 12950 5200
Wire Wire Line
	12000 4700 12000 5200
Wire Wire Line
	12100 5100 12100 4700
Wire Wire Line
	12200 5000 15150 5000
Wire Wire Line
	12200 4700 12200 5000
Wire Wire Line
	12300 4900 16250 4900
Wire Wire Line
	12300 4700 12300 4900
Wire Wire Line
	12300 2300 12300 2100
Wire Wire Line
	12200 2300 12200 2000
Wire Wire Line
	12100 2300 12100 1900
Wire Wire Line
	12000 1800 12300 1800
Wire Wire Line
	12000 1800 12000 2300
Wire Notes Line
	11650 850  14300 850 
Wire Wire Line
	12200 2000 13500 2000
Wire Wire Line
	12100 1900 12900 1900
Connection ~ 11900 1700
Wire Wire Line
	13100 1700 13100 1450
Wire Wire Line
	12500 1450 12500 1700
Wire Wire Line
	13700 1700 13700 1450
Wire Wire Line
	11900 1700 12500 1700
Wire Wire Line
	11900 1450 11900 1700
Wire Notes Line
	11650 850  11650 2750
Wire Notes Line
	5000 4750 5000 1500
Wire Wire Line
	6600 5050 6400 5050
Wire Wire Line
	7600 1850 7600 1950
Wire Wire Line
	7800 1850 8100 1850
Wire Wire Line
	7100 4800 7100 5050
Wire Wire Line
	7100 5050 6900 5050
Connection ~ 7600 1950
Wire Wire Line
	7600 1950 8250 1950
Wire Wire Line
	9500 1100 9350 1100
Wire Wire Line
	9500 1950 9500 1100
Wire Wire Line
	8650 1100 8850 1100
Wire Wire Line
	8650 1100 8650 2050
Wire Wire Line
	8850 1850 8850 1950
Wire Wire Line
	9050 1850 9350 1850
Connection ~ 8850 1950
Wire Wire Line
	8850 1950 9500 1950
Wire Notes Line
	8950 5700 7650 5700
Wire Notes Line
	8950 4100 7650 4100
Wire Notes Line
	8950 4100 8950 5700
Wire Wire Line
	8000 4900 8500 4900
Wire Wire Line
	8500 4800 8500 4900
Wire Notes Line
	7650 4100 7650 5700
Wire Wire Line
	8100 5050 7900 5050
Wire Wire Line
	8600 4800 8600 5050
Wire Wire Line
	8600 5050 8400 5050
Wire Notes Line
	10450 5700 9150 5700
Wire Notes Line
	10450 4100 9150 4100
Wire Notes Line
	10450 4100 10450 5700
Wire Wire Line
	9500 4900 10000 4900
Wire Notes Line
	9150 4100 9150 5700
Wire Wire Line
	9600 5050 9400 5050
Wire Wire Line
	10100 5050 9900 5050
Wire Wire Line
	10650 1100 10500 1100
Wire Wire Line
	10650 1950 10650 1100
Wire Wire Line
	9800 1100 10000 1100
Wire Wire Line
	9800 1100 9800 2050
Wire Wire Line
	10000 1850 10000 1950
Wire Wire Line
	10200 1850 10500 1850
Connection ~ 10000 1950
Wire Wire Line
	10000 1950 10650 1950
Wire Wire Line
	7500 7700 7500 9400
Wire Wire Line
	7400 7800 7400 9500
Wire Wire Line
	7300 7900 7300 9600
Wire Wire Line
	7200 8000 7200 9700
Wire Wire Line
	9050 9300 7600 9300
Wire Wire Line
	7600 9300 7600 7600
Wire Wire Line
	7600 7600 10200 7600
Wire Wire Line
	9050 9200 7700 9200
Wire Wire Line
	7700 9200 7700 7500
Wire Wire Line
	7700 7500 8700 7500
Wire Wire Line
	9050 9100 7800 9100
Wire Wire Line
	7800 9100 7800 7400
Wire Wire Line
	7800 7400 7200 7400
Wire Wire Line
	11900 5300 11900 6550
Wire Wire Line
	11900 1700 11900 2300
Wire Wire Line
	12300 1450 12300 1800
Wire Wire Line
	12900 1450 12900 1900
Wire Wire Line
	13500 1450 13500 2000
Wire Wire Line
	14100 1450 14100 2100
Wire Wire Line
	12300 2100 14100 2100
Connection ~ 12500 1700
Wire Wire Line
	12500 1700 13100 1700
Connection ~ 13100 1700
Wire Wire Line
	13100 1700 13700 1700
Wire Notes Line
	14300 850  14300 2750
Wire Notes Line
	11650 2750 14300 2750
Text Notes 7250 15750 0    50   ~ 0
Дополительное мигание стробоскопа\n(управляется контроллером):\n1 раз в сек. - скорость слишком медленная\n2 раз в сек. - скорость слишком большая\n4 раз в сек. - ошибка (если несколько ошибок - сделать разные мигания)
Text Notes 9000 10000 2    50   ~ 0
OUT_STROBE/ERROR
Text Notes 9000 10100 2    50   ~ 0
OUT_AUDIO_ALLOW (UNUSED)
Wire Wire Line
	9050 9800 7200 9800
Wire Wire Line
	9050 9900 7300 9900
Text Notes 9000 9200 2    50   ~ 0
IN_TABLE_ROTATE_PULSE
Text Notes 9000 9300 2    50   ~ 0
IN_AUTOSTOP
Text Notes 9000 9500 2    50   ~ 0
IN_BTN_AUTOSTOP
Text Notes 9000 9700 2    50   ~ 0
IN_BTN_STOP
Text Notes 9000 9900 2    50   ~ 0
OUT_LIFT
Connection ~ 11800 12450
Connection ~ 11800 13700
Connection ~ 11800 14200
Wire Wire Line
	11800 13700 12100 13700
$Comp
L Isolator:PC817 U1
U 1 1 5C265855
P 9400 14900
F 0 "U1" H 9400 15225 50  0000 C CNN
F 1 "PC817" H 9400 15134 50  0000 C CNN
F 2 "g602:PC817" H 9200 14700 50  0001 L CIN
F 3 "http://www.soselectronic.cz/a_info/resource/d/pc817.pdf" H 9400 14900 50  0001 L CNN
	1    9400 14900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 10000 7400 10000
Wire Wire Line
	7400 10000 7400 14800
Wire Wire Line
	11800 11900 11800 12000
Wire Wire Line
	11800 11900 12050 11900
Wire Wire Line
	9100 15000 8750 15000
Wire Wire Line
	10250 15850 10250 15450
Wire Wire Line
	11300 14800 11300 15950
Wire Wire Line
	11300 15950 2150 15950
Wire Wire Line
	9700 14800 11300 14800
Wire Notes Line
	1000 1500 1000 4750
Wire Notes Line
	1000 1500 5000 1500
Wire Notes Line
	1000 4750 5000 4750
Wire Wire Line
	2250 15850 10250 15850
Wire Wire Line
	6500 3450 6500 4900
Connection ~ 6500 3450
Wire Wire Line
	8000 3450 8000 4900
Wire Wire Line
	6500 3450 8000 3450
Connection ~ 8000 3450
Wire Wire Line
	8000 3450 9500 3450
Wire Wire Line
	9500 3450 9500 4900
Wire Wire Line
	6400 3650 6400 5050
Connection ~ 6400 3650
Wire Wire Line
	6400 3650 7900 3650
Wire Wire Line
	7900 3650 7900 5050
Connection ~ 7900 3650
Wire Wire Line
	7900 3650 9400 3650
Wire Wire Line
	9400 3650 9400 5050
$Comp
L Isolator:PC817 U2
U 1 1 5BEC372F
P 12850 6250
F 0 "U2" H 12650 6450 50  0000 L CNN
F 1 "PC817" H 12850 6450 50  0000 L CNN
F 2 "g602:PC817" H 12650 6050 50  0001 L CIN
F 3 "" H 12850 6250 50  0001 L CNN
	1    12850 6250
	0    1    1    0   
$EndComp
$Comp
L Device:R R19
U 1 1 5BEC3736
P 12950 5750
F 0 "R19" V 13030 5750 50  0000 C CNN
F 1 "510" V 12950 5750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 12880 5750 50  0001 C CNN
F 3 "" H 12950 5750 50  0000 C CNN
	1    12950 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	12600 5900 12750 5900
Wire Wire Line
	12750 5900 12750 5950
Text Notes 13000 7050 0    60   ~ 0
OUT
$Comp
L Device:R R20
U 1 1 5BEC3741
P 12950 6850
F 0 "R20" V 13030 6850 50  0000 C CNN
F 1 "1k" V 12950 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 12880 6850 50  0001 C CNN
F 3 "" H 12950 6850 50  0000 C CNN
	1    12950 6850
	-1   0    0    1   
$EndComp
Wire Wire Line
	12950 6550 12950 6650
Text Notes 12400 5550 0    60   ~ 0
GND
Wire Wire Line
	12750 6550 12750 6600
Wire Wire Line
	12750 6600 12600 6600
Wire Wire Line
	12600 6600 12600 5900
Wire Wire Line
	12950 5900 12950 5950
Text Notes 12750 5550 0    60   ~ 0
BTN
Wire Notes Line
	12400 5550 13300 5550
Wire Notes Line
	13300 7050 12400 7050
Wire Notes Line
	12400 5550 12400 7050
Wire Notes Line
	13300 5550 13300 7050
Wire Wire Line
	11900 5300 12600 5300
Wire Wire Line
	12600 5900 12600 5300
Connection ~ 12600 5900
Wire Wire Line
	12950 5200 12950 5600
Wire Wire Line
	12950 7000 12950 7700
$Comp
L Isolator:PC817 U3
U 1 1 5BF1A336
P 13950 6250
F 0 "U3" H 13750 6450 50  0000 L CNN
F 1 "PC817" H 13950 6450 50  0000 L CNN
F 2 "g602:PC817" H 13750 6050 50  0001 L CIN
F 3 "" H 13950 6250 50  0001 L CNN
	1    13950 6250
	0    1    1    0   
$EndComp
$Comp
L Device:R R22
U 1 1 5BF1A33D
P 14050 5750
F 0 "R22" V 14130 5750 50  0000 C CNN
F 1 "510" V 14050 5750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 13980 5750 50  0001 C CNN
F 3 "" H 14050 5750 50  0000 C CNN
	1    14050 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	13700 5900 13850 5900
Wire Wire Line
	13850 5900 13850 5950
Text Notes 14100 7050 0    60   ~ 0
OUT
$Comp
L Device:R R23
U 1 1 5BF1A348
P 14050 6850
F 0 "R23" V 14130 6850 50  0000 C CNN
F 1 "1k" V 14050 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 13980 6850 50  0001 C CNN
F 3 "" H 14050 6850 50  0000 C CNN
	1    14050 6850
	-1   0    0    1   
$EndComp
Wire Wire Line
	14050 6550 14050 6650
Text Notes 13500 5550 0    60   ~ 0
GND
Wire Wire Line
	13850 6550 13850 6600
Wire Wire Line
	13850 6600 13700 6600
Wire Wire Line
	13700 6600 13700 5900
Wire Wire Line
	14050 5900 14050 5950
Text Notes 13850 5550 0    60   ~ 0
BTN
Wire Notes Line
	13500 5550 14400 5550
Wire Notes Line
	14400 7050 13500 7050
Wire Notes Line
	13500 5550 13500 7050
Wire Notes Line
	14400 5550 14400 7050
$Comp
L Isolator:PC817 U4
U 1 1 5BF30C41
P 15050 6250
F 0 "U4" H 14850 6450 50  0000 L CNN
F 1 "PC817" H 15050 6450 50  0000 L CNN
F 2 "g602:PC817" H 14850 6050 50  0001 L CIN
F 3 "" H 15050 6250 50  0001 L CNN
	1    15050 6250
	0    1    1    0   
$EndComp
$Comp
L Device:R R25
U 1 1 5BF30C48
P 15150 5750
F 0 "R25" V 15230 5750 50  0000 C CNN
F 1 "510" V 15150 5750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 15080 5750 50  0001 C CNN
F 3 "" H 15150 5750 50  0000 C CNN
	1    15150 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	14800 5900 14950 5900
Wire Wire Line
	14950 5900 14950 5950
Text Notes 15200 7050 0    60   ~ 0
OUT
$Comp
L Device:R R26
U 1 1 5BF30C53
P 15150 6850
F 0 "R26" V 15230 6850 50  0000 C CNN
F 1 "1k" V 15150 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 15080 6850 50  0001 C CNN
F 3 "" H 15150 6850 50  0000 C CNN
	1    15150 6850
	-1   0    0    1   
$EndComp
Wire Wire Line
	15150 6550 15150 6650
Text Notes 14600 5550 0    60   ~ 0
GND
Wire Wire Line
	14950 6550 14950 6600
Wire Wire Line
	14950 6600 14800 6600
Wire Wire Line
	14800 6600 14800 5900
Connection ~ 14800 5900
Wire Wire Line
	15150 5900 15150 5950
Text Notes 14950 5550 0    60   ~ 0
BTN
Wire Notes Line
	14600 5550 15500 5550
Wire Notes Line
	15500 7050 14600 7050
Wire Notes Line
	14600 5550 14600 7050
Wire Notes Line
	15500 5550 15500 7050
Wire Wire Line
	14800 5300 14800 5900
$Comp
L Isolator:PC817 U5
U 1 1 5BF61A36
P 16150 6250
F 0 "U5" H 15950 6450 50  0000 L CNN
F 1 "PC817" H 16150 6450 50  0000 L CNN
F 2 "g602:PC817" H 15950 6050 50  0001 L CIN
F 3 "" H 16150 6250 50  0001 L CNN
	1    16150 6250
	0    1    1    0   
$EndComp
$Comp
L Device:R R28
U 1 1 5BF61A3D
P 16250 5750
F 0 "R28" V 16330 5750 50  0000 C CNN
F 1 "510" V 16250 5750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 16180 5750 50  0001 C CNN
F 3 "" H 16250 5750 50  0000 C CNN
	1    16250 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	15900 5900 16050 5900
Wire Wire Line
	16050 5900 16050 5950
Text Notes 16300 7050 0    60   ~ 0
OUT
Wire Wire Line
	16250 6550 16250 6650
Text Notes 15700 5550 0    60   ~ 0
GND
Wire Wire Line
	16050 6550 16050 6600
Wire Wire Line
	16050 6600 15900 6600
Wire Wire Line
	15900 6600 15900 5900
Connection ~ 15900 5900
Wire Wire Line
	16250 5900 16250 5950
Text Notes 16050 5550 0    60   ~ 0
BTN
Wire Notes Line
	15700 5550 16600 5550
Wire Notes Line
	15700 5550 15700 7050
Wire Notes Line
	16600 5550 16600 7050
Wire Wire Line
	15900 5300 15900 5900
Wire Wire Line
	14050 7000 14050 7800
Wire Wire Line
	15150 7000 15150 7900
Wire Wire Line
	7200 8000 16250 8000
Wire Wire Line
	16250 7000 16250 8000
Wire Wire Line
	15150 5000 15150 5600
Wire Wire Line
	16250 4900 16250 5600
Connection ~ 12600 5300
Wire Wire Line
	12100 5100 14050 5100
Wire Wire Line
	14050 5600 14050 5100
Connection ~ 14800 5300
Wire Wire Line
	14800 5300 15900 5300
Wire Wire Line
	12600 5300 13700 5300
Wire Wire Line
	13700 5900 13700 5300
Connection ~ 13700 5900
Connection ~ 13700 5300
Wire Wire Line
	13700 5300 14800 5300
Wire Wire Line
	11950 8800 11950 9900
$Comp
L power:GNDREF #PWR07
U 1 1 5BA307BF
P 11950 10150
F 0 "#PWR07" H 11950 9900 50  0001 C CNN
F 1 "GNDREF" H 11950 10000 50  0000 C CNN
F 2 "" H 11950 10150 50  0001 C CNN
F 3 "" H 11950 10150 50  0001 C CNN
	1    11950 10150
	1    0    0    -1  
$EndComp
Wire Wire Line
	10150 8800 11950 8800
Connection ~ 12050 8700
Wire Wire Line
	12050 8700 12050 8200
Wire Wire Line
	10150 8700 12050 8700
Text Notes 14300 9100 0    50   ~ 0
10кОм - рекомендуется?????\n100кОм - мой\n
Wire Wire Line
	7800 1850 7800 2150
Wire Wire Line
	7700 2050 7700 2150
Wire Wire Line
	8950 2050 8950 2150
Wire Wire Line
	9050 1850 9050 2150
Wire Wire Line
	7200 4800 7200 5200
Wire Wire Line
	7200 5600 7200 7400
Wire Wire Line
	8700 5300 8700 5200
Wire Wire Line
	8700 5600 8700 7500
Wire Wire Line
	10200 5550 10200 7600
$Comp
L Device:R_Small R16
U 1 1 5D714BA1
P 11150 11900
F 0 "R16" H 11180 11920 50  0000 L CNN
F 1 "10" H 11180 11860 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" H 11150 11900 50  0001 C CNN
F 3 "" H 11150 11900 50  0001 C CNN
	1    11150 11900
	0    -1   -1   0   
$EndComp
$Comp
L g602:IRL2203NPBF Q1
U 1 1 5BA31B5A
P 10700 11900
F 0 "Q1" V 10950 11850 50  0000 L CNN
F 1 "IRL2203NPBF" V 10850 11650 50  0000 L CNN
F 2 "g602:IRL2203NPBF" H 10950 11825 50  0001 L CIN
F 3 "" H 10700 11900 50  0001 L CNN
	1    10700 11900
	0    1    1    0   
$EndComp
Wire Wire Line
	10700 11350 10700 11450
$Comp
L Device:R_Small R12
U 1 1 5D9430B6
P 10400 11450
F 0 "R12" H 10430 11470 50  0000 L CNN
F 1 "50k" H 10430 11410 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" H 10400 11450 50  0001 C CNN
F 3 "" H 10400 11450 50  0001 C CNN
	1    10400 11450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10500 11450 10700 11450
Connection ~ 10700 11450
Wire Wire Line
	10300 11450 10200 11450
$Comp
L Device:R_Small R17
U 1 1 5DA31D27
P 11200 13700
F 0 "R17" H 11230 13720 50  0000 L CNN
F 1 "200" H 11230 13660 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" H 11200 13700 50  0001 C CNN
F 3 "" H 11200 13700 50  0001 C CNN
	1    11200 13700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10900 13700 11100 13700
$Comp
L Device:R_Small R15
U 1 1 5DAD4711
P 10700 13050
F 0 "R15" H 10750 13000 50  0000 L CNN
F 1 "10" H 10750 13100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" H 10700 13050 50  0001 C CNN
F 3 "" H 10700 13050 50  0001 C CNN
	1    10700 13050
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R13
U 1 1 5DB1B0CE
P 10400 13300
F 0 "R13" H 10430 13320 50  0000 L CNN
F 1 "50k" H 10430 13260 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" H 10400 13300 50  0001 C CNN
F 3 "" H 10400 13300 50  0001 C CNN
	1    10400 13300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10700 13150 10700 13300
Wire Wire Line
	10500 13300 10700 13300
Connection ~ 10700 13300
Wire Wire Line
	10700 13300 10700 13400
Wire Wire Line
	10300 13300 10200 13300
$Comp
L power:GNDREF #PWR04
U 1 1 5E2B8AC1
P 10200 12050
F 0 "#PWR04" H 10200 11800 50  0001 C CNN
F 1 "GNDREF" H 10205 11877 50  0000 C CNN
F 2 "" H 10200 12050 50  0001 C CNN
F 3 "" H 10200 12050 50  0001 C CNN
	1    10200 12050
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR05
U 1 1 5E2D1207
P 10200 13850
F 0 "#PWR05" H 10200 13600 50  0001 C CNN
F 1 "GNDREF" H 10205 13677 50  0000 C CNN
F 2 "" H 10200 13850 50  0001 C CNN
F 3 "" H 10200 13850 50  0001 C CNN
	1    10200 13850
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR03
U 1 1 5E301421
P 8750 15100
F 0 "#PWR03" H 8750 14850 50  0001 C CNN
F 1 "GNDREF" H 8755 14927 50  0000 C CNN
F 2 "" H 8750 15100 50  0001 C CNN
F 3 "" H 8750 15100 50  0001 C CNN
	1    8750 15100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 15100 8750 15000
$Comp
L power:GNDREF #PWR01
U 1 1 5E31A5F0
P 5750 3950
F 0 "#PWR01" H 5750 3700 50  0001 C CNN
F 1 "GNDREF" H 5755 3777 50  0000 C CNN
F 2 "" H 5750 3950 50  0001 C CNN
F 3 "" H 5750 3950 50  0001 C CNN
	1    5750 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 3950 5750 3450
Connection ~ 5750 3450
Wire Wire Line
	5750 3450 6500 3450
Text Notes 4750 3650 0    60   ~ 0
+5V
Wire Wire Line
	5150 4150 5150 14200
Wire Wire Line
	12050 8200 9950 8200
Connection ~ 5250 8200
Connection ~ 5250 3650
Wire Wire Line
	5250 3650 5250 8200
Wire Wire Line
	11300 13700 11800 13700
Wire Wire Line
	10700 11450 10700 11600
Connection ~ 11800 11900
Wire Wire Line
	5250 8200 5250 12450
Wire Wire Line
	10700 11150 10700 11050
Wire Wire Line
	10700 11050 7200 11050
Wire Wire Line
	7200 11050 7200 9800
Wire Wire Line
	7300 9900 7300 12850
Wire Wire Line
	7300 12850 10700 12850
Wire Wire Line
	10700 12850 10700 12950
$Comp
L Diode_Bridge:DF01M D2
U 1 1 5D797B67
P 2650 2500
F 0 "D2" V 2604 2841 50  0000 L CNN
F 1 "DF01M" V 2695 2841 50  0000 L CNN
F 2 "g602:DF01M" H 2800 2625 50  0001 L CNN
F 3 "" H 2650 2500 50  0001 C CNN
	1    2650 2500
	0    1    1    0   
$EndComp
Wire Wire Line
	2350 2500 2350 1750
Wire Wire Line
	2650 2200 3750 2200
Wire Wire Line
	2650 2800 3250 2800
$Comp
L g602:DC-DC PS1
U 1 1 5D9D3BDE
P 4250 3550
F 0 "PS1" H 4250 3235 50  0000 C CNN
F 1 "DC-DC" H 4250 3326 50  0000 C CNN
F 2 "g602:DC-DC-converter" H 4250 3550 50  0001 C CNN
F 3 "" H 4250 3550 50  0001 C CNN
	1    4250 3550
	1    0    0    1   
$EndComp
Wire Wire Line
	4600 3650 5250 3650
Wire Wire Line
	3250 2800 3250 3150
Wire Wire Line
	3900 3450 3750 3450
Wire Wire Line
	3900 3650 3250 3650
Connection ~ 3250 3650
Wire Wire Line
	3250 3650 3250 4150
Wire Wire Line
	3750 2200 3750 3150
Wire Wire Line
	4600 3450 5750 3450
$Comp
L Device:CP C1
U 1 1 5DB1F298
P 3500 3150
F 0 "C1" V 3755 3150 50  0000 C CNN
F 1 "2500uF" V 3664 3150 50  0000 C CNN
F 2 "Capacitor_THT:C_Radial_D10.0mm_H12.5mm_P5.00mm" H 3538 3000 50  0001 C CNN
F 3 "~" H 3500 3150 50  0001 C CNN
	1    3500 3150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3650 3150 3750 3150
Connection ~ 3750 3150
Wire Wire Line
	3750 3150 3750 3450
Wire Wire Line
	3350 3150 3250 3150
Connection ~ 3250 3150
Wire Wire Line
	3250 3150 3250 3650
Wire Wire Line
	3250 4150 5150 4150
Text Notes 4650 4150 0    60   ~ 0
+23.5V
$Comp
L Connector:Conn_01x02_Male XP6
U 1 1 5E952123
P 12400 12350
F 0 "XP6" H 12400 12450 50  0000 C CNN
F 1 "Motor" H 12500 12200 50  0000 C CNN
F 2 "g602:Connector_01x02" H 12400 12350 50  0001 C CNN
F 3 "" H 12400 12350 50  0001 C CNN
	1    12400 12350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	11800 12450 12200 12450
Wire Wire Line
	12200 12350 12050 12350
Wire Wire Line
	12050 11900 12050 12350
$Comp
L Connector:Conn_01x02_Male XP7
U 1 1 5EA0E029
P 12450 14100
F 0 "XP7" H 12450 14150 50  0000 C CNN
F 1 "Microlift" H 12450 13950 50  0000 C CNN
F 2 "g602:Connector_01x02" H 12450 14100 50  0001 C CNN
F 3 "" H 12450 14100 50  0001 C CNN
	1    12450 14100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	11800 14200 12250 14200
Wire Wire Line
	12250 14100 12100 14100
Wire Wire Line
	12100 13700 12100 14100
$Comp
L Device:R R1
U 1 1 5BA3E025
P 6750 5050
F 0 "R1" V 6830 5050 50  0000 C CNN
F 1 "510" V 6750 5050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 6680 5050 50  0001 C CNN
F 3 "" H 6750 5050 50  0001 C CNN
	1    6750 5050
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Female XS7
U 1 1 5D8AFC72
P 13200 14100
F 0 "XS7" H 13200 14150 50  0000 C CNN
F 1 "Microlift" H 13200 13950 50  0000 C CNN
F 2 "" H 13200 14100 50  0001 C CNN
F 3 "" H 13200 14100 50  0001 C CNN
	1    13200 14100
	-1   0    0    -1  
$EndComp
$Comp
L Motor:Motor_DC M1
U 1 1 5D8B09CE
P 13350 12350
F 0 "M1" V 13050 12250 50  0000 L CNN
F 1 "Motor_DC_PRM-33-1,9" V 13150 12250 50  0000 L CNN
F 2 "" H 13350 12260 50  0001 C CNN
F 3 "~" H 13350 12260 50  0001 C CNN
	1    13350 12350
	0    1    1    0   
$EndComp
Wire Wire Line
	12900 12350 13050 12350
$Comp
L Device:Electromagnetic_Actor L1
U 1 1 5D8F0DD5
P 13900 14300
F 0 "L1" H 14030 14396 50  0000 L CNN
F 1 "Electromagnetic_microlift 15V" H 14030 14305 50  0000 L CNN
F 2 "" V 13875 14400 50  0001 C CNN
F 3 "~" V 13875 14400 50  0001 C CNN
	1    13900 14300
	1    0    0    -1  
$EndComp
Wire Wire Line
	13400 14100 13900 14100
Wire Wire Line
	13400 14200 13650 14200
Wire Wire Line
	13650 14200 13650 14400
Wire Wire Line
	13650 14400 13900 14400
$Comp
L Connector:Conn_01x03_Female XS8
U 1 1 5D83E33C
P 13850 9800
F 0 "XS8" H 13850 9600 50  0000 C CNN
F 1 "Manual_speed_adjust" H 13850 10000 50  0000 C CNN
F 2 "" H 13850 9800 50  0001 C CNN
F 3 "" H 13850 9800 50  0001 C CNN
	1    13850 9800
	-1   0    0    1   
$EndComp
Wire Wire Line
	14950 9900 14050 9900
Wire Wire Line
	14700 9600 14700 9800
Wire Wire Line
	14700 9800 14050 9800
Wire Wire Line
	12050 8700 12050 9700
Wire Wire Line
	14050 9700 14450 9700
$Comp
L Device:R_POT_Dual_Separate RV1
U 1 1 5C0F76C0
P 14700 9450
F 0 "RV1" V 14493 9450 50  0000 C CNN
F 1 "R_POT_Dual_Separate" V 14584 9450 50  0000 C CNN
F 2 "" H 14700 9450 50  0001 C CNN
F 3 "~" H 14700 9450 50  0001 C CNN
	1    14700 9450
	0    1    1    0   
$EndComp
Wire Wire Line
	12700 9900 11950 9900
Wire Wire Line
	12050 9700 12700 9700
$Comp
L Connector:Conn_01x03_Male XP8
U 1 1 5D8384F3
P 12900 9800
F 0 "XP8" H 12900 9600 50  0000 C CNN
F 1 "Manual_speed_adjust" H 12900 10000 50  0000 C CNN
F 2 "g602:Connector_01x03" H 12900 9800 50  0001 C CNN
F 3 "" H 12900 9800 50  0001 C CNN
	1    12900 9800
	-1   0    0    1   
$EndComp
Wire Wire Line
	10150 9800 12700 9800
Connection ~ 11950 9900
Wire Wire Line
	11950 9900 11950 10150
Wire Wire Line
	14850 9450 14950 9450
Wire Wire Line
	14950 9450 14950 9900
Wire Wire Line
	14550 9450 14450 9450
Wire Wire Line
	14450 9450 14450 9700
Wire Wire Line
	7600 1950 7600 2150
Wire Wire Line
	7700 2050 7400 2050
Wire Wire Line
	8850 1950 8850 2150
Wire Wire Line
	8950 2050 8650 2050
Wire Wire Line
	10100 2050 9800 2050
$Comp
L Connector:Conn_01x02_Male XP1
U 1 1 5DD539FB
P 750 1750
F 0 "XP1" H 750 1850 50  0000 C CNN
F 1 "~~ 50Hz, 17V" H 750 1550 50  0000 C CNN
F 2 "g602:Connector_01x02" H 750 1750 50  0001 C CNN
F 3 "" H 750 1750 50  0001 C CNN
	1    750  1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 2500 2950 1850
Wire Wire Line
	9700 15000 9900 15000
Wire Wire Line
	10100 15000 10250 15000
Wire Wire Line
	7400 14800 8650 14800
Wire Wire Line
	8850 14800 9100 14800
$Comp
L Connector:Conn_01x02_Male XP4
U 1 1 5DE9ED6C
P 10850 15450
F 0 "XP4" H 10850 15550 50  0000 C CNN
F 1 "Strobe" H 10850 15300 50  0000 C CNN
F 2 "g602:Connector_01x02" H 10850 15450 50  0001 C CNN
F 3 "" H 10850 15450 50  0001 C CNN
	1    10850 15450
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female XS4
U 1 1 5DE9F144
P 12400 15450
F 0 "XS4" H 12400 15550 50  0000 C CNN
F 1 "Strobe" H 12400 15300 50  0000 C CNN
F 2 "" H 12400 15450 50  0001 C CNN
F 3 "" H 12400 15450 50  0001 C CNN
	1    12400 15450
	-1   0    0    1   
$EndComp
Wire Wire Line
	10650 15450 10250 15450
Wire Wire Line
	10250 15000 10250 15350
Wire Wire Line
	10250 15350 10650 15350
Wire Wire Line
	13050 15200 13050 15350
Wire Wire Line
	13050 15350 12600 15350
Wire Wire Line
	13400 15200 13550 15200
Wire Wire Line
	13550 15200 13550 15450
Wire Wire Line
	13550 15450 12600 15450
Wire Wire Line
	13050 15200 13200 15200
$Comp
L Device:D D3
U 1 1 5BA35653
P 11800 12150
F 0 "D3" H 11800 12250 50  0000 C CNN
F 1 "D" H 11800 12050 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P10.16mm_Horizontal" H 11800 12150 50  0001 C CNN
F 3 "" H 11800 12150 50  0001 C CNN
	1    11800 12150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	11500 12050 11500 11900
Connection ~ 11500 11900
Wire Wire Line
	11500 11900 11800 11900
Wire Wire Line
	10000 1950 10000 4900
Wire Wire Line
	10100 2050 10100 5050
Wire Wire Line
	10200 1850 10200 5150
Wire Wire Line
	11500 12350 11500 12450
Connection ~ 11500 12450
Wire Wire Line
	11500 12450 11800 12450
Wire Wire Line
	11050 11900 10900 11900
Wire Wire Line
	11250 11900 11500 11900
Text Notes 3800 3050 0    60   ~ 0
Конденсатор С1 необязателен, \nпоскольку он присутствует\nна DC-DC-конвертере
Text Notes 4100 3600 0    50   ~ 0
XL4015
Wire Wire Line
	10200 11450 10200 11900
Wire Wire Line
	10500 11900 10200 11900
Connection ~ 10200 11900
Wire Wire Line
	10200 11900 10200 12050
Wire Wire Line
	5250 12450 11500 12450
$Comp
L Connector:Conn_01x02_Female XS6
U 1 1 5D88BF50
P 12700 12350
F 0 "XS6" H 12700 12450 50  0000 C CNN
F 1 "Motor" H 12700 12150 50  0000 C CNN
F 2 "" H 12700 12350 50  0001 C CNN
F 3 "" H 12700 12350 50  0001 C CNN
	1    12700 12350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	12900 12450 12900 12600
Wire Wire Line
	13550 12350 13650 12350
Wire Wire Line
	13650 12350 13650 12600
Wire Wire Line
	12900 12600 13650 12600
Connection ~ 10200 13700
Wire Wire Line
	10200 13700 10200 13300
Wire Wire Line
	10200 13700 10200 13850
Wire Wire Line
	10200 13700 10500 13700
Wire Wire Line
	5150 14200 11800 14200
$Comp
L Device:R R29
U 1 1 5BF61A48
P 16250 6850
F 0 "R29" V 16330 6850 50  0000 C CNN
F 1 "1k" V 16250 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 16180 6850 50  0001 C CNN
F 3 "" H 16250 6850 50  0000 C CNN
	1    16250 6850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R27
U 1 1 5D8EDE31
P 16050 6850
F 0 "R27" V 16130 6850 50  0000 C CNN
F 1 "10k" V 16050 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 15980 6850 50  0001 C CNN
F 3 "" H 16050 6850 50  0000 C CNN
	1    16050 6850
	-1   0    0    1   
$EndComp
Wire Wire Line
	16250 6700 16250 6650
Wire Wire Line
	16250 6650 16050 6650
Wire Wire Line
	16050 6650 16050 6700
Connection ~ 12050 8200
Wire Wire Line
	16050 8200 16050 7000
Wire Notes Line
	16600 7050 15700 7050
Connection ~ 16250 6650
Wire Wire Line
	12050 8200 12750 8200
$Comp
L Device:R R24
U 1 1 5DC0ABB0
P 14950 6850
F 0 "R24" V 15030 6850 50  0000 C CNN
F 1 "10k" V 14950 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 14880 6850 50  0001 C CNN
F 3 "" H 14950 6850 50  0000 C CNN
	1    14950 6850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R21
U 1 1 5DC0AF63
P 13850 6850
F 0 "R21" V 13930 6850 50  0000 C CNN
F 1 "10k" V 13850 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 13780 6850 50  0001 C CNN
F 3 "" H 13850 6850 50  0000 C CNN
	1    13850 6850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R18
U 1 1 5DC0B2AF
P 12750 6850
F 0 "R18" V 12830 6850 50  0000 C CNN
F 1 "10k" V 12750 6850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 12680 6850 50  0001 C CNN
F 3 "" H 12750 6850 50  0000 C CNN
	1    12750 6850
	-1   0    0    1   
$EndComp
Wire Wire Line
	12750 6700 12750 6650
Wire Wire Line
	12750 6650 12950 6650
Connection ~ 12950 6650
Wire Wire Line
	12950 6650 12950 6700
Wire Wire Line
	13850 6700 13850 6650
Wire Wire Line
	13850 6650 14050 6650
Connection ~ 14050 6650
Wire Wire Line
	14050 6650 14050 6700
Wire Wire Line
	14950 6700 14950 6650
Wire Wire Line
	14950 6650 15150 6650
Connection ~ 15150 6650
Wire Wire Line
	15150 6650 15150 6700
Wire Wire Line
	14950 7000 14950 8200
Connection ~ 14950 8200
Wire Wire Line
	14950 8200 16050 8200
Wire Wire Line
	13850 7000 13850 8200
Connection ~ 13850 8200
Wire Wire Line
	13850 8200 14950 8200
Wire Wire Line
	12750 7000 12750 8200
Connection ~ 12750 8200
Wire Wire Line
	12750 8200 13850 8200
$Comp
L Device:R R9
U 1 1 5DCF1AE1
P 9950 5400
F 0 "R9" V 10030 5400 50  0000 C CNN
F 1 "10k" V 9950 5400 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 9880 5400 50  0001 C CNN
F 3 "" H 9950 5400 50  0000 C CNN
	1    9950 5400
	-1   0    0    1   
$EndComp
Wire Wire Line
	9950 5250 9950 5150
Wire Wire Line
	9950 5150 10200 5150
Connection ~ 10200 5150
Wire Wire Line
	10200 5150 10200 5250
Wire Wire Line
	9950 5550 9950 8200
Connection ~ 9950 8200
Wire Wire Line
	9950 8200 8500 8200
$Comp
L Device:R R5
U 1 1 5DF0DAC4
P 8500 5450
F 0 "R5" V 8580 5450 50  0000 C CNN
F 1 "10k" V 8500 5450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 8430 5450 50  0001 C CNN
F 3 "" H 8500 5450 50  0000 C CNN
	1    8500 5450
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 5DF0E3B3
P 7000 5450
F 0 "R2" V 7080 5450 50  0000 C CNN
F 1 "10k" V 7000 5450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 6930 5450 50  0001 C CNN
F 3 "" H 7000 5450 50  0000 C CNN
	1    7000 5450
	-1   0    0    1   
$EndComp
Wire Wire Line
	8500 5300 8500 5200
Wire Wire Line
	8500 5200 8700 5200
Connection ~ 8700 5200
Wire Wire Line
	8700 5200 8700 4800
Wire Wire Line
	7000 5300 7000 5200
Wire Wire Line
	7000 5200 7200 5200
Connection ~ 7200 5200
Wire Wire Line
	7200 5200 7200 5300
Wire Wire Line
	8500 5600 8500 8200
Connection ~ 8500 8200
Wire Wire Line
	8500 8200 7000 8200
Wire Wire Line
	7000 5600 7000 8200
Connection ~ 7000 8200
Wire Wire Line
	7000 8200 5250 8200
$Comp
L Device:D D1
U 1 1 5D9B2598
P 2150 2500
F 0 "D1" V 2200 2650 50  0000 R CNN
F 1 "D" V 2100 2650 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P10.16mm_Horizontal" H 2150 2500 50  0001 C CNN
F 3 "~" H 2150 2500 50  0001 C CNN
	1    2150 2500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2150 2650 2150 15950
Connection ~ 2250 1850
Wire Wire Line
	2250 1850 2950 1850
Wire Wire Line
	2250 1850 2250 15850
Wire Wire Line
	950  1850 2250 1850
Wire Wire Line
	2150 2350 2150 1750
Connection ~ 2150 1750
Wire Wire Line
	2150 1750 2350 1750
Wire Wire Line
	950  1750 2150 1750
Wire Wire Line
	9400 3650 11250 3650
Wire Wire Line
	11250 3650 11250 5200
Wire Wire Line
	11250 5200 11900 5200
Wire Wire Line
	11900 5200 11900 4700
Connection ~ 9400 3650
Text Notes 1200 4700 0    60   ~ 0
pulse 0 - +16.5 V
$EndSCHEMATC