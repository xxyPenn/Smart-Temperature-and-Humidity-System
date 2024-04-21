#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    60

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);           //  setup serial
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
}

void loop() {

  int val = analogRead(A0);  // read the input pin
  Serial.println(val);          // debug value
  if(val < 12){
    // delay 800ms to check if it's really dark
    delay(1200);
    // If really dark then turn on the lights
    if(analogRead(A0) < 12){
      for (int num = 0; num < NUM_LEDS; num++){
        leds[num] = CRGB(160, 32, 240);
        FastLED.show();
      }
    }
  } else {
      for (int num = 0; num < NUM_LEDS; num++){
        leds[num] = CRGB(0,0,0);
        FastLED.show();
    }
  }
}