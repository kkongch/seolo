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
const int analogPin = A3;  // 아두이노 나노 iot의 A3 아날로그 입력 핀 (GPIO 15)
const float voltageDivider = 2.0;  // 전압 분배 계수 (10K옴 + 10K옴 = 입력 전압의 반)
const float referenceVoltage = 3.3;  // nano iot 기준 전압
const int resolution = 4095;  // nano iot ADC 해상도 (0-4095)

BLEService messageService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic stringCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 200);

const char* savedToken = "";
const char* savedMachine = "";

// 초기 설정
void setup() {
    Serial.begin(9600);
    while (!Serial);

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

    BLE.advertise();
    delay(2500); // 1초마다 advertise
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


    Serial.print("companyCode : ");
    Serial.println(companyCode);
    Serial.print("code : ");
    Serial.println(code);
    Serial.print("token : ");
    Serial.println(token);
    Serial.print("machine : ");
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
    // battery 계산
    int adcValue = analogRead(analogPin);
    float batteryVoltage = (adcValue * referenceVoltage / resolution) * voltageDivider;
    int battery = mapBatteryVoltageToPercentage(batteryVoltage);

    message += ",";
    message += battery;
    stringCharacteristic.writeValue(message.c_str());
}

int mapBatteryVoltageToPercentage(float voltage)
{
    if (voltage >= 4.09) return 100;  // 100%
    if (voltage >= 4.0) return 95;  // 95%
    if (voltage >= 3.9) return 90;  // 90%
    if (voltage >= 3.8) return 85;  // 85%
    if (voltage >= 3.7) return 75;  // 75%
    if (voltage >= 3.6) return 65;  // 65%
    if (voltage >= 3.5) return 55;  // 55%
    if (voltage >= 3.4) return 45;  // 45%
    if (voltage >= 3.3) return 35;  // 35%
    if (voltage >= 3.2) return 25;  // 25%
    if (voltage >= 3.1) return 15;  // 15%
    if (voltage >= 3.0) return 5;   // 5%
    return 0;  // 이외의 경우 0%
}
