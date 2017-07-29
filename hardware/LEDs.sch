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
LIBS:ESP32-footprints-Shem-Lib
LIBS:mic23050
LIBS:siliconlabs
LIBS:erc13265-1
LIBS:sp0503bahtg
LIBS:mic2288
LIBS:apa102
LIBS:74hct245
LIBS:pam2301
LIBS:erc12864-1
LIBS:mma8452q
LIBS:max9723
LIBS:trrs
LIBS:max4468
LIBS:badge2017-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title "Crypto and Privacy Village Badge 2017"
Date "2017-06-21"
Rev "C"
Comp "Karl Koscher"
Comment1 "https://cryptovillage.org"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L APA102 LED1
U 1 1 586B6765
P 3900 2500
F 0 "LED1" H 3900 2750 60  0000 C CNN
F 1 "APA102" H 3900 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 3900 2500 60  0001 C CNN
F 3 "" H 3900 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    3900 2500
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED2
U 1 1 586B676C
P 4850 2500
F 0 "LED2" H 4850 2750 60  0000 C CNN
F 1 "APA102" H 4850 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 4850 2500 60  0001 C CNN
F 3 "" H 4850 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    4850 2500
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED3
U 1 1 586B6773
P 5800 2500
F 0 "LED3" H 5800 2750 60  0000 C CNN
F 1 "APA102" H 5800 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 5800 2500 60  0001 C CNN
F 3 "" H 5800 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    5800 2500
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED4
U 1 1 586B677A
P 6750 2500
F 0 "LED4" H 6750 2750 60  0000 C CNN
F 1 "APA102" H 6750 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 6750 2500 60  0001 C CNN
F 3 "" H 6750 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6750 2500
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED5
U 1 1 586B6781
P 7700 2500
F 0 "LED5" H 7700 2750 60  0000 C CNN
F 1 "APA102" H 7700 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 7700 2500 60  0001 C CNN
F 3 "" H 7700 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    7700 2500
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED6
U 1 1 586B6788
P 8650 2500
F 0 "LED6" H 8650 2750 60  0000 C CNN
F 1 "APA102" H 8650 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 8650 2500 60  0001 C CNN
F 3 "" H 8650 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    8650 2500
	1    0    0    -1  
$EndComp
$Comp
L C C26
U 1 1 586B67AE
P 3900 2900
F 0 "C26" H 3925 3000 50  0000 L CNN
F 1 "1uF" H 3925 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3938 2750 50  0001 C CNN
F 3 "" H 3900 2900 50  0000 C CNN
F 4 "Capacitor" H 3900 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    3900 2900
	0    1    1    0   
$EndComp
$Comp
L C C27
U 1 1 586B67B5
P 4850 2900
F 0 "C27" H 4875 3000 50  0000 L CNN
F 1 "1uF" H 4875 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4888 2750 50  0001 C CNN
F 3 "" H 4850 2900 50  0000 C CNN
F 4 "Capacitor" H 4850 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    4850 2900
	0    1    1    0   
$EndComp
$Comp
L C C28
U 1 1 586B67BC
P 5800 2900
F 0 "C28" H 5825 3000 50  0000 L CNN
F 1 "1uF" H 5825 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5838 2750 50  0001 C CNN
F 3 "" H 5800 2900 50  0000 C CNN
F 4 "Capacitor" H 5800 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    5800 2900
	0    1    1    0   
$EndComp
$Comp
L C C29
U 1 1 586B67C3
P 6750 2900
F 0 "C29" H 6775 3000 50  0000 L CNN
F 1 "1uF" H 6775 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6788 2750 50  0001 C CNN
F 3 "" H 6750 2900 50  0000 C CNN
F 4 "Capacitor" H 6750 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    6750 2900
	0    1    1    0   
$EndComp
$Comp
L C C30
U 1 1 586B67CA
P 7700 2900
F 0 "C30" H 7725 3000 50  0000 L CNN
F 1 "1uF" H 7725 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 7738 2750 50  0001 C CNN
F 3 "" H 7700 2900 50  0000 C CNN
F 4 "Capacitor" H 7700 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    7700 2900
	0    1    1    0   
$EndComp
$Comp
L C C31
U 1 1 586B67D1
P 8650 2900
F 0 "C31" H 8675 3000 50  0000 L CNN
F 1 "1uF" H 8675 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8688 2750 50  0001 C CNN
F 3 "" H 8650 2900 50  0000 C CNN
F 4 "Capacitor" H 8650 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    8650 2900
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR46
U 1 1 586B67F4
P 11100 2700
F 0 "#PWR46" H 11100 2550 50  0001 C CNN
F 1 "+5V" H 11100 2840 50  0000 C CNN
F 2 "" H 11100 2700 50  0000 C CNN
F 3 "" H 11100 2700 50  0000 C CNN
	1    11100 2700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR45
