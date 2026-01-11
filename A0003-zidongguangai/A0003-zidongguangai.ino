#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define IIC_SDA_PIN                 41
#define IIC_SCL_PIN                 42

#define ADC1_PIN                    11
#define ADC2_PIN                    12
#define MOTOR_PIN                   10        
#define LED_PIN                     40        

#define MIN_WATER_LINE              4095      // 没水就是4095
#define FLOWER_POT_HUMIDITY_MIN     700      // 干花盆状态       花盆越湿读到的值越大
#define FLOWER_POT_HUMIDITY_MAX     1500      // 浇水完成状态     花盆越湿读到的值越大

#define SCREEN_WIDTH                128
#define SCREEN_HEIGHT               64
#define OLED_RESET                  -1
#define SCREEN_ADDRESS              0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void watering_run()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Motor RUN");
  display.display();

  digitalWrite(MOTOR_PIN, HIGH);
}

void watering_stop()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Motor STOP");
  display.display();

  digitalWrite(MOTOR_PIN, LOW);
}
// 缺水报警
void water_shortage()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Water shortage");
  display.display();

  for(int i=0; i<10;i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ADC1_PIN, INPUT);
  pinMode(ADC2_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Wire.begin(IIC_SDA_PIN, IIC_SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Automatic watering");
  display.display();
}

void loop() {
  int Value1 = analogRead(ADC1_PIN);
  int Value2 = analogRead(ADC2_PIN);
  Serial.printf("shuiwei: %d  shidu: %d\r\n", Value1, Value2);

  // 缺水报警
  if(Value1 == MIN_WATER_LINE)
  {
    watering_stop();
    water_shortage();
  }

  // 花盆太干了 并且不是没水状态
  if(Value2 < FLOWER_POT_HUMIDITY_MIN && Value1 != MIN_WATER_LINE)
  {
    watering_run();
  }

  // 花盆浇水已经够了
  if(Value2 > FLOWER_POT_HUMIDITY_MAX)
  {
    watering_stop();
  }

  delay(1000);
}
