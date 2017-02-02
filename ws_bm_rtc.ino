#include <Wire.h>
#include "Adafruit_INA219.h"
#include "RTClib.h"

RTC_DS1307 rtc;
Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x41);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
	//while (!Serial1);     // will pause Zero, Leonardo, etc until serial console opens
	uint32_t currentFrequency;

	Serial1.begin(57600);
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

	shuntvoltage_A = ina219_A.getShuntVoltage_mV();
	busvoltage_A = ina219_A.getBusVoltage_V();
	current_mA_A = ina219_A.getCurrent_mA();
	loadvoltage_A = busvoltage_A + (shuntvoltage_A / 1000);

	shuntvoltage_B = ina219_B.getShuntVoltage_mV();
	busvoltage_B = ina219_B.getBusVoltage_V();
	current_mA_B = ina219_B.getCurrent_mA();
	loadvoltage_B = busvoltage_B + (shuntvoltage_B / 1000);

	delay(1000);

	Serial1.print("$LOAD:"); Serial1.print(now.unixtime());
	Serial1.print(":BusVolt:"); Serial1.print(busvoltage_A); Serial1.println("V*");

	delay(200);

	Serial1.print("$LOAD:"); Serial1.print(now.unixtime());
	Serial1.print(":ShuntVolt:"); Serial1.print(shuntvoltage_A); Serial1.println("mV*");

	delay(200);

	Serial1.print("$LOAD:"); Serial1.print(now.unixtime());
	Serial1.print(":LoadVolt:"); Serial1.print(loadvoltage_A); Serial1.println("V*");

	delay(200);

	Serial1.print("$LOAD:"); Serial1.print(now.unixtime());
	Serial1.print(":Current:"); Serial1.print(current_mA_A); Serial1.println("mA*");

	delay(200);

	//delay(1000);

	Serial1.print("$SOLAR:"); Serial1.print(now.unixtime());
	Serial1.print(":BusVolt:"); Serial1.print(busvoltage_B); Serial1.println("V*");

	delay(200);

	Serial1.print("$SOLAR:"); Serial1.print(now.unixtime());
	Serial1.print(":ShuntVolt:"); Serial1.print(shuntvoltage_B); Serial1.println("mV*");

	delay(200);

	Serial1.print("$SOLAR:"); Serial1.print(now.unixtime());
	Serial1.print(":LoadVolt:"); Serial1.print(loadvoltage_B); Serial1.println("V*");

	delay(200);

	Serial1.print("$SOLAR:"); Serial1.print(now.unixtime());
	Serial1.print(":Current:"); Serial1.print(current_mA_B); Serial1.println("mA*");
	Serial1.println("");

	//delay(1000);
	//delay(3000);
	delay(3200);
}
