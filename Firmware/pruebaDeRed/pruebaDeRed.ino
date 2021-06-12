#include "RedController.h"

RedController redController;

String netName = "Familia_Duran";
String pass = "rodriguez";
String deviceSsid = "arduino546";
String devicePass = "password123";

void setup() {
  Serial.begin(115200);
  Serial.println();
  redController.init(deviceSsid, devicePass, netName, pass);
  redController.setup_wifi();
}

void loop() {
  redController.handleClient();
  redController.handleConnection();
}
