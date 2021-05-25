#include "RedController.h"


ESP8266WebServer RedController::server;

RedController::RedController() {

}

void RedController::init(String deviceSSID, String devicePassword, String redSSID, String redPassword) {
  this->deviceSSID = deviceSSID;
  this->devicePassword = devicePassword;
  this->ssid = redSSID;
  this->password = redPassword;
  shouldConnectToAccessPoint = redSSID != "";
  mdnsName = deviceSSID;
}

void RedController::setup_wifi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(deviceSSID.c_str(), devicePassword.c_str());
  configServer();
}

void RedController::connectToAccessPoint() {
  WiFi.begin(ssid, password);  //Nos conectamos a un AccessPoint con acceso a Internet
}

boolean RedController::isConnectedToAccessPoint() {
  return WiFi.status() == WL_CONNECTED;
}


void RedController::configServer() {
  server.onNotFound(std::bind(&RedController::handleNotFound, this));
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.serveStatic("/", SPIFFS, "/index.html");
  }
  server.begin();
  
}

void RedController::handleNotFound() {
  server.send(404, "text/plain", "Error 404. Not Found");
}

void RedController::update_timeNTP(int dateTime[]) {
  NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);
  timeClient.begin();
  timeClient.update();
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  dateTime[0] = ti->tm_year + 1900;
  dateTime[1] = ti->tm_mon + 1;
  dateTime[2] = ti->tm_mday;
  dateTime[3] = ti->tm_hour;
  dateTime[4] = ti->tm_min;
  dateTime[5] = ti->tm_sec;
}

void RedController::handleClient() {
  server.handleClient();
}

void RedController::setShouldConnectToAccessPoint(boolean shouldConnectToAP) {
  this->shouldConnectToAccessPoint = shouldConnectToAccessPoint;
}

void RedController::setSsid(String ssid) {
  this->ssid = ssid;
}

void RedController::setPassword(String password) {
  this->password = password;
}

void RedController::setDeviceSSID(String deviceSsid) {
  this->deviceSSID = deviceSsid;
}

void RedController::setDevicePassword(String devicePassword) {
  this->devicePassword = devicePassword;
}

void RedController::setDeviceCredentials(String deviceSsid, String devicePassword) {
  boolean anyChange = false;
  if (deviceSsid != "") {
    setDeviceSSID(deviceSsid);
    anyChange = true;
  }
  if (devicePassword != "") {
    setDevicePassword(devicePassword);
    anyChange = true;
  }
  //if (anyChange) ESP.restart();
}


boolean RedController::getShouldConnectToAccessPoint() {
  return shouldConnectToAccessPoint;
}

String RedController::getSsid() {
  return ssid;
}

String RedController::getDeviceSSID() {
  return deviceSSID;
}

String RedController::getPassword() {
  return password;
}

String RedController::getDevicePassword() {
  return devicePassword;
}
