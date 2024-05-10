#include <WiFiNINA.h>


const char ssid = "C104";      // WiFi ID
const char pass = "C104c104";  // WiFi PASSWORD


int status = WL_IDLE_STATUS;  // WiFi STATUS


void setup() {
  Serial.begin(9600);


  // LED GND
  pinMode(LED_BUILTIN, OUTPUT);


  // WAIT FOR CONNECTED
  while (!Serial) {
    ; // IF NEEDED
  }


  // CHECK WIFI MODULE
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true);
  }


  // WHILE WIFI CONNECTED
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);


    // WAIT 1000 AND REPEAT
    delay(1000);
  }


  // WiFi CONNECTED
  Serial.println("Connected to WiFi");
}


void loop() {
  // IF WIFI CONNECTED
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
} 


