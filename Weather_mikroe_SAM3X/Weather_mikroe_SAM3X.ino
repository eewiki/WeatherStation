#include <SPI.h>
#include <Wire.h>
#include "flip_click_defs.h"
#include "SparkFun_AS3935.h"

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

const int TMP116_address    = 0x48;
const int TMP116_temp_reg   = 0x00;  // Temperature register
const int TMP116_config_reg = 0x01;  // Configuration register

void setup()
{
  Serial.begin(57600);
  Serial.println("MIKROE SAM3X WeatherDemo");

  SPI.begin();
  SPI.setDataMode( SPI_MODE1 );

  thunder_init();

  Wire.begin();

  tmp116_init();
}

void loop()
{
  double temp = TMP116_ReadTempSensor();

  Serial.println(temp);
  
  if(digitalRead(AS3935_INT) == HIGH){
    AS3935_intVal = lightning.readInterruptReg();
    if(AS3935_intVal == AS3935_NOISE_INT){
      Serial.println("debug: Noise."); 
    }
    else if(AS3935_intVal == AS3935_DISTURBER_INT){
      Serial.println("debug: Disturber."); 
    }
    else if(AS3935_intVal == AS3935_LIGHTNING_INT){
      Serial.println("debug: Lightning Strike Detected!"); 
      // Lightning! Now how far away is it? Distance estimation takes into
      // account any previously seen events in the last 15 seconds. 
      byte distance = lightning.distanceToStorm(); 
      Serial.print("Approximately: "); 
      Serial.print(distance); 
      Serial.println("km away!"); 
    }
  }

  delay(5000); // wait 5 seconds for the next I2C scan
}

void thunder_init()
{
  pinMode(AS3935_CS, OUTPUT);
  digitalWrite(AS3935_CS, HIGH);
  pinMode(AS3935_INT, INPUT);

  if( !lightning.beginSPI(AS3935_CS, 2000000) ){ 
    Serial.println ("debug: Lightning Detector did not start up, freezing!"); 
    while(1); 
  }
  else
    Serial.println("debug: Lightning Detector Ready!");

  lightning.setIndoorOutdoor(AS3935_OUTDOOR); 
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
