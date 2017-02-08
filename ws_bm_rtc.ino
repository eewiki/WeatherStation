#include <Wire.h>
#include <avr/power.h>
#include "Adafruit_INA219.h"
#include "RTClib.h"

RTC_DS1307 rtc;
Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x44);

/* 16Mhz */
#define ws_baud 57600
#define ws_one_second 1000
#define ws_short_delay 200

/* 8Mhz */
//#define ws_baud 115200
//#define ws_one_second 500
//#define ws_short_delay 100

const int XBee_wake = 9;

int sensorPin = 0;

void setup () {
	//clock_prescale_set(clock_div_2);

	Serial1.begin(ws_baud);
	Serial1.println("Hello!");

	if (! rtc.begin()) {
		Serial1.println("Couldn't find RTC");
		while (1);
	}

	if (! rtc.isrunning()) {
		Serial1.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2017, 2, 1, 21, 23, 0));
	}

	// Initialize the INA219.
	// By default the initialization will use the largest range (32V, 2A).  However
	// you can call a setCalibration function to change this range (see comments).
	ina219_A.begin();
	ina219_B.begin();
	// To use a slightly lower 32V, 1A range (higher precision on amps):
	//ina219.setCalibration_32V_1A();
	// Or to use a lower 16V, 400mA range (higher precision on volts and amps):
	//ina219.setCalibration_16V_400mA();

	Serial1.println("Measuring voltage and current with INA219 ...");
}

void loop () {
	float shuntvoltage_A = 0;
	float busvoltage_A = 0;
	float current_mA_A = 0;
	float loadvoltage_A = 0;

	float shuntvoltage_B = 0;
	float busvoltage_B = 0;
	float current_mA_B = 0;
	float loadvoltage_B = 0;

	DateTime now = rtc.now();

	shuntvoltage_A = ina219_A.getShuntVoltage_mV();
	busvoltage_A = ina219_A.getBusVoltage_V();
	current_mA_A = ina219_A.getCurrent_mA();

	shuntvoltage_B = ina219_B.getShuntVoltage_mV();
	busvoltage_B = ina219_B.getBusVoltage_V();
	current_mA_B = ina219_B.getCurrent_mA();

	int reading = analogRead(sensorPin);
	float voltage = reading * 5.0;
	voltage /= 1024.0;
	float temperatureC = (voltage - 0.5) * 100;
	float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

	// wake up the XBee
	pinMode(XBee_wake, OUTPUT);
	digitalWrite(XBee_wake, LOW);
	delay(ws_short_delay);

	Serial1.print("$TEMP:");
	Serial1.print(now.unixtime());
	Serial1.print(":RAW:");
	Serial1.print(voltage);
	Serial1.print(":CALC:");
	Serial1.print(temperatureF); Serial1.println("F*");
	delay(ws_short_delay);

	Serial1.print("$DATE:");
	Serial1.print(now.year(), DEC);
	Serial1.print('/');
	Serial1.print(now.month(), DEC);
	Serial1.print('/');
	Serial1.print(now.day(), DEC);
	Serial1.print("_");
	Serial1.print(now.hour(), DEC);
	Serial1.print(':');
	Serial1.print(now.minute(), DEC);
	Serial1.print(':');
	Serial1.print(now.second(), DEC);
	Serial1.print('*');
	Serial1.println();

	Serial1.print("$LOAD:");
	Serial1.print(now.unixtime());
	Serial1.print(":BusVolt:");
	Serial1.print(busvoltage_A);
	Serial1.println("V*");
	delay(ws_short_delay);

	Serial1.print("$LOAD:");
	Serial1.print(now.unixtime());
	Serial1.print(":Current:");
	Serial1.print(current_mA_A);
	Serial1.println("mA*");
	delay(ws_short_delay);

	shuntvoltage_A = ina219_A.getShuntVoltage_mV();
	busvoltage_A = ina219_A.getBusVoltage_V();
	current_mA_A = ina219_A.getCurrent_mA();

	Serial1.print("$CHARGER:");
	Serial1.print(now.unixtime());
	Serial1.print(":BusVolt:");
	Serial1.print(busvoltage_B);
	Serial1.println("V*");
	delay(ws_short_delay);

	Serial1.print("$CHARGER:");
	Serial1.print(now.unixtime());
	Serial1.print(":Current:");
	Serial1.print(current_mA_B);
	Serial1.println("mA*");

	Serial1.print("$LOADXB:");
	Serial1.print(now.unixtime());
	Serial1.print(":BusVolt:");
	Serial1.print(busvoltage_A);
	Serial1.println("V*");
	delay(ws_short_delay);

	Serial1.print("$LOADXB:");
	Serial1.print(now.unixtime());
	Serial1.print(":Current:");
	Serial1.print(current_mA_A);
	Serial1.println("mA*");
	delay(ws_short_delay);

	Serial1.println("");
	delay(ws_short_delay);

	// put the XBee to sleep
	pinMode(XBee_wake, INPUT); // put pin in a high impedence state
	digitalWrite(XBee_wake, HIGH);

	delay(ws_one_second);
	delay(ws_one_second);
	delay(ws_one_second);
	delay(ws_one_second);

	delay(ws_one_second);
	delay(ws_one_second);
	delay(ws_one_second);
	delay(ws_one_second);
	delay(ws_one_second);
}
