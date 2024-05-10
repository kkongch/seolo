#include "seolo_ble.h"

const char* deviceName = "SSAFY LOCK 1";
const char* authenticationCode = "SFY001KOR";

seolo_ble ble(deviceName, authenticationCode);

void setup() {
    ble.ledService = BLEService("19B10000-E8F2-537E-4F6C-D104768A1214");
    ble.stringCharacteristic = BLECharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 20);
    
    ble.begin();
}

void loop() {
    ble.poll();
}