U 1 1 586B67FE
P 3500 3450
F 0 "#PWR45" H 3500 3200 50  0001 C CNN
F 1 "GND" H 3500 3300 50  0000 C CNN
F 2 "" H 3500 3450 50  0000 C CNN
F 3 "" H 3500 3450 50  0000 C CNN
	1    3500 3450
	1    0    0    -1  
$EndComp
$Comp
L 74HCT245 U8
U 1 1 586B6812
P 2300 2850
F 0 "U8" H 2400 3425 50  0000 L BNN
F 1 "74HCT245" H 2350 2275 50  0000 L TNN
F 2 "Housings_SSOP:TSSOP-20_4.4x6.5mm_Pitch0.65mm" H 2300 2850 50  0001 C CNN
F 3 "" H 2300 2850 50  0000 C CNN
F 4 "Level Shifter" H 0   0   50  0001 C CNN "Description"
F 5 "NXP USA Inc." H 0   0   50  0001 C CNN "Manufacturer"
F 6 "74HCT245PW,118" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "20" H 0   0   50  0001 C CNN "NumPads"
F 8 "TSSOP20" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "568-8174-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    2300 2850
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR44
U 1 1 586B6DB2
P 2300 3750
F 0 "#PWR44" H 2300 3500 50  0001 C CNN
F 1 "GND" H 2300 3600 50  0000 C CNN
F 2 "" H 2300 3750 50  0000 C CNN
F 3 "" H 2300 3750 50  0000 C CNN
	1    2300 3750
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR43
U 1 1 586B6DE0
P 2300 1900
F 0 "#PWR43" H 2300 1750 50  0001 C CNN
F 1 "+5V" H 2300 2040 50  0000 C CNN
F 2 "" H 2300 1900 50  0000 C CNN
F 3 "" H 2300 1900 50  0000 C CNN
	1    2300 1900
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED7
U 1 1 5899CA4A
P 9600 2500
F 0 "LED7" H 9600 2750 60  0000 C CNN
F 1 "APA102" H 9600 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 9600 2500 60  0001 C CNN
F 3 "" H 9600 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    9600 2500
	1    0    0    -1  
$EndComp
$Comp
L APA102 LED8
U 1 1 5899CA9A
P 10550 2500
F 0 "LED8" H 10550 2750 60  0000 C CNN
F 1 "APA102" H 10550 2250 60  0000 C CNN
F 2 "lib:APA102-2020" H 10550 2500 60  0001 C CNN
F 3 "" H 10550 2500 60  0001 C CNN
F 4 "RGB LED" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    10550 2500
	1    0    0    -1  
$EndComp
$Comp
L C C43
U 1 1 5899CE9E
P 9600 2900
F 0 "C43" H 9625 3000 50  0000 L CNN
F 1 "1uF" H 9625 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 9638 2750 50  0001 C CNN
F 3 "" H 9600 2900 50  0000 C CNN
F 4 "Capacitor" H 9600 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    9600 2900
	0    1    1    0   
$EndComp
$Comp
L C C44
U 1 1 5899CF0B
P 10550 2900
F 0 "C44" H 10575 3000 50  0000 L CNN
F 1 "1uF" H 10575 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10588 2750 50  0001 C CNN
F 3 "" H 10550 2900 50  0000 C CNN
F 4 "Capacitor" H 10550 2900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 8 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    10550 2900
	0    1    1    0   
$EndComp
$Comp
L TEST_1P W6
U 1 1 58F000B3
P 10950 2350
F 0 "W6" H 10950 2620 50  0000 C CNN
F 1 "SDO" H 10950 2550 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 11150 2350 50  0001 C CNN
F 3 "" H 11150 2350 50  0000 C CNN
F 4 "Test Pad" H 0   0   50  0001 C CNN "Description"
F 5 "0" H 0   0   50  0001 C CNN "NumPads"
F 6 "DNP" H 0   0   50  0001 C CNN "Package"
	1    10950 2350
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P W7
U 1 1 58F00132
P 11100 2350
F 0 "W7" H 11100 2620 50  0000 C CNN
F 1 "CKO" H 11100 2550 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 11300 2350 50  0001 C CNN
F 3 "" H 11300 2350 50  0000 C CNN
F 4 "Test Pad" H 0   0   50  0001 C CNN "Description"
F 5 "0" H 0   0   50  0001 C CNN "NumPads"
F 6 "DNP" H 0   0   50  0001 C CNN "Package"
	1    11100 2350
	1    0    0    -1  
$EndComp
Text HLabel 1200 2350 0    60   Input ~ 0
LED_SDI_3V3
Text HLabel 1200 2450 0    60   Input ~ 0
LED_CKI_3V3
Wire Wire Line
	4300 2500 4450 2500
Wire Wire Line
	4300 2350 4450 2350
Wire Wire Line
	5250 2350 5400 2350
Wire Wire Line
	5250 2500 5400 2500
Wire Wire Line
	6200 2350 6350 2350
Wire Wire Line
	7150 2350 7300 2350
