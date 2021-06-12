#include "RedController.h"


ESP8266WebServer RedController::server;

RedController::RedController() {

}

void RedController::init(String deviceSSID, String devicePassword, String redSSID, String redPassword) {
  this->deviceSSID = deviceSSID;
  this->devicePassword = devicePassword;
  this->ssid = redSSID;
  this->password = redPassword;
  this->lastConnectionAttempt = 0;
  this->connectionAttemptInterval = 10000;
  this->checkConnectionInterval = 1000;
  this->wasConnectedToAccessPoint = false;
  shouldConnectToAccessPoint = redSSID != "";
  mdnsName = deviceSSID;
}

void RedController::setup_wifi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(deviceSSID);
  WiFi.softAP(deviceSSID.c_str(), devicePassword.c_str());
  myIP = WiFi.softAPIP();
  configServer();
}

void RedController::connectToAccessPoint() {
  WiFi.begin(ssid, password);  //Nos conectamos a un AccessPoint con acceso a Internet
}

boolean RedController::isConnectedToAccessPoint() {
  boolean actualStatus = WiFi.status() == WL_CONNECTED;
  if (actualStatus && !wasConnectedToAccessPoint) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    if (MDNS.begin(deviceSSID)) {
      Serial.print("MDNS responder started at URL: ");
      Serial.print(deviceSSID);
      Serial.println(".local");
    }
  }
  wasConnectedToAccessPoint = actualStatus;
  return actualStatus;
}

void RedController::handleConnection() {
  MDNS.update();
  if (millis() > lastCheckConnection + checkConnectionInterval) {
    lastCheckConnection = millis();
    if (shouldConnectToAccessPoint && !isConnectedToAccessPoint()) {
      if (millis() > lastConnectionAttempt + connectionAttemptInterval) {
        lastConnectionAttempt = millis();
        connectToAccessPoint();
        Serial.print("Trying to Connect to: ");
        Serial.println(ssid);
      }
    }
  }
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
  if (!handleFileRead(server.uri())){
   server.send(404, "text/plain", "404: Not Found"); 
  }
}

String RedController::getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  return "text/plain";
}

bool RedController::handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path = "/index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
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
