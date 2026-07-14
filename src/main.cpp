#include <Arduino.h>
#include "ESP32Dashboard.h"
#include "modules/DigitalPinModule.h"
#include "modules/AnalogPinModule.h"
#include "modules/DigitalInputModule.h"
#include "modules/AnalogInputModule.h"
#include "LittleFS.h"


// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";






ESP32Dashboard dashboard(ssid, password);
DigitalPinModule pinModule2(27);
DigitalPinModule pinModule3(14, "pin_alias", "Relay", "Turns off lights");
AnalogPinModule pinAnalog(25);
DigitalInputModule pinInputModule1(35);
AnalogInputModule pinInputAnalogModule1(34,3,200);

void checkLittleFS() {
  size_t freeHeap = esp_get_free_heap_size();
  
  // 2. Lowest amount of free memory ever reached since boot
  size_t minFreeHeap = esp_get_minimum_free_heap_size();
  
  // 3. Largest contiguous block of memory available for allocation
  size_t maxAllocHeap = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);

  Serial.printf("Free Heap: %d bytes\n", freeHeap);
  Serial.printf("Min Free Heap Ever: %d bytes\n", minFreeHeap);
  Serial.printf("Max Allocatable Block: %d bytes\n", maxAllocHeap);

}

void setup() {
    Serial.begin(115200);
    dashboard.addModule(&pinModule2);
    dashboard.addModule(&pinModule3);
    dashboard.addModule(&pinAnalog);
    dashboard.addModule(&pinInputModule1);
    dashboard.addModule(&pinInputAnalogModule1);
    dashboard.begin();
}


void loop() {
    dashboard.handleClient();
    checkLittleFS();
}
