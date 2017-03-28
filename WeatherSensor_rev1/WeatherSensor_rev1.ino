/*
 Weather Shield Example
 By: Nathan Seidle
 SparkFun Electronics
 Date: June 10th, 2016
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example prints the current humidity, air pressure, temperature and light levels.

 The weather shield is capable of a lot. Be sure to checkout the other more advanced examples for creating
 your own weather station.

 */

#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFunHTU21D.h" //Humidity sensor - Search "SparkFun HTU21D" and install from Library Manager

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

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

	//Configure the humidity sensor
	myHumidity.begin();

	lastSecond = millis();

	Serial1.println("Weather Shield online!");
}

void loop()
{
	//Print readings every second
	if (millis() - lastSecond >= 10000)
	{
		digitalWrite(STAT_BLUE, HIGH); //Blink stat LED

		lastSecond += 10000;

		//Check Humidity Sensor
		float humidity = myHumidity.readHumidity();

		if (humidity == 998) //Humidty sensor failed to respond
		{
			Serial1.println("I2C communication to sensors is not working. Check solder connections.");

			//Try re-initializing the I2C comm and the sensors
			myPressure.begin(); 
			myPressure.setModeBarometer();
			myPressure.setOversampleRate(7);
			myPressure.enableEventFlags();
			myHumidity.begin();
		} else {
			//Check Pressure Sensor
			float pressure = myPressure.readPressure();

			Serial1.print("$XWBP:");
			Serial1.print(counter);
			Serial1.print(":Pres:");
			Serial1.print(pressure);
			Serial1.println("Pa*");

			//Check Temperature Sensor
			float tempf = myPressure.readTempF();
			Serial1.print("$XWBT:");
			Serial1.print(counter);
			Serial1.print(":TempF:");
			Serial1.print(tempf, 2);
			Serial1.println("F*");

			//Check Humidity Sensor
			Serial1.print("$XWBH:");
			Serial1.print(counter);
			Serial1.print(":Humid:");
			Serial1.print(humidity);
			Serial1.println("P*");

			Serial1.println("");

			counter++;
			if (counter == 100)
			{
				counter = 10;
			}
		}

		digitalWrite(STAT_BLUE, LOW); //Turn off stat LED
	}

	delay(100);
}
