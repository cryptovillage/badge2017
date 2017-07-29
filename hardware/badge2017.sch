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
Sheet 1 2
Title "Crypto and Privacy Village Badge 2017"
Date "2017-06-21"
Rev "C"
Comp "Karl Koscher"
Comment1 "https://cryptovillage.org"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 1150 7050 950  450 
U 586B60C4
F0 "LEDs" 60
F1 "LEDs.sch" 60
F2 "LED_SDI_3V3" I L 1150 7200 60 
F3 "LED_CKI_3V3" I L 1150 7350 60 
$EndSheet
$Comp
L MCP73831 U2
U 1 1 586840CA
P 3000 2450
F 0 "U2" H 2800 2650 60  0000 C CNN
F 1 "MCP73831" H 3000 2250 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-5" H 3000 2450 60  0001 C CNN
F 3 "" H 3000 2450 60  0000 C CNN
F 4 "LiPo Charger IC" H 0   0   50  0001 C CNN "Description"
F 5 "Microchip" H 3000 2450 60  0001 C CNN "Manufacturer"
F 6 "MCP73831T-2ACI/OT" H 3000 2450 60  0001 C CNN "MfgPartNum"
F 7 "5" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23-5" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 3000 2450 60  0001 C CNN "Supplier1"
F 10 "MCP73831T-2ACI/OTCT-ND" H 3000 2450 60  0001 C CNN "Supplier1PartNum"
	1    3000 2450
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 586840D1
P 2150 2550
F 0 "C3" H 2175 2650 50  0000 L CNN
F 1 "4.7uF" H 2175 2450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2188 2400 50  0001 C CNN
F 3 "" H 2150 2550 50  0000 C CNN
F 4 "Capacitor" H 2150 2550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    2150 2550
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 586840D8
P 3850 2550
F 0 "C4" H 3875 2650 50  0000 L CNN
F 1 "4.7uF" H 3875 2450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3888 2400 50  0001 C CNN
F 3 "" H 3850 2550 50  0000 C CNN
F 4 "Capacitor" H 3850 2550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    3850 2550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 586840E1
P 4400 2550
F 0 "P3" H 4400 2700 50  0000 C CNN
F 1 "Battery" V 4500 2550 50  0000 C CNN
F 2 "lib:JST_PH_SMD" H 4400 2550 50  0001 C CNN
F 3 "" H 4400 2550 50  0000 C CNN
F 4 "Battery Connector" H 0   0   50  0001 C CNN "Description"
F 5 "4" H 0   0   50  0001 C CNN "NumPads"
	1    4400 2550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 586840E8
P 3550 2800
F 0 "#PWR01" H 3550 2550 50  0001 C CNN
F 1 "GND" H 3550 2650 50  0000 C CNN
F 2 "" H 3550 2800 50  0000 C CNN
F 3 "" H 3550 2800 50  0000 C CNN
	1    3550 2800
	1    0    0    -1  
$EndComp
$Comp
L ESP32-WROOM U7
U 1 1 586841D8
P 8000 4800
F 0 "U7" H 7300 6050 60  0000 C CNN
F 1 "ESP32-WROOM" H 8500 6050 60  0000 C CNN
F 2 "lib:ESP32-WROOM" H 8350 6150 60  0001 C CNN
F 3 "" H 7550 5250 60  0001 C CNN
F 4 "WiFi Module" H 8000 4800 60  0001 C CNN "Description"
F 5 "Espressif" H 8000 4800 60  0001 C CNN "Manufacturer"
F 6 "ESP-WROOM-32" H 8000 4800 60  0001 C CNN "MfgPartNum"
F 7 "39" H 0   0   50  0001 C CNN "NumPads"
F 8 "ESP-WROOM-32" H 8000 4800 60  0001 C CNN "Suppler1PartNum"
F 9 "Espressif" H 8000 4800 60  0001 C CNN "Supplier"
F 10 "Espressif" H 0   0   50  0001 C CNN "Supplier1"
F 11 "ESP-WROOM-32" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    8000 4800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5868426B
P 9050 5600
F 0 "#PWR02" H 9050 5350 50  0001 C CNN
F 1 "GND" H 9050 5450 50  0000 C CNN
F 2 "" H 9050 5600 50  0000 C CNN
F 3 "" H 9050 5600 50  0000 C CNN
	1    9050 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 58684351
P 7550 6250
F 0 "#PWR03" H 7550 6000 50  0001 C CNN
F 1 "GND" H 7550 6100 50  0000 C CNN
F 2 "" H 7550 6250 50  0000 C CNN
F 3 "" H 7550 6250 50  0000 C CNN
	1    7550 6250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 5868436F
P 7000 5950
F 0 "#PWR04" H 7000 5700 50  0001 C CNN
F 1 "GND" H 7000 5800 50  0000 C CNN
F 2 "" H 7000 5950 50  0000 C CNN
F 3 "" H 7000 5950 50  0000 C CNN
	1    7000 5950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 58684504
P 850 3150
F 0 "#PWR05" H 850 2900 50  0001 C CNN
F 1 "GND" H 850 3000 50  0000 C CNN
F 2 "" H 850 3150 50  0000 C CNN
F 3 "" H 850 3150 50  0000 C CNN
	1    850  3150
	1    0    0    -1  
$EndComp
$Comp
L F_Small F1
U 1 1 58684672
P 1800 2350
F 0 "F1" H 1760 2410 50  0000 L CNN
F 1 "1.1A" H 1680 2290 50  0000 L CNN
F 2 "Resistors_SMD:R_1210" H 1800 2350 50  0001 C CNN
F 3 "" H 1800 2350 50  0000 C CNN
F 4 "Resettable Fuse" H 0   0   50  0001 C CNN "Description"
F 5 "Bourns Inc." H 0   0   50  0001 C CNN "Manufacturer"
F 6 "MF-USMF110-2" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "2" H 0   0   50  0001 C CNN "NumPads"
F 8 "1210" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "MF-USMF110-2CT-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    1800 2350
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 5868492F
P 5150 2300
F 0 "R4" V 5050 2300 50  0000 C CNN
F 1 "100k" V 5150 2300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5080 2300 50  0001 C CNN
F 3 "" H 5150 2300 50  0000 C CNN
F 4 "Resistor" V 5150 2300 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    5150 2300
	1    0    0    -1  
$EndComp
$Comp
L Q_PMOS_GSD Q1
U 1 1 58684958
P 5550 2050
F 0 "Q1" H 5500 2150 50  0000 R CNN
F 1 "PMOS" H 5500 1950 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 5750 2150 50  0001 C CNN
F 3 "" H 5550 2050 50  0000 C CNN
F 4 "MOSFET Transistor" H 0   0   50  0001 C CNN "Description"
F 5 "Diodes Incorporated" H 5550 2050 60  0001 C CNN "Manufacturer"
F 6 "DMG2307L-7" H 5550 2050 60  0001 C CNN "MfgPartNum"
F 7 "3" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 5550 2050 60  0001 C CNN "Supplier1"
F 10 "DMG2307L-7DICT-ND" H 5550 2050 60  0001 C CNN "Supplier1PartNum"
	1    5550 2050
	0    -1   1    0   
$EndComp
$Comp
L D_Schottky D1
U 1 1 586849D3
P 5900 2000
F 0 "D1" H 5900 2100 50  0000 C CNN
F 1 "D_Schottky" H 5900 1900 50  0000 C CNN
F 2 "Diodes_SMD:SOD-323" H 5900 2000 50  0001 C CNN
F 3 "" H 5900 2000 50  0000 C CNN
F 4 "Schottky Diode" H 0   0   50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "BAT20JFILM" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "2" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOD-323" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "497-3381-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    5900 2000
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR06
U 1 1 58684ACD
P 5250 2500
F 0 "#PWR06" H 5250 2250 50  0001 C CNN
F 1 "GND" H 5250 2350 50  0000 C CNN
F 2 "" H 5250 2500 50  0000 C CNN
F 3 "" H 5250 2500 50  0000 C CNN
	1    5250 2500
	1    0    0    -1  
$EndComp
$Comp
L SPST SW2
U 1 1 58684C59
P 4600 2150
F 0 "SW2" H 4600 2250 50  0000 C CNN
F 1 "SPST" H 4600 2050 50  0000 C CNN
F 2 "lib:DPDT_SMD" H 4600 2150 50  0001 C CNN
F 3 "" H 4600 2150 50  0000 C CNN
F 4 "Switch" H 0   0   50  0001 C CNN "Description"
F 5 "C&K Components" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "JS202011SCQN" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "6" H 0   0   50  0001 C CNN "NumPads"
F 8 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 9 "401-2002-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    4600 2150
	1    0    0    -1  
$EndComp
$Comp
L C C13
U 1 1 5868529D
P 6800 2650
F 0 "C13" H 6825 2750 50  0000 L CNN
F 1 "10uF" H 6825 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6838 2500 50  0001 C CNN
F 3 "" H 6800 2650 50  0000 C CNN
F 4 "Capacitor" H 6800 2650 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    6800 2650
	1    0    0    -1  
$EndComp
$Comp
L C C15
U 1 1 5868534C
P 8750 2350
F 0 "C15" H 8775 2450 50  0000 L CNN
F 1 "22uF" H 8775 2250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8788 2200 50  0001 C CNN
F 3 "" H 8750 2350 50  0000 C CNN
F 4 "Capacitor" H 8750 2350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "10V" H 0   0   50  0001 C CNN "Voltage"
	1    8750 2350
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR07
U 1 1 58685869
P 8750 1900
F 0 "#PWR07" H 8750 1750 50  0001 C CNN
F 1 "+3V3" H 8750 2040 50  0000 C CNN
F 2 "" H 8750 1900 50  0000 C CNN
F 3 "" H 8750 1900 50  0000 C CNN
	1    8750 1900
	1    0    0    -1  
$EndComp
$Comp
L EFM8UB1 U3
U 1 1 586868EE
P 3250 4100
F 0 "U3" H 3250 4200 60  0000 C CNN
F 1 "EFM8UB1" H 3250 4100 60  0000 C CNN
F 2 "Housings_DFN_QFN:QFN-28-1EP_5x5mm_Pitch0.5mm" H 2800 4750 60  0001 C CNN
F 3 "" H 2800 4750 60  0000 C CNN
F 4 "Microcontroller" H 0   0   50  0001 C CNN "Description"
F 5 "Silicon Labs" H 3250 4100 60  0001 C CNN "Manufacturer"
F 6 "EFM8UB10F16G-C-QFN28" H 3250 4100 60  0001 C CNN "MfgPartNum"
F 7 "29" H 0   0   50  0001 C CNN "NumPads"
F 8 "QFN28 (5x5)" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 3250 4100 60  0001 C CNN "Supplier1"
F 10 "336-3409-5-ND" H 3250 4100 60  0001 C CNN "Supplier1PartNum"
	1    3250 4100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 58687244
P 2100 4800
F 0 "#PWR08" H 2100 4550 50  0001 C CNN
F 1 "GND" H 2100 4650 50  0000 C CNN
F 2 "" H 2100 4800 50  0000 C CNN
F 3 "" H 2100 4800 50  0000 C CNN
	1    2100 4800
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR09
U 1 1 58687346
P 1650 4150
F 0 "#PWR09" H 1650 4000 50  0001 C CNN
F 1 "+3V3" H 1650 4290 50  0000 C CNN
F 2 "" H 1650 4150 50  0000 C CNN
F 3 "" H 1650 4150 50  0000 C CNN
	1    1650 4150
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 58687D3E
P 9500 4250
F 0 "R9" V 9400 4250 50  0000 C CNN
F 1 "1k" V 9500 4250 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 9430 4250 50  0001 C CNN
F 3 "" H 9500 4250 50  0000 C CNN
F 4 "Resistor" V 9500 4250 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    9500 4250
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 58687DAF
P 9500 4350
F 0 "R10" V 9580 4350 50  0000 C CNN
F 1 "1k" V 9500 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 9430 4350 50  0001 C CNN
F 3 "" H 9500 4350 50  0000 C CNN
F 4 "Resistor" V 9500 4350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    9500 4350
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR010
U 1 1 58688620
P 6900 4000
F 0 "#PWR010" H 6900 3850 50  0001 C CNN
F 1 "+3V3" H 6900 4140 50  0000 C CNN
F 2 "" H 6900 4000 50  0000 C CNN
F 3 "" H 6900 4000 50  0000 C CNN
	1    6900 4000
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 58688AA5
P 1450 4550
F 0 "C2" H 1475 4650 50  0000 L CNN
F 1 "0.1uF" H 1475 4450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1488 4400 50  0001 C CNN
F 3 "" H 1450 4550 50  0000 C CNN
F 4 "Capacitor" H 1450 4550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    1450 4550
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 58688AEE
P 1250 4550
F 0 "C1" H 1100 4650 50  0000 L CNN
F 1 "4.7uF" H 1000 4450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1288 4400 50  0001 C CNN
F 3 "" H 1250 4550 50  0000 C CNN
F 4 "Capacitor" H 1250 4550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    1250 4550
	1    0    0    -1  
