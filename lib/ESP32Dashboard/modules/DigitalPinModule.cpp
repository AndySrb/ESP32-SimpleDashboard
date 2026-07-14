#include "DigitalPinModule.h"

DigitalPinModule::DigitalPinModule(int pin) : _pin(pin), _state(false) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, 0);
    mPinAlias = _pin;
    mName = String("Digital Pin Toggle PIN" + mPinAlias);
    mDescription = "";
}

DigitalPinModule::DigitalPinModule(int pin, String pinAlias, String name, String description) :
_pin(pin), _state(false), mPinAlias(pinAlias), mName(name), mDescription(description) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, 0);
}

void DigitalPinModule::handleRequest(WebServer& server) {
    // Shared toggle logic
    auto handleToggle = [this, &server]() {
        toggle();
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["message"] = "Digital pin toggled";
        response["pin"] = _pin;
        response["pin_name"] = mPinAlias;
        response["state"] = _state ? "ON" : "OFF";
        response["state_value"] = _state;
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    };
    
    server.on("/api/" + mPinAlias + "/toggle", HTTP_GET, [this, &server, handleToggle]() {
        handleToggle();
    });
    
    server.on("/api/" + String(_pin) + "/toggle", HTTP_GET, [this, &server, handleToggle](void) {
        handleToggle();
    });

    auto handleStatus = [this, &server]() {
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["pin"] = _pin;
        response["pin_name"] = mPinAlias;
        response["state"] = _state ? "ON" : "OFF";
        response["state_value"] = _state;
        
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

/*
void DigitalPinModule::handleRequest(WebServer& server) {

        server.on("/api/" + mName + "/toggle", HTTP_GET, [this, &server]() {
        toggle();
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["message"] = "Digital pin toggled";
        response["pin"] = _pin;
        response["pin_name"] = mName;
        response["state"] = _state ? "ON" : "OFF";
        response["state_value"] = _state;
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    });

    server.on("/api/" + mName + "/status", HTTP_GET, [this, &server]() {
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["pin"] = _pin;
        response["pin_name"] = mName;
        response["state"] = _state ? "ON" : "OFF";
        response["state_value"] = _state;
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    });

}
*/

String DigitalPinModule::getHTML() {
    const String pin = String(_pin);
    String html = "<div><h2>" + mName + "</h2>";
    html += "<button id='" + pin + "' onclick=\"fetch('/api/" + pin + "/toggle').then(() => { if(window.ws && ws.readyState === WebSocket.OPEN) ws.send('getStatus'); })\">";
    html += (_state ? "Turn OFF" : "Turn ON");
    html += "</button></div>";
    return html;
}

void DigitalPinModule::toggle() {
    _state = !_state;
    digitalWrite(_pin, _state ? HIGH : LOW);
}

void DigitalPinModule::sendUpdate(WebSocketsServer& ws) {
    DynamicJsonDocument doc(100);
    doc[String(_pin)] = _state ? "ON" : "OFF";
    String json;
    serializeJson(doc, json);
    ws.broadcastTXT(json);
}