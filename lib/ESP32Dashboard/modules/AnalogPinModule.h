#ifndef PWM_MODULE_H
#define PWM_MODULE_H

#include "ESP32Dashboard.h"

class AnalogPinModule : public DashboardModule {
public:
    AnalogPinModule(int pin);
    AnalogPinModule(int pin, String pinAlias, String name, String description = "");
    void handleRequest(WebServer& server) override;
    String getHTML() override;
    void sendUpdate(WebSocketsServer& ws) override;
    void setValue(int value);

private:
    int _pin;
    int _value;
    String mName;
    String mPinAlias;
    String mDescription;
};

#endif