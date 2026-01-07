#include "FastLED.h"

#define NUM_LEDS            192
#define NUM_COLOR           13
#define DAYTIME_LIGHT_VAL   1000    // 最大4096 越亮值越大

#define DATA_PIN    1
#define PIR_PIN     14
#define LS_PIN      21

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define MAX_BRIGHT  100
uint8_t max_bright = 100;
CRGB leds[NUM_LEDS];
CRGB led_color[NUM_COLOR] = {CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Cyan, CRGB::Magenta, CRGB::Orange, CRGB::Purple, CRGB::Pink, CRGB::Brown, CRGB::Teal, CRGB::Black};
bool light_on = false;

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LS_PIN, INPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHT);
}


void loop() 
{
  int lightValue = analogRead(LS_PIN);
  
  if(lightValue < DAYTIME_LIGHT_VAL)
  {
    if(digitalRead(PIR_PIN) == HIGH && light_on == false)
    {
      FastLED.setBrightness(MAX_BRIGHT);
      for(int led_index = 0; led_index < NUM_LEDS; led_index++){
        leds[led_index] = led_color[0];
        delay(10);
        FastLED.show();
      }
      light_on = true;
    }
  }

  if(digitalRead(PIR_PIN) == LOW && light_on == true)
  {
    light_on = false;
    for(int j=MAX_BRIGHT; j>0; j--){
      delay(10);
      FastLED.setBrightness(j);
      FastLED.show();
    }

    for(int led_index = 0; led_index < NUM_LEDS; led_index++){
      leds[led_index] = led_color[12];
    }
    FastLED.setBrightness(0);
    FastLED.show();
  }

  delay(100);
}