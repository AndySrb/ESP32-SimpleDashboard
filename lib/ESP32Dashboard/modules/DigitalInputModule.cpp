#include "DigitalInputModule.h"

DigitalInputModule::DigitalInputModule(int pin, bool pullup)
    : _pin(pin), _pullup(pullup), _lastState(false) {
    pinMode(_pin, pullup ? INPUT_PULLUP : INPUT);
    mPinAlias = String(_pin);
    mName = String("Digital Input PIN" + mPinAlias);
    mDescription = "";
}

DigitalInputModule::DigitalInputModule(int pin, String pinAlias, String name, String description, bool pullup)
    : _pin(pin), _pullup(pullup), _lastState(false), mPinAlias(pinAlias), mName(name), mDescription(description) {
    pinMode(_pin, pullup ? INPUT_PULLUP : INPUT);
}

void DigitalInputModule::handleRequest(WebServer& server) {
    auto handleStatus = [this, &server]() {
        int state = digitalRead(_pin);
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["pin"] = _pin;
        response["pin_name"] = mPinAlias;
        response["state"] = state ? "HIGH" : "LOW";
        response["state_value"] = state;
        response["pullup"] = _pullup;

        String jsonResponse;
        serializeJson(response, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    };

    server.on("/api/" + String(_pin) + "/status", HTTP_GET, [this, &server, handleStatus]() {
        handleStatus();
    });
    server.on("/api/" + mPinAlias + "/status", HTTP_GET, [this, &server, handleStatus]() {
        handleStatus();
    });
}

String DigitalInputModule::getHTML() {
    String pinStr = String(_pin);
    int state = digitalRead(_pin);
    String html = "<h2>" + mName + "</h2>";
    html += "<p>Pin " + pinStr + " is currently <strong id='input_" + pinStr + "'>";
    html += state ? "HIGH" : "LOW";
    html += "</strong></p>";
    html += "<p><em>" + mDescription + "</em></p>";
    return html;
}

void DigitalInputModule::sendUpdate(WebSocketsServer& ws) {
    int state = digitalRead(_pin);
    if (state == _lastState) return; // reduce traffic
    _lastState = state;

    DynamicJsonDocument doc(100);
    doc["type"] = "digital_input";
    doc["pin"] = _pin;
    doc["state"] = state ? "HIGH" : "LOW";
    doc["state_value"] = state;

    String json;
    serializeJson(doc, json);
    ws.broadcastTXT(json);
}

int DigitalInputModule::read() {
    return digitalRead(_pin);
}

bool DigitalInputModule::isHigh() {
    return digitalRead(_pin) == HIGH;
}

bool DigitalInputModule::isLow() {
    return digitalRead(_pin) == LOW;
}