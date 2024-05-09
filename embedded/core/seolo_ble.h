#ifndef SELOO_BLE_H
#define SELOO_BLE_H

#include <ArduinoBLE.h>

class seolo_ble {
public:
    // Constructor
    seolo_ble(const char* deviceName, const char* authenticationCode);

    // Methods
    void begin();
    void poll();

    // Member variables
    const char* _deviceName;
    const char* _authenticationCode;

    // Static member variables
    static const int ledPin;
    static const char* AUTHENTICATION_CODE;

    // BLE service and characteristic
    BLEService ledService;
    BLECharacteristic stringCharacteristic;

private:
    // Private methods
    static void blePeripheralConnectHandler(BLEDevice central);
    static void blePeripheralDisconnectHandler(BLEDevice central);
    static void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic);
};

#endif
