/*
Arduino NANO 33 IoT 기준
코어로직 코드
*/

#include <sstream>
#include <ArduinoBLE.h>
#include <vector>
#include <string>

// ,로 문자열 분리하는 코드
std::vector<std::string> splitString(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// 사용되는 변수들 설정
const char* AUTHENTICATION_CODE = "SFY001KOR";
const char* UID = "1DA24G10";
const int ledPin = LED_BUILTIN;

BLEService messageService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic stringCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 200);

const char* savedToken = "";
const char* savedMachine = "";

// 초기 설정
void setup() {
    Serial.begin(9600);
    while (!Serial);

    pinMode(ledPin, OUTPUT);

    if (!BLE.begin()) {
        Serial.println("Bluetooth® Low Energy failed to Start!");
        while (1);
    }

    BLE.setLocalName("SEOLO LOCK 1");
    BLE.setAdvertisedService(messageService);
    messageService.addCharacteristic(stringCharacteristic);
    BLE.addService(messageService);

    BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
    BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
    stringCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);

    Serial.println("SEOLO");
}

// loop 설정
void loop() {
    BLE.poll();

    // 계속 찾아지도록 설정
    BLE.advertise();
    Serial.println("started");
    delay(1000);

    // 배터리 30% 이하이면 LED 켜기
    // if (battery < 30) {

    // }
}

// BLE로 값을 입력받았을 경우 설정
void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
    const uint8_t *data = characteristic.value();
    int length = characteristic.valueLength();
    
    std::string receivedString = std::string((const char *)data, length);

    // 쉼표로 문자열을 분할
    std::vector<std::string> tokens = splitString(receivedString, ',');

    // 토큰들을 순서대로 할당
    const char* companyCode = (tokens.size() > 0) ? tokens[0].c_str() : "";
    const char* code = (tokens.size() > 1) ? tokens[1].c_str() : "";
    const char* token = (tokens.size() > 2) ? tokens[2].c_str() : "";
    const char* machine = (tokens.size() > 3) ? tokens[3].c_str() : "";


    Serial.print("companyCode");
    Serial.println(companyCode);
    Serial.print("code");
    Serial.println(code);
    Serial.print("token");
    Serial.println(token);
    Serial.print("machine");
    Serial.println(machine);
    
    checkCodeAvailable(companyCode, code, token, machine);
}

// BLE 연결된 경우
void blePeripheralConnectHandler(BLEDevice central) {
    Serial.print("Connected: ");
    Serial.println(central.address());
}

// BLE 연결 끊길 경우
void blePeripheralDisconnectHandler(BLEDevice central) {
    Serial.print("Disconnected: ");
    Serial.println(central.address());
}

// 상태 코드 및 인증 코드 검증 함수
void checkCodeAvailable(const char* companyCode, const char* code, const char* token, const char* machine) {
    String message = "";

    if (strcmp(companyCode, AUTHENTICATION_CODE) == 0) {
        if (strcmp(code, "INIT") == 0) {
            if (strcmp(savedToken, "") != 0) {
                message += "CHECK";
                message += ",";
                message += UID;
                message += ",";
                message += machine;
            } else {
                message += "WRITE";
                message += ",";
                message += UID;
            }
        } else if (strcmp(code, "LOCKED") == 0) {
            if (strcmp(savedToken, "") == 0) {
                message += "ALERT";
            } else if (strcmp(savedToken, token) == 0) {
                message += "UNLOCK";
                message += ",";
                message += UID;
                message += ",";
                message += savedToken;

                // 자물쇠 열기

                // 내장된 정보 삭제
                savedToken = "";
                savedMachine = "";
            } else {
                message += "CHECK";
                message += ",";
                message += UID;
                message += ",";
                message += savedMachine;
            }
        } else if (strcmp(code, "LOCK") == 0) {
            if (strcmp(token, "") != 0 && strcmp(savedToken, "") == 0) {
                // 자물쇠에 정보 저장
                savedMachine = machine;
                savedToken = token;

                // 자물쇠 잠금

                message += "LOCKED";
                message += ",";
                message += UID;
                message += ",";
                message += savedToken;
            } else if (strcmp(token, savedToken) == 0) {
                // 자물쇠 잠금

                message += "LOCKED";
                message += ",";
                message += UID;
                message += ",";
                message += savedToken;
            } else {
                message += "NOTOKENSENT";
            }
        }
    } else {
        BLE.disconnect();
    }

    message += ",";
    // battery = 배터리계산하는 함수();
    // message += battery;
    stringCharacteristic.writeValue(message.c_str());
}
