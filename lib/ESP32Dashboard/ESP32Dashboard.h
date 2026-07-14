#ifndef ESP32_DASHBOARD_H
#define ESP32_DASHBOARD_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <vector>
#include <queue>

class DashboardModule {
public:
    virtual ~DashboardModule() = default;
    virtual void handleRequest(WebServer& server) = 0;
    virtual String getHTML() = 0;
    virtual void sendUpdate(WebSocketsServer& ws) { /* Optional */ }
};

enum NotificaionConstraint {EQUAL, HIGHER, LOWER, ON_POWER, OFF_POWER};

class DashboardNotificationModule {
    DashboardNotificationModule(int pin0, NotificaionConstraint constrain);
    NotificaionConstraint constraint;
    std::queue<String> history;
};

class ESP32Dashboard {
public:
    ESP32Dashboard(const char* ssid, const char* password);
    void begin();
    void addModule(DashboardModule* module);
    void addNotificationModule(DashboardNotificationModule* notification_module);
    void handleClient();

private:
    void setupRoutes();
    void handleRoot();
    void handleNotFound();
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
    void broadcastUpdate();

    WebServer server;
    WebSocketsServer webSocket;
    std::vector<DashboardModule*> modules;
    std::vector<DashboardNotificationModule*> notificaion_module;

    const char* _ssid;
    const char* _password;
};

#endif