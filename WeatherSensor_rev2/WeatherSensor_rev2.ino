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
    }
    else
    {
	Serial1.print("$WBH:");
	Serial1.print(counter);
	Serial1.print(":Humid:");
	Serial1.print(humidity);
	Serial1.println("P*");

//      Serial1.print("Humidity = ");
//      Serial1.print(humidity);
//      Serial1.print("%,");
//      float temp_h = myHumidity.readTemperature();
//      Serial1.print(" temp_h = ");
//      Serial1.print(temp_h, 2);
//      Serial1.print("C,");

      //Check Pressure Sensor
      float pressure = myPressure.readPressure();
//      Serial1.print(" Pressure = ");
//      Serial1.print(pressure);
//      Serial1.print("Pa,");

	Serial1.print("$WBP:");
	Serial1.print(counter);
	Serial1.print(":Pres:");
	Serial1.print(pressure);
	Serial1.println("Pa*");

      //Check tempf from pressure sensor
      float tempf = myPressure.readTempF();
//      Serial1.print(" temp_p = ");
//      Serial1.print(tempf, 2);
//      Serial1.print("F,");

	Serial1.print("$WBT:");
	Serial1.print(counter);
	Serial1.print(":TempF:");
	Serial1.print(tempf, 2);
	Serial1.println("F*");
	Serial1.println("");

//      //Check light sensor
//      float light_lvl = get_light_level();
//      Serial1.print(" light_lvl = ");
//      Serial1.print(light_lvl);
//      Serial1.print("V,");

//      //Check batt level
//      float batt_lvl = get_battery_level();
//      Serial1.print(" VinPin = ");
//      Serial1.print(batt_lvl);
//      Serial1.print("V");

//      Serial1.println();
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

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float lightSensor = analogRead(LIGHT);

  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

  lightSensor = operatingVoltage * lightSensor;

  return (lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float rawVoltage = analogRead(BATT);

  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

  return (rawVoltage);
}
