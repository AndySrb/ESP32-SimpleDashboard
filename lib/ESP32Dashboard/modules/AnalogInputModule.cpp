#include "AnalogInputModule.h"

AnalogInputModule::AnalogInputModule(int pin, int atten, int samples)
    : _pin(pin), _attenuation(atten), _samples(samples), _lastRaw(0) {
    analogReadResolution(12);
    pinMode(_pin, INPUT);
    analogSetPinAttenuation(_pin, (adc_attenuation_t)_attenuation);
    mPinAlias = String(_pin);
    mName = String("Analog Input PIN" + mPinAlias);
    mDescription = "";
}

AnalogInputModule::AnalogInputModule(int pin, String pinAlias, String name, String description, int atten, int samples)
    : _pin(pin), _attenuation(atten), _samples(samples), _lastRaw(0), mPinAlias(pinAlias), mName(name), mDescription(description) {
    analogReadResolution(12);
    pinMode(_pin, INPUT);
    analogSetPinAttenuation(_pin, (adc_attenuation_t)_attenuation);
}

void AnalogInputModule::handleRequest(WebServer& server) {
    auto handleStatus = [this, &server]() {
        int raw = readAverage(_pin, _samples);
        float volts = (raw / 4095.0) * 3.3;
        DynamicJsonDocument response(256);
        response["success"] = true;
        response["pin"] = _pin;
        response["pin_name"] = mPinAlias;
        response["raw"] = raw;
        response["voltage"] = volts;
        response["percent"] = (raw * 100) / 4095;
        response["samples"] = _samples;

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

String AnalogInputModule::getHTML() {
    String pinStr = String(_pin);
    int raw = readAverage(_pin, _samples);
    float volts = (raw / 4095.0) * 3.3;
    String html = "<h2>" + mName + "</h2>";
    html += "<p>Pin " + pinStr + ": <strong id='analog_raw_" + pinStr + "'>" + String(raw) + "</strong> raw";
    html += " (<span id='analog_volt_" + pinStr + "'>" + String(volts, 2) + "</span> V)</p>";
    html += "<progress id='progress_" + pinStr + "' value='" + String(raw) + "' max='4095'></progress>";
    html += "<p><em>" + mDescription + "</em></p>";
    html += "<p><small>Averaged over " + String(_samples) + " samples</small></p>";
    return html;
}

void AnalogInputModule::sendUpdate(WebSocketsServer& ws) {
    int raw = readAverage(_pin, _samples);
    if (abs(raw - _lastRaw) < 5) return; // reduced threshold due to averaging
    _lastRaw = raw;

    DynamicJsonDocument doc(150);
    doc["type"] = "analog_input";
    doc["pin"] = _pin;
    doc["raw"] = raw;
    doc["voltage"] = (raw / 4095.0) * 3.3;

    String json;
    serializeJson(doc, json);
    ws.broadcastTXT(json);
}

// Private method: read and average multiple samples
int AnalogInputModule::readAverage(int pin, int samples) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(pin);
        delayMicroseconds(100);  // Allow ADC to settle
    }
    return sum / samples;
}

// Public method: returns averaged reading
int AnalogInputModule::readRaw() {
    return readAverage(_pin, _samples);
}

// Public method: returns single reading (no averaging)
int AnalogInputModule::readSingle() {
    return analogRead(_pin);
}

// Public method: returns voltage based on averaged reading
float AnalogInputModule::readVoltage() {
    return (readAverage(_pin, _samples) / 4095.0) * 3.3;
}

// Public method: change sample count at runtime
void AnalogInputModule::setSamples(int samples) {
    if (samples >= 1 && samples <= 100) {  // reasonable bounds
        _samples = samples;
    }
}