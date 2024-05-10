/*
Arduino NANO 33 IoT 기준
write value가 utf-8일 때, 회사코드가 같으면 led를 켜고 연결을 유지하고, 다르면 led를 끄고 연결도 해제는 코드
*/

#include <ArduinoBLE.h>

#define AUTHENTICATION_CODE "SFY001KOR"
#define UID "1DA24G10"

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic stringCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 32);

const int ledPin = LED_BUILTIN;

String receivedString = "";
bool isFirstConnection = true;
bool isAuthenticationDone = false;

void setup() {
    Serial.begin(9600);
    while (!Serial);

    pinMode(ledPin, OUTPUT);

    if (!BLE.begin()) {
        Serial.println("starting Bluetooth® Low Energy module failed!");
        while (1);
    }

    BLE.setLocalName("SSAFY LOCK 1");
    BLE.setAdvertisedService(ledService);
    ledService.addCharacteristic(stringCharacteristic);
    BLE.addService(ledService);

    BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
    BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
    stringCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);

    BLE.advertise();

    Serial.println("SEOLO");
}

void loop() {
    BLE.poll();

    // AUTHENTICATION_CODE에 따라 연결 처리

    // AUTHENTICATION_CODE가 맞으면 
    checkCodeAvailable
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
    Serial.print("Characteristic event, written: ");

    const uint8_t *data = characteristic.value();
    int length = characteristic.valueLength();
    receivedString = "";
    int cnt = 0;

    for (int i = 0; i < length; i++) {
        receivedString += (char)data[i];
    }

    Serial.println(receivedString);

    if (isFirstConnection) {
        if (receivedString != AUTHENTICATION_CODE) {
            digitalWrite(ledPin, LOW);
            central.disconnect();
            isFirstConnection = true;
            return;
        }

        digitalWrite(ledPin, HIGH);
        checkCodeAvailable(receivedString);
        isFirstConnection = false;
        isAuthenticationDone = true;
    } else {
        // 인증 후에는 특정 상태 코드 검증 가능
        if (isAuthenticationDone) {
            checkCodeAvailable(receivedString);
        }
    }
}

void blePeripheralConnectHandler(BLEDevice central) {
    Serial.print("Connected event, central: ");
    Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
    Serial.print("Disconnected event, central: ");
    Serial.println(central.address());
}

// 상태코드 및 인증 코드 검증 함수
void checkCodeAvailable(String receivedString) {
    if (receivedString == "INIT") {
        Serial.println("INIT");
    } else if (receivedString == "LOCKED") {
        Serial.println("LOCKED");
    } else if (receivedString == "LOCK") {
        Serial.println("LOCK");
    } else {
        Serial.println("Unknown code received.");
    }
}
