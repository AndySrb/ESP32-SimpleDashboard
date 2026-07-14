#ifndef ANALOG_INPUT_MODULE_H
#define ANALOG_INPUT_MODULE_H

#include "ESP32Dashboard.h"

class AnalogInputModule : public DashboardModule {
public:
    // Constructor with pin only (auto alias/name)
    AnalogInputModule(int pin, int atten = ADC_11db, int samples = 10);
    // Full constructor
    AnalogInputModule(int pin, String pinAlias, String name, String description = "", int atten = ADC_11db, int samples = 10);

    void handleRequest(WebServer& server) override;
    String getHTML() override;
    void sendUpdate(WebSocketsServer& ws) override;

    int readRaw();           // returns averaged 0…4095 (or single read if samples=1)
    int readSingle();        // returns single 0…4095 (no averaging)
    float readVoltage();     // returns voltage (0…3.3V approx.)
    void setSamples(int samples);  // change number of samples for averaging

private:
    int readAverage(int pin, int samples);
    
    int _pin;
    int _attenuation;       // ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
    int _samples;           // number of samples for averaging
    int _lastRaw;
    String mName;
    String mPinAlias;
    String mDescription;
};

#endif