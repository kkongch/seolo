/*
Arduino NANO 33 ESP32 기준
write value가 utf-8일 때, 회사코드가 같으면 led를 켜고 연결을 유지하고, 다르면 led를 끄고 연결도 해제는 코드

수정필요함!!!!!!! 작동안함 :(
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define AUTHENTICATION_CODE "SFY001KOR"

BLEServer* pServer;
BLECharacteristic* pCharacteristic;

const int ledPin = LED_BUILTIN;

String receivedString = "";

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Connected event");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Disconnected event");
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
      Serial.println("Characteristic event, written: ");

      // Convert the data to a string
      // std::string rxValue = pCharacteristic->getValue();
     
     
      std::string rxValue = pCharacteristic->getValue().c_str();
      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++)
          receivedString += (char)rxValue[i];
      }

      Serial.println(receivedString);

      // Process received string here
      if (receivedString == AUTHENTICATION_CODE) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
        pServer->disconnect(pServer->getConnectedHandle());
      }
    }
};

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(ledPin, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("SSAFY LOCK 1");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService("19B10000-E8F2-537E-4F6C-D104768A1214");

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      "19B10001-E8F2-537E-4F6C-D104768A1214",
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Set the characteristic's value
  pCharacteristic->setValue("Hello World");

  // Set the characteristic's event handler
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();

  Serial.println("SEOLO");
}

void loop() {
  // Nothing to do here
}