$EndComp
$Comp
L ATECC508A U4
U 1 1 58689F79
P 5150 4300
F 0 "U4" H 5150 4550 60  0000 C CNN
F 1 "ATECC508A" H 5150 4050 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5350 4300 60  0001 C CNN
F 3 "" H 5350 4300 60  0000 C CNN
F 4 "Crypto Chip" H 0   0   50  0001 C CNN "Description"
F 5 "Atmel" H 5150 4300 60  0001 C CNN "Manufacturer"
F 6 "ATECC508A-SSHDA-B" H 5150 4300 60  0001 C CNN "MfgPartNum"
F 7 "8" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOIC-8" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 5150 4300 60  0001 C CNN "Suppler1"
F 10 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 11 "ATECC508A-SSHDA-B-ND" H 5150 4300 60  0001 C CNN "Supplier1PartNum"
	1    5150 4300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 5868A22B
P 5700 4550
F 0 "#PWR011" H 5700 4300 50  0001 C CNN
F 1 "GND" H 5700 4400 50  0000 C CNN
F 2 "" H 5700 4550 50  0000 C CNN
F 3 "" H 5700 4550 50  0000 C CNN
	1    5700 4550
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5868A269
P 5850 4350
F 0 "C6" H 5875 4450 50  0000 L CNN
F 1 "1uF" H 5875 4250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5888 4200 50  0001 C CNN
F 3 "" H 5850 4350 50  0000 C CNN
F 4 "Capacitor" H 5850 4350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    5850 4350
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5868A501
P 4500 4050
F 0 "R2" V 4580 4050 50  0000 C CNN
F 1 "4k7" V 4500 4050 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4430 4050 50  0001 C CNN
F 3 "" H 4500 4050 50  0000 C CNN
F 4 "Resistor" V 4500 4050 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.1" H 0   0   50  0001 C CNN "Tolerance"
	1    4500 4050
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 5868A58C
P 4650 4050
F 0 "R3" V 4730 4050 50  0000 C CNN
F 1 "4k7" V 4650 4050 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4580 4050 50  0001 C CNN
F 3 "" H 4650 4050 50  0000 C CNN
F 4 "Resistor" V 4650 4050 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.1" H 0   0   50  0001 C CNN "Tolerance"
	1    4650 4050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 5868BA36
P 6400 7600
F 0 "#PWR012" H 6400 7350 50  0001 C CNN
F 1 "GND" H 6400 7450 50  0000 C CNN
F 2 "" H 6400 7600 50  0000 C CNN
F 3 "" H 6400 7600 50  0000 C CNN
	1    6400 7600
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR013
U 1 1 5868BA7C
P 6750 6500
F 0 "#PWR013" H 6750 6350 50  0001 C CNN
F 1 "+3V3" H 6750 6640 50  0000 C CNN
F 2 "" H 6750 6500 50  0000 C CNN
F 3 "" H 6750 6500 50  0000 C CNN
	1    6750 6500
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 5868BB5C
P 6600 7550
F 0 "C12" H 6625 7650 50  0000 L CNN
F 1 "10uF" H 6625 7450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6638 7400 50  0001 C CNN
F 3 "" H 6600 7550 50  0000 C CNN
F 4 "Capacitor" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6600 7550
	0    1    1    0   
$EndComp
$Comp
L C C7
U 1 1 5868BF6C
P 6100 4350
F 0 "C7" H 6125 4450 50  0000 L CNN
F 1 "10uF" H 6125 4250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6138 4200 50  0001 C CNN
F 3 "" H 6100 4350 50  0000 C CNN
F 4 "Capacitor" H 6100 4350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6100 4350
	1    0    0    -1  
$EndComp
$Comp
L C C18
U 1 1 5868D4BE
P 10050 2550
F 0 "C18" H 10000 2650 50  0000 L CNN
F 1 "1uF" H 10000 2450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10088 2400 50  0001 C CNN
F 3 "" H 10050 2550 50  0000 C CNN
F 4 "Capacitor" H 10050 2550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10050 2550
	0    1    1    0   
$EndComp
$Comp
L C C19
U 1 1 5868D50F
P 10050 2850
F 0 "C19" H 10075 2950 50  0000 L CNN
F 1 "1uF" H 10075 2750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10088 2700 50  0001 C CNN
F 3 "" H 10050 2850 50  0000 C CNN
F 4 "Capacitor" H 10050 2850 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10050 2850
	0    1    1    0   
$EndComp
$Comp
L C C20
U 1 1 5868D56A
P 10050 3050
F 0 "C20" H 10075 3150 50  0000 L CNN
F 1 "1uF" H 10075 2950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10088 2900 50  0001 C CNN
F 3 "" H 10050 3050 50  0000 C CNN
F 4 "Capacitor" H 10050 3050 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10050 3050
	0    1    1    0   
$EndComp
$Comp
L GND #PWR014
U 1 1 5868D9D1
P 9650 3300
F 0 "#PWR014" H 9650 3050 50  0001 C CNN
F 1 "GND" H 9650 3150 50  0000 C CNN
F 2 "" H 9650 3300 50  0000 C CNN
F 3 "" H 9650 3300 50  0000 C CNN
	1    9650 3300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR015
U 1 1 5868DAD7
P 9550 2200
F 0 "#PWR015" H 9550 2050 50  0001 C CNN
F 1 "+3V3" H 9550 2340 50  0000 C CNN
F 2 "" H 9550 2200 50  0000 C CNN
F 3 "" H 9550 2200 50  0000 C CNN
	1    9550 2200
	1    0    0    -1  
$EndComp
$Comp
L C C17
U 1 1 5868DBE6
P 9300 2750
F 0 "C17" H 9325 2850 50  0000 L CNN
F 1 "10uF" H 9325 2650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 9338 2600 50  0001 C CNN
F 3 "" H 9300 2750 50  0000 C CNN
F 4 "Capacitor" H 9300 2750 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    9300 2750
	1    0    0    -1  
$EndComp
$Comp
L C C23
U 1 1 5868EC08
P 10200 3150
F 0 "C23" H 10225 3250 50  0000 L CNN
F 1 "1uF" H 10225 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10238 3000 50  0001 C CNN
F 3 "" H 10200 3150 50  0000 C CNN
F 4 "Capacitor" H 10200 3150 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10200 3150
	0    1    1    0   
$EndComp
$Comp
L C C21
U 1 1 5868ECA5
P 10050 3250
F 0 "C21" H 10075 3350 50  0000 L CNN
F 1 "1uF" H 10075 3150 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10088 3100 50  0001 C CNN
F 3 "" H 10050 3250 50  0000 C CNN
F 4 "Capacitor" H 10050 3250 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10050 3250
	0    1    1    0   
$EndComp
$Comp
L C C24
U 1 1 5868ED59
P 10200 3350
F 0 "C24" H 10225 3450 50  0000 L CNN
F 1 "1uF" H 10225 3250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10238 3200 50  0001 C CNN
F 3 "" H 10200 3350 50  0000 C CNN
F 4 "Capacitor" H 10200 3350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10200 3350
	0    1    1    0   
$EndComp
$Comp
L C C22
U 1 1 5868EDBC
P 10050 3450
F 0 "C22" H 10075 3550 50  0000 L CNN
F 1 "1uF" H 10075 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10088 3300 50  0001 C CNN
F 3 "" H 10050 3450 50  0000 C CNN
F 4 "Capacitor" H 10050 3450 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10050 3450
	0    1    1    0   
$EndComp
$Comp
L C C25
U 1 1 5868EE1D
P 10200 3550
F 0 "C25" H 10225 3650 50  0000 L CNN
F 1 "1uF" H 10225 3450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10238 3400 50  0001 C CNN
F 3 "" H 10200 3550 50  0000 C CNN
F 4 "Capacitor" H 10200 3550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10200 3550
	0    1    1    0   
$EndComp
$Comp
L C C8
U 1 1 586A2450
P 6350 4350
F 0 "C8" H 6375 4450 50  0000 L CNN
F 1 "0.1uF" H 6375 4250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6388 4200 50  0001 C CNN
F 3 "" H 6350 4350 50  0000 C CNN
F 4 "Capacitor" H 6350 4350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6350 4350
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR016
U 1 1 586A3B50
P 5100 3800
F 0 "#PWR016" H 5100 3650 50  0001 C CNN
F 1 "+3V3" H 5100 3940 50  0000 C CNN
F 2 "" H 5100 3800 50  0000 C CNN
F 3 "" H 5100 3800 50  0000 C CNN
	1    5100 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 586A3F73
P 6250 4550
F 0 "#PWR017" H 6250 4300 50  0001 C CNN
F 1 "GND" H 6250 4400 50  0000 C CNN
F 2 "" H 6250 4550 50  0000 C CNN
F 3 "" H 6250 4550 50  0000 C CNN
	1    6250 4550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P6
U 1 1 586A457D
P 11100 4200
F 0 "P6" H 11100 4050 50  0000 C CNN
F 1 "Backlight" V 11100 4550 50  0000 C CNN
F 2 "lib:JST_PH_SMD" H 11100 4200 50  0001 C CNN
F 3 "" H 11100 4200 50  0000 C CNN
F 4 "Backlight Connector" H 0   0   50  0001 C CNN "Description"
F 5 "4" H 0   0   50  0001 C CNN "NumPads"
F 6 "EastRising" H 0   0   50  0001 C CNN "Supplier1"
	1    11100 4200
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 586A45EE
P 10500 4250
F 0 "R14" V 10580 4250 50  0000 C CNN
F 1 "15" V 10500 4250 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 10430 4250 50  0001 C CNN
F 3 "" H 10500 4250 50  0000 C CNN
F 4 "Resistor" V 10500 4250 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.1" H 0   0   50  0001 C CNN "Tolerance"
F 8 "1/10W" H 0   0   50  0001 C CNN "Wattage"
	1    10500 4250
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR018
U 1 1 586A484D
P 10250 4100
F 0 "#PWR018" H 10250 3950 50  0001 C CNN
F 1 "+3V3" H 10250 4240 50  0000 C CNN
F 2 "" H 10250 4100 50  0000 C CNN
F 3 "" H 10250 4100 50  0000 C CNN
	1    10250 4100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 586A4A46
P 10800 5250
F 0 "#PWR019" H 10800 5000 50  0001 C CNN
F 1 "GND" H 10800 5100 50  0000 C CNN
F 2 "" H 10800 5250 50  0000 C CNN
F 3 "" H 10800 5250 50  0000 C CNN
	1    10800 5250
	1    0    0    -1  
$EndComp
$Comp
L Crystal_Small Y1
U 1 1 586A5700
P 6500 4800
F 0 "Y1" H 6500 4900 50  0000 C CNN
F 1 "32.768k" H 6800 4900 50  0000 C CNN
F 2 "lib:abs25_crystal" H 6500 4800 50  0001 C CNN
F 3 "" H 6500 4800 50  0000 C CNN
F 4 "32kHz Crystal" H 0   0   50  0001 C CNN "Description"
F 5 "Abracon LLC" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "ABS25-32.768KHZ-T" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "4" H 0   0   50  0001 C CNN "NumPads"
F 8 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 9 "535-9166-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    6500 4800
	0    1    1    0   
$EndComp
$Comp
L CONN_02X02 P2
U 1 1 586A76DA
P 1000 3800
F 0 "P2" H 1000 3950 50  0000 C CNN
F 1 "CONN_02X02" H 1000 3650 50  0000 C CNN
F 2 "lib:Socket_Strip_Straight_2x02_TP" H 1000 2600 50  0001 C CNN
F 3 "" H 1000 2600 50  0000 C CNN
F 4 "Debug Port" H 0   0   50  0001 C CNN "Description"
F 5 "0" H 0   0   50  0001 C CNN "NumPads"
F 6 "DNP" H 0   0   50  0001 C CNN "Package"
	1    1000 3800
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR020
U 1 1 586A789B
P 700 3200
F 0 "#PWR020" H 700 3050 50  0001 C CNN
F 1 "+3V3" H 700 3340 50  0000 C CNN
F 2 "" H 700 3200 50  0000 C CNN
F 3 "" H 700 3200 50  0000 C CNN
	1    700  3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR021
