EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:atecc508a
LIBS:mcp73831
LIBS:badge2017-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCP73831 U?
U 1 1 58651D43
P 5850 3700
F 0 "U?" H 5650 3900 60  0000 C CNN
F 1 "MCP73831" H 5850 3500 60  0000 C CNN
F 2 "" H 5850 3700 60  0000 C CNN
F 3 "" H 5850 3700 60  0000 C CNN
	1    5850 3700
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 58651DAE
P 5000 3800
F 0 "C?" H 5025 3900 50  0000 L CNN
F 1 "4.7uF" H 5025 3700 50  0000 L CNN
F 2 "" H 5038 3650 50  0000 C CNN
F 3 "" H 5000 3800 50  0000 C CNN
	1    5000 3800
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 58651DEB
P 6700 3800
F 0 "C?" H 6725 3900 50  0000 L CNN
F 1 "4.7uF" H 6725 3700 50  0000 L CNN
F 2 "" H 6738 3650 50  0000 C CNN
F 3 "" H 6700 3800 50  0000 C CNN
	1    6700 3800
	1    0    0    -1  
$EndComp
Text HLabel 4850 3600 0    60   Input ~ 0
VUSB
Text HLabel 5450 3350 2    60   Output ~ 0
STAT
$Comp
L CONN_01X02 P?
U 1 1 58651EF5
P 7250 3800
F 0 "P?" H 7250 3950 50  0000 C CNN
F 1 "CONN_01X02" V 7350 3800 50  0000 C CNN
F 2 "" H 7250 3800 50  0000 C CNN
F 3 "" H 7250 3800 50  0000 C CNN
	1    7250 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 58679354
P 6400 4050
F 0 "#PWR?" H 6400 3800 50  0001 C CNN
F 1 "GND" H 6400 3900 50  0000 C CNN
F 2 "" H 6400 4050 50  0000 C CNN
F 3 "" H 6400 4050 50  0000 C CNN
	1    6400 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 3600 5400 3600
Wire Wire Line
	5000 3650 5000 3600
Connection ~ 5000 3600
Wire Wire Line
	5400 3800 5250 3800
Wire Wire Line
	5250 3800 5250 3350
Wire Wire Line
	5250 3350 5450 3350
Wire Wire Line
	6300 3800 6400 3800
Wire Wire Line
	6400 3800 6400 4050
Wire Wire Line
	6700 4000 6700 3950
Wire Wire Line
	5000 4000 7050 4000
Connection ~ 6400 4000
Wire Wire Line
	5000 4000 5000 3950
Wire Wire Line
	6700 3400 6700 3650
Wire Wire Line
	6300 3600 7050 3600
Connection ~ 6700 3600
Connection ~ 6700 4000
Wire Wire Line
	7050 3600 7050 3750
Wire Wire Line
	7050 4000 7050 3850
Text HLabel 6800 3400 2    60   Output ~ 0
VBAT
Wire Wire Line
	6700 3400 6800 3400
$EndSCHEMATC
