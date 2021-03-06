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

int counter=10;

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by

const int XBee_wake = 9;

void setup()
{
	Serial.begin(57600);
	Serial.println("Weather Shield Example");

	pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
	pinMode(STAT_GREEN, OUTPUT); //Status LED Green

	//Configure the pressure sensor
	myPressure.begin(); // Get sensor online
	myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
	myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
	myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

	//Configure the humidity sensor
	myHumidity.begin();

	lastSecond = millis();

	Serial.println("Weather Shield online!");
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
			Serial.println("I2C communication to sensors is not working. Check solder connections.");

			//Try re-initializing the I2C comm and the sensors
			myPressure.begin(); 
			myPressure.setModeBarometer();
			myPressure.setOversampleRate(7);
			myPressure.enableEventFlags();
			myHumidity.begin();
		} else {

			// wake up the XBee
			pinMode(XBee_wake, OUTPUT);
			digitalWrite(XBee_wake, LOW);

			//Check Pressure Sensor
			float pressure = myPressure.readPressure();

			Serial.print("$XWBP:");
			Serial.print(counter);
			Serial.print(":Pres:");
			Serial.print(pressure);
			Serial.println("Pa*");

			//Check Temperature Sensor
			float tempf = myPressure.readTempF();
			Serial.print("$XWBT:");
			Serial.print(counter);
			Serial.print(":TempF:");
			Serial.print(tempf, 2);
			Serial.println("F*");

			//Check Humidity Sensor
			Serial.print("$XWBH:");
			Serial.print(counter);
			Serial.print(":Humid:");
			Serial.print(humidity);
			Serial.println("P*");

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

		digitalWrite(STAT_BLUE, LOW); //Turn off stat LED
	}

	delay(100);
}
