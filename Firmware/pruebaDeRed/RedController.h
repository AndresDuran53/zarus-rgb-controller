#ifndef RedController_h
#define RedController_h

#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "Logger.h"


class RedController {
  private:
    static ESP8266WebServer server;
    boolean (*isValidArgument)(String commandName);
    boolean (*setValues)(String argumentName, String argumentValue);
    String (*getValue)(String commandName);
    const long UTC_OFFSET_IN_SECONDS = -6 * (60 * 60);
    String ssid;
    String password;
    String deviceSSID;
    String devicePassword;
    boolean shouldConnectToAccessPoint;
    String mdnsName;
    MDNSResponder mdns;
    WiFiUDP ntpUDP;
    IPAddress myIP;
    boolean wasConnectedToAccessPoint;
    int connectionAttemptInterval;
    unsigned long lastConnectionAttempt = 0;
    int checkConnectionInterval;
    unsigned long lastCheckConnection = 0;
    String* allCommandNames;
    void configServer();
    void handleNotFound();
    String getContentType(String filename);
    bool handleFileRead(String path);
    void handleGetRedInformation();
    void handleSetValues();
  public:
    RedController();
    void init(String deviceSSID, String devicePassword, String redSSID, String redPassword);
    void setup_wifi();
    void updateIfShouldConnectToAccessPoint();
    void connectToAccessPoint();
    boolean isConnectedToAccessPoint();
    void update_timeNTP(int dateTime[]);
    void handleConnection();
    void handleClient();
    void setValue(String value);
    void setSsid(String ssid);
    void setPassword(String password);
    void setDeviceSSID(String ssid);
    void setDevicePassword(String password);
    void setDeviceCredentials(String deviceId, String password);
    void setShouldConnectToAccessPoint(boolean shouldConnectToAP);
    void setIsValidArgument(boolean (setFunction)(String commandName));
    void setSetValues(boolean (setFunction)(String argumentName, String argumentValue));
    void setGetValue(String (setFuntion)(String commandName));
    String getSsid();
    String getDeviceSSID();
    boolean getShouldConnectToAccessPoint();
    String getPassword();
    String getDevicePassword();
    String getIp();
};
#endif
