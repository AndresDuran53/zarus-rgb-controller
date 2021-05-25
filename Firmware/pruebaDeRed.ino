#include "RedController.h"

RedController redController;

boolean haveToUpdateTime = true;
boolean isRtcWorking;
boolean isRtcUpdating;

String netName = "Familia Masis";
String pass = "mizu7805";
String deviceName = "arduino";
String devicePass = "pass";

void setup() {
  Serial.begin(115200);
  Serial.println();
  redController.init(deviceName, devicePass, netName, pass);
  redController.setup_wifi();
  
  

}

void loop() {
  redController.handleClient();

}
