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
  Logger::log("Setting RGB Value: " + String(r) + String(g) + String(b), Logger::DEBUG_LOG);
  Logger::log("Actual Brightness: " + String(actualBrightness), Logger::DEBUG_LOG);
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
      setActualBrightness(String(100));
      fadeEffect();
      break;
    case 4:
      setActualBrightness(String(100));
      smoothEffect();
      break;
    case 5:
      breathEffect();
      break;
    case 6:
      attackEffect();
      break;
    case 7:
      setActualBrightness(String(100));
      nightClubEffect();
      break;
    default:
      break;
  }
  display_step++;
}

void setEffect(String newValue) {
  display_mode = newValue.toInt();
}

void flashEffect() {
  int WheelPos = (display_step % 255);
  int WheelPos2 = (display_step % 35);
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
  String hexValue = rgbToHex(red, green, blue);
  if (WheelPos2 == 0) {
    setColorFromHex(hexValue);
  }
} //flashEffect()

void strobeEffect() {
  int WheelPos = (display_step % 255) / 8;
  if (WheelPos % 2 == 0) {
    setActualBrightness(String(100));
  }
  else {
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
  String hexValue = rgbToHex(red, green, blue);
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
  String hexValue = rgbToHex(red, green, blue);
  setColorFromHex(hexValue);
} //smoothEffect()

void breathEffect() {
  int WheelPos = (display_step % 255);
  int WheelPos2 = WheelPos / 8;
  if (WheelPos > 167) {
    WheelPos = 251 - WheelPos + 42;
  }
  WheelPos = map(WheelPos, 42, 125, 0, 100);
  WheelPos = min(100, max(0, WheelPos));
  if (WheelPos2 % 2 == 0) {
    setActualBrightness(String(WheelPos));
  }
} //strobeEffect()

void attackEffect() {
  int WheelPos = (display_step % 63)*4;
  int WheelPos2 = WheelPos / 8;
  WheelPos = map(WheelPos, 42, 125, 100, 0);
  WheelPos = min(100, max(0, WheelPos));
  if (WheelPos2 % 2 == 0) {
    setActualBrightness(String(WheelPos));
  }
} //strobeEffect()

void nightClubEffect() {
  int WheelPos = (display_step % 64) * 4;
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
  String hexValue = rgbToHex(red, green, blue);
  setColorFromHex(hexValue);
} //fadeEffect()

//---------------------------------//

void verifyIRrecieved(String value) {
  if (value == "ON") {
    setEffect("0");
    setActualBrightness("100");
  }
  else if (value == "OFF") {
    setEffect("0");
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
    setEffect("1");
    //ToDo
  }
  else if (value == "STROBE") {
    setEffect("2");
    //ToDo
  }
  else if (value == "FADE") {
    setEffect("3");
    //ToDo
  }
  else if (value == "SMOOTH") {
    setEffect("4");
    //ToDo
  }
  else if (value == "Ninguno") {
    return;
  }
  else {
    setEffect("0");
    setColorFromHex(value);
  }
}

void checkIRrecieved() {
  if (irrecv.decode(&results)) {
    String valueRecieved = rgbRemote.getIRDecodeValue(results.value);
    Logger::log("New IR value recieved: " + String(valueRecieved), Logger::DEBUG_LOG);
    verifyIRrecieved(valueRecieved);
    irrecv.resume();  // Receive the next value
  }
}

void IoTConfig() {
  IoTController::addTimer(100, checkIRrecieved);
  IoTController::addTimer(20, stepAnimation);
  IoTController::setup(deviceType, consoleLevel, deviceToken);
  IoTController::addCommonData("rgb_color_hex", "rgbh", 8, "#000000", "String", [](String hexValue) {
    setColorFromHex(hexValue);
  });
  IoTController::addCommonData("rgb_color_value", "rgbv", 12, "0,0,0", "String", [](String rgbValue) {
    setColorFromRgbString(rgbValue);
  });
  IoTController::addCommonData("actual_brightness", "lhbr", 4, String(actualBrightness), "int", [](String newValue) {
    setActualBrightness(newValue);
  });
  IoTController::addCommonData("display_mode", "dsmd", 2, "0", "String", [](String newValue) {
    setEffect(newValue);
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
}

void setColorFromRgbString(String rgbValue){
  String hexvalue = rgbStringToHex(rgbValue);
  setColorFromHex(hexvalue);
}

String rgbStringToHex(String rgbValue){
  int valuesRGB[3];
  String valueAux = rgbValue;
  int index;
  String redValue;
  if(rgbValue.length()<5 || rgbValue.length()>11){
    return actualHexValue;
  }
  for(int i = 0; i<3; i++){
    index = valueAux.indexOf(',');
    valuesRGB[i] = valueAux.substring(0,index).toInt();
    valueAux = valueAux.substring(index+1,valueAux.length());
  }
  return rgbToHex(valuesRGB[0],valuesRGB[1],valuesRGB[2]);
}

String rgbToHex(int red, int green, int blue) {
  if(red>=0 && green>=0 && blue>=0 && red<=255,green<=255,blue<=255){
    char hex[8] = {0};
    sprintf(hex, "#%02X%02X%02X", red, green, blue);
    hex[7] = '\0';
    String hexValue = String(hex);
    return hexValue;
  }
  else return actualHexValue;
}
