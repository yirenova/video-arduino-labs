#include "FastLED.h"

#define NUM_LEDS            192
#define NUM_COLOR           13
#define DAYTIME_LIGHT_VAL   50    // 最大4096 越亮值越大

#define DATA_PIN    1
#define PIR_PIN     21
#define LS_PIN      14

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define MAX_BRIGHT  50

// 滤波器参数
#define FILTER_SIZE 10     // 滑动平均滤波窗口大小

CRGB leds[NUM_LEDS];
CRGB led_color[NUM_COLOR] = {CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Cyan, CRGB::Magenta, CRGB::Orange, CRGB::Purple, CRGB::Pink, CRGB::Brown, CRGB::Teal, CRGB::Black};
bool light_on = false;

// 滤波器变量
int lightReadings[FILTER_SIZE];  // 存储光照读数的数组
int readIndex = 0;               // 当前读数的索引
long lightSum = 0;               // 读数总和
int lightAverage = 0;            // 平均值
bool filterInitialized = false;  // 滤波器是否已初始化

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LS_PIN, INPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHT);
  Serial.begin(115200);
}


void loop() 
{
  int lightValue = analogRead(LS_PIN);
  
  // 滑动平均滤波
  if (!filterInitialized) {
    // 初始化滤波器数组
    for (int i = 0; i < FILTER_SIZE; i++) {
      lightReadings[i] = lightValue;
    }
    lightSum = lightValue * FILTER_SIZE;
    filterInitialized = true;
  }
  
  // 减去最旧的读数
  lightSum = lightSum - lightReadings[readIndex];
  // 添加新的读数
  lightReadings[readIndex] = lightValue;
  lightSum = lightSum + lightValue;
  // 更新索引
  readIndex = (readIndex + 1) % FILTER_SIZE;
  // 计算平均值
  lightAverage = lightSum / FILTER_SIZE;
  
  Serial.printf("Current Light value is %d, Filtered: %d\r\n", lightValue, lightAverage);
  if(lightAverage < DAYTIME_LIGHT_VAL)  // 使用滤波后的值进行判断
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