#include "Logger.h"
#include "CommonData.h"
#include "RedController.h"

#define DEVICE_NAME             "RGB_LED_123"
#define DEVICE_PASSWORD         "password123"
#define DEVICE_NAME_COMMAND_NAME             "rcdid"
#define DEVICE_PASSWORD_COMMAND_NAME         "rcdpa"
#define RED_NAME_COMMAND_NAME             "rcsid"
#define RED_PASSWORD_COMMAND_NAME         "rcpid"

//-------Objects Creation-------//
RedController redController;


//-----Variables Definition-----//
String redName = "Familia_Duran";
String pass = "rodriguez";

void configRedController() {
  redController.init(CommonData::getValueFromCommonDataByCommandName(DEVICE_NAME_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(DEVICE_PASSWORD_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(RED_NAME_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(RED_PASSWORD_COMMAND_NAME));
  redController.setIsValidArgument(CommonData::checkIfCommandNameExist);
  redController.setSetValues(CommonData::setValueByCommandName);
  redController.setGetValue(CommonData::getValueFromCommonDataByCommandName);
}

void fillCommonData() {
  Logger::log("Filling Common data...", Logger::INFO_LOG);
  CommonData::createCommonData("red_deviceSsid", DEVICE_NAME_COMMAND_NAME, 32, DEVICE_NAME, "String", [](String deviceSsid) {
    redController.setDeviceSSID(deviceSsid);
  });
  CommonData::createCommonData("red_devicePassword", DEVICE_PASSWORD_COMMAND_NAME, 32, DEVICE_PASSWORD, "String", [](String devicePassword) {
    redController.setDevicePassword(devicePassword);
  });
  CommonData::createCommonData("red_ssid", RED_NAME_COMMAND_NAME, 32, redName, "String", [](String redSsid) {
    redController.setSsid(redSsid);
  });
  CommonData::createCommonData("red_password", RED_PASSWORD_COMMAND_NAME, 32, pass, "String", [](String redPassword) {
    redController.setPassword(redPassword);
  });
  CommonData::getDataFromEeprom();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Logger::systemLogLevel = Logger::NONE_LOG;
  Logger::log("Starting Systems...", Logger::INFO_LOG);
  Logger::log("The Logger level is: " + String(Logger::systemLogLevel), Logger::INFO_LOG);
  Logger::log("DEBUG MODE ON", Logger::DEBUG_LOG);
  fillCommonData();
  configRedController();
}

void loop() {
  redController.handleClient();
  redController.handleConnection();
}
