# ELEC49XDemoCode

ELEC 49X Capstone Project - Group 29

Members: Steven Crosby, Erin Hacker, Courtney Russo, Ke-Jun (Tom) Sung

Term: Fall 2019 / Winter 2020

Queen's University in Kingston, Ontario, Canada (https://www.queensu.ca)


## What this program does
* The Arduino program will demonstrate reading the capacitance on CAP1 of the FDC1004 and changing capdac to try to auto-range the chip.
* The program will then send the capacitance value over BLE, which can then be read on a BLE-capable device with Bluetooth notify feature.
* The FDC1004 Library provides basic functionality for the FDC1004 Capacitance to Digital Converter.
* The FDC1004 chip provides pico-farad precision for measuring capacitances up to 100pF.

## Requirements
 * Arduino Nano BLE 33
 * FDC1004

## Limitations
 * This library does not support differential capacitive measurements
 
## Credits
All code relating to the operation of the FDC1004 chip is written by Benjamin Shaya (bshaya@alum.mit.edu).

Everything else is written by ELEC 49X Group 29 member Ke-Jun (Tom) Sung (sung.tom@queensu.ca)

## Source
https://github.com/beshaya/FDC1004
