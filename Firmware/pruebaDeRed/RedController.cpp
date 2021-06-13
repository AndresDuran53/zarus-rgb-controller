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
  updateIfShouldConnectToAccessPoint();
  mdnsName = deviceSSID;
  setup_wifi();
}

void RedController::setup_wifi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(deviceSSID);
  WiFi.softAP(deviceSSID.c_str(), devicePassword.c_str());
  myIP = WiFi.softAPIP();
  configServer();
}

void RedController::updateIfShouldConnectToAccessPoint() {
  this->shouldConnectToAccessPoint = ssid != "";
}

void RedController::connectToAccessPoint() {
  WiFi.begin(ssid, password);  //Nos conectamos a un AccessPoint con acceso a Internet
}

boolean RedController::isConnectedToAccessPoint() {
  boolean actualStatus = WiFi.status() == WL_CONNECTED;
  if (actualStatus && !wasConnectedToAccessPoint) {
    Logger::log("Connected to : " + ssid, Logger::DEBUG_LOG);
    Logger::log("IP address: " + getIp(), Logger::DEBUG_LOG);
    if (MDNS.begin(deviceSSID)) {
      Logger::log("MDNS responder started at URL: " + deviceSSID + ".local", Logger::DEBUG_LOG);
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
        Logger::log("Trying to Connect to: " + ssid, Logger::DEBUG_LOG);
      }
    }
  }
}

void RedController::configServer() {
  server.on("/getRedInformation", std::bind(&RedController::handleGetRedInformation, this));
  server.on("/setValues", std::bind(&RedController::handleSetValues, this));
  server.onNotFound(std::bind(&RedController::handleNotFound, this));
  if (!SPIFFS.begin())
  {
    Logger::log("SPIFFS Mount failed", Logger::DEBUG_LOG);
  } else {
    Logger::log("SPIFFS Mount succesfull", Logger::DEBUG_LOG);
    server.serveStatic("/", SPIFFS, "/index.html");
    server.serveStatic("/style.css", SPIFFS, "/style.css");
  }
  server.begin();
}

void RedController::handleNotFound() {
  if (!handleFileRead(server.uri())) {
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
  Logger::log("handleFileRead: " + path, Logger::DEBUG_LOG);
  if (path.endsWith("/")) path = "/index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Logger::log("\tFile Not Found", Logger::DEBUG_LOG);
  return false;                                         // If the file doesn't exist, return false
}

void RedController::handleGetRedInformation() {
  String jsonResponse = "{\"deviceSSID\":\""+deviceSSID+"\",\"ssid\":\""+ssid+"\"}";
  server.send(200, "text/json", jsonResponse);
}

void RedController::handleSetValues() {
  String jsonResponse;
  for (uint8_t i = 0; i < server.args(); i++) {
    String argumentName = server.argName(i);
    String argumentValue = server.arg(i);
    if ((*RedController::isValidArgument)(argumentName)) {
      Logger::log("argumentName Exists!", Logger::INFO_LOG);
      boolean wasExecuted = (*RedController::setValues)(argumentName, argumentValue);
      if (wasExecuted) jsonResponse = "{\"result\":\"true\"}";
      else jsonResponse = "{\"result\":\"false\"}";
    }
    else{
    }
  }
  server.send(200, "text/json", jsonResponse);
}

void RedController::handleClient() {
  server.handleClient();
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

void RedController::setShouldConnectToAccessPoint(boolean shouldConnectToAP) {
  this->shouldConnectToAccessPoint = shouldConnectToAccessPoint;
}

void RedController::setSsid(String ssid) {
  this->ssid = ssid;
  updateIfShouldConnectToAccessPoint();
}

void RedController::setPassword(String password) {
  this->password = password;
}

void RedController::setDeviceSSID(String deviceSsid) {
  Logger::log("Changing Device Name: " + deviceSsid, Logger::DEBUG_LOG);
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

void RedController::setIsValidArgument(boolean (setFunction)(String commandName)) {
  this->isValidArgument = setFunction;
}

void RedController::setSetValues(boolean (setFunction)(String argumentName, String argumentValue)) {
  this->setValues = setFunction;
}

void RedController::setGetValue(String (setFuntion)(String commandName)) {
  this->getValue = setFuntion;
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

String RedController::getIp() {
  return WiFi.localIP().toString();
}