Wire Wire Line
	8100 2350 8250 2350
Wire Wire Line
	8250 2500 8100 2500
Wire Wire Line
	7300 2500 7150 2500
Wire Wire Line
	6350 2500 6200 2500
Wire Wire Line
	9050 3100 9050 2650
Wire Wire Line
	8250 2650 8250 3250
Wire Wire Line
	8250 2900 8500 2900
Wire Wire Line
	8100 3100 8100 2650
Wire Wire Line
	8100 2900 7850 2900
Wire Wire Line
	7550 2900 7300 2900
Wire Wire Line
	7300 2650 7300 3250
Wire Wire Line
	7150 3100 7150 2650
Wire Wire Line
	7150 2900 6900 2900
Wire Wire Line
	6600 2900 6350 2900
Wire Wire Line
	6350 2650 6350 3250
Wire Wire Line
	6200 3100 6200 2650
Wire Wire Line
	6200 2900 5950 2900
Wire Wire Line
	5650 2900 5400 2900
Wire Wire Line
	5400 2650 5400 3250
Wire Wire Line
	5250 2650 5250 3100
Wire Wire Line
	5250 2900 5000 2900
Wire Wire Line
	4700 2900 4450 2900
Wire Wire Line
	4450 2650 4450 3250
Wire Wire Line
	4300 2650 4300 3100
Wire Wire Line
	4300 2900 4050 2900
Wire Wire Line
	3750 2900 3500 2900
Wire Wire Line
	3500 2650 3500 3450
Wire Wire Line
	4300 3100 10950 3100
Wire Wire Line
	10150 3250 3500 3250
Wire Wire Line
	3000 2350 3500 2350
Wire Wire Line
	3500 2500 3350 2500
Wire Wire Line
	3350 2500 3350 2450
Wire Wire Line
	3350 2450 3000 2450
Wire Wire Line
	2300 2000 2300 1900
Wire Wire Line
	2300 3700 2300 3750
Wire Wire Line
	1350 2550 1350 3700
Wire Wire Line
	1350 2550 1600 2550
Wire Wire Line
	1600 2650 1350 2650
Wire Wire Line
	1600 2750 1350 2750
Wire Wire Line
	1600 2850 1350 2850
Wire Wire Line
	1600 2950 1350 2950
Wire Wire Line
	1600 3050 1350 3050
Wire Wire Line
	3100 3700 3100 3250
Wire Wire Line
	3100 3350 3000 3350
Wire Wire Line
	3100 3250 3000 3250
Wire Wire Line
	1200 2350 1600 2350
Wire Wire Line
	1200 2450 1600 2450
Wire Wire Line
	10950 3100 10950 2650
Wire Wire Line
	10000 2650 10000 3100
Wire Wire Line
	9050 2900 8800 2900
Wire Wire Line
	10150 2650 10150 3250
Wire Wire Line
	9200 2650 9200 3250
Wire Wire Line
	9450 2900 9200 2900
Wire Wire Line
	9750 2900 10000 2900
Wire Wire Line
	10400 2900 10150 2900
Wire Wire Line
	10700 2900 11100 2900
Wire Wire Line
	11100 2900 11100 2700
Wire Wire Line
	9050 2350 9200 2350
Wire Wire Line
	9200 2500 9050 2500
Wire Wire Line
	10000 2350 10150 2350
Wire Wire Line
	10000 2500 10150 2500
Wire Wire Line
	1350 3700 3100 3700
Wire Wire Line
	10950 2500 11100 2500
Wire Wire Line
	11100 2500 11100 2350
Connection ~ 9050 2900
Connection ~ 4300 2900
Connection ~ 3500 2900
Connection ~ 8250 2900
Connection ~ 3500 3250
Connection ~ 1350 2650
Connection ~ 1350 2750
Connection ~ 1350 2850
Connection ~ 1350 2950
Connection ~ 1350 3050
Connection ~ 3100 3350
Connection ~ 5250 3100
Connection ~ 5250 2900
Connection ~ 6200 3100
Connection ~ 6200 2900
Connection ~ 7150 3100
Connection ~ 7150 2900
Connection ~ 8100 3100
Connection ~ 8100 2900
Connection ~ 7300 3250
Connection ~ 7300 2900
Connection ~ 6350 3250
Connection ~ 6350 2900
Connection ~ 5400 3250
Connection ~ 5400 2900
Connection ~ 4450 3250
Connection ~ 4450 2900
Connection ~ 9050 3100
Connection ~ 8250 3250
Connection ~ 9200 3250
Connection ~ 9200 2900
Connection ~ 10000 2900
Connection ~ 10150 2900
Connection ~ 10950 2900
Connection ~ 10000 3100
Connection ~ 2300 3700
NoConn ~ 3000 2550
NoConn ~ 3000 2650
NoConn ~ 3000 2750
NoConn ~ 3000 2850
NoConn ~ 3000 2950
NoConn ~ 3000 3050
$EndSCHEMATC
