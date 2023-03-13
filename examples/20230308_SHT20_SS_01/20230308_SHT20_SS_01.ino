// 20230308_SHT20_SS_01

// Original version: https://github.com/TronixLab/ModBus-RTU-Temperature-Humidity-SHT20-Sensor/blob/main/Examples/2-SoftwareSerial_Temperature_and_Humidity__SHT20__Sensor/
// Modified version: 2023-03-08 JST by @hohno_at_kuimc

/*
  This code demonstrates how to interact with an Arduino Mega 2560 and
  a Modbus RTU temperature and humidity sensor (SHT20). It reads the
  temperature and humidity values every 1 seconds and display data to
  the serial monitor.

  Note: Serial Port 0 is not used to connect the RS485 Converter (MAX485)
  because its used for debugging. The Serial Port 1 (TX1, RX1) is used
  for ModBus communication interface.

  Wiring of Sensor, Arduino, and MAX485 TTL to RS485 Converter:
  ___________________________________________________________________________________________
  | Sensor (SHT20)   |   MAX485 TTL to RS485 Converter
  |  A (Yellow)      |        A (Terminal block)
  |  B (White)       |        B (Terminal block)
  |  GND (Black)     |       GND (External Supply)
  |  Vs (Red)        |      9-30V (External Supply)
  ___________________________________________________________________________________________
  | MAX485 TTL to RS485 Converter  |  Arduino (Hardware Serial)  |  Arduino (Software Serial)
  |     RO (Reciever Output)       |        D19 (RX1)            |          D9 (RX)
  |     RE (Reciever Enable)       |        D2                   |          D2
  |     DE (Driver Enable)         |        D3                   |          D3
  |     DI (Driver Input)          |        D18 (TX1)            |          D10 (TX)
  ___________________________________________________________________________________________
*/

#include <ModbusMaster.h> 
// See also
//  https://github.com/4-20ma/ModbusMaster
//  https://github.com/4-20ma/ModbusMaster/blob/master/src/ModbusMaster.h

#include <SoftwareSerial.h>

#define MAX485_RE_NEG  2
#define MAX485_DE      3
#define SSERIAL_RX_PIN 9  // 10 // RS485 to Arduino
#define SSERIAL_TX_PIN 10 // 11 // Arduino to RS485

SoftwareSerial RS485Serial(SSERIAL_RX_PIN, SSERIAL_TX_PIN);
ModbusMaster node;

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void setup() {
  // Initialize control pins
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);

  // Modbus communication runs at 9600 baud
  Serial.begin(57600);
  RS485Serial.begin(9600);

  // Modbus slave ID 1
  const uint8_t ModbusSlaveID = 1;
  node.begin(ModbusSlaveID, RS485Serial);

  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  // Request 2 registers starting at 0x0001
  const uint16_t ReadAddress = 0x0001;
  uint8_t result = node.readInputRegisters(ReadAddress, 2); // readInputRegisters(ReadAddress, Qty)
  Serial.println("Data Requested");

  if (result == node.ku8MBSuccess) {
    // Get response data from sensor
    Serial.print("Temperature: ");
    Serial.print(float(node.getResponseBuffer(0) / 10.00F));
    Serial.print("   Humidity: ");
    Serial.println(float(node.getResponseBuffer(1) / 10.00F));
  }
  delay(1000);
}
