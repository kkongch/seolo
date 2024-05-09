#include "seolo_ble.h"

seolo_ble::seolo_ble(const char* localName, const char* authCode)
    : ledService("19B10000-E8F2-537E-4F6C-D104768A1214"),
      stringCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 20),
      localName(localName),
      authCode(authCode)
{
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

    BLE.setLocalName(localName);
    BLE.setAdvertisedService(ledService);
    ledService.addCharacteristic(stringCharacteristic);
    BLE.addService(ledService);

    BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
    BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
    stringCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);

    BLE.advertise();

    Serial.println(("SEOLO"));
}

void seolo_ble::switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic)
{
    const uint8_t *data = characteristic.value();
    int length = characteristic.valueLength();
    String receivedString;

    for (int i = 0; i < length; i++)
    {
        receivedString += (char)data[i];
    }

    Serial.println(receivedString);

    if (receivedString == authCode)
    {
        digitalWrite(ledPin, HIGH);
    }
    else
    {
        digitalWrite(ledPin, LOW);
        central.disconnect();
    }
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
