#ifndef DIGITAL_INPUT_MODULE_H
#define DIGITAL_INPUT_MODULE_H

#include "ESP32Dashboard.h"

class DigitalInputModule : public DashboardModule {
public:
    // Constructor with pin only (auto‑generated alias and name)
    DigitalInputModule(int pin, bool pullup = true);
    // Full constructor with custom alias, name, description and pull‑up option
    DigitalInputModule(int pin, String pinAlias, String name, String description = "", bool pullup = true);

    void handleRequest(WebServer& server) override;
    String getHTML() override;
    void sendUpdate(WebSocketsServer& ws) override;

    int read();          // returns current state (0/1)
    bool isHigh();       // convenience: state == HIGH
    bool isLow();        // convenience: state == LOW

private:
    int _pin;
    bool _pullup;        // true = INPUT_PULLUP, false = INPUT
    bool _lastState;     // for change detection (optional)
    String mName;
    String mPinAlias;
    String mDescription;
};

#endif