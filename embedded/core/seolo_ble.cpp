#include "seolo_ble.h"

// Static member variable initialization
const int seolo_ble::ledPin = LED_BUILTIN;
const char* seolo_ble::AUTHENTICATION_CODE = nullptr;

seolo_ble::seolo_ble(const char* deviceName, const char* authenticationCode) {
    _deviceName = deviceName;
    _authenticationCode = authenticationCode;
}

void seolo_ble::begin()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    pinMode(ledPin, OUTPUT);

    if (!BLE.begin())
    {
        Serial.println("starting Bluetooth® Low Energy module failed!");
        while (1)
            ;
    }

    BLE.setLocalName(this->_deviceName);

    this->ledService.addCharacteristic(this->stringCharacteristic);
    BLE.addService(this->ledService);

    BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
    BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
    this->stringCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
    BLE.advertise();
}

void seolo_ble::poll()
{
    BLE.poll();
}

void seolo_ble::blePeripheralConnectHandler(BLEDevice central)
{
    Serial.print("Connected event, central: ");
    Serial.println(central.address());
}

void seolo_ble::blePeripheralDisconnectHandler(BLEDevice central)
{
    Serial.print("Disconnected event, central: ");
    Serial.println(central.address());
}

void seolo_ble::switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic)
{
    // central wrote new value to characteristic, update LED
    Serial.print("Characteristic event, written: ");

    // Convert the data to a string
    const uint8_t *data = characteristic.value();
    int length = characteristic.valueLength();
    String receivedString;

    for (int i = 0; i < length; i++)
    {
        receivedString += (char)data[i];
    }

    Serial.println(receivedString);

    // Process received string here
    if (receivedString == AUTHENTICATION_CODE)
    {
        digitalWrite(ledPin, HIGH);
    }
    else
    {
        digitalWrite(ledPin, LOW);
        central.disconnect();
    }
}
