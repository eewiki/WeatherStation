/***************************************************************************
 * 
 * Copyright (c) 2020 Robert Nelson <robert.nelson@digikey.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Slot: A: XBee Click
 * Slot: B: Thunder Click
 * Slot: C: Weather Click
 * Slot: D: Air Quality 4 Click
 * 
 ***************************************************************************/

#include <SPI.h>
#include <Wire.h>

/* Adafruit BME280 Library: 2.0.2 */
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/* Adafruit SGP30 Sensor: 1.0.5 */
#include <Adafruit_SGP30.h>

/* Sparkfun AS3935 Lightning Detector Arduino Library 1.4.0 */
#include "SparkFun_AS3935.h"

#include "flip_click_defs.h"

Adafruit_BME280 bme; // I2C
Adafruit_SGP30 sgp;

unsigned long delayTime;

#define AS3935_INDOOR 0x12 
#define AS3935_OUTDOOR 0xE
#define AS3935_LIGHTNING_INT 0x08
#define AS3935_DISTURBER_INT 0x04
#define AS3935_NOISE_INT 0x01

#define AS3935_CS  B_CS
#define AS3935_INT B_INT

SparkFun_AS3935 lightning;

int AS3935_intVal = 0;
int AS3935_noise = 2; // Value between 1-7 
int AS3935_disturber = 2; // Value between 1-10

int counter=1000;
int counter_sgp = 0;

bool has_bme = true;
bool has_sgp = true;
bool has_thunder = true;

volatile float bme_temperature = 0;
volatile float bme_humidty = 0;

/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [°C]
* @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void bump_counter() {
	counter++;
	if (counter == 10000) {
		counter = 1000;
	}
}

void setup()
{
	/* Serial = Board USB, Serial1 = Slot A Tx/Rx */
	Serial1.begin(57600);
	Serial1.println("MIKROESAM3X_DEBUG:MIKROE SAM3X WeatherDemo:#");
	
	bme_init();
	sgp_init();

	SPI.begin();
	SPI.setDataMode( SPI_MODE1 );

	thunder_init();
}

void loop()
{
	if (has_thunder) {
		if(digitalRead(AS3935_INT) == HIGH) {
			AS3935_intVal = lightning.readInterruptReg();
			if(AS3935_intVal == AS3935_NOISE_INT) {
				Serial1.println("MIKROESAM3X_DEBUG: Noise:#");
			}
			else if(AS3935_intVal == AS3935_DISTURBER_INT) {
				Serial1.println("MIKROESAM3X_DEBUG: Disturber:#"); 
			}
			else if(AS3935_intVal == AS3935_LIGHTNING_INT) {
				bump_counter();
				Serial1.print("MIKROESAM3X_AS3935:"); 
				Serial1.print(counter);
				Serial1.print(":Lightning:");
				// Lightning! Now how far away is it? Distance estimation takes into
				// account any previously seen events in the last 15 seconds. 
				byte distance = lightning.distanceToStorm(); 
				Serial1.print(distance); 
				Serial1.println("km:#");
			}
		}
	}

	if (has_bme) {
		bme_read();
	}

	if (has_sgp) {
		sgp_read();
	}

	delay(5000); // wait 5 seconds for the next I2C scan
}

void bme_init() {
	if (! bme.begin(0x76, &Wire)) {
		Serial1.println("MIKROESAM3X_DEBUG:BME280: Sensor not found!:#");
		has_bme=false;
	}  
}

void bme_read() {
	bump_counter();
	bme.takeForcedMeasurement(); // has no effect in normal mode
	Serial1.print("MIKROESAM3X_WC:");
	Serial1.print(counter);
	Serial1.print(":Temp:");
	bme_temperature = bme.readTemperature();
	float bme_temp_f = ((bme_temperature * 1.8) + 32);
	Serial1.print(bme_temp_f);
	Serial1.print("F:Press:");
	Serial1.print(bme.readPressure() / 100.0F);
	Serial1.print("mB:Humidity:");
	bme_humidty = bme.readHumidity();
	Serial1.print(bme_humidty);
	Serial1.println("%:#");
}

void sgp_init() {
	if (! sgp.begin()) {
		Serial1.println("MIKROESAM3X_DEBUG:SGP30: Sensor not found!:#");
        has_sgp=false;
    }
	Serial1.print("MIKROESAM3X_DEBUG:Found SGP30 serial:");
	Serial1.print(sgp.serialnumber[0], HEX);
	Serial1.print(sgp.serialnumber[1], HEX);
	Serial1.print(sgp.serialnumber[2], HEX);
	Serial1.println(":#");
}

void sgp_read() {
	bump_counter();
	// If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
	//float bme_temperature = 22.1; // [°C]
	//float bme_humidty = 45.2; // [%RH]
	if (has_bme) {
		sgp.setHumidity(getAbsoluteHumidity(bme_temperature, bme_humidty));
	}
	
	Serial1.print("MIKROESAM3X_AQC:");
	Serial1.print(counter);
	Serial1.print(":TVOC:");
	Serial1.print(sgp.TVOC);
	Serial1.print(":ppb:eCO2:");
	Serial1.print(sgp.eCO2);
	Serial1.println(":ppm:#");

	delay(1000);

	counter_sgp++;
	if (counter_sgp == 30) {
		counter_sgp = 0;

		uint16_t TVOC_base, eCO2_base;
		if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
			Serial1.println("MIKROESAM3X_DEBUG:Failed to get baseline readings:#");
			return;
		}
		Serial1.print("MIKROESAM3X_DEBUG:Baseline values: eCO2: 0x");
		Serial1.print(eCO2_base, HEX);
		Serial1.print(" & TVOC: 0x");
		Serial1.print(TVOC_base, HEX);
		Serial1.println(":#");
	}
}

void thunder_init()
{
	pinMode(AS3935_CS, OUTPUT);
	digitalWrite(AS3935_CS, HIGH);
	pinMode(AS3935_INT, INPUT);

	if( !lightning.beginSPI(AS3935_CS, 2000000) ){ 
		Serial1.println ("MIKROESAM3X_DEBUG:Lightning Detector did not start up, freezing:#");; 
		has_thunder=false;
	}
	else
		Serial1.println("MIKROESAM3X_DEBUG:Lightning Detector Ready:#");

	if (has_thunder) {
		lightning.setIndoorOutdoor(AS3935_OUTDOOR); 
	}
}
