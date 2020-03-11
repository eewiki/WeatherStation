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

void setup()
{
  Serial.begin(57600);
  Serial.println("MIKROE SAM3X WeatherDemo");

  SPI.begin();
  SPI.setDataMode( SPI_MODE1 );

  thunder_init();
}

void loop()
{
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
  delay(100);
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
