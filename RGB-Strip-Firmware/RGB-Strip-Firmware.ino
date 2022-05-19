#include <IoTController.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "RgbRemote.h"

#define RED_PIN                 12
#define GREEN_PIN               14
#define BLUE_PIN                13
#define MAX_BRIGHTNESS          100

String deviceType = "rgb-light";
String deviceToken = "zzz999";
uint8_t consoleLevel = 3;

const uint16_t kRecvPin = 5;
unsigned long key_value = 0;
IRrecv irrecv(kRecvPin);
decode_results results;
RgbRemote rgbRemote(0);

//-----Variables Definition-----//
int actualBrightness = MAX_BRIGHTNESS;
String actualHexValue = "#000000";
int display_step = 0;
int display_mode = 0;

void configGPIO() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(kRecvPin, INPUT);
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

void setColorFromRGB(int r, int g, int b) {
  Logger::log("Setting RGB Value: " + String(r) + String(g) + String(b), Logger::INFO_LOG);
  Logger::log("Actual Brightness: " + String(actualBrightness), Logger::INFO_LOG);
  r = map((r * (actualBrightness / 100.0)), 255, 0, 0, 1024);
  g = map((g * (actualBrightness / 100.0)), 255, 0, 0, 1024);
  b = map((b * (actualBrightness / 100.0)), 255, 0, 0, 1024);
  int r_aux = min(1024, r);
  int g_aux = min(1024, g);
  int b_aux = min(1024, b);
  analogWrite(RED_PIN, r_aux);
  analogWrite(GREEN_PIN, g_aux);
  analogWrite(BLUE_PIN, b_aux);
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

void setActualBrightness(String newValue) {
  Logger::log("Setting new Brightness: " + String(newValue), Logger::DEBUG_LOG);
  int newValueMapped = newValue.toInt();
  newValueMapped = max(0, min(100, newValueMapped));
  newValueMapped = max(0, newValueMapped);
  actualBrightness = newValueMapped;
  setColorFromHex(actualHexValue);
}

//Animations
void stepAnimation() {
  switch (display_mode) {
    case 1:
    flashEffect();
      break;
    case 2:
    strobeEffect();
      break;
    case 3:
      fadeEffect();
      break;
    case 4:
      smoothEffect();
      break;
    default:
      break;
  }
  display_step++;
}

void flashEffect() {
  int WheelPos = (display_step % 255) / 4;
  int WheelPos2 = (display_step % 80) / 20;
  int red = 0;
  int green = 0;
  int blue = 0;
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    red = 255 - WheelPos * 3;
    green = 0;
    blue = WheelPos * 3;
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    red = 0;
    green = WheelPos * 3;
    blue = 255 - WheelPos * 3;
  }
  else {
    WheelPos -= 170;
    red = WheelPos * 3;
    green = 255 - WheelPos * 3;
    blue = 0;
  }
  String hexValue = rgbToHex(red,green,blue);
  setColorFromHex(hexValue);
  if(WheelPos2%2==0){
    setActualBrightness(String(100));
  }
  else{
    setActualBrightness(String(0));
  }
} //flashEffect()

void strobeEffect() {
  int WheelPos = (display_step % 255) / 8;
  if(WheelPos%2==0){
    setActualBrightness(String(100));
  }
  else{
    setActualBrightness(String(0));
  }
} //strobeEffect()

void fadeEffect() {
  int WheelPos = display_step % 255;
  int red = 0;
  int green = 0;
  int blue = 0;
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    red = 255 - WheelPos * 3;
    green = 0;
    blue = WheelPos * 3;
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    red = 0;
    green = WheelPos * 3;
    blue = 255 - WheelPos * 3;
  }
  else {
    WheelPos -= 170;
    red = WheelPos * 3;
    green = 255 - WheelPos * 3;
    blue = 0;
  }
  String hexValue = rgbToHex(red,green,blue);
  setColorFromHex(hexValue);
} //fadeEffect()

void smoothEffect() {
  int WheelPos = (display_step % 1024) / 4;
  int red = 0;
  int green = 0;
  int blue = 0;
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    red = 255 - WheelPos * 3;
    green = 0;
    blue = WheelPos * 3;
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    red = 0;
    green = WheelPos * 3;
    blue = 255 - WheelPos * 3;
  }
  else {
    WheelPos -= 170;
    red = WheelPos * 3;
    green = 255 - WheelPos * 3;
    blue = 0;
  }
  String hexValue = rgbToHex(red,green,blue);
  setColorFromHex(hexValue);
} //smoothEffect()

//---------------------------------//

void verifyIRrecieved(String value) {
  if (value == "ON") {
    display_mode = 0;
    setActualBrightness("100");
  }
  else if (value == "OFF") {
    display_mode = 0;
    setActualBrightness("0");
  }
  else if (value == "BRILLO_ARRIBA") {
    int actualBrightnessAux = map(actualBrightness, 0, MAX_BRIGHTNESS, 0, 100);
    setActualBrightness(String(actualBrightnessAux + 10));
  }
  else if (value == "BRILLO_ABAJO") {
    int actualBrightnessAux = map(actualBrightness, 0, MAX_BRIGHTNESS, 0, 100);
    setActualBrightness(String(actualBrightnessAux - 10));
  }
  else if (value == "FLASH") {
    display_mode = 1;
    //ToDo
  }
  else if (value == "STROBE") {
    display_mode = 2;
    //ToDo
  }
  else if (value == "FADE") {
    display_mode = 3;
    //ToDo
  }
  else if (value == "SMOOTH") {
    display_mode = 4;
    //ToDo
  }
  else if (value == "Ninguno") {
    return;
  }
  else {
    display_mode = 0;
    setColorFromHex(value);
  }
}

void checkIRrecieved() {
  if (irrecv.decode(&results)) {
    String valueRecieved = rgbRemote.getIRDecodeValue(results.value);
    Logger::log("New IR value recieved: " + String(valueRecieved), Logger::INFO_LOG);
    verifyIRrecieved(valueRecieved);
    irrecv.resume();  // Receive the next value
  }
}

void IoTConfig() {
  IoTController::addTimer(100, checkIRrecieved);
  IoTController::addTimer(20, stepAnimation);
  IoTController::setup(deviceType, consoleLevel, deviceToken);
  IoTController::addCommonData("red_color_value", "rgbh", 9, "#000000", "String", [](String hexValue) {
    setColorFromHex(hexValue);
  });
  IoTController::addCommonData("actual_brightness", "lhbr", 9, String(actualBrightness), "int", [](String newValue) {
    setActualBrightness(newValue);
  });
  IoTController::init();
}

void setup() {
  irrecv.enableIRIn();
  IoTConfig();
  configGPIO();
  startLights();
}

void loop() {
  IoTController::loop();
  //stepAnimation();
  //delay(10);
}

String rgbToHex(int red, int green, int blue){
  char hex[8] = {0};
  sprintf(hex,"#%02X%02X%02X",red,green,blue);
  hex[7]='\0';
  String hexValue = String(hex);
  return hexValue;
}
