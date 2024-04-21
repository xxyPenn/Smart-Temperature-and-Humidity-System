#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    60
#define LUX_LIMIT   25

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);           //  setup serial
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {

  int val = analogRead(A0);  // read the input pin
  Serial.println(val);          // debug value
  if(val < LUX_LIMIT){
    // delay 800ms to check if it's really dark
    delay(1200);
    // If really dark then turn on the lights
    if(analogRead(A0) < LUX_LIMIT){
      for(int i = 100; i < 256; i+=30){
        for(int j = 0; j < 256; j+=30) {
          for(int k = 0; k < 256; k+=30){
            if (analogRead(A0) > LUX_LIMIT){
              break;
            }
            for (int num = 0; num < NUM_LEDS; num++){
              leds[num] = CRGB(i, j, k);
              FastLED.show();
              delay(10);
              if (analogRead(A0) > LUX_LIMIT){
                break;
              }
            }
          }
        }
      }

    }
  } else {
      for (int num = 0; num < NUM_LEDS; num++){
        leds[num] = CRGB(0,0,0);
        FastLED.show();
    }
  }
}