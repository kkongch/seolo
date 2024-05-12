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
#define AUTHENTICATION_CODE "SFY001KOR"
#define UID "1DA24G10"
String savedToken = "";
String savedMachine = "";

BLEService messageService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic stringCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 200);

const int ledPin = LED_BUILTIN;

String receivedString = "";
String companyCode = "";
String code = "";
String token = "";
String machine = "";
int battery = 0;

// 초기 설정
void setup() {
    Serial.begin(9600);
    while (!Serial);

    pinMode(ledPin, OUTPUT);

    if (!BLE.begin()) {
        Serial.println("starting Bluetooth® Low Energy module failed!");
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
    delay(100);
}

// BLE로 값을 입력받았을 경우 설정
void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
    const uint8_t *data = characteristic.value();
    int length = characteristic.valueLength();
    
    std::string receivedString = std::string((const char *)data, length);

    // 쉼표로 문자열을 분할
    std::vector<std::string> tokens = splitString(receivedString, ',');

    // 토큰들을 순서대로 할당
    companyCode = (tokens.size() > 0) ? tokens[0].c_str() : "";
    code = (tokens.size() > 1) ? tokens[1].c_str() : "";
    token = (tokens.size() > 2) ? tokens[2].c_str() : "";
    machine = (tokens.size() > 3) ? tokens[3].c_str() : "";

    Serial.println(companyCode);
    Serial.println(code);
    Serial.println(token);
    Serial.println(machine);
    
    checkCodeAvailable(companyCode, code, token, machine);
}

// BLE연결된 경우
void blePeripheralConnectHandler(BLEDevice central) {
    Serial.print("Connected event, central: ");
    Serial.println(central.address());
}

// BLE연결 끊길 경우
void blePeripheralDisconnectHandler(BLEDevice central) {
    Serial.print("Disconnected event, central: ");
    Serial.println(central.address());
}

// 상태코드 및 인증 코드 검증 함수
void checkCodeAvailable(String companyCode, String code, String token, String machine) {
    String message = "";
    // 항상 배터리 정보 주기!
    // int battery = 배터리계산하는함수();

    if (companyCode == AUTHENTICATION_CODE) {
        if (code == "INIT") {
            if (savedToken != "") {
                // "CHECK, UID, MachineId, BATTERY" 전송
                message += "CHECK";
                message += ",";
                message += UID;
                message += ",";
                message += machine;
                message += ",";
                // message += battery;
            } else {
                // "WRITE, UID, BATTERY" 전송
                message += "WRITE";
                message += ",";
                message += UID;
                message += ",";
                // message += battery;
            }
        } else if (code == "LOCKED") {
            if (savedToken == "") {
                // "ALERT, BATTERY" 전송
                message += "ALERT";
                message += ",";
                // message += battery;
            } else if (savedToken == token) {
                // 자물쇠 여는 로직
                // "UNLOCK, UID, BATTERY, TOKEN" 전송
                message += "UNLOCK";
                message += ",";
                message += UID;
                message += ",";
                message += battery;
                message += ",";
                message += savedToken;

                // 자물쇠 열기

                // 내장된 정보 삭제
                savedToken = "";
                savedMachine = "";
            } else {
                // "CHECK, UID, machineId, BATTERY" 전송
                message += "CHECK";
                message += ",";
                message += UID;
                message += ",";
                message += savedMachine;
                message += ",";
                // message += battery;
            }
        } else if (code == "LOCK") {
            if (token != "" && savedToken == "") {
                // 자물쇠에 정보 저장
                savedMachine = machine;
                savedToken = token;

                // 자물쇠 잠금

                // 잠금되면 데이터 전송("LOCKED", "TOKEN", "UID", "BATTERY")
                message += "LOCKED";
                message += ",";
                message += savedToken;
                message += ",";
                message += UID;
                message += ",";
                // message += battery;
            } else if (token == savedToken) {
                // 자물쇠 잠금

                // 잠금되면 데이터 전송("LOCKED", "TOKEN", "UID", "BATTERY")
                message += "LOCKED";
                message += ",";
                message += savedToken;
                message += ",";
                message += UID;
                message += ",";
                // message += battery;
            } else {
                // 로직 없음
                // message += battery;
            }
        }
    } else {
        // 블루투스 연결 끊기
        BLE.disconnect();
    }

    // message 전송
    stringCharacteristic.writeValue(message.c_str());
}
