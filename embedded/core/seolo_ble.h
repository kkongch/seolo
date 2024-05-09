#ifndef SEOLO_BLE_H
#define SEOLO_BLE_H

#include <ArduinoBLE.h>

class seolo_ble
{
public:
    seolo_ble(const char* localName, const char* authCode);
    void begin();
    void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic);
    void blePeripheralConnectHandler(BLEDevice central);
    void blePeripheralDisconnectHandler(BLEDevice central);

private:
    BLEService ledService;
    BLECharacteristic stringCharacteristic;
    String receivedString;
    const int ledPin = LED_BUILTIN;
    const char* localName;
    const char* authCode;
};

#endif
