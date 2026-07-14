#include "AnalogPinModule.h"

AnalogPinModule::AnalogPinModule(int pin) : _pin(pin), _value(0) {
    ledcSetup(0, 5000, 8);
    ledcAttachPin(_pin, 0);
    ledcWrite(0, 0);
    mPinAlias = _pin;
    mName = String("Analog pin control PIN" + mPinAlias);
    mDescription = "";
}

AnalogPinModule::AnalogPinModule(int pin, String pinAlias, String name, String description) :
_pin(pin), mPinAlias(pinAlias), mName(name), mDescription(description), _value(0) {
    ledcSetup(0, 5000, 8);
    ledcAttachPin(_pin, 0);
    ledcWrite(0, 0);
}

void AnalogPinModule::handleRequest(WebServer& server) {

auto handleSetRequest = [this, &server](const String& endpoint) {
    if (server.hasArg("value")) {
        int value = server.arg("value").toInt();
        value = constrain(value, 0, 255);  // Clamp value
        setValue(value);
        
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["pin"] = _pin;
        response["pin_name"] = mPinAlias;
        response["value"] = _value;
        response["set_value"] = value;
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    } else {
        DynamicJsonDocument errorResponse(128);
        errorResponse["success"] = false;
        errorResponse["error"] = "Missing value parameter";
        errorResponse["message"] = "Please provide a 'value' parameter (0-255)";
        
        String jsonResponse;
        serializeJson(errorResponse, jsonResponse);
        server.send(400, "application/json", jsonResponse);
    }
};

server.on(String("/api/"+String(_pin)+ "/set"), HTTP_GET, [this, &server, handleSetRequest]() {  
    handleSetRequest(String(_pin));
});

server.on(String("/api/"+String(mPinAlias)+ "/set"), HTTP_GET, [this, &server, handleSetRequest]() {  
    handleSetRequest(String(mPinAlias));
});


    auto handleStatus = [this, &server]() {
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["pin"] = _pin;
        response["pin_name"] = mPinAlias;
        response["value"] = _value;
        response["state_value"] = _value;
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    };

    server.on("/api/" + String(_pin) + "/status", HTTP_GET, [this, &server, handleStatus]() {
        handleStatus();
    });

    server.on("/api/" + String(mPinAlias) + "/status", HTTP_GET, [this, &server, handleStatus]() {
        handleStatus();
    });


}

String AnalogPinModule::getHTML() {
    String pin = String(_pin);
    String html = "<div><h2>" + mName + "</h2>";
    html += "<input type='range' min='0' max='255' id='"+ pin +"' value='";
    html += String(_value);
    html += "' onchange=\"fetch(`/api/"+ pin + "/set?value=${this.value}`).then(() => ws.send('getStatus'))\">";
    html += "<span> Value: <span id='"+ pin +"Value'>" + String(_value) + "</span></span>";
    html += "</div>";
    return html;
}

void AnalogPinModule::setValue(int value) {
    _value = constrain(value, 0, 255);
    ledcWrite(0, _value);
    Serial.printf("Analaog pin %d value set to: %d\n",_pin, _value);
}

void AnalogPinModule::sendUpdate(WebSocketsServer& ws) {
    DynamicJsonDocument doc(100);
    doc[String(_pin)] = _value;
    String json;
    serializeJson(doc, json);
    ws.broadcastTXT(json);
}