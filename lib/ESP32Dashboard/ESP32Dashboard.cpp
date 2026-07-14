#include "ESP32Dashboard.h"

ESP32Dashboard::ESP32Dashboard(const char* ssid, const char* password)
    : server(80), webSocket(81), _ssid(ssid), _password(password) {}

void ESP32Dashboard::begin() {
    WiFi.begin(_ssid, _password);
    Serial.println("Connecting to WiFi...");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nWiFi Connection Failed!");
        return;
    }

    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    setupRoutes();
    server.begin();
    webSocket.begin();
    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        webSocketEvent(num, type, payload, length);
    });
}

void ESP32Dashboard::addModule(DashboardModule* module) {
    modules.push_back(module);
}

void ESP32Dashboard::handleClient() {
    server.handleClient();
    webSocket.loop();
}

void ESP32Dashboard::setupRoutes() {
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.onNotFound([this]() { handleNotFound(); });

    for (auto module : modules) {
        module->handleRequest(server);
    }
}

void ESP32Dashboard::handleRoot() {
    String html = R"raw(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <title>ESP32 Dashboard</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }
        .module { background: white; border: 1px solid #ddd; margin: 15px 0; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        button { padding: 8px 16px; font-size: 16px; cursor: pointer; background: #007bff; color: white; border: none; border-radius: 4px; }
        button:hover { background: #0056b3; }
        input[type='range'] { width: 200px; margin: 10px 0; }
        progress { width: 100%; height: 20px; }
        .value { font-weight: bold; color: #007bff; }
    </style>
</head>
<body>
    <h1>ESP32 Dashboard</h1>
    <div id="modules">
)raw";

    for (auto module : modules) {
        html += "<div class='module'>";
        html += module->getHTML();
        html += "</div>";
    }

    html += R"raw(
    </div>
    <script>
        var ws = new WebSocket('ws://' + window.location.hostname + ':81');
        
        ws.onmessage = function(event) {
            var data = JSON.parse(event.data);
            // Handle different module types
            if (data.type === 'digital_input') {
                var span = document.getElementById('input_' + data.pin);
                if (span) span.textContent = data.state;
            }
            else if (data.type === 'analog_input') {
                var rawSpan = document.getElementById('analog_raw_' + data.pin);
                var voltSpan = document.getElementById('analog_volt_' + data.pin);
                if (rawSpan) rawSpan.textContent = data.raw;
                if (voltSpan) voltSpan.textContent = data.voltage.toFixed(2);
                var prog = document.getElementById('progress_' + data.pin);
                if (prog) prog.value = data.raw;
            }
            else {
                // Output modules: data key = pin number (as string)
                for (var key in data) {
                    var element = document.getElementById(key);
                    if (element) {
                        if (element.tagName === 'INPUT' && element.type === 'range') {
                            element.value = data[key];
                            var valueDisplay = document.getElementById(key + 'Value');
                            if (valueDisplay) valueDisplay.textContent = data[key];
                        } else if (element.tagName === 'BUTTON') {
                            element.textContent = data[key] === 'ON' ? 'Turn OFF' : 'Turn ON';
                        } else {
                            element.textContent = data[key];
                        }
                    }
                }
            }
        };
        
        ws.onopen = function() {
            console.log('WebSocket connected');
            // Request status every 2 seconds
            setInterval(function() { ws.send('getStatus'); }, 2000);
        };
        
        ws.onerror = function(e) {
            console.error('WebSocket error', e);
        };
    </script>
</body>
</html>)raw";

    server.send(200, "text/html", html);
}

void ESP32Dashboard::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT:
            broadcastUpdate();
            break;
        case WStype_CONNECTED:
            Serial.printf("WebSocket client %u connected\n", num);
            broadcastUpdate();
            break;
        case WStype_DISCONNECTED:
            Serial.printf("WebSocket client %u disconnected\n", num);
            break;
        default:
            break;
    }
}

void ESP32Dashboard::broadcastUpdate() {
    for (auto module : modules) {
        module->sendUpdate(webSocket);
    }
}

void ESP32Dashboard::handleNotFound() {
    server.send(404, "text/plain", "Not Found");
}