#include "Logger.h"
#include "CommonData.h"
#include "RedController.h"

#define RED_PIN                 12
#define GREEN_PIN               14
#define BLUE_PIN                13
#define MAX_BRIGHTNESS          718
#define DEVICE_NAME             "RGB_LED_123"
#define DEVICE_PASSWORD         "password123"
#define DEVICE_NAME_COMMAND_NAME          "rcdid"
#define DEVICE_PASSWORD_COMMAND_NAME      "rcdpa"
#define RED_NAME_COMMAND_NAME             "rcsid"
#define RED_PASSWORD_COMMAND_NAME         "rcpid"

//-------Objects Creation-------//
RedController redController;


//-----Variables Definition-----//
String redName = "Familia_Duran";
String pass = "rodriguez";
int actualBrightness = MAX_BRIGHTNESS/2;
String actualHexValue = "#000000";

void setupLogger() {
  Serial.begin(115200);
  Serial.println();
  Logger::systemLogLevel = Logger::DEBUG_LOG;
  Logger::log("Starting Systems...", Logger::INFO_LOG);
  Logger::log("The Logger level is: " + String(Logger::systemLogLevel), Logger::INFO_LOG);
  Logger::log("DEBUG MODE ON", Logger::DEBUG_LOG);
}

void configGPIO() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
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
  CommonData::createCommonData("red_color_value", "rgbh", 9, "#000000", "String", [](String hexValue) {
    setColorFromHex(hexValue);
  });
  CommonData::createCommonData("actual_brightness", "lhbr", 9, String(actualBrightness), "int", [](String newValue) {
    setActualBrightness(newValue);
  });
  CommonData::getDataFromEeprom();
}

void configRedController() {
  redController.init(CommonData::getValueFromCommonDataByCommandName(DEVICE_NAME_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(DEVICE_PASSWORD_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(RED_NAME_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(RED_PASSWORD_COMMAND_NAME));
  redController.setIsValidArgument(CommonData::checkIfCommandNameExist);
  redController.setSetValues(CommonData::setValueByCommandName);
  redController.setGetValue(CommonData::getValueFromCommonDataByCommandName);
}

void startLights() {
  setColorFromRGB(255, 0, 0);
  delay(250);
  setColorFromRGB(0, 255, 0);
  delay(250);
  setColorFromRGB(0, 0, 255);
  delay(250);
  setColorFromRGB(255, 255, 255);
  delay(250);
  setColorFromRGB(0, 0, 0);
}

void setup() {
  setupLogger();
  configGPIO();
  fillCommonData();
  configRedController();
  startLights();
}

void loop() {
  redController.handleClient();
  redController.handleConnection();
}

void setColorFromRGB(int r, int g, int b) {
  Logger::log("Setting RGB Value: " + String(r) + String(g) + String(b), Logger::INFO_LOG);
  r = map(r, 0, 255, 0, actualBrightness);
  g = map(g, 0, 255, 0, actualBrightness);
  b = map(b, 0, 255, 0, actualBrightness);
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void setColorFromHex(String hexValue) {
  actualHexValue = hexValue;
  Logger::log("Setting Hex Value: " + hexValue, Logger::DEBUG_LOG);
  int number = (int) strtol( &hexValue[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  setColorFromRGB(r, g, b);

}

void setActualBrightness(String newValue){
  int newValueMapped = map(newValue.toInt(), 0, 100, 0, MAX_BRIGHTNESS);
  actualBrightness = newValueMapped;
  setColorFromHex(actualHexValue);
}
