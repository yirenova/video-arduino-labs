#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <String.h>

#include <ESP32Servo.h>

#define BLE_NAME                "ESP32BLE"
#define SERVICE_UUID            "0000ffb0-0000-1000-8000-00805f9b34fb"    // BLE server
#define CHARACTERISTIC_UUID_RX  "0000ffb1-0000-1000-8000-00805f9b34fb"    // Receive data
#define CHARACTERISTIC_UUID_TX  "0000ffb2-0000-1000-8000-00805f9b34fb"    // Send data
#define BLE_SEND_DATA_MAX_LEN   20

#define LED_PIN                 3
#define SERVO_PIN               45

#define OPEN_ANGLE              45
#define DEFAULE_ANGLE           90
#define CLOSE_ANGLE             135


BLECharacteristic *Characteristic;    // Create a BLE characteristic
bool device_connected_flg = false;    // Connection status flag
Servo servo1; // 创建舵机对象

void ble_notify_sensor_data(void)
{
  uint8_t ble_send_data[BLE_SEND_DATA_MAX_LEN] = {0};
  
  // Prepare data to reply

  Characteristic->setValue(ble_send_data, BLE_SEND_DATA_MAX_LEN);
  Characteristic->notify();     // Notify the connected client
}

// Server callback for connection related events
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    device_connected_flg = true; // Set flag when device is connected
  };
  
  void onDisconnect(BLEServer *pServer)
  {
    device_connected_flg = false; // Reset flag when device is disconnected
    pServer->startAdvertising(); // Restart advertising after disconnection
    Serial.println("Client disconnected, advertising again...");
  }
};

// Characteristic callback for data related events
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    String rxValue = pCharacteristic->getValue(); // Get the received value
    if (rxValue.length() > 0)
    {
      if(rxValue[0] == 1)
      {
        digitalWrite(LED_PIN, HIGH);
        servo1.write(OPEN_ANGLE); 
        delay(500);
        servo1.write(DEFAULE_ANGLE); 
      }
      if(rxValue[0] == 0)
      {
        digitalWrite(LED_PIN, LOW);
        servo1.write(CLOSE_ANGLE); 
        delay(500);
        servo1.write(DEFAULE_ANGLE); 
      }
      // rxValue[0] is the received data buffer
      // ble_notify_sensor_data(); // Optionally notify with sensor data
    }
  }
};

void setup_ble(String BLEName)
{
  const char *ble_name = BLEName.c_str(); // Convert the passed BLE name to a pointer
  BLEDevice::init(ble_name);              // Initialize a Bluetooth device
 
  BLEServer *pServer = BLEDevice::createServer(); // Create a Bluetooth server
  pServer->setCallbacks(new MyServerCallbacks()); // Set server callback to MyServerCallbacks
 
  BLEService *pService = pServer->createService(SERVICE_UUID); // Create a BLE service
 
  Characteristic = pService->createCharacteristic(
                                                  CHARACTERISTIC_UUID_TX, 
                                                  BLECharacteristic::PROPERTY_READ   |
                                                  BLECharacteristic::PROPERTY_WRITE  |
                                                  BLECharacteristic::PROPERTY_NOTIFY |
                                                  BLECharacteristic::PROPERTY_INDICATE
                                                  );

  // Create a characteristic for reading with notification
  Characteristic->addDescriptor(new BLE2902());
  // Add a descriptor for the characteristic
  BLECharacteristic *Characteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  // Create a characteristic for writing
  Characteristic->setCallbacks(new MyCallbacks());
  // Set a callback for the characteristic
 
  pService->start();                  // Start the service
  pServer->getAdvertising()->start(); // Start advertising the server
  Serial.println("Waiting for a client connection to notify...");
}

void setup()
{
  pinMode(LED_PIN,OUTPUT);
  servo1.setPeriodHertz(100);          // 设置PWM频率为50Hz
  servo1.attach(SERVO_PIN, 500, 2500); // 引脚，脉冲宽度范围500-2500微秒
  servo1.write(DEFAULE_ANGLE);                     // 初始化舵机角度为90度

  Serial.begin(115200); // Start serial communication at 115200 baud
  setup_ble(BLE_NAME); // Set up Bluetooth with the specified name
}
 
void loop()
{

}