U 1 1 586A7A30
P 700 3950
F 0 "#PWR021" H 700 3700 50  0001 C CNN
F 1 "GND" H 700 3800 50  0000 C CNN
F 2 "" H 700 3950 50  0000 C CNN
F 3 "" H 700 3950 50  0000 C CNN
	1    700  3950
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 586A7EFB
P 1000 3400
F 0 "R1" V 1080 3400 50  0000 C CNN
F 1 "1k" V 1000 3400 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 930 3400 50  0001 C CNN
F 3 "" H 1000 3400 50  0000 C CNN
F 4 "Resistor" V 1000 3400 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    1000 3400
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 586A8739
P 5250 2300
F 0 "R5" V 5330 2300 50  0000 C CNN
F 1 "100k" V 5250 2300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5180 2300 50  0001 C CNN
F 3 "" H 5250 2300 50  0000 C CNN
F 4 "Resistor" V 5250 2300 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    5250 2300
	1    0    0    -1  
$EndComp
$Comp
L C C16
U 1 1 586AAAF1
P 9100 2750
F 0 "C16" H 8900 2850 50  0000 L CNN
F 1 "0.1uF" H 8850 2650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 9138 2600 50  0001 C CNN
F 3 "" H 9100 2750 50  0000 C CNN
F 4 "Capacitor" H 9100 2750 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    9100 2750
	1    0    0    -1  
$EndComp
$Comp
L SP0503BAHTG U1
U 1 1 586AC229
P 1400 1800
F 0 "U1" H 1200 2000 60  0000 C CNN
F 1 "SP0503BAHTG" H 1400 1850 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-143" H 1400 1800 60  0001 C CNN
F 3 "" H 1400 1800 60  0001 C CNN
F 4 "ESD Protection Diode" H 1400 1800 60  0001 C CNN "Description"
F 5 "Littelfuse Inc." H 1400 1800 60  0001 C CNN "Manufacturer"
F 6 "SP0503BAHTG" H 1400 1800 60  0001 C CNN "MfgPartNum"
F 7 "4" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-143" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 1400 1800 60  0001 C CNN "Supplier1"
F 10 "F2715CT-ND" H 1400 1800 60  0001 C CNN "Supplier1PartNum"
	1    1400 1800
	1    0    0    -1  
$EndComp
$Comp
L D_Schottky D2
U 1 1 586ADC4B
P 8000 1000
F 0 "D2" H 8000 1100 50  0000 C CNN
F 1 "D_Schottky" H 8000 900 50  0000 C CNN
F 2 "Diodes_SMD:SOD-323" H 8000 1000 50  0001 C CNN
F 3 "" H 8000 1000 50  0000 C CNN
F 4 "Schottky Diode" H 0   0   50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "BAT20JFILM" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "2" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOD-323" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "497-3381-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    8000 1000
	-1   0    0    1   
$EndComp
$Comp
L R R7
U 1 1 586ADE2E
P 8250 1200
F 0 "R7" V 8330 1200 50  0000 C CNN
F 1 "5k62" V 8250 1200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 8180 1200 50  0001 C CNN
F 3 "" H 8250 1200 50  0000 C CNN
F 4 "Resistor" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.02" H 0   0   50  0001 C CNN "Tolerance"
	1    8250 1200
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 586ADEC1
P 8250 1600
F 0 "R8" V 8330 1600 50  0000 C CNN
F 1 "1k87" V 8250 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 8180 1600 50  0001 C CNN
F 3 "" H 8250 1600 50  0000 C CNN
F 4 "Resistor" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.02" H 0   0   50  0001 C CNN "Tolerance"
	1    8250 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 586AE1FA
P 7800 1900
F 0 "#PWR022" H 7800 1650 50  0001 C CNN
F 1 "GND" H 7800 1750 50  0000 C CNN
F 2 "" H 7800 1900 50  0000 C CNN
F 3 "" H 7800 1900 50  0000 C CNN
	1    7800 1900
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 586AEC03
P 6550 1450
F 0 "C5" H 6575 1550 50  0000 L CNN
F 1 "4.7uF" H 6575 1350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6588 1300 50  0001 C CNN
F 3 "" H 6550 1450 50  0000 C CNN
F 4 "Capacitor" H 6550 1450 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    6550 1450
	1    0    0    -1  
$EndComp
$Comp
L C C14
U 1 1 586AF0CD
P 8500 1400
F 0 "C14" H 8525 1500 50  0000 L CNN
F 1 "22uF" H 8525 1300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8538 1250 50  0001 C CNN
F 3 "" H 8500 1400 50  0000 C CNN
F 4 "Capacitor" H 8500 1400 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "10V" H 0   0   50  0001 C CNN "Voltage"
	1    8500 1400
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR023
U 1 1 586AF5D6
P 8500 900
F 0 "#PWR023" H 8500 750 50  0001 C CNN
F 1 "+5V" H 8500 1040 50  0000 C CNN
F 2 "" H 8500 900 50  0000 C CNN
F 3 "" H 8500 900 50  0000 C CNN
	1    8500 900 
	1    0    0    -1  
$EndComp
$Comp
L MIC2288 U5
U 1 1 586B004A
P 7300 1350
F 0 "U5" H 7150 1600 60  0000 C CNN
F 1 "MIC2288" H 7325 1100 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-5" H 7300 1350 60  0001 C CNN
F 3 "" H 7300 1350 60  0000 C CNN
F 4 "5V Boost Regulator" H 0   0   50  0001 C CNN "Description"
F 5 "Microchip" H 7300 1350 60  0001 C CNN "Manufacturer"
F 6 "MIC2288YD5-TR" H 7300 1350 60  0001 C CNN "MfgPartNum"
F 7 "5" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23-5" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 7300 1350 60  0001 C CNN "Supplier1"
F 10 "576-1729-1-ND" H 7300 1350 60  0001 C CNN "Supplier1PartNum"
	1    7300 1350
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 586AE540
P 6350 4700
F 0 "C9" H 6200 4800 50  0000 L CNN
F 1 "12pF" H 6375 4600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6388 4550 50  0001 C CNN
F 3 "" H 6350 4700 50  0000 C CNN
F 4 "Capacitor" H 6350 4700 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6350 4700
	0    1    1    0   
$EndComp
$Comp
L C C10
U 1 1 586AE5F7
P 6350 4900
F 0 "C10" H 6375 5000 50  0000 L CNN
F 1 "12pF" H 6375 4800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6388 4750 50  0001 C CNN
F 3 "" H 6350 4900 50  0000 C CNN
F 4 "Capacitor" H 6350 4900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6350 4900
	0    1    1    0   
$EndComp
$Comp
L GND #PWR024
U 1 1 586AE9C5
P 6100 4850
F 0 "#PWR024" H 6100 4600 50  0001 C CNN
F 1 "GND" H 6100 4700 50  0000 C CNN
F 2 "" H 6100 4850 50  0000 C CNN
F 3 "" H 6100 4850 50  0000 C CNN
	1    6100 4850
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 586AF04C
P 6400 3750
F 0 "R6" V 6300 3750 50  0000 C CNN
F 1 "1k" V 6400 3750 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6330 3750 50  0001 C CNN
F 3 "" H 6400 3750 50  0000 C CNN
F 4 "Resistor" V 6400 3750 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6400 3750
	0    1    1    0   
$EndComp
$Comp
L C C11
U 1 1 586AF107
P 6600 3950
F 0 "C11" H 6450 4050 50  0000 L CNN
F 1 "0.1uF" H 6350 3850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6638 3800 50  0001 C CNN
F 3 "" H 6600 3950 50  0000 C CNN
F 4 "Capacitor" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6600 3950
	1    0    0    -1  
$EndComp
$Comp
L USB_OTG P1
U 1 1 586C6DE0
P 850 2550
F 0 "P1" H 1175 2425 50  0000 C CNN
F 1 "USB" H 850 2900 50  0000 C CNN
F 2 "lib:USB_Micro-B-cpv" V 800 2450 50  0001 C CNN
F 3 "" V 800 2450 50  0000 C CNN
F 4 "USB Connector" H 0   0   50  0001 C CNN "Description"
F 5 "Amphenol" H 850 2550 60  0001 C CNN "Manufacturer"
F 6 "10118192-0001LF" H 850 2550 60  0001 C CNN "MfgPartNum"
F 7 "9" H 0   0   50  0001 C CNN "NumPads"
F 8 "609-4613-1-ND" H 850 2550 60  0001 C CNN "Suppler1PartNum"
F 9 "Digikey" H 850 2550 60  0001 C CNN "Supplier1"
F 10 "609-4613-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    850  2550
	1    0    0    -1  
$EndComp
$Comp
L L L1
U 1 1 586C71EA
P 7350 1000
F 0 "L1" V 7300 1000 50  0000 C CNN
F 1 "4.7uH" V 7425 1000 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" H 7350 1000 50  0001 C CNN
F 3 "" H 7350 1000 50  0000 C CNN
F 4 "Inductor" H 0   0   50  0001 C CNN "Description"
F 5 "Murata Electronics North America" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "LQM21PN4R7MGSD" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "2" H 0   0   50  0001 C CNN "NumPads"
F 8 "0603" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "490-12067-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    7350 1000
	0    1    1    0   
$EndComp
$Comp
L L L2
U 1 1 586C80B9
P 8200 2150
F 0 "L2" V 8150 2150 50  0000 C CNN
F 1 "4.7uH" V 8275 2150 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" H 8200 2150 50  0001 C CNN
F 3 "" H 8200 2150 50  0000 C CNN
F 4 "Inductor" H 0   0   50  0001 C CNN "Description"
F 5 "Murata Electronics North America" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "LQM21PN4R7MGSD" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "2" H 0   0   50  0001 C CNN "NumPads"
F 8 "0603" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "490-12067-1-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    8200 2150
	0    1    1    0   
$EndComp
$Comp
L Rotary_Encoder_Switch SW1
U 1 1 586C8FBD
P 3250 7450
F 0 "SW1" H 3250 7710 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 3250 7190 50  0000 C CNN
F 2 "lib:EN12_RotaryEncoder" H 3150 7610 50  0001 C CNN
F 3 "" H 3250 7710 50  0001 C CNN
F 4 "Rotary Encoder" H 0   0   50  0001 C CNN "Description"
F 5 "TT Electronics/BI" H 3250 7450 60  0001 C CNN "Manufacturer"
F 6 "EN12-HS22AF20" H 3250 7450 60  0001 C CNN "MfgPartNum"
F 7 "7" H 0   0   50  0001 C CNN "NumPads"
F 8 "Digikey" H 3250 7450 60  0001 C CNN "Supplier1"
F 9 "987-1195-ND" H 3250 7450 60  0001 C CNN "Supplier1PartNum"
	1    3250 7450
	1    0    0    -1  
$EndComp
$Comp
L Micro_SD_Card CON1
U 1 1 586C9C4D
P 5200 6900
F 0 "CON1" H 4550 7500 50  0000 C CNN
F 1 "Micro_SD_Card" H 5850 7500 50  0000 R CNN
F 2 "lib:MICROSD" H 6350 7200 50  0001 C CNN
F 3 "" H 5200 6900 50  0000 C CNN
F 4 "SD Card Holder" H 0   0   50  0001 C CNN "Description"
F 5 "Molex" H 5200 6900 60  0001 C CNN "Manufacturer"
F 6 "472192001" H 5200 6900 60  0001 C CNN "MfgPartNum"
F 7 "12" H 0   0   50  0001 C CNN "NumPads"
F 8 "Digikey" H 5200 6900 60  0001 C CNN "Supplier1"
F 9 "WM6698CT-ND" H 5200 6900 60  0001 C CNN "Supplier1PartNum"
	1    5200 6900
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 586CB8F1
P 4300 7600
F 0 "#PWR025" H 4300 7350 50  0001 C CNN
F 1 "GND" H 4300 7450 50  0000 C CNN
F 2 "" H 4300 7600 50  0000 C CNN
F 3 "" H 4300 7600 50  0000 C CNN
	1    4300 7600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 5886DEF9
P 7400 2900
F 0 "#PWR026" H 7400 2650 50  0001 C CNN
F 1 "GND" H 7400 2750 50  0000 C CNN
F 2 "" H 7400 2900 50  0000 C CNN
F 3 "" H 7400 2900 50  0000 C CNN
	1    7400 2900
	1    0    0    -1  
$EndComp
$Comp
L ERC12864-1 P5
U 1 1 58872945
P 10600 2300
F 0 "P5" H 10800 3800 60  0000 C CNN
F 1 "ERC12864-1" V 10950 2350 60  0000 C CNN
F 2 "lib:LCD_zif_connector" H 10600 2550 60  0001 C CNN
F 3 "" H 10600 2550 60  0001 C CNN
F 4 "LCD Connector" H 0   0   50  0001 C CNN "Description"
F 5 "32" H 0   0   50  0001 C CNN "NumPads"
F 6 "EastRising" H 0   0   50  0001 C CNN "Supplier1"
	1    10600 2300
	1    0    0    -1  
