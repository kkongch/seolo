#include <sstream>
#include <vector>
#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

std::vector<std::string> splitString(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

#define SERVICE_UUID "20240520-C104-C104-C104-012345678910"
#define CHARACTERISTIC_UUID "20240521-C104-C104-C104-012345678910"
#define AUTHENTICATION_CODE "SFY001KOR"
#define UID "1DA24G10"
String savedToken = "";
String savedMachine = "";
BLECharacteristic *pCharacteristic;
BLEServer *pServer = NULL;
BLEService *messageService = NULL;
BLECharacteristic *stringCharacteristic = NULL;

String receivedString = "";
String companyCode = "";
String code = "";
String token = "";
String machine = "";
int battery = 0;

class MyCallbacks: public BLECharacteristicCallbacks {
public:
    void onWrite(BLECharacteristic *characteristic) {
        std::string receivedString = characteristic->getValue();
        int length = receivedString.length();

        // 데이터가 없을 경우 예외처리
        if(receivedString.empty()) {
            Serial.println("Empty data received");
            return;
        }
        
        // 쉼표로 문자열을 분할
        std::vector<std::string> tokens = splitString(receivedString, ',');

        // 토큰이 4개 미만인 경우 예외처리
        if (tokens.size() < 4) {
            Serial.println("Insufficient data received");
            return;
        }

        // 토큰들을 순서대로 할당
        companyCode = tokens[0].c_str();
        code = tokens[1].c_str();
        token = tokens[2].c_str();
        machine = tokens[3].c_str();

        Serial.println(companyCode.c_str());
        Serial.println(code.c_str());
        Serial.println(token.c_str());
        Serial.println(machine.c_str());

        // 회사 코드가 일치하지 않으면 연결 종료
        // if (companyCode != AUTHENTICATION_CODE) {
        //     BLEServer* pServer = characteristic->getServer();
        //     if (pServer != nullptr) {
        //         pServer->disconnect(characteristic->getConnectionId());
        //     }
        //     return;
        // }

        checkCodeAvailable(companyCode, code, token, machine);
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) {
        int connectedCount = pServer->getConnectedCount() + 1;
        Serial.print("Connected devices count: ");
        Serial.println(connectedCount);      
        pServer->startAdvertising();
    }

    void onDisconnect(BLEServer* pServer) {
        int connectedCount = pServer->getConnectedCount() - 1;
        Serial.print("Connected devices count: ");
        Serial.println(connectedCount);
        pServer->startAdvertising();
    }
};

MyServerCallbacks serverCallbacks;

void checkCodeAvailable(String companyCode, String code, String token, String machine) {
    String message = "";

    if (companyCode == AUTHENTICATION_CODE) {
        if (code == "INIT") {
            if (savedToken != "") {
                // "CHECK, UID, MachineId, BATTERY" 전송
                message += "CHECK";
                message += ",";
                message += UID;
                message += ",";
                message += savedMachine;
                message += ",";
            } else {
                // "WRITE, UID, BATTERY" 전송
                message += "WRITE";
                message += ",";
                message += UID;
                message += ",";
                message += ",";
            }
        } else if (code == "LOCKED") {
            if (savedToken == "") {
                // "ALERT, BATTERY" 전송
                message += "ALERT";
                message += ",";
                message += ",";
                message += ",";
            } else if (savedToken == token) {
                // 자물쇠 여는 로직
                // "UNLOCK, UID, BATTERY, TOKEN" 전송
                message += "UNLOCK";
                message += ",";
                message += UID;
                message += ",";
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
            }
        } else if (code == "LOCK") {
            if (token != "" && savedToken == "") {
                // 자물쇠에 정보 저장
                savedMachine = machine;
                savedToken = token;

                // 자물쇠 잠금

                // 잠금되면 데이터 전송("LOCKED", "TOKEN", "UID", "BATTERY")
                message += "LOCKED";
                message += UID;
                message += ",";
                message += ",";
                message += savedToken;
            } else if (token == savedToken) {
                // 자물쇠 잠금

                // 잠금되면 데이터 전송("LOCKED", "TOKEN", "UID", "BATTERY")
                message += "LOCKED";
                message += ",";
                message += UID;
                message += ",";
                message += savedMachine;
                message += ",";
                message += savedToken;
            } else {
                // 로직 없음
            }
        }
    } else {
        // 블루투스 연결 끊기
        // pServer->onDisconnect();
    }
    
    message += ",";
    message += battery;

    // message 전송
    Serial.println(message);
    stringCharacteristic->setValue(message.c_str());
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    BLEDevice::init("SEOLO LOCK 1");

    pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID));

    pCharacteristic = pService->createCharacteristic(
        BLEUUID(CHARACTERISTIC_UUID),
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE | 
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();

    Serial.println("SEOLO LOCK 1");

    // stringCharacteristic, battery 초기화
    stringCharacteristic = pCharacteristic;
    battery = 0;

    // 연결 및 연결 해제 이벤트 핸들러 등록
    pServer->setCallbacks(&serverCallbacks);
}

void loop() {
    delay(100);
}
