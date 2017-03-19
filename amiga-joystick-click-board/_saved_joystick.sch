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
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
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
$Comp
L +5V #PWR01
U 1 1 58839CB1
P 5200 3200
F 0 "#PWR01" H 5200 3050 50  0001 C CNN
F 1 "+5V" H 5200 3340 50  0000 C CNN
F 2 "" H 5200 3200 50  0000 C CNN
F 3 "" H 5200 3200 50  0000 C CNN
	1    5200 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 58839CC5
P 5600 3250
F 0 "#PWR02" H 5600 3000 50  0001 C CNN
F 1 "GND" H 5600 3100 50  0000 C CNN
F 2 "" H 5600 3250 50  0000 C CNN
F 3 "" H 5600 3250 50  0000 C CNN
	1    5600 3250
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 58839CD9
P 3900 3850
F 0 "R1" V 3980 3850 50  0000 C CNN
F 1 "R" V 3900 3850 50  0000 C CNN
F 2 "Resistors_THT:Resistor_Horizontal_RM10mm" V 3830 3850 50  0001 C CNN
F 3 "" H 3900 3850 50  0000 C CNN
	1    3900 3850
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 58839D0A
P 4100 3850
F 0 "R2" V 4180 3850 50  0000 C CNN
F 1 "R" V 4100 3850 50  0000 C CNN
F 2 "Resistors_THT:Resistor_Horizontal_RM10mm" V 4030 3850 50  0001 C CNN
F 3 "" H 4100 3850 50  0000 C CNN
	1    4100 3850
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 58839D28
P 4300 3850
F 0 "R3" V 4380 3850 50  0000 C CNN
F 1 "R" V 4300 3850 50  0000 C CNN
F 2 "Resistors_THT:Resistor_Horizontal_RM10mm" V 4230 3850 50  0001 C CNN
F 3 "" H 4300 3850 50  0000 C CNN
	1    4300 3850
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 58839D47
P 4500 3850
F 0 "R4" V 4580 3850 50  0000 C CNN
F 1 "R" V 4500 3850 50  0000 C CNN
F 2 "Resistors_THT:Resistor_Horizontal_RM10mm" V 4430 3850 50  0001 C CNN
F 3 "" H 4500 3850 50  0000 C CNN
	1    4500 3850
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 58839D6D
P 4700 3850
F 0 "R5" V 4780 3850 50  0000 C CNN
F 1 "R" V 4700 3850 50  0000 C CNN
F 2 "Resistors_THT:Resistor_Horizontal_RM10mm" V 4630 3850 50  0001 C CNN
F 3 "" H 4700 3850 50  0000 C CNN
	1    4700 3850
	1    0    0    -1  
$EndComp
$Comp
L DB9 J1
U 1 1 58839E87
P 4300 2750
F 0 "J1" H 4300 3300 50  0000 C CNN
F 1 "DB9" H 4300 2200 50  0000 C CNN
F 2 "Connectors:DB9MC" H 4300 2750 50  0001 C CNN
F 3 "" H 4300 2750 50  0000 C CNN
	1    4300 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4700 3700 4700 3200
Wire Wire Line
	4600 3200 4600 3650
Wire Wire Line
	4600 3650 4500 3650
Wire Wire Line
	4500 3650 4500 3700
Wire Wire Line
	4500 3200 4500 3600
Wire Wire Line
	4500 3600 4300 3600
Wire Wire Line
	4300 3600 4300 3700
$Comp
L CONN_01X07 P2
U 1 1 5883A0EA
P 5300 4450
F 0 "P2" H 5300 4850 50  0000 C CNN
F 1 "CONN_01X07" V 5400 4450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x07_Pitch2.54mm" H 5300 4450 50  0001 C CNN
F 3 "" H 5300 4450 50  0000 C CNN
	1    5300 4450
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X07 P1
U 1 1 5883A130
P 3450 4450
F 0 "P1" H 3450 4850 50  0000 C CNN
F 1 "CONN_01X07" V 3550 4450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x07_Pitch2.54mm" H 3450 4450 50  0001 C CNN
F 3 "" H 3450 4450 50  0000 C CNN
	1    3450 4450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4300 3200 4300 3500
Wire Wire Line
	4300 3500 4100 3500
Wire Wire Line
	4100 3500 4100 3700
Wire Wire Line
	4100 3200 4100 3350
Wire Wire Line
	4100 3350 3900 3350
Wire Wire Line
	3900 3350 3900 3700
Wire Wire Line
	3900 4000 3900 4150
Wire Wire Line
	3900 4150 3650 4150
Wire Wire Line
	4100 4000 4100 4250
Wire Wire Line
	4100 4250 3650 4250
Wire Wire Line
	4300 4000 4300 4350
Wire Wire Line
	4300 4350 3650 4350
Wire Wire Line
	4500 4000 4500 4450
Wire Wire Line
	4500 4450 3650 4450
Wire Wire Line
	4700 4000 4700 4550
Wire Wire Line
	4700 4550 3650 4550
Wire Wire Line
	4400 3200 5050 3200
Wire Wire Line
	5050 3200 5050 4650
Wire Wire Line
	5050 4650 5100 4650
Wire Wire Line
	4200 3200 4200 3350
Wire Wire Line
	4200 3350 5000 3350
Wire Wire Line
	5000 3350 5000 4750
Wire Wire Line
	5000 4750 5100 4750
$EndSCHEMATC
