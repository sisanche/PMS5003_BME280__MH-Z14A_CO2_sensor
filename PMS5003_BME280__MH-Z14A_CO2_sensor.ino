/*Connecting the BME280 Sensor with I2C Interface with ESP32
Sensor              ->  Board
-----------------------------
Vin (Voltage In)    ->  3.3V
Gnd (Ground)        ->  Gnd
SDA (Serial Data)   ->  GPIO21 on ESP32
SCK (Serial Clock)  ->  GPIO22 on ESP32

Connecting the PMS5003 Sensor 
GPIO1, GPIO3 (TX/RX pin on ESP32)
GPIO2 (D4 pin on ESP32)
Voltage supply - 5V
GND - GND

Connecting the CO2 sensor:
analog pin 35 - Data
VCC - 5V
GND - GND*/

#include <BME280I2C.h>
#include <Wire.h>
#include "PMS.h"
#define SERIAL_BAUD 115200
#define RX_PIN 40 //RX pin for PMS5003 sensor
#define TX_PIN 41 //TX pin for PMS5003 sensor
int analogPin = 35;

BME280I2C bme;    
PMS pms(Serial);
PMS::DATA data;

void setup()
{
  Serial.begin(115200);   
  Serial.begin(9600);
  while(!Serial) {} // Wait

  Wire.begin();

  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  // bme.chipID(); // Deprecated. See chipModel().
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
}

void loop()
{
   printBME280Data(&Serial);
   delay(500);
  int adcVal = analogRead(analogPin);
  float voltage = adcVal*(3.3/4095.0);
   if (pms.read(data))
   {
    Serial1.print("PM 1.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_1_0);

    Serial1.print("PM 2.5 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_2_5);

    Serial1.print("PM 10.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_10_0);

    Serial1.println();
   }
   if(voltage == 0)
  {
    Serial.println("A problem has occurred with the sensor.");
  }
  else if(voltage < 0.4)
  {
    Serial.println("Pre-heating the sensor...");
  }
  else
  {

    float voltageDiference=voltage-0.4;
    float concentration=(voltageDiference*5000.0)/1.6;

    Serial.print("voltage:");
    Serial.print(voltage);
    Serial.println("V");

    Serial.print(concentration);
    Serial.println("ppm");
  }

  delay(2000); 

}

void printBME280Data
(
   Stream* client
)
{
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);

   client->print("Temp: ");
   client->print(temp);
   client->print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   client->print("\t\tHumidity: ");
   client->print(hum);
   client->print("% RH");
   client->print("\t\tPressure: ");
   client->print(pres);
   client->println(" Pa");

   delay(1000);
}