$EndComp
$Comp
L C C32
U 1 1 588751F9
P 10200 2650
F 0 "C32" H 10150 2750 50  0000 L CNN
F 1 "1uF" H 10225 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 10238 2500 50  0001 C CNN
F 3 "" H 10200 2650 50  0000 C CNN
F 4 "Capacitor" H 10200 2650 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "X5R/X7R" H 0   0   50  0001 C CNN "Type"
F 9 "25V" H 0   0   50  0001 C CNN "Voltage"
	1    10200 2650
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR027
U 1 1 58875BE7
P 9550 900
F 0 "#PWR027" H 9550 750 50  0001 C CNN
F 1 "+3V3" H 9550 1040 50  0000 C CNN
F 2 "" H 9550 900 50  0000 C CNN
F 3 "" H 9550 900 50  0000 C CNN
	1    9550 900 
	1    0    0    -1  
$EndComp
$Comp
L R R21
U 1 1 58918B25
P 2750 800
F 0 "R21" V 2830 800 50  0000 C CNN
F 1 "10k" V 2750 800 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2680 800 50  0001 C CNN
F 3 "" H 2750 800 50  0000 C CNN
F 4 "Resistor" V 2750 800 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    2750 800 
	1    0    0    -1  
$EndComp
$Comp
L R R23
U 1 1 58918CE8
P 4500 800
F 0 "R23" V 4580 800 50  0000 C CNN
F 1 "2k" V 4500 800 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4430 800 50  0001 C CNN
F 3 "" H 4500 800 50  0000 C CNN
F 4 "Resistor" V 4500 800 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    4500 800 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR028
U 1 1 589193D1
P 2750 1650
F 0 "#PWR028" H 2750 1400 50  0001 C CNN
F 1 "GND" H 2750 1500 50  0000 C CNN
F 2 "" H 2750 1650 50  0000 C CNN
F 3 "" H 2750 1650 50  0000 C CNN
	1    2750 1650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR029
U 1 1 5891947F
P 4500 1650
F 0 "#PWR029" H 4500 1400 50  0001 C CNN
F 1 "GND" H 4500 1500 50  0000 C CNN
F 2 "" H 4500 1650 50  0000 C CNN
F 3 "" H 4500 1650 50  0000 C CNN
	1    4500 1650
	1    0    0    -1  
$EndComp
$Comp
L MMA8452Q U9
U 1 1 5891B04C
P 1900 6300
F 0 "U9" H 1900 6350 60  0000 C CNN
F 1 "MMA8452Q" H 1900 6250 60  0000 C CNN
F 2 "lib:MMA8452Q" H 1950 6300 60  0001 C CNN
F 3 "" H 1950 6300 60  0000 C CNN
F 4 "Accelerometer" H 0   0   50  0001 C CNN "Description"
F 5 "NXP" H 1900 6300 60  0001 C CNN "Manufacturer"
F 6 "MMA8452QT" H 1900 6300 60  0001 C CNN "MfgPartNum"
F 7 "16" H 0   0   50  0001 C CNN "NumPads"
F 8 "QFN16 (3x3)" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 1900 6300 60  0001 C CNN "Suppler1"
F 10 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 11 "MMA8452QT-ND" H 1900 6300 60  0001 C CNN "Supplier1PartNum"
	1    1900 6300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR030
U 1 1 5891B215
P 2000 5700
F 0 "#PWR030" H 2000 5550 50  0001 C CNN
F 1 "+3V3" H 2000 5840 50  0000 C CNN
F 2 "" H 2000 5700 50  0000 C CNN
F 3 "" H 2000 5700 50  0000 C CNN
	1    2000 5700
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR031
U 1 1 5891B2C9
P 1150 6000
F 0 "#PWR031" H 1150 5850 50  0001 C CNN
F 1 "+3V3" H 1150 6140 50  0000 C CNN
F 2 "" H 1150 6000 50  0000 C CNN
F 3 "" H 1150 6000 50  0000 C CNN
	1    1150 6000
	1    0    0    -1  
$EndComp
$Comp
L C C35
U 1 1 5891B6C2
P 2300 5750
F 0 "C35" H 2325 5850 50  0000 L CNN
F 1 "4.7uF" H 2325 5650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2338 5600 50  0001 C CNN
F 3 "" H 2300 5750 50  0000 C CNN
F 4 "Capacitor" H 2300 5750 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    2300 5750
	0    1    1    0   
$EndComp
$Comp
L C C33
U 1 1 5891C135
P 850 6100
F 0 "C33" H 700 6200 50  0000 L CNN
F 1 "0.1uF" H 650 6000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 888 5950 50  0001 C CNN
F 3 "" H 850 6100 50  0000 C CNN
F 4 "Capacitor" H 850 6100 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    850  6100
	0    1    1    0   
$EndComp
$Comp
L C C34
U 1 1 5891C3D3
P 1050 6200
F 0 "C34" H 1075 6300 50  0000 L CNN
F 1 "0.1uF" H 1075 6100 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1088 6050 50  0001 C CNN
F 3 "" H 1050 6200 50  0000 C CNN
F 4 "Capacitor" H 1050 6200 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    1050 6200
	0    1    1    0   
$EndComp
$Comp
L GND #PWR032
U 1 1 5891D6EE
P 2650 6600
F 0 "#PWR032" H 2650 6350 50  0001 C CNN
F 1 "GND" H 2650 6450 50  0000 C CNN
F 2 "" H 2650 6600 50  0000 C CNN
F 3 "" H 2650 6600 50  0000 C CNN
	1    2650 6600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR033
U 1 1 5891D7A8
P 650 6550
F 0 "#PWR033" H 650 6300 50  0001 C CNN
F 1 "GND" H 650 6400 50  0000 C CNN
F 2 "" H 650 6550 50  0000 C CNN
F 3 "" H 650 6550 50  0000 C CNN
	1    650  6550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 5891DF81
P 1900 6850
F 0 "#PWR034" H 1900 6600 50  0001 C CNN
F 1 "GND" H 1900 6700 50  0000 C CNN
F 2 "" H 1900 6850 50  0000 C CNN
F 3 "" H 1900 6850 50  0000 C CNN
	1    1900 6850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR035
U 1 1 5892046A
P 3750 7600
F 0 "#PWR035" H 3750 7350 50  0001 C CNN
F 1 "GND" H 3750 7450 50  0000 C CNN
F 2 "" H 3750 7600 50  0000 C CNN
F 3 "" H 3750 7600 50  0000 C CNN
	1    3750 7600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 5892082A
P 2200 7600
F 0 "#PWR036" H 2200 7350 50  0001 C CNN
F 1 "GND" H 2200 7450 50  0000 C CNN
F 2 "" H 2200 7600 50  0000 C CNN
F 3 "" H 2200 7600 50  0000 C CNN
	1    2200 7600
	1    0    0    -1  
$EndComp
$Comp
L MAX9723 U10
U 1 1 589930C2
P 4650 5350
F 0 "U10" H 4400 5800 60  0000 C CNN
F 1 "MAX9723" H 4650 5350 60  0000 C CNN
F 2 "Housings_DFN_QFN:QFN-16-1EP_4x4mm_Pitch0.65mm" H 4650 5200 60  0001 C CNN
F 3 "" H 4650 5200 60  0001 C CNN
F 4 "Headphone Amp" H 0   0   50  0001 C CNN "Description"
F 5 "Maxim Integrated" H 4650 5350 60  0001 C CNN "Manufacturer"
F 6 "MAX9723DETE+" H 4650 5350 60  0001 C CNN "MfgPartNum"
F 7 "17" H 0   0   50  0001 C CNN "NumPads"
F 8 "QFN16 (4x4)" H 0   0   50  0001 C CNN "Package"
F 9 "MAX9723DETE+-ND" H 4650 5350 60  0001 C CNN "Suppler1PartNum"
F 10 "Digikey" H 4650 5350 60  0001 C CNN "Supplier1"
F 11 "MAX9723DETE+-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    4650 5350
	1    0    0    -1  
$EndComp
$Comp
L C C36
U 1 1 58993A26
P 3650 5350
F 0 "C36" H 3675 5450 50  0000 L CNN
F 1 "0.47uF" H 3675 5250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3688 5200 50  0001 C CNN
F 3 "" H 3650 5350 50  0000 C CNN
F 4 "Capacitor" H 3650 5350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    3650 5350
	0    1    1    0   
$EndComp
$Comp
L C C37
U 1 1 58993B0F
P 3650 5550
F 0 "C37" H 3675 5650 50  0000 L CNN
F 1 "0.47uF" H 3675 5450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3688 5400 50  0001 C CNN
F 3 "" H 3650 5550 50  0000 C CNN
F 4 "Capacitor" H 3650 5550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    3650 5550
	0    1    1    0   
$EndComp
$Comp
L C C38
U 1 1 58993BF0
P 3650 5800
F 0 "C38" H 3675 5900 50  0000 L CNN
F 1 "1uF" H 3675 5700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3688 5650 50  0001 C CNN
F 3 "" H 3650 5800 50  0000 C CNN
F 4 "Capacitor" H 3650 5800 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    3650 5800
	0    1    1    0   
$EndComp
$Comp
L C C39
U 1 1 58993CEF
P 4200 6200
F 0 "C39" H 4225 6300 50  0000 L CNN
F 1 "1uF" H 4225 6100 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4238 6050 50  0001 C CNN
F 3 "" H 4200 6200 50  0000 C CNN
F 4 "Capacitor" H 0   0   50  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "16V" H 0   0   50  0001 C CNN "Voltage"
	1    4200 6200
	1    0    0    -1  
$EndComp
$Comp
L C C40
U 1 1 58993DC8
P 4950 4750
F 0 "C40" H 4975 4850 50  0000 L CNN
F 1 "1uF" H 4975 4650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4988 4600 50  0001 C CNN
F 3 "" H 4950 4750 50  0000 C CNN
F 4 "Capacitor" H 4950 4750 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    4950 4750
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR037
U 1 1 58994E01
P 4650 4650
F 0 "#PWR037" H 4650 4500 50  0001 C CNN
F 1 "+3V3" H 4650 4790 50  0000 C CNN
F 2 "" H 4650 4650 50  0000 C CNN
F 3 "" H 4650 4650 50  0000 C CNN
	1    4650 4650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 58995A54
P 4200 6500
F 0 "#PWR038" H 4200 6250 50  0001 C CNN
F 1 "GND" H 4200 6350 50  0000 C CNN
F 2 "" H 4200 6500 50  0000 C CNN
F 3 "" H 4200 6500 50  0000 C CNN
	1    4200 6500
	1    0    0    -1  
$EndComp
$Comp
L TRRS CON2
U 1 1 58995D14
P 6550 5900
F 0 "CON2" H 6550 5500 60  0000 C CNN
F 1 "TRRS" H 6550 6000 60  0000 C CNN
F 2 "lib:TRRS_jack" H 6700 5900 60  0001 C CNN
F 3 "" H 6700 5900 60  0000 C CNN
F 4 "Headphone Jack" H 0   0   50  0001 C CNN "Description"
F 5 "CUI, Inc." H 6550 5900 60  0001 C CNN "Manufacturer"
F 6 "SJ-43516-SMT-TR" H 6550 5900 60  0001 C CNN "MfgPartNum"
F 7 "6" H 0   0   50  0001 C CNN "NumPads"
F 8 "Digikey" H 6550 5900 60  0001 C CNN "Supplier1"
F 9 "CP-43516SJCT-ND" H 6550 5900 60  0001 C CNN "Supplier1PartNum"
	1    6550 5900
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR039
U 1 1 5899635D
P 6050 6200
F 0 "#PWR039" H 6050 5950 50  0001 C CNN
F 1 "GND" H 6050 6050 50  0000 C CNN
F 2 "" H 6050 6200 50  0000 C CNN
F 3 "" H 6050 6200 50  0000 C CNN
	1    6050 6200
	1    0    0    -1  
$EndComp
$Comp
L R R30
U 1 1 58996626
P 5750 5200
F 0 "R30" V 5830 5200 50  0000 C CNN
F 1 "47k" V 5750 5200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5680 5200 50  0001 C CNN
F 3 "" H 5750 5200 50  0000 C CNN
F 4 "Resistor" V 5750 5200 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5750 5200
	0    1    1    0   
$EndComp
$Comp
L R R29
U 1 1 58996E0C
P 5600 5600
F 0 "R29" V 5680 5600 50  0000 C CNN
F 1 "47k" V 5600 5600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5530 5600 50  0001 C CNN
F 3 "" H 5600 5600 50  0000 C CNN
F 4 "Resistor" V 5600 5600 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5600 5600
	0    1    1    0   
$EndComp
$Comp
L R R27
U 1 1 58997798
P 5300 5900
F 0 "R27" V 5200 5900 50  0000 C CNN
F 1 "22k" V 5300 5900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5230 5900 50  0001 C CNN
F 3 "" H 5300 5900 50  0000 C CNN
F 4 "Resistor" V 5300 5900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5300 5900
	1    0    0    -1  
