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

#include "flip_click_defs.h"
//#include "SparkFun_AS3935.h"

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

//SparkFun_AS3935 lightning;

int AS3935_intVal = 0;
int AS3935_noise = 2; // Value between 1-7 
int AS3935_disturber = 2; // Value between 1-10

const int TMP116_address    = 0x48;
const int TMP116_temp_reg   = 0x00;  // Temperature register
const int TMP116_config_reg = 0x01;  // Configuration register

int counter=10;

bool has_bme = true;
bool has_sgp = true;

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

void setup()
{
	Serial.begin(57600);
	Serial.println("MIKROE SAM3X WeatherDemo:#");

	bme_init();
	sgp_init();

//  SPI.begin();
//  SPI.setDataMode( SPI_MODE1 );

//  thunder_init();

//  Wire.begin();

//  tmp116_init();
}

int counter_sgp = 0;
void loop()
{
//  double temp = TMP116_ReadTempSensor();
//  Serial.println(temp);
  
//  if(digitalRead(AS3935_INT) == HIGH){
//    AS3935_intVal = lightning.readInterruptReg();
//    if(AS3935_intVal == AS3935_NOISE_INT){
//      Serial.println("debug: Noise."); 
//    }
//    else if(AS3935_intVal == AS3935_DISTURBER_INT){
//      Serial.println("debug: Disturber."); 
//    }
//    else if(AS3935_intVal == AS3935_LIGHTNING_INT){
//      Serial.println("debug: Lightning Strike Detected!"); 
//      // Lightning! Now how far away is it? Distance estimation takes into
//      // account any previously seen events in the last 15 seconds. 
//      byte distance = lightning.distanceToStorm(); 
//      Serial.print("Approximately: "); 
//      Serial.print(distance); 
//      Serial.println("km away!"); 
//    }
//  }

	if (has_bme) {
		bme_read();
	}

	if (has_sgp) {
		sgp_read();
	}

	counter++;
	if (counter == 100) {
		counter = 10;
	}

	delay(5000); // wait 5 seconds for the next I2C scan
}

void bme_init() {
	if (! bme.begin(0x76, &Wire)) {
		Serial.println("BME280: Sensor not found!:#");
		has_bme=false;
	}  
}

void bme_read() {
	bme.takeForcedMeasurement(); // has no effect in normal mode
	Serial.print("MIKROESAM3X_WC:");
	Serial.print(counter);
	Serial.print(":Temp:");
	bme_temperature = bme.readTemperature();
	float bme_temp_f = ((bme_temperature * 1.8) + 32);
	Serial.print(bme_temp_f);
	Serial.print("F:Press:");
	Serial.print(bme.readPressure() / 100.0F);
	Serial.print("mB:Humidity:");
	bme_humidty = bme.readHumidity();
	Serial.print(bme_humidty);
	Serial.println("%:#");
}

void sgp_init() {
	if (! sgp.begin()) {
		Serial.println("SGP30: Sensor not found!:#");
        has_sgp=false;
    }
	Serial.print("Found SGP30 serial #");
	Serial.print(sgp.serialnumber[0], HEX);
	Serial.print(sgp.serialnumber[1], HEX);
	Serial.print(sgp.serialnumber[2], HEX);
	Serial.println(":#");
}

void sgp_read() {
	// If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
	//float bme_temperature = 22.1; // [°C]
	//float bme_humidty = 45.2; // [%RH]
	if (has_bme) {
		sgp.setHumidity(getAbsoluteHumidity(bme_temperature, bme_humidty));
	}

	Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
	Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

	if (! sgp.IAQmeasureRaw()) {
		Serial.println("Raw Measurement failed");
		return;
	}
	Serial.print("Raw H2 "); Serial.print(sgp.rawH2); Serial.print(" \t");
	Serial.print("Raw Ethanol "); Serial.print(sgp.rawEthanol); Serial.println("");
	
	delay(1000);

	counter_sgp++;
	if (counter_sgp == 30) {
		counter_sgp = 0;

		uint16_t TVOC_base, eCO2_base;
		if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
			Serial.println("Failed to get baseline readings");
			return;
		}
		Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
		Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
	}
}

void thunder_init()
{
  pinMode(AS3935_CS, OUTPUT);
  digitalWrite(AS3935_CS, HIGH);
  pinMode(AS3935_INT, INPUT);

//  if( !lightning.beginSPI(AS3935_CS, 2000000) ){ 
//    Serial.println ("debug: Lightning Detector did not start up, freezing!"); 
//    while(1); 
//  }
//  else
//    Serial.println("debug: Lightning Detector Ready!");

  //lightning.setIndoorOutdoor(AS3935_OUTDOOR); 
}

double TMP116_ReadTempSensor(void){
  uint8_t data[2]; 
  int16_t datac;

  Wire.beginTransmission(TMP116_address); 
  Wire.write(TMP116_temp_reg); 
  Wire.endTransmission(); 

  delay(10); 

  Wire.requestFrom(TMP116_address,2); 

  if(Wire.available() <= 2){  
    data[0] = Wire.read(); 
    data[1] = Wire.read(); 

    datac = ((data[0] << 8) | data[1]); 

    return datac*0.0078125; 
  }
}

void tmp116_init(){
  // Configuration Register: Default is 0x02, 0x20
  // 11:10 MOD[1:0]
  // 00: Continuous conversion (CC) - default
  // 01: Shutdown (SD)
  // 10: Continuous conversion (CC), same as 00 (reads back = 00)
  // 11: One-shot conversion (OS)
  // 9:7 CONV[2:0] http://www.ti.com/lit/ds/symlink/tmp116.pdf
  // 100: - default
  // 6:5 AVG[1:0]
  // 01: - default 
  // Default = 8 samples, 1 second
  i2cwrite(TMP116_address, TMP116_config_reg, 0x02, 0x20);  
}

double i2cwrite(int device_addr, int register_addr, int high_bytes, int low_bytes){
  Wire.beginTransmission(device_addr); 
  Wire.write(register_addr);
  Wire.write(high_bytes);
  Wire.write(low_bytes);
  Wire.endTransmission();
  delay(10);
}
