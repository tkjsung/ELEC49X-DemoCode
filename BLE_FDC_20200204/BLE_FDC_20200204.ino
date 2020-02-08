/**************************************************************
    ELEC 49X Capstone Project - Group 29
    Members: Steven Crosby, Erin Hacker, Courtney Russo, Ke-Jun (Tom) Sung
    Term: Fall 2019 / Winter 2020
    Queen's University in Kingston, Ontario, Canada
 **************************************************************
    Arduino Bluetooth Low Energy (BLE) with FDC1004 by Texas Instruments
    
    This program will demonstrates reading the capacitance on CAP1 of the FDC1004
      and changing capdac to try to auto-range the chip.
    The program will then send the capacitance value over BLE, which can
      then be read on a BLE-capable device with Bluetooth notify feature.
 **************************************************************
    Setup:
    
    Connect ONLY the 3.3V and Ground pins to the FDC1004
    Connect SDA and SCL to your Arduino and FDC1004
      (Connect pull up resistors in series from 3.3 V to SDA/SCL)
    Power on and run this code
    Open Serial Monitor at baud rate of 115200
  **************************************************************
    Side Notes:
    
    This program works on Arduino Nano BLE 33 and FDC1004.
    Note that the output (capacitance) is a signed integer, so values greater than
      7FFF FFFF are actually negative!
    For any clarifications regarding the code, please consult the FDC1004 Data Sheet:
      http://www.ti.com/lit/ds/symlink/fdc1004.pdf
 **************************************************************
    Credits:
    
    Arduino BLE Code:
    Modified from OSC example "LED.ino" for Arduino Nano BLE 33
    Written by Ke-Jun (Tom) Sung of ELEC 49X Group 29
    sung.tom@queensu.ca
    
    FDC1004 Implementation Code:
    Source Code names: "FDC1004.cpp", "FDC1004.h", and function fdcFunc().
    Written by Benjamin Shaya for Rest Devices
    bshaya@alum.mit.edu
    https://github.com/beshaya/FDC1004
 **************************************************************/

#include <ArduinoBLE.h>
#include <Wire.h>
#include "FDC1004.h"

// BLE Global Variables (with random UUIDs)
BLEService fdcService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedCharCharacteristic fdcCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLENotify);

// Global Variable Initialization
int capdac = 11; // Random CAPDAC value for initialization
unsigned char fdcData = 0;
FDC1004 fdc;
int counter = 1;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);

  // Begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // Built-in LED Pin
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Set advertised local name and service UUID
  BLE.setLocalName("CS02");
  BLE.setAdvertisedService(fdcService);

  // Add characteristic to service
  fdcService.addCharacteristic(fdcCharacteristic);

  // Add Service
  BLE.addService(fdcService);

  // Initialize characteristic with a value written to it
  fdcData = 0;
  fdcCharacteristic.writeValue(fdcData);

  // Start advertising BLE
  BLE.advertise();

  Serial.println("ELEC 49X Group 29 Capstone Project");
  Serial.println("Set-up completed.");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    // while the central is still connected to peripheral:
    while (central.connected()) {
            
      // Read FDC value here
      fdcFunc();
      Serial.print("Counter: "); Serial.print(counter);
      Serial.print(". Data: ");
      Serial.println(fdcData);
      fdcCharacteristic.writeValue(fdcData);
      counter++;
    }

    // when the central disconnects, print it out:
    Serial.print(("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);
    counter = 1;
  }
}

void fdcFunc() {
  uint8_t measurement = 0;
  uint8_t channel = 0;
  char result[100];

  fdc.configureMeasurementSingle(measurement, channel, capdac);
  fdc.triggerSingleMeasurement(measurement, FDC1004_100HZ);
  //wait for completion
  delay(15);
  uint16_t value[2];
  if (! fdc.readMeasurement(measurement, value)) {

    // calculate capacitance;
    // The absolute capacitance is a function of the capdac and the measurement
    // We only use the msb because the FDC1004 only has 16bits effective resolution;
    // the last 8 bits are more or less random noise.
    int16_t msb = (int16_t) value[0];
    int32_t capacitance = ((int32_t)457) * ((int32_t)msb); //in attofarads

    capacitance /= 1000; //in femtofarads
    capacitance += ((int32_t)3028) * ((int32_t)capdac);
    capacitance = capacitance / 1000; // converted to picofarads
    fdcData = capacitance; // store to global variable for BLE transmission
    
    //adjust capdac
    int16_t upper_bound = 0x4000;
    int16_t lower_bound = -1 * upper_bound;
    if (msb > upper_bound) {
      if (capdac < FDC1004_CAPDAC_MAX) capdac++;
    } else if (msb < lower_bound) {
      if (capdac > 0) capdac--;
    }
  }
  delay(200); // This dictates sampling time.
}