$EndComp
$Comp
L R R28
U 1 1 58997887
P 5400 5900
F 0 "R28" V 5480 5900 50  0000 C CNN
F 1 "22k" V 5400 5900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5330 5900 50  0001 C CNN
F 3 "" H 5400 5900 50  0000 C CNN
F 4 "Resistor" V 5400 5900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5400 5900
	1    0    0    -1  
$EndComp
$Comp
L C C41
U 1 1 58999DAB
P 5550 5900
F 0 "C41" H 5575 6000 50  0000 L CNN
F 1 "0.1uF" H 5575 5800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5588 5750 50  0001 C CNN
F 3 "" H 5550 5900 50  0000 C CNN
F 4 "Capacitor" H 5550 5900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    5550 5900
	1    0    0    -1  
$EndComp
$Comp
L C C42
U 1 1 58999E84
P 5750 5900
F 0 "C42" H 5775 6000 50  0000 L CNN
F 1 "0.1uF" H 5775 5800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5788 5750 50  0001 C CNN
F 3 "" H 5750 5900 50  0000 C CNN
F 4 "Capacitor" H 5750 5900 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    5750 5900
	1    0    0    -1  
$EndComp
$Comp
L MAX4468 U11
U 1 1 5899EACE
P 5900 3050
F 0 "U11" H 6250 3300 60  0000 C CNN
F 1 "MAX4468" H 5900 3050 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-8" H 5900 3050 60  0001 C CNN
F 3 "" H 5900 3050 60  0001 C CNN
F 4 "Microphone Amp" H 0   0   50  0001 C CNN "Description"
F 5 "Maxim Integrated" H 5900 3050 60  0001 C CNN "Manufacturer"
F 6 "MAX4468EKA+T" H 5900 3050 60  0001 C CNN "MfgPartNum"
F 7 "8" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23-8" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 5900 3050 60  0001 C CNN "Supplier1"
F 10 "MAX4468EKA+CT-ND" H 5900 3050 60  0001 C CNN "Supplier1PartNum"
	1    5900 3050
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR040
U 1 1 5899F749
P 6550 2750
F 0 "#PWR040" H 6550 2600 50  0001 C CNN
F 1 "+3V3" H 6550 2890 50  0000 C CNN
F 2 "" H 6550 2750 50  0000 C CNN
F 3 "" H 6550 2750 50  0000 C CNN
	1    6550 2750
	1    0    0    -1  
$EndComp
$Comp
L R R32
U 1 1 589A0674
P 5000 3000
F 0 "R32" V 4900 3000 50  0000 C CNN
F 1 "470k" V 5000 3000 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4930 3000 50  0001 C CNN
F 3 "" H 5000 3000 50  0000 C CNN
F 4 "Resistor" V 5000 3000 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5000 3000
	0    1    1    0   
$EndComp
$Comp
L R R31
U 1 1 589A0771
P 5450 2700
F 0 "R31" V 5350 2700 50  0000 C CNN
F 1 "1k" V 5450 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5380 2700 50  0001 C CNN
F 3 "" H 5450 2700 50  0000 C CNN
F 4 "Resistor" V 5450 2700 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5450 2700
	0    1    1    0   
$EndComp
$Comp
L R R33
U 1 1 589A0E53
P 5000 3100
F 0 "R33" V 5080 3100 50  0000 C CNN
F 1 "20k" V 5000 3100 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4930 3100 50  0001 C CNN
F 3 "" H 5000 3100 50  0000 C CNN
F 4 "Resistor" V 5000 3100 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5000 3100
	0    1    1    0   
$EndComp
$Comp
L R R35
U 1 1 589A1B4C
P 5850 2700
F 0 "R35" V 5750 2700 50  0000 C CNN
F 1 "2.2k" V 5850 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5780 2700 50  0001 C CNN
F 3 "" H 5850 2700 50  0000 C CNN
F 4 "Resistor" V 5850 2700 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5850 2700
	0    1    1    0   
$EndComp
$Comp
L C C47
U 1 1 589A1F34
P 5450 2500
F 0 "C47" H 5475 2600 50  0000 L CNN
F 1 "10uF" H 5475 2400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5488 2350 50  0001 C CNN
F 3 "" H 5450 2500 50  0000 C CNN
F 4 "Capacitor" H 5450 2500 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 20%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "Ceramic" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    5450 2500
	0    -1   -1   0   
$EndComp
$Comp
L C C45
U 1 1 589A2AE8
P 4600 2950
F 0 "C45" H 4625 3050 50  0000 L CNN
F 1 "0.1uF" H 4625 2850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4638 2800 50  0001 C CNN
F 3 "" H 4600 2950 50  0000 C CNN
F 4 "Capacitor" H 4600 2950 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    4600 2950
	1    0    0    -1  
$EndComp
$Comp
L R R34
U 1 1 589A3516
P 5450 3450
F 0 "R34" V 5530 3450 50  0000 C CNN
F 1 "100k" V 5450 3450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5380 3450 50  0001 C CNN
F 3 "" H 5450 3450 50  0000 C CNN
F 4 "Resistor" V 5450 3450 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5450 3450
	0    1    1    0   
$EndComp
$Comp
L GND #PWR041
U 1 1 589A3C13
P 5750 3500
F 0 "#PWR041" H 5750 3250 50  0001 C CNN
F 1 "GND" H 5750 3350 50  0000 C CNN
F 2 "" H 5750 3500 50  0000 C CNN
F 3 "" H 5750 3500 50  0000 C CNN
	1    5750 3500
	1    0    0    -1  
$EndComp
$Comp
L C C46
U 1 1 589A4188
P 5450 3650
F 0 "C46" H 5475 3750 50  0000 L CNN
F 1 "0.01uF" H 5475 3550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5488 3500 50  0001 C CNN
F 3 "" H 5450 3650 50  0000 C CNN
F 4 "Capacitor" H 5450 3650 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    5450 3650
	0    1    1    0   
$EndComp
$Comp
L R R36
U 1 1 589A4A75
P 6750 3200
F 0 "R36" V 6800 3000 50  0000 C CNN
F 1 "200k" V 6750 3200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6680 3200 50  0001 C CNN
F 3 "" H 6750 3200 50  0000 C CNN
F 4 "Resistor" V 6750 3200 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    6750 3200
	0    1    1    0   
$EndComp
$Comp
L C C48
U 1 1 589A4B7E
P 6750 3350
F 0 "C48" H 6775 3450 50  0000 L CNN
F 1 "47pF" H 6775 3250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6788 3200 50  0001 C CNN
F 3 "" H 6750 3350 50  0000 C CNN
F 4 "Capacitor" H 6750 3350 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "+/- 10%" H 0   0   50  0001 C CNN "Tolerance"
F 8 "NP0/C0G" H 0   0   50  0001 C CNN "Type"
F 9 "6.3V" H 0   0   50  0001 C CNN "Voltage"
	1    6750 3350
	0    1    1    0   
$EndComp
$Comp
L TEST_1P W1
U 1 1 58AADE97
P 9550 5200
F 0 "W1" H 9550 5470 50  0000 C CNN
F 1 "T0" H 9550 5400 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 9750 5200 50  0001 C CNN
F 3 "" H 9750 5200 50  0000 C CNN
F 4 "Touch Pad" H 0   0   50  0001 C CNN "Description"
F 5 "0" H 0   0   50  0001 C CNN "NumPads"
F 6 "DNP" H 0   0   50  0001 C CNN "Package"
	1    9550 5200
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P W3
U 1 1 58AAE579
P 6500 5200
F 0 "W3" H 6500 5470 50  0000 C CNN
F 1 "T2" H 6500 5400 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 6700 5200 50  0001 C CNN
F 3 "" H 6700 5200 50  0000 C CNN
F 4 "Touch Pad" H 0   0   50  0001 C CNN "Description"
F 5 "0" H 0   0   50  0001 C CNN "NumPads"
F 6 "DNP" H 0   0   50  0001 C CNN "Package"
	1    6500 5200
	0    -1   -1   0   
$EndComp
$Comp
L R R38
U 1 1 58BBA8A4
P 3150 2150
F 0 "R38" V 3050 2150 50  0000 C CNN
F 1 "160k" V 3150 2150 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 3080 2150 50  0001 C CNN
F 3 "" H 3150 2150 50  0000 C CNN
F 4 "Resistor" V 3150 2150 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    3150 2150
	0    1    1    0   
$EndComp
$Comp
L R R37
U 1 1 58BBA9E5
P 2750 2150
F 0 "R37" V 2650 2150 50  0000 C CNN
F 1 "100k" V 2750 2150 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2680 2150 50  0001 C CNN
F 3 "" H 2750 2150 50  0000 C CNN
F 4 "Resistor" V 2750 2150 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    2750 2150
	0    1    1    0   
$EndComp
$Comp
L R R40
U 1 1 58BBBC11
P 5100 1600
F 0 "R40" V 5180 1600 50  0000 C CNN
F 1 "100k" V 5100 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5030 1600 50  0001 C CNN
F 3 "" H 5100 1600 50  0000 C CNN
F 4 "Resistor" V 5100 1600 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    5100 1600
	0    1    1    0   
$EndComp
$Comp
L R R39
U 1 1 58BBBD14
P 4750 1600
F 0 "R39" V 4830 1600 50  0000 C CNN
F 1 "100k" V 4750 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4680 1600 50  0001 C CNN
F 3 "" H 4750 1600 50  0000 C CNN
F 4 "Resistor" V 4750 1600 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
F 7 "0.05" H 0   0   50  0001 C CNN "Tolerance"
	1    4750 1600
	0    1    1    0   
$EndComp
$Comp
L TEST_1P W4
U 1 1 58BDA492
P 4450 3450
F 0 "W4" H 4450 3720 50  0000 C CNN
F 1 "MISO" H 4450 3650 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 4650 3450 50  0001 C CNN
F 3 "" H 4650 3450 50  0000 C CNN
F 4 "Test Pad" H 0   0   50  0001 C CNN "Description"
F 5 "0" H 0   0   50  0001 C CNN "NumPads"
F 6 "DNP" H 0   0   50  0001 C CNN "Package"
	1    4450 3450
	1    0    0    -1  
$EndComp
$Comp
L R R41
U 1 1 58F4F829
P 700 3600
F 0 "R41" V 780 3600 50  0000 C CNN
F 1 "1k" V 700 3600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 630 3600 50  0001 C CNN
F 3 "" H 700 3600 50  0001 C CNN
F 4 "Resistor" V 700 3600 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    700  3600
	1    0    0    -1  
$EndComp
$Comp
L PAM2305 U6
U 1 1 58F7FB50
P 7400 2300
F 0 "U6" H 7200 2600 60  0000 C CNN
F 1 "PAM2305" H 7400 2350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-5" H 7250 2400 60  0001 C CNN
F 3 "" H 7250 2400 60  0001 C CNN
F 4 "3.3V Regulator" H 0   0   50  0001 C CNN "Description"
F 5 "Diodes Incorporated" H 7400 2300 60  0001 C CNN "Manufacturer"
F 6 "PAM2305AAB330" H 7400 2300 60  0001 C CNN "MfgPartNum"
F 7 "5" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23-5" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 7400 2300 60  0001 C CNN "Supplier1"
F 10 "PAM2305AAB330DIDKR-ND" H 7400 2300 60  0001 C CNN "Supplier1PartNum"
	1    7400 2300
	1    0    0    -1  
$EndComp
$Comp
L R R42
U 1 1 58FDD6A5
P 6600 6600
F 0 "R42" V 6600 6850 50  0000 C CNN
F 1 "10k" V 6600 6600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 6600 50  0001 C CNN
F 3 "" H 6600 6600 50  0001 C CNN
F 4 "Resistor" V 6600 6600 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6600 6600
	0    1    1    0   
$EndComp
$Comp
L R R43
U 1 1 58FDD8B4
P 6600 6700
F 0 "R43" V 6600 6950 50  0000 C CNN
F 1 "10k" V 6600 6700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 6700 50  0001 C CNN
F 3 "" H 6600 6700 50  0001 C CNN
F 4 "Resistor" V 6600 6700 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6600 6700
	0    1    1    0   
$EndComp
$Comp
L R R44
U 1 1 58FDDA9A
P 6600 6800
F 0 "R44" V 6600 7050 50  0000 C CNN
F 1 "10k" V 6600 6800 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 6800 50  0001 C CNN
F 3 "" H 6600 6800 50  0001 C CNN
F 4 "Resistor" V 6600 6800 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6600 6800
	0    1    1    0   
$EndComp
$Comp
L R R45
U 1 1 58FDDC7A
P 6600 7000
F 0 "R45" V 6600 7250 50  0000 C CNN
F 1 "10k" V 6600 7000 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 7000 50  0001 C CNN
F 3 "" H 6600 7000 50  0001 C CNN
F 4 "Resistor" V 6600 7000 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6600 7000
	0    1    1    0   
