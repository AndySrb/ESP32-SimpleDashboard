#ifndef LED_MODULE_H
#define LED_MODULE_H

#include "ESP32Dashboard.h"

class DigitalPinModule : public DashboardModule {
public:
    DigitalPinModule(int pin);
    DigitalPinModule(int pin, String pinAlias, String name, String description = "");
    void handleRequest(WebServer& server) override;
    String getHTML() override;
    void sendUpdate(WebSocketsServer& ws) override;
    void toggle();

private:
    int _pin;
    bool _state;
    String mName;
    String mPinAlias; // Pin name in API
    String mDescription;
};

#endif