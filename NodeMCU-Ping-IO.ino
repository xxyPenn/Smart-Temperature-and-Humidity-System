// ESE5150 Final Project
// Team STHS

#include "config.h"
#include "DHT.h"
#include <Servo.h>

// --------------------Setup Servo----------------------------------------
Servo servo;
// -----------------------------------------------------------------------
// Setup DHT11 Humidity and Temp sensor
#define DHT_SENSOR_TYPE DHT11 //notice our sensor number here
#define DHT_SENSOR_PIN D1 // assign name of variable and pin number
DHT dht( DHT_SENSOR_PIN, DHT_SENSOR_TYPE ); //command specific to library
unsigned long time_value = 0;
unsigned long time_value2 = 0;
// ---------------------Adafruit Setup-------------------------------------
// set up the 'soil_moisture', 'temperatureDHT' and 'humidityDHT' feeds
AdafruitIO_Feed *light_intensity  = io.feed("light_intensity");
AdafruitIO_Feed *temperatureDHT = io.feed("temperatureDHT");
AdafruitIO_Feed *humidityDHT = io.feed("humidityDHT");
// ------------------------------------------------------------------------

// these correspond to the pins on your NodeMCU
#define fanPin D2    //D2 in Node MCU to drive watering pump
#define threshold 20 //after tinkering with your moisture sensor, feel free to change 'threshold' to a desired value

float light_int; // Light sensor reading
bool door_position = true; // Door position - true for open and false for closed
bool hand = true;
bool checking = false;  // Indicate if the light sensor is in the process of telling hand or ambient
float temperature = 0; // DHT Temperature Reading float variables allow for decimals
float humidity = 0; // DHT Humidity Reading

//-----------------------Lighting Section-----------------------------------
// Sense the light intensity and turn on the light when
// 1. Ambient lighting is dark
// 2. Sensed user approaching
void send_photoresistor(){
  light_int = analogRead(A0); //input from soil moisture sensor
  Serial.println(light_int);
  light_intensity->save(light_int);

  if (light_int < 28) {
    // Within 1.5 second, if the light sensor reads low and high, means that it's the hand movement.
    // If previously detcted hand gesture, continue to time until passes 1.5s
    checking = true;
    if (millis() > 1500+time_value2) {
      checking = false;
    } else {

    }
    // Get time stamp every time light intensity drops below limit
    if (time_value+2000 < millis()) {  // Check if this time's check is at least 2 seconds after the last time to prevent frequent state changing
      // If light sensor covered, negate the door position. i.e.: open to closed or closed to open
      door_position = !door_position;
      time_value = millis();
    }
  } else {

  }
}

void operate_door(){
  if(door_position){
    // Open the door if door_position is true
    servo.write(0);
  } else {
    servo.write(180);  // Otherwise close the door
  }
}
//--------------------------------------------------------------------------

//-----------------------DHT Section----------------------------------------
void send_dht_Sensor()
{
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temperature = dht.readTemperature();
    float t = dht.readTemperature();
    Serial.print("humidity is:");
    Serial.println(h);
    Serial.print("Temp is:");
    Serial.println(t);
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      delay(1000);
    }
    else {
      temperatureDHT->save(temperature);
      delay(500);
      humidityDHT->save(humidity);
      delay(500);
    }   
 
}

void demoisturize ()
{
  if (isnan(humidity)| humidity>100){
      // Serial.println("demois");
      // digitalWrite (fanPin , HIGH);
      // delay(3000);
      // digitalWrite (fanPin, LOW); //prevent pump from staying on
    delay(200);
  } else {
    if(humidity < threshold) //this means soil is wet and doesn't need to be watered
    {
      Serial.println("no need to demois");
      digitalWrite (fanPin , LOW);
      delay(3000);
    }
    else
    {
      Serial.println("demois");
      digitalWrite (fanPin , HIGH);
      delay(3000);
      digitalWrite (fanPin, LOW); //prevent pump from staying on
    }
  }
}
//--------------------------------------------------------------------------

void setup() {
  // declare pins as inputs and outputs
  servo.attach(D3);
  servo.write(0);
  delay(2000);
  pinMode (fanPin, OUTPUT);
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  // io.run();

  // Sense Humidity from DHT11
  // send_dht_Sensor();

  // Demoisturize if needed
  // demoisturize();

  // Check ambient lighting intensity
  send_photoresistor();
  // Operate the door
  operate_door();
}