$EndComp
$Comp
L R R46
U 1 1 58FDE58C
P 6600 7200
F 0 "R46" V 6600 7450 50  0000 C CNN
F 1 "10k" V 6600 7200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 7200 50  0001 C CNN
F 3 "" H 6600 7200 50  0001 C CNN
F 4 "Resistor" V 6600 7200 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6600 7200
	0    1    1    0   
$EndComp
$Comp
L R R47
U 1 1 58FDE778
P 6600 7300
F 0 "R47" V 6600 7550 50  0000 C CNN
F 1 "10k" V 6600 7300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 7300 50  0001 C CNN
F 3 "" H 6600 7300 50  0001 C CNN
F 4 "Resistor" V 6600 7300 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    6600 7300
	0    1    1    0   
$EndComp
$Comp
L R R48
U 1 1 58F9365A
P 2400 1050
F 0 "R48" V 2480 1050 50  0000 C CNN
F 1 "1k" V 2400 1050 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2330 1050 50  0001 C CNN
F 3 "" H 2400 1050 50  0001 C CNN
F 4 "Resistor" V 2400 1050 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    2400 1050
	1    0    0    -1  
$EndComp
$Comp
L R R49
U 1 1 58F93F5F
P 4150 1050
F 0 "R49" V 4230 1050 50  0000 C CNN
F 1 "1k" V 4150 1050 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4080 1050 50  0001 C CNN
F 3 "" H 4150 1050 50  0001 C CNN
F 4 "Resistor" V 4150 1050 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    4150 1050
	1    0    0    -1  
$EndComp
$Comp
L R R13
U 1 1 58FC3C5F
P 10450 5000
F 0 "R13" V 10350 5000 50  0000 C CNN
F 1 "100k" V 10450 5000 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 10380 5000 50  0001 C CNN
F 3 "" H 10450 5000 50  0000 C CNN
F 4 "Resistor" V 10450 5000 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    10450 5000
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q2
U 1 1 58FC4FDB
P 10700 4600
F 0 "Q2" H 10900 4650 50  0000 L CNN
F 1 "NMOS" H 10900 4550 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 10900 4700 50  0001 C CNN
F 3 "" H 10700 4600 50  0001 C CNN
F 4 "MOSFET Transistor" H 0   0   50  0001 C CNN "Description"
F 5 "Fairchild/ON Semiconductor" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "MMBF170" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "3" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "MMBF170CT-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    10700 4600
	1    0    0    -1  
$EndComp
$Comp
L R R50
U 1 1 58FC6716
P 10200 4600
F 0 "R50" V 10100 4600 50  0000 C CNN
F 1 "1k" V 10200 4600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 10130 4600 50  0001 C CNN
F 3 "" H 10200 4600 50  0000 C CNN
F 4 "Resistor" V 10200 4600 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    10200 4600
	0    1    1    0   
$EndComp
$Comp
L Q_NMOS_GSD Q3
U 1 1 58FC8714
P 2650 1250
F 0 "Q3" H 2850 1300 50  0000 L CNN
F 1 "NMOS" H 2850 1200 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 2850 1350 50  0001 C CNN
F 3 "" H 2650 1250 50  0001 C CNN
F 4 "MOSFET Transistor" H 0   0   50  0001 C CNN "Description"
F 5 "Fairchild/ON Semiconductor" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "MMBF170" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "3" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "MMBF170CT-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    2650 1250
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q4
U 1 1 58FC8B26
P 4400 1250
F 0 "Q4" H 4600 1300 50  0000 L CNN
F 1 "NMOS" H 4600 1200 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 4600 1350 50  0001 C CNN
F 3 "" H 4400 1250 50  0001 C CNN
F 4 "MOSFET Transistor" H 0   0   50  0001 C CNN "Description"
F 5 "Fairchild/ON Semiconductor" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "MMBF170" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "3" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "MMBF170CT-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    4400 1250
	1    0    0    -1  
$EndComp
$Comp
L R R20
U 1 1 58FCA08D
P 2400 1400
F 0 "R20" V 2480 1400 50  0000 C CNN
F 1 "100k" V 2400 1400 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2330 1400 50  0001 C CNN
F 3 "" H 2400 1400 50  0001 C CNN
F 4 "Resistor" V 2400 1400 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    2400 1400
	1    0    0    -1  
$EndComp
$Comp
L R R22
U 1 1 58FCA74F
P 4150 1400
F 0 "R22" V 4230 1400 50  0000 C CNN
F 1 "100k" V 4150 1400 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4080 1400 50  0001 C CNN
F 3 "" H 4150 1400 50  0001 C CNN
F 4 "Resistor" V 4150 1400 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    4150 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR042
U 1 1 592F6C27
P 9550 6300
F 0 "#PWR042" H 9550 6050 50  0001 C CNN
F 1 "GND" H 9550 6150 50  0000 C CNN
F 2 "" H 9550 6300 50  0000 C CNN
F 3 "" H 9550 6300 50  0000 C CNN
	1    9550 6300
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q5
U 1 1 592F85F9
P 8800 6100
F 0 "Q5" H 9000 6150 50  0000 L CNN
F 1 "NMOS" H 9000 6050 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 9000 6200 50  0001 C CNN
F 3 "" H 8800 6100 50  0001 C CNN
F 4 "MOSFET Transistor" H 0   0   50  0001 C CNN "Description"
F 5 "Fairchild/ON Semiconductor" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "MMBF170" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "3" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "MMBF170CT-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    8800 6100
	0    -1   1    0   
$EndComp
$Comp
L Q_NMOS_GSD Q6
U 1 1 592F87BA
P 9650 5550
F 0 "Q6" H 9850 5600 50  0000 L CNN
F 1 "NMOS" H 9850 5500 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 9850 5650 50  0001 C CNN
F 3 "" H 9650 5550 50  0001 C CNN
F 4 "MOSFET Transistor" H 0   0   50  0001 C CNN "Description"
F 5 "Fairchild/ON Semiconductor" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "MMBF170" H 0   0   50  0001 C CNN "MfgPartNum"
F 7 "3" H 0   0   50  0001 C CNN "NumPads"
F 8 "SOT-23" H 0   0   50  0001 C CNN "Package"
F 9 "Digikey" H 0   0   50  0001 C CNN "Supplier1"
F 10 "MMBF170CT-ND" H 0   0   50  0001 C CNN "Supplier1PartNum"
	1    9650 5550
	-1   0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 592FCF0B
P 10100 5550
F 0 "R11" V 10000 5550 50  0000 C CNN
F 1 "1k" V 10100 5550 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 10030 5550 50  0001 C CNN
F 3 "" H 10100 5550 50  0000 C CNN
F 4 "Resistor" V 10100 5550 60  0001 C CNN "Description"
F 5 "2" H 0   0   50  0001 C CNN "NumPads"
F 6 "0603" H 0   0   50  0001 C CNN "Package"
	1    10100 5550
	0    1    1    0   
$EndComp
Text Label 2350 2550 0    60   ~ 0
CSTAT
Text Label 3900 2150 0    60   ~ 0
VBAT
Text Label 2200 1850 0    60   ~ 0
VUSB
Text Label 2800 5250 1    60   ~ 0
VUSB
Text Label 4250 4250 0    60   ~ 0
SDA
Text Label 4250 4400 0    60   ~ 0
SCL
Text Label 3100 3300 1    60   ~ 0
USB_TX
Text Label 3250 3300 1    60   ~ 0
USB_RX
Text Label 8900 4250 0    60   ~ 0
ESP32_TX
Text Label 8900 4350 0    60   ~ 0
ESP32_RX
Text Label 9700 4250 0    60   ~ 0
USB_RX
Text Label 9700 4350 0    60   ~ 0
USB_TX
Text Label 5800 3750 0    60   ~ 0
ESP32_EN
Text Label 7650 6150 1    60   ~ 0
SD3
Text Label 6700 5400 0    60   ~ 0
SD2
Text Label 6700 5300 0    60   ~ 0
SDCLK
Text Label 8350 6150 1    60   ~ 0
SDCMD
Text Label 9000 4150 0    60   ~ 0
SCL
Text Label 9000 4450 0    60   ~ 0
SDA
Text Label 6700 5000 0    60   ~ 0
DAC_1
Text Label 6700 5100 0    60   ~ 0
DAC_2
Text Label 8450 6150 1    60   ~ 0
SD0
Text Label 6150 6600 0    60   ~ 0
SD2
Text Label 6150 6700 0    60   ~ 0
SD3
Text Label 6150 6800 0    60   ~ 0
SDCMD
Text Label 10000 2150 0    60   ~ 0
LCD_SCK
Text Label 10000 2250 0    60   ~ 0
LCD_SI
Text Label 10000 1250 0    60   ~ 0
LCD_DI
Text Label 10000 1150 0    60   ~ 0
LCD_RES
Text Label 10000 1050 0    60   ~ 0
LCD_CS
Text Label 8900 4050 0    60   ~ 0
LCD_SI
Text Label 8950 4750 0    60   ~ 0
LCD_SCK
Text Label 8950 4850 0    60   ~ 0
LCD_CS
Text Label 8950 4950 0    60   ~ 0
LCD_RES
Text Label 8950 5050 0    60   ~ 0
LCD_DI
Text Label 2950 5250 1    60   ~ 0
C2CK
Text Label 3100 5250 1    60   ~ 0
C2D
Text Label 9600 4600 0    60   ~ 0
LCD_EN
Text Label 1300 3750 0    60   ~ 0
C2D
Text Label 1300 3850 0    60   ~ 0
C2CK
Text Label 6150 7200 0    60   ~ 0
SD0
Text Label 6150 7300 0    60   ~ 0
SD1
Text Label 6150 7000 0    60   ~ 0
SDCLK
Text Label 2150 1350 1    60   ~ 0
BC_EN_1
Text Label 3900 1350 1    60   ~ 0
BC_EN_2
Text Label 1000 6400 0    60   ~ 0
SCL
Text Label 1500 6850 0    60   ~ 0
SDA
Text Label 600  7200 0    60   ~ 0
LED_SDI
Text Label 600  7350 0    60   ~ 0
LED_CKI
Text Label 2500 7350 0    60   ~ 0
ENC_A
Text Label 2500 7550 0    60   ~ 0
ENC_B
Text Label 3600 7350 0    60   ~ 0
ENC_SW
Text Label 3800 5050 0    60   ~ 0
SCL
Text Label 3800 5150 0    60   ~ 0
SDA
Text Label 6950 3000 2    60   ~ 0
MIC_SHDN
Text Label 6950 3100 2    60   ~ 0
MIC_AMP
Text Label 6650 6150 2    60   ~ 0
MIC_IN
Text Label 6400 2700 2    60   ~ 0
MIC_IN
Text Label 3200 5350 0    60   ~ 0
DAC_1
Text Label 3200 5550 0    60   ~ 0
DAC_2
Text Label 2150 3250 3    60   ~ 0
ENC_A
Text Label 2800 2950 3    60   ~ 0
ENC_B
Text Label 2950 2950 3    60   ~ 0
ENC_SW
Text Label 3250 5250 1    60   ~ 0
BC_EN_2
Text Label 3400 5250 1    60   ~ 0
BC_EN_1
Text Label 3550 5250 1    60   ~ 0
LCD_EN
Text Label 3700 5250 1    60   ~ 0
MIC_SHDN
Text Label 4200 3250 2    60   ~ 0
LED_CKI
Text Label 4300 3250 3    60   ~ 0
LED_SDI
Text Label 4550 4550 2    60   ~ 0
ESP32_EN
Text Label 4900 3750 2    60   ~ 0
CSTAT
Text Label 6700 4700 0    60   ~ 0
MIC_AMP
Text Label 5050 3600 2    60   ~ 0
ESP32_BOOT
Text Label 8950 5150 0    60   ~ 0
SD1
Text Label 4550 4800 2    60   ~ 0
ESP32_EN_DELAY
Text Label 6750 3750 3    60   ~ 0
ESP32_EN_DELAY
Text Label 8950 5250 0    60   ~ 0
IO0
Text Label 10800 5550 2    60   ~ 0
ESP32_BOOT
Wire Wire Line
	2150 1850 2150 2400
Wire Wire Line
	2550 2550 2350 2550
Wire Wire Line
	3450 2550 3550 2550
Wire Wire Line
	3550 2550 3550 2800
Wire Wire Line
	3850 2750 3850 2700
Wire Wire Line
	2150 2750 4200 2750
Wire Wire Line
	2150 2750 2150 2700
Wire Wire Line
	3850 2150 3850 2400
Wire Wire Line
	3450 2350 4200 2350
Wire Wire Line
	4200 2350 4200 2500
Wire Wire Line
	4200 2750 4200 2600
Wire Wire Line
	3300 2150 4100 2150
Wire Wire Line
	8900 5450 9050 5450
Wire Wire Line
	9050 5350 9050 5600
Wire Wire Line
	8900 5350 9050 5350
Wire Wire Line
	7050 5500 7000 5500
Wire Wire Line
	7000 5500 7000 5950
Wire Wire Line
	7550 5850 7550 6250
Wire Wire Line
	1900 2350 2550 2350
Wire Wire Line
	1150 2350 1700 2350
Wire Wire Line
	5750 2150 6900 2150
Wire Wire Line
	2150 1850 5900 1850
Wire Wire Line
	5150 1850 5150 2150
Wire Wire Line
	5100 2150 5350 2150
Wire Wire Line
	1150 2650 1850 2650
Wire Wire Line
	1150 2550 1950 2550
Wire Wire Line
	2250 3950 2100 3950
Wire Wire Line
	2100 3950 2100 4800
Wire Wire Line
	2800 4900 2800 5250
Wire Wire Line
	1250 4400 2250 4400
Wire Wire Line
	1650 4150 1650 4550
Wire Wire Line
	4200 4250 4700 4250
Wire Wire Line
	4200 4400 4500 4400
Wire Wire Line
	3100 3300 3100 2950
Wire Wire Line
	3250 3300 3250 2950
Wire Wire Line
	9350 4250 8900 4250
Wire Wire Line
	8900 4350 9350 4350
Wire Wire Line
	9650 4250 10100 4250
Wire Wire Line
	9650 4350 10100 4350
Wire Wire Line
	6900 4200 6900 4000
Wire Wire Line
	1650 4550 2250 4550
Wire Wire Line
	1250 4700 1250 4750
Wire Wire Line
	1250 4750 2100 4750
Wire Wire Line
	1450 4700 1450 4750
Wire Wire Line
	7050 4300 6750 4300
Wire Wire Line
	7650 5850 7650 6150
Wire Wire Line
	7050 5400 6700 5400
Wire Wire Line
	7050 5300 6700 5300
Wire Wire Line
	8350 5850 8350 6150
Wire Wire Line
	8900 4150 9350 4150
Wire Wire Line
	8900 4450 9350 4450
Wire Wire Line
	7050 5000 6700 5000
Wire Wire Line
	7050 5100 6700 5100
Wire Wire Line
	8450 5850 8450 6200
Wire Wire Line
	4500 4350 4700 4350
Wire Wire Line
	4500 4400 4500 4350
Wire Wire Line
	5600 4250 5700 4250
Wire Wire Line
	5700 4250 5700 4200
Wire Wire Line
	5600 4350 5700 4350
Wire Wire Line
	5700 4350 5700 4550
Wire Wire Line
	4500 4200 4500 4250
Wire Wire Line
	4650 4200 4650 4350
Wire Wire Line
	4500 3900 4500 3850
Wire Wire Line
	4500 3850 5850 3850
Wire Wire Line
	4650 3850 4650 3900
Wire Wire Line
	5850 3850 5850 4200
Wire Wire Line
	6100 6800 6450 6800
Wire Wire Line
	6100 6700 6450 6700
Wire Wire Line
	6100 6600 6450 6600
Wire Wire Line
	10400 2750 9850 2750
Wire Wire Line
	9850 2650 9850 2850
Wire Wire Line
	9850 2850 9900 2850
Wire Wire Line
	10200 2850 10400 2850
Wire Wire Line
	10400 2950 9850 2950
Wire Wire Line
	9850 2950 9850 3050
Wire Wire Line
	9850 3050 9900 3050
Wire Wire Line
	10200 3050 10400 3050
Wire Wire Line
	10200 2550 10400 2550
Wire Wire Line
	9650 2450 10400 2450
Wire Wire Line
	9550 2350 9550 2200
Wire Wire Line
	9300 2350 9300 2600
Wire Wire Line
	9300 2900 9300 3150
Wire Wire Line
	9500 2550 9900 2550
Wire Wire Line
	9500 3150 9500 2550
Wire Wire Line
	10400 3150 10350 3150
Wire Wire Line
	10400 3250 10200 3250
Wire Wire Line
	10400 3350 10350 3350
Wire Wire Line
	10400 3450 10200 3450
Wire Wire Line
	10400 3550 10350 3550
Wire Wire Line
	9100 3150 10050 3150
Wire Wire Line
	9850 3150 9850 3750
Wire Wire Line
	9850 3450 9900 3450
Wire Wire Line
	10050 3350 9850 3350
Wire Wire Line
	9850 3550 10050 3550
Wire Wire Line
	9100 2350 10400 2350
Wire Wire Line
	9650 2450 9650 3300
Wire Wire Line
	10400 1350 10300 1350
Wire Wire Line
	10300 1350 10300 2050
Wire Wire Line
	10300 1450 10400 1450
Wire Wire Line
	10300 1550 10400 1550
Wire Wire Line
	9550 1650 10400 1650
Wire Wire Line
	10300 1750 10400 1750
Wire Wire Line
	10300 1850 10400 1850
Wire Wire Line
	10300 1950 10400 1950
Wire Wire Line
	10300 2050 10400 2050
Wire Wire Line
	10400 2150 10000 2150
Wire Wire Line
	10400 2250 10000 2250
Wire Wire Line
	10400 1250 10000 1250
Wire Wire Line
	10400 1150 10000 1150
Wire Wire Line
	10400 1050 10000 1050
Wire Wire Line
	9850 3750 10400 3750
Wire Wire Line
	10400 3650 9850 3650
Wire Wire Line
	8900 4050 9350 4050
Wire Wire Line
	8900 4750 9350 4750
Wire Wire Line
	8900 4850 9350 4850
Wire Wire Line
	8900 4950 9350 4950
Wire Wire Line
	8900 5050 9350 5050
Wire Wire Line
	2950 4900 2950 5250
Wire Wire Line
	3100 4900 3100 5250
Wire Wire Line
	5850 4200 5700 4200
Wire Wire Line
	5500 4500 5850 4500
Wire Wire Line
	5100 3800 5100 3850
Wire Wire Line
	6100 4200 7050 4200
Wire Wire Line
	6100 4500 6600 4500
Wire Wire Line
	6250 4550 6250 4500
Wire Wire Line
	6500 4900 7050 4900
Wire Wire Line
	7050 4800 6650 4800
Wire Wire Line
	6650 4800 6650 4700
Wire Wire Line
	6650 4700 6500 4700
Wire Wire Line
	700  3750 750  3750
Wire Wire Line
	750  3850 700  3850
Wire Wire Line
	700  3850 700  3950
Wire Wire Line
	1250 3750 1500 3750
Wire Wire Line
	1250 3850 1600 3850
Wire Wire Line
	1600 3400 1150 3400
Wire Wire Line
	5250 1600 5250 2150
Wire Wire Line
	5250 2500 5250 2450
Wire Wire Line
	5150 2500 5300 2500
Wire Wire Line
	550  2950 850  2950
Wire Wire Line
	1600 3850 1600 3400
Wire Wire Line
	9100 2350 9100 2600
Wire Wire Line
	9100 2900 9100 3150
Wire Wire Line
	1250 2200 1250 2350
Wire Wire Line
	1400 1400 550  1400
Wire Wire Line
	550  1400 550  2950
Wire Wire Line
	1400 1400 1400 1450
Wire Wire Line
	6550 1250 6900 1250
Wire Wire Line
	6800 1000 6800 2500
Wire Wire Line
	6900 1350 6800 1350
Wire Wire Line
	6800 1000 7200 1000
Wire Wire Line
	7500 1000 7850 1000
Wire Wire Line
	7800 1000 7800 1250
Wire Wire Line
	7800 1250 7750 1250
Wire Wire Line
	8250 1050 8250 1000
Wire Wire Line
	6900 1800 8500 1800
Wire Wire Line
	6900 1450 6900 1800
Wire Wire Line
	7800 1900 7800 1800
Wire Wire Line
	8250 1350 8250 1450
Wire Wire Line
	8250 1400 7850 1400
Wire Wire Line
	7850 1400 7850 1350
Wire Wire Line
	7850 1350 7750 1350
Wire Wire Line
	8250 1800 8250 1750
Wire Wire Line
	6550 1250 6550 1300
Wire Wire Line
	6550 1600 6550 1650
Wire Wire Line
	6550 1650 6900 1650
Wire Wire Line
	8500 1800 8500 1550
Wire Wire Line
	8500 900  8500 1250
Wire Wire Line
	6200 4700 6200 4900
Wire Wire Line
	6200 4800 6100 4800
Wire Wire Line
	6100 4800 6100 4850
Wire Wire Line
	6750 4300 6750 3750
Wire Wire Line
	6750 3750 6550 3750
Wire Wire Line
	6600 4500 6600 4100
Wire Wire Line
	6600 3800 6600 3750
Wire Wire Line
	6250 3750 5800 3750
Wire Wire Line
	6750 6900 6100 6900
Wire Wire Line
	6100 7000 6450 7000
Wire Wire Line
	6100 7200 6450 7200
Wire Wire Line
	6100 7300 6450 7300
Wire Wire Line
	6400 7100 6400 7600
Wire Wire Line
	6400 7100 6100 7100
Wire Wire Line
	4400 7500 4300 7500
Wire Wire Line
	4300 7500 4300 7600
Wire Wire Line
	6800 2350 6900 2350
Wire Wire Line
	7400 2800 7400 2900
Wire Wire Line
	8050 2150 7900 2150
Wire Wire Line
	7900 2350 8450 2350
Wire Wire Line
	8450 2350 8450 2150
Wire Wire Line
	8350 2150 8750 2150
Wire Wire Line
	8750 1900 8750 2200
Wire Wire Line
	8750 2850 8750 2500
Wire Wire Line
	6800 2850 8750 2850
Wire Wire Line
	6800 2850 6800 2800
Wire Wire Line
	9900 3250 9850 3250
Wire Wire Line
	10400 2650 10350 2650
Wire Wire Line
	10050 2650 9850 2650
Wire Wire Line
	9550 900  9550 1650
Wire Wire Line
	2750 650  2750 550 
Wire Wire Line
	2750 550  4500 550 
Wire Wire Line
	4500 550  4500 650 
Wire Wire Line
	3450 2450 3550 2450
Wire Wire Line
	3550 2450 3550 550 
Wire Wire Line
	2750 1450 2750 1650
Wire Wire Line
	4500 1450 4500 1650
Wire Wire Line
	4150 1600 4600 1600
Wire Wire Line
	3900 1350 3900 850 
Wire Wire Line
	2150 1350 2150 850 
Wire Wire Line
	2000 5700 2000 5800
Wire Wire Line
	1000 6100 1250 6100
Wire Wire Line
	1150 6100 1150 6000
Wire Wire Line
	2550 6200 2650 6200
Wire Wire Line
	2650 5750 2650 6600
Wire Wire Line
	2650 5750 2450 5750
Wire Wire Line
	2150 5750 2000 5750
Wire Wire Line
	1250 6200 1200 6200
Wire Wire Line
	900  6200 650  6200
Wire Wire Line
	650  6100 650  6550
Wire Wire Line
	650  6100 700  6100
Wire Wire Line
	650  6500 1250 6500
Wire Wire Line
	2650 6400 2550 6400
Wire Wire Line
	1250 6400 1000 6400
Wire Wire Line
	1800 6800 1800 6850
Wire Wire Line
	1800 6850 1500 6850
Wire Wire Line
	1900 6800 1900 6850
Wire Wire Line
	1150 7200 600  7200
Wire Wire Line
	1150 7350 600  7350
Wire Wire Line
	2450 7350 2950 7350
Wire Wire Line
	2200 7450 2950 7450
Wire Wire Line
	2450 7550 2950 7550
Wire Wire Line
	3550 7350 4000 7350
Wire Wire Line
	3550 7550 3750 7550
Wire Wire Line
	3750 7550 3750 7600
Wire Wire Line
	2200 7450 2200 7600
Wire Wire Line
	4050 5050 3800 5050
Wire Wire Line
	4050 5150 3800 5150
Wire Wire Line
	3800 5350 4050 5350
Wire Wire Line
	4050 5450 3900 5450
Wire Wire Line
	3900 5450 3900 5550
Wire Wire Line
	3900 5550 3800 5550
Wire Wire Line
	4050 5550 3950 5550
Wire Wire Line
	3950 5550 3950 5700
Wire Wire Line
	3950 5700 3450 5700
Wire Wire Line
	3450 5700 3450 5800
Wire Wire Line
	3450 5800 3500 5800
Wire Wire Line
	3800 5800 4000 5800
Wire Wire Line
	4000 5800 4000 5650
Wire Wire Line
	4000 5650 4050 5650
Wire Wire Line
	3500 5350 3200 5350
Wire Wire Line
	3500 5550 3200 5550
Wire Wire Line
	4650 4750 4650 4650
Wire Wire Line
	5500 4500 5500 4750
Wire Wire Line
	5500 4750 5100 4750
