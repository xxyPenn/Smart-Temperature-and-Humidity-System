// ESE5150 Final Project
// Team STHS

#include "config.h"
#include "DHT.h"
#include <Servo.h>

// --------------------Setup Servo----------------------------------------
Servo servo;
#define servo_open 180
#define servo_close 270 
// -----------------------------------------------------------------------
// Setup DHT11 Humidity and Temp sensor
#define DHT_SENSOR_TYPE DHT11 //notice our sensor number here
#define DHT_SENSOR_PIN D1 // assign name of variable and pin number
DHT dht( DHT_SENSOR_PIN, DHT_SENSOR_TYPE ); //command specific to library
unsigned long time_value = 0;
unsigned long time_value_demois = 0;
// ---------------------Adafruit Setup-------------------------------------
// set up the 'soil_moisture', 'temperatureDHT' and 'humidityDHT' feeds
AdafruitIO_Feed *light_intensity  = io.feed("light_intensity");
AdafruitIO_Feed *temperatureDHT = io.feed("temperatureDHT");
AdafruitIO_Feed *humidityDHT = io.feed("humidityDHT");
// ------------------------------------------------------------------------

// these correspond to the pins on your NodeMCU
#define fanPin D2    //D2 in Node MCU to drive watering pump
#define threshold 30 //after tinkering with your moisture sensor, feel free to change 'threshold' to a desired value
#define light_threshold 55

float light_int; // Light sensor reading
bool door_position = false; // Door position - true for open and false for closed
bool hand = true;
float temperature = 0; // DHT Temperature Reading float variables allow for decimals
float humidity = 0; // DHT Humidity Reading

//-----------------------Lighting Section-----------------------------------
// Sense the light intensity and turn on the light when
// 1. Ambient lighting is dark
// 2. Sensed user approaching

// unsigned long light_timecount = 0;
bool change = false;  // Indicate whether to enable motor state change
bool bright = true;  // Indicate if the light sensor is in the process of telling hand or ambient; False means previous state is bright and True dark.
void send_photoresistor(){
  // Serial.println("running send_photores");
  light_int = analogRead(A0); //input from soil moisture sensor
  // Serial.println(light_int);
  light_intensity->save(light_int);

  if (light_int < light_threshold) {
    // Record the time stamp each time the light drops below threshold if there's consecutive darkness
    if(bright){
      change = true;
      bright = false;
    } else {
      change = false;
    }
    if (change) {
      // Get time stamp every time light intensity drops below limit
      if (time_value+2000 < millis()) {  // Check if this time's check is at least 2 seconds after the last time to prevent frequent state changing
        // If light sensor covered, negate the door position. i.e.: open to closed or closed to open
        door_position = !door_position;
        time_value = millis();
      }
    }
  } else {
    bright = true;
  }
}

void operate_door(){
  // Serial.println("running operate door");
  if(door_position){
    // Open the door if door_position is true
    servo.write(servo_open);
  } else {
    servo.write(servo_close);  // Otherwise close the door
  }
}
//--------------------------------------------------------------------------

//-----------------------DHT Section----------------------------------------
void send_dht_Sensor()
{
  // Serial.println("running send_dht");
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    delay(20);
    float h = dht.readHumidity();
    delay(20);
    // Read temperature as Celsius (the default)
    temperature = dht.readTemperature();
    float t = dht.readTemperature();
    Serial.print("humidity is:");
    Serial.println(h);
    Serial.print("Temp is:");
    Serial.println(t);
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      // delay(1000);
    }
    else {
      temperatureDHT->save(temperature);
      // delay(500);
      humidityDHT->save(humidity);
      // delay(500);
    }   
 
}

void demoisturize ()
{
  // Serial.println("running demois");
  if (isnan(humidity)| humidity>100){
      // Serial.println("demois");
      // digitalWrite (fanPin , HIGH);
      // delay(3000);
      // digitalWrite (fanPin, LOW); //prevent pump from staying on
    // delay(200);
  } else if (millis()>3000+time_value_demois){
    digitalWrite (fanPin, LOW);
    if(humidity < threshold) //this means soil is wet and doesn't need to be watered
    {
      Serial.println("no need to demois");
      digitalWrite (fanPin , LOW);
      // delay(3000);
    }
    else
    {
      Serial.println("demois");
      time_value_demois = millis();
      digitalWrite (fanPin , HIGH);
      // delay(3000);
      // digitalWrite (fanPin, LOW); //prevent pump from staying on
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
  send_dht_Sensor();

  // Demoisturize if needed
  demoisturize();

  // // Check ambient lighting intensity
  send_photoresistor();
  // // Operate the door
  operate_door();
}
