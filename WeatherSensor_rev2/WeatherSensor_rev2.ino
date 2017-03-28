#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Wire.h>
#include "Adafruit_Si7021.h"
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager

#define WDTCR |= _BV(WDIE)

Adafruit_Si7021 sensor = Adafruit_Si7021();

volatile byte watchdog_counter;
volatile byte endofline;

MPL3115A2 myPressure; //Create an instance of the pressure sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte STAT_BLUE = 7;
const byte STAT_GREEN = 8;

const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;

int counter=10;

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by

void setup()
{
	//wdt_enable(WDTO_8S);

	Serial1.begin(57600);
	Serial1.println("Weather Shield Example");

	pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
	pinMode(STAT_GREEN, OUTPUT); //Status LED Green

	pinMode(REFERENCE_3V3, INPUT);
	pinMode(LIGHT, INPUT);

	//Configure the pressure sensor
	myPressure.begin(); // Get sensor online
	myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
	myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
	myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

	sensor.begin();

	lastSecond = millis();

	Serial1.println("Weather Shield online!");
}

void loop()
{
	//  while (watchdog_counter < 4) //wait for watchdog counter reched the limit (WDTO_8S * 4 = 32sec.)
	//  {
	//    goto_sleep();
	//  }

	//Print readings every second
	if (millis() - lastSecond >= 10000)
	{
		digitalWrite(STAT_BLUE, HIGH); //Blink stat LED

		lastSecond += 10000;

		//watchdog_counter = 0;
		endofline = 0;
		//power_all_enable();
		//delay(5);

		//Check Pressure Sensor
		float pressure = myPressure.readPressure();

		Serial1.print("$WBP:");
		Serial1.print(counter);
		Serial1.print(":Pres:");
		Serial1.print(pressure);
		Serial1.println("Pa*");

		//Check Temperature Sensor
		float tempc = sensor.readTemperature();
		if (tempc <= 100) {
			float tempf = ((tempc * 1.8)+32);
			Serial1.print("$WBT:");
			Serial1.print(counter);
			Serial1.print(":TempF:");
			Serial1.print(tempf, 2);
			Serial1.println("F*");
			endofline=1;
		} else {
			Serial1.println("WBT:FAILURE");
		}

		//Check Humidity Sensor
		float humidity = sensor.readHumidity();
		if (humidity <= 100) {
			Serial1.print("$WBH:");
			Serial1.print(counter);
			Serial1.print(":Humid:");
			Serial1.print(humidity);
			Serial1.println("P*");
			endofline=1;
		} else {
			Serial1.println("WBH:FAILURE");
		}

		if ( endofline == 1) {
			Serial1.println("");
		}

		counter++;
		if (counter == 100)
		{
			counter = 10;
		}

		digitalWrite(STAT_BLUE, LOW); //Turn off stat LED
	}

	delay(100);
}

//void goto_sleep()
//{
//	//power_all_disable();
//	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//	sleep_mode();
//}

//ISR(WDT_vect)
//{
//	watchdog_counter++;
//}