Wire Wire Line
	4200 6050 4600 6050
Wire Wire Line
	4700 6050 4700 6200
Wire Wire Line
	4350 6200 4800 6200
Wire Wire Line
	4350 6200 4350 6400
Wire Wire Line
	4350 6400 4200 6400
Wire Wire Line
	4200 6350 4200 6500
Wire Wire Line
	6150 5700 6200 5700
Wire Wire Line
	6150 5050 6150 5700
Wire Wire Line
	6150 5050 5250 5050
Wire Wire Line
	5950 5800 6200 5800
Wire Wire Line
	5950 5450 5950 5800
Wire Wire Line
	5950 5450 5250 5450
Wire Wire Line
	6200 5600 6050 5600
Wire Wire Line
	6050 5600 6050 6200
Wire Wire Line
	6150 5200 5900 5200
Wire Wire Line
	5250 5200 5600 5200
Wire Wire Line
	5250 5600 5450 5600
Wire Wire Line
	5950 5600 5750 5600
Wire Wire Line
	5300 5200 5300 5750
Wire Wire Line
	5300 6050 5300 6100
Wire Wire Line
	5300 6100 6050 6100
Wire Wire Line
	5400 6100 5400 6050
Wire Wire Line
	5400 5500 5400 5750
Wire Wire Line
	5550 5750 5550 5700
Wire Wire Line
	5550 5700 5300 5700
Wire Wire Line
	5750 5750 5750 5500
Wire Wire Line
	5750 5500 5400 5500
Wire Wire Line
	5550 6050 5550 6100
Wire Wire Line
	5750 6050 5750 6100
Wire Wire Line
	4800 6200 4800 6050
Wire Wire Line
	4050 5250 4050 4800
Wire Wire Line
	6550 3000 6950 3000
Wire Wire Line
	6550 2900 6550 2900
Wire Wire Line
	6550 2900 6550 2750
Wire Wire Line
	6550 3100 6950 3100
Wire Wire Line
	6200 5900 6150 5900
Wire Wire Line
	6150 5900 6150 6150
Wire Wire Line
	6150 6150 6650 6150
Wire Wire Line
	5300 2900 4800 2900
Wire Wire Line
	5150 3000 5300 3000
Wire Wire Line
	6000 2700 6400 2700
Wire Wire Line
	6050 2800 6050 2700
Wire Wire Line
	5150 3100 5300 3100
Wire Wire Line
	5300 3450 5250 3450
Wire Wire Line
	5750 3300 5750 3500
Wire Wire Line
	5600 3450 5750 3450
Wire Wire Line
	5750 3300 5300 3300
Wire Wire Line
	5300 3300 5300 3200
Wire Wire Line
	5250 3650 5300 3650
Wire Wire Line
	5650 3650 5600 3650
Wire Wire Line
	5650 3450 5650 3650
Wire Wire Line
	6950 3100 6950 3350
Wire Wire Line
	6950 3200 6900 3200
Wire Wire Line
	6600 3200 6400 3200
Wire Wire Line
	6400 3200 6400 3350
Wire Wire Line
	5200 3100 5200 3350
Wire Wire Line
	6950 3350 6900 3350
Wire Wire Line
	5200 3350 6600 3350
Wire Wire Line
	2250 3800 1750 3800
Wire Wire Line
	1750 3800 1750 5450
Wire Wire Line
	1750 5450 2750 5450
Wire Wire Line
	2750 5450 2750 6300
Wire Wire Line
	2750 6300 2550 6300
Wire Wire Line
	2950 3300 2950 2950
Wire Wire Line
	2800 3300 2800 2950
Wire Wire Line
	2250 3650 2150 3650
Wire Wire Line
	2150 3650 2150 3250
Wire Wire Line
	3250 4900 3250 5250
Wire Wire Line
	3400 4900 3400 5250
Wire Wire Line
	3550 4900 3550 5250
Wire Wire Line
	3700 4900 3700 5250
Wire Wire Line
	2900 2150 3000 2150
Wire Wire Line
	2600 2150 2300 2150
Wire Wire Line
	2300 2150 2300 2750
Wire Wire Line
	3650 3050 3650 2000
Wire Wire Line
	3650 2000 2950 2000
Wire Wire Line
	2950 2000 2950 2150
Wire Wire Line
	4950 1600 4900 1600
Wire Wire Line
	4900 1600 4900 2000
Wire Wire Line
	4900 2000 3750 2000
Wire Wire Line
	3750 2000 3750 3150
Wire Wire Line
	3750 3150 3400 3150
Wire Wire Line
	5600 2700 5700 2700
Wire Wire Line
	4800 3000 4850 3000
Wire Wire Line
	4800 2700 4800 3000
Wire Wire Line
	5300 2700 4800 2700
Wire Wire Line
	4850 3100 4600 3100
Wire Wire Line
	4600 2800 6050 2800
Wire Wire Line
	5650 2500 5650 2700
Wire Wire Line
	5150 2500 5150 2450
Wire Wire Line
	5650 2500 5600 2500
Wire Wire Line
	5250 3000 5250 3650
Wire Wire Line
	3700 3300 3700 3250
Wire Wire Line
	3700 3250 4200 3250
Wire Wire Line
	4200 3800 4300 3800
Wire Wire Line
	4300 3800 4300 3250
Wire Wire Line
	4200 3650 4450 3650
Wire Wire Line
	4450 3650 4450 3450
Wire Wire Line
	4200 4550 4550 4550
Wire Wire Line
	3400 3150 3400 3300
Wire Wire Line
	3650 3050 3550 3050
Wire Wire Line
	3550 3050 3550 3300
Wire Wire Line
	4200 4100 4400 4100
Wire Wire Line
	4400 4100 4400 3750
Wire Wire Line
	4400 3750 4900 3750
Wire Wire Line
	7050 4700 6700 4700
Wire Wire Line
	850  2950 850  3150
Wire Wire Line
	700  3200 700  3450
Wire Wire Line
	850  3400 700  3400
Wire Wire Line
	4050 4800 4550 4800
Wire Wire Line
	4800 4750 4650 4750
Wire Wire Line
	10650 4250 10900 4250
Wire Wire Line
	4200 3950 4350 3950
Wire Wire Line
	4350 3950 4350 3700
Wire Wire Line
	4350 3700 4500 3700
Wire Wire Line
	4500 3700 4500 3600
Wire Wire Line
	4500 3600 5050 3600
Wire Wire Line
	8900 5150 9350 5150
Wire Wire Line
	6450 7550 6400 7550
Wire Wire Line
	6750 6500 6750 7550
Wire Wire Line
	10050 4600 9600 4600
Wire Wire Line
	10350 4250 10250 4250
Wire Wire Line
	10250 4250 10250 4100
Wire Wire Line
	10900 4150 10800 4150
Wire Wire Line
	10800 4150 10800 4400
Wire Wire Line
	10800 4800 10800 5250
Wire Wire Line
	10450 5150 10450 5200
Wire Wire Line
	10450 5200 10800 5200
Wire Wire Line
	10350 4600 10500 4600
Wire Wire Line
	10450 4600 10450 4850
Wire Wire Line
	4500 1050 4500 950 
Wire Wire Line
	2750 1050 2750 950 
Wire Wire Line
	2400 1550 2750 1550
Wire Wire Line
	2450 1250 2400 1250
Wire Wire Line
	4150 1600 4150 1550
Wire Wire Line
	4200 1250 4150 1250
Wire Wire Line
	4150 1250 4150 1200
Wire Wire Line
	2400 1250 2400 1200
Wire Wire Line
	4150 900  4150 850 
Wire Wire Line
	4150 850  3900 850 
Wire Wire Line
	2150 850  2400 850 
Wire Wire Line
	2400 850  2400 900 
Wire Wire Line
	7050 5200 6500 5200
Wire Wire Line
	8150 1000 8500 1000
Wire Wire Line
	1400 2200 1400 2650
Wire Wire Line
	1550 2200 1550 2550
Wire Wire Line
	1850 2650 1850 4250
Wire Wire Line
	1850 4250 2250 4250
Wire Wire Line
	2250 4100 1950 4100
Wire Wire Line
	1950 4100 1950 2550
Wire Wire Line
	8900 5250 9550 5250
Wire Wire Line
	9550 5200 9550 5350
Wire Wire Line
	9850 5550 9950 5550
Wire Wire Line
	10250 5550 10800 5550
Wire Wire Line
	8450 6200 8600 6200
Wire Wire Line
	8800 5900 8800 5850
Wire Wire Line
	8800 5850 9900 5850
Wire Wire Line
	9900 5850 9900 5550
Wire Wire Line
	9550 5750 9550 6300
Wire Wire Line
	9000 6200 9550 6200
Connection ~ 2150 2350
Connection ~ 3550 2750
Connection ~ 3850 2350
Connection ~ 3850 2750
Connection ~ 9050 5450
Connection ~ 5550 1850
Connection ~ 5150 1850
Connection ~ 5900 2150
Connection ~ 1650 4400
Connection ~ 1450 4400
Connection ~ 2100 4750
Connection ~ 1450 4750
Connection ~ 6900 4200
Connection ~ 5850 4200
Connection ~ 5700 4500
Connection ~ 4500 4250
Connection ~ 4650 4350
Connection ~ 4650 3850
Connection ~ 9550 2350
Connection ~ 9500 3150
Connection ~ 9850 3250
Connection ~ 9850 3350
Connection ~ 9850 3450
Connection ~ 9650 3150
Connection ~ 10300 1450
Connection ~ 10300 1550
Connection ~ 10300 1650
Connection ~ 10300 1750
Connection ~ 10300 1850
Connection ~ 10300 1950
Connection ~ 9850 3550
Connection ~ 9850 3650
Connection ~ 5100 3850
Connection ~ 6350 4200
Connection ~ 6250 4500
Connection ~ 5250 2150
Connection ~ 9300 2350
Connection ~ 9300 3150
Connection ~ 1250 2350
Connection ~ 750  2950
Connection ~ 6800 2150
Connection ~ 6800 1350
Connection ~ 6800 1250
Connection ~ 7800 1000
Connection ~ 7800 1800
Connection ~ 8250 1400
Connection ~ 6900 1650
Connection ~ 8250 1800
Connection ~ 8250 1000
Connection ~ 8500 1000
Connection ~ 6200 4800
Connection ~ 6500 4900
Connection ~ 6500 4700
Connection ~ 6350 4500
Connection ~ 6600 3750
Connection ~ 6750 6900
Connection ~ 8450 2150
Connection ~ 7400 2850
Connection ~ 8750 2150
Connection ~ 6800 2350
Connection ~ 9850 2750
Connection ~ 9850 3150
Connection ~ 3550 550 
Connection ~ 4500 1600
Connection ~ 2000 5750
Connection ~ 1150 6100
Connection ~ 650  6200
Connection ~ 2650 6200
Connection ~ 2650 6400
Connection ~ 650  6500
Connection ~ 4500 6050
Connection ~ 4700 6200
Connection ~ 4200 6400
Connection ~ 6150 5200
Connection ~ 5950 5600
Connection ~ 5300 5200
Connection ~ 5400 5600
Connection ~ 6050 6100
Connection ~ 5400 6100
Connection ~ 5300 5700
Connection ~ 5550 6100
Connection ~ 5750 6100
Connection ~ 4650 4750
Connection ~ 6050 2700
Connection ~ 5750 3450
Connection ~ 5650 3450
Connection ~ 5200 3100
Connection ~ 6950 3200
Connection ~ 3850 2150
Connection ~ 2300 2750
Connection ~ 2950 2150
Connection ~ 4900 1600
Connection ~ 5650 2700
Connection ~ 4800 2900
Connection ~ 6400 3350
Connection ~ 5250 2500
Connection ~ 5250 3000
Connection ~ 5250 3450
Connection ~ 850  2950
Connection ~ 700  3400
Connection ~ 6400 7550
Connection ~ 6750 7300
Connection ~ 6750 7200
Connection ~ 6750 7000
Connection ~ 6750 6800
Connection ~ 6750 6700
Connection ~ 6750 6600
Connection ~ 10800 5200
Connection ~ 10450 4600
Connection ~ 2750 1550
Connection ~ 4150 1250
Connection ~ 2400 1250
Connection ~ 1400 2650
Connection ~ 1550 2550
Connection ~ 9550 5250
Connection ~ 9900 5550
Connection ~ 9550 6200
NoConn ~ 1150 2750
NoConn ~ 2550 6500
NoConn ~ 7050 4600
NoConn ~ 7050 4500
NoConn ~ 8900 4650
NoConn ~ 7050 4400
NoConn ~ 7750 5850
NoConn ~ 7850 5850
NoConn ~ 7950 5850
NoConn ~ 8050 5850
NoConn ~ 8150 5850
NoConn ~ 8250 5850
$EndSCHEMATC
