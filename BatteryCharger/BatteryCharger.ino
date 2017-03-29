#include <Wire.h>
#include <avr/power.h>
#include "Adafruit_INA219.h"
#include "LowPower.h"

Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x44);

/* 8Mhz (fuse/bootloader) */
#define ws_baud 57600
#define ws_one_second 1000
#define ws_short_delay 200

int counter=10;

const int XBee_wake = 9;

float shuntvoltage_A = 0;
float busvoltage_A = 0;
float current_mA_A = 0;
float loadvoltage_A = 0;

float shuntvoltage_B = 0;
float busvoltage_B = 0;
float current_mA_B = 0;
float loadvoltage_B = 0;

void setup () {
	//clock_prescale_set(clock_div_4);

	Serial.begin(ws_baud);
	Serial.println("Hello!");

	// Initialize the INA219.
	// By default the initialization will use the largest range (32V, 2A).  However
	// you can call a setCalibration function to change this range (see comments).
	ina219_A.begin();
	ina219_B.begin();
	// To use a slightly lower 32V, 1A range (higher precision on amps):
	//ina219.setCalibration_32V_1A();
	// Or to use a lower 16V, 400mA range (higher precision on volts and amps):
	//ina219.setCalibration_16V_400mA();

	Serial.println("Measuring voltage and current with INA219 ...");
}

void loop () {
	float shuntvoltage_A = ina219_A.getShuntVoltage_mV();
	float busvoltage_A = ina219_A.getBusVoltage_V();
	float current_mA_A = ina219_A.getCurrent_mA();

	float shuntvoltage_B = ina219_B.getShuntVoltage_mV();
	float busvoltage_B = ina219_B.getBusVoltage_V();
	float current_mA_B = ina219_B.getCurrent_mA();

//	// wake up the XBee
//	pinMode(XBee_wake, OUTPUT);
//	digitalWrite(XBee_wake, LOW);
//	delay(ws_short_delay);

	Serial.print("$LOAD:");
	Serial.print(counter);
	Serial.print(":BusVolt:");
	Serial.print(busvoltage_A);
	Serial.println("V*");

	Serial.print("$LOAD:");
	Serial.print(counter);
	Serial.print(":Current:");
	Serial.print(current_mA_A);
	Serial.println("mA*");

	float shuntvoltage_C = ina219_A.getShuntVoltage_mV();
	float busvoltage_C = ina219_A.getBusVoltage_V();
	float current_mA_C = ina219_A.getCurrent_mA();

	Serial.print("$CHARGER:");
	Serial.print(counter);
	Serial.print(":BusVolt:");
	Serial.print(busvoltage_B);
	Serial.println("V*");

	Serial.print("$CHARGER:");
	Serial.print(counter);
	Serial.print(":Current:");
	Serial.print(current_mA_B);
	Serial.println("mA*");

	Serial.print("$LOADXB:");
	Serial.print(counter);
	Serial.print(":BusVolt:");
	Serial.print(busvoltage_C);
	Serial.println("V*");

	Serial.print("$LOADXB:");
	Serial.print(counter);
	Serial.print(":Current:");
	Serial.print(current_mA_C);
	Serial.println("mA*");
	Serial.println("");

		counter++;
		if (counter == 100)
		{
			counter = 10;
		}

//	// put the XBee to sleep
//	pinMode(XBee_wake, INPUT); // put pin in a high impedence state
//	digitalWrite(XBee_wake, HIGH);

	LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
