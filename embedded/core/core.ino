#include "seolo_ble.h"

const char* deviceName = "SSAFY LOCK 1";
const char* authenticationCode = "SFY001KOR";

seolo_ble ble(deviceName, authenticationCode);

void setup()
{
    ble.begin();
}

void loop()
{
    
}
