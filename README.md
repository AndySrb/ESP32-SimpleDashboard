# **ESP32-SimpleDashboard Library Documentation**
This library provides a modular web-based dashboard system for the ESP32 using **WebSockets**, **HTTP API**, and **HTML controls** to interact with GPIO pins (both digital and analog). It supports:

- **Digital Input/Output** (buttons, switches)
- **Analog Output (PWM)** (sliders, dimmers)
- **Analog Input** (sensors)

The library is designed for **PlatformIO** and works with **ESP32**.

---

## **1. Library Structure**
### **Core Classes**
| Class | Description |
|--------|------------|
| `ESP32Dashboard` | Main dashboard class that initializes WebServer & WebSockets. |
| `DigitalPinModule` | Controls digital output pins (toggle buttons). |
| `DigitalInputModule` | Reads digital input states (buttons, switches). |
| `AnalogPinModule` | Controls PWM-capable analog output pins (sliders). |
| `AnalogInputModule` | Reads analog input values (sensors). |

---

## **2. Setup & Initialization**
### **Installation**
1. Add the library to your PlatformIO project:
   ```ini
   [env:esp32dev]
   platform = espressif32
   framework = arduino
   lib_deps =
       ESPAsyncWebServer
       ArduinoJson
   ```
2. Include headers in `main.cpp`:
   ```cpp
   #include "ESP32Dashboard.h"
   #include "DigitalPinModule.h"
   #include "AnalogPinModule.h"
   ```

### **Basic Initialization**
```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "ESP32Dashboard.h"

const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

ESP32Dashboard dashboard;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Initialize Dashboard
    dashboard.begin();

    // Add Modules
    dashboard.addModule(new DigitalPinModule(13));  // GPIO13 as digital output toggle
    dashboard.addModule(new AnalogPinModule(25, "LED_Brightness", "LED Brightness Control"));  // PWM on GPIO25

    Serial.println("Dashboard started at: http://" + WiFi.localIP().toString());
}

void loop() {
    dashboard.handleClient();  // Handle WebSocket & HTTP requests
}
```

---

## **3. Module Examples**
### **(A) Digital Output (Toggle Button)**
**Use Case:** Control an LED or relay with a button.

#### **Code Example**
```cpp
#include "DigitalPinModule.h"

// Initialize in setup()
dashboard.addModule(new DigitalPinModule(13, "LED_Control", "LED Toggle", "Turns GPIO13 ON/OFF"));

```

#### **How It Works**
- A button appears on the dashboard.
- Clicking it sends an HTTP request to `/api/13/toggle`.
- The ESP32 toggles GPIO13 and broadcasts the new state via WebSocket.

---

### **(B) Analog Output (PWM Slider)**
**Use Case:** Control LED brightness or motor speed.

#### **Code Example**
```cpp
#include "AnalogPinModule.h"
// Initialize in setup()
dashboard.addModule(new AnalogPinModule(25, "LED_Brightness", "LED Brightness Control"));
```

#### **How It Works**
- A slider appears on the dashboard.
- Moving it sends an HTTP request to `/api/25/set?value=X`.
- The ESP32 updates PWM duty cycle and broadcasts the new value via WebSocket.

---

### **(C) Digital Input (Button/Switch)**
**Use Case:** Read a physical button or switch state.

#### **Code Example**
```cpp
#include "DigitalInputModule.h"

// Initialize in setup()
dashboard.addModule(new DigitalInputModule(12, "Door_Sensor", "Door Status", "Reads GPIO12 as input"));

```

#### **How It Works**
- The module reads `digitalRead(12)` periodically.
- If the state changes, it broadcasts via WebSocket:
  ```json
  {"type": "digital_input", "pin": 12, "state": "HIGH"}
  ```

---

### **(D) Analog Input (Sensor)**
**Use Case:** Read a potentiometer or sensor value.

#### **Code Example**
```cpp
#include "AnalogInputModule.h"

// Initialize in setup()
dashboard.addModule(new AnalogInputModule(34, "Potentiometer", "Potentiometer Value"));
```

#### **How It Works**
- The module reads `analogRead(34)` periodically.
- If the value changes, it broadcasts via WebSocket:
  ```json
  {"type": "analog_input", "pin": 34, "value": 128}
  ```

---

## **4. API Endpoints**
| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/<pin>/toggle` | `GET` | Toggles a digital output pin. |
| `/api/<pin>/set?value=X` | `GET` | Sets PWM value (0-255). |
| `/api/<pin>/status` | `GET` | Returns current state/value. |

---

## **5. WebSocket Communication**
The dashboard uses WebSockets for real-time updates:
```javascript
// Client-side JavaScript (auto-injected)
if (window.ws && ws.readyState === WebSocket.OPEN) {
    ws.send('getStatus');  // Request initial state
}
```
- The ESP32 broadcasts changes automatically.

---

## **6. Full Example: LED + Button Control**
```cpp
#include <WiFi.h>
#include "ESP32Dashboard.h"
#include "DigitalPinModule.h"
#include "AnalogPinModule.h"

const char* ssid = "Your_WiFi";
const char* password = "Your_Password";

ESP32Dashboard dashboard;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) delay(500);

    dashboard.begin();

    // Add a toggle button for GPIO13
    dashboard.addModule(new DigitalPinModule(13, "LED_Toggle", "LED Control"));

    // Add a PWM slider for GPIO25
    dashboard.addModule(new AnalogPinModule(25, "LED_Brightness", "Brightness Control"));

    Serial.println("Dashboard running at: http://" + WiFi.localIP().toString());
}

void loop() {
    dashboard.handleClient();
}
```

### **Expected Dashboard UI**
1. A toggle button for GPIO13.
2. A slider to control PWM on GPIO25.

---

## **7. Customization**
- Override `getHTML()` in derived classes for custom UI.
- Modify `sendUpdate()` to change WebSocket behavior.
- Use `handleRequest()` to add extra API endpoints.

---

## **8. Troubleshooting**
| Issue | Solution |
|-------|----------|
| Dashboard not loading | Check WiFi connection & firewall settings. |
| WebSocket errors | Ensure client-side JS is properly injected. |
| PWM not working | Verify `ledcSetup()` and pin configuration. |

---

## **Conclusion**
This library provides a **modular, real-time dashboard** for ESP32 GPIO control with:
✅ **Digital Input/Output**
✅ **Analog PWM Control**
✅ **WebSocket Updates**
✅ **Customizable UI**

Use it to build **IoT dashboards**, **home automation**, or **robotics controls** easily! 🚀
