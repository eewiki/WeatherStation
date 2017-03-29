#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Wire.h>
#include "Adafruit_INA219.h"

Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x44);

int counter=10;

long lastSecond; //The millis counter to see when a second rolls by

const int XBee_wake = 9;

float shuntvoltage_A = 0;
float busvoltage_A = 0;
float current_mA_A = 0;
float loadvoltage_A = 0;

float shuntvoltage_B = 0;
float busvoltage_B = 0;
float current_mA_B = 0;
float loadvoltage_B = 0;

//with xbee on
float shuntvoltage_C = 0;
float busvoltage_C = 0;
float current_mA_C = 0;
float loadvoltage_C = 0;

void setup () {
	//clock_prescale_set(clock_div_4);

	Serial.begin(57600);
	Serial.println("Charger Example");

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

	lastSecond = millis();
}

void loop () {
	//Print readings every second
	if (millis() - lastSecond >= 10000)
	{

		lastSecond += 10000;

		shuntvoltage_A = ina219_A.getShuntVoltage_mV();
		busvoltage_A = ina219_A.getBusVoltage_V();
		current_mA_A = ina219_A.getCurrent_mA();

		// wake up the XBee
		pinMode(XBee_wake, OUTPUT);
		digitalWrite(XBee_wake, LOW);

		shuntvoltage_B = ina219_B.getShuntVoltage_mV();
		busvoltage_B = ina219_B.getBusVoltage_V();
		current_mA_B = ina219_B.getCurrent_mA();

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

		shuntvoltage_C = ina219_A.getShuntVoltage_mV();
		busvoltage_C = ina219_A.getBusVoltage_V();
		current_mA_C = ina219_A.getCurrent_mA();

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

		// put the XBee to sleep
		pinMode(XBee_wake, INPUT); // put pin in a high impedence state
		digitalWrite(XBee_wake, HIGH);

		counter++;
		if (counter == 100)
		{
			counter = 10;
		}

	}

	delay(100);
}
