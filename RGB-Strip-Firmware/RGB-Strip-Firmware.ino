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
String hexValueTemp = actualHexValue;
int brightnessTemp = actualBrightness;

void setColorFromHex(String hexValue,int brightness=-1);

void configGPIO() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(kRecvPin, INPUT);
}

void startLights() {
  setColorFromRGB(255, 0, 0, MAX_BRIGHTNESS);
  delay(150);
  setColorFromRGB(0, 255, 0, MAX_BRIGHTNESS);
  delay(150);
  setColorFromRGB(0, 0, 255, MAX_BRIGHTNESS);
  delay(150);
  setColorFromRGB(255, 255, 255, MAX_BRIGHTNESS);
  delay(150);
  setColorFromRGB(0, 0, 0, 0);
}

void setColorFromRGB(int r, int g, int b, int brightness) {
  Logger::log("Setting RGB Value: " + String(r) + String(g) + String(b), Logger::DEBUG_LOG);
  Logger::log("Passed Brightness: " + String(brightness), Logger::DEBUG_LOG);
  r = map((r * (brightness / 100.0)), 255, 0, 0, 1024);
  g = map((g * (brightness / 100.0)), 255, 0, 0, 1024);
  b = map((b * (brightness / 100.0)), 255, 0, 0, 1024);
  int r_aux = min(1024, r);
  int g_aux = min(1024, g);
  int b_aux = min(1024, b);
  analogWrite(RED_PIN, r_aux);
  analogWrite(GREEN_PIN, g_aux);
  analogWrite(BLUE_PIN, b_aux);
}

String getRGBStringFromHex(String hexValue){
  int number = (int) strtol( &hexValue[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  return String(r)+","+String(g)+","+String(b)+",";
}

void setColorFromHex(String hexValue,int brightness) {
  if(brightness==-1) brightness = actualBrightness;
  actualHexValue = hexValue;
  Logger::log("Setting Hex Value: " + hexValue, Logger::DEBUG_LOG);
  Logger::log("Actual Brightness: " + String(actualBrightness), Logger::DEBUG_LOG);
  int number = (int) strtol( &hexValue[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  setColorFromRGB(r, g, b, brightness);
}

void setColorHexFromMqtt(String hexValue){
  setColorFromHex(hexValue,actualBrightness);
  String rgbv = getRGBStringFromHex(hexValue);
  String jsonResponse = "{\"CID\":\"0\",\"rgbv\":\""+rgbv+"\"}";
  IoTController::sendMqttMessage(jsonResponse);
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

void setColorFromRgbString(String rgbValue){
  String hexvalue = rgbStringToHex(rgbValue);
  setColorFromHex(hexvalue);
}

int mapBrightness(int brightness){
  int newValueMapped = max(0, min(MAX_BRIGHTNESS, brightness));
  Logger::log("newValueMapped" + String(newValueMapped), Logger::DEBUG_LOG);
  return newValueMapped;
}

void setActualBrightness(String newValue) {
  Logger::log("Setting new Brightness: " + String(newValue), Logger::DEBUG_LOG);
  int newValueMapped = newValue.toInt();
  actualBrightness = mapBrightness(newValueMapped);
  setColorFromHex(actualHexValue);
}

//Animations
void stepAnimation() {
  switch (display_mode) {
    case 1:
      flashEffect();
      setColorFromHex(hexValueTemp);
      break;
    case 2:
      strobeEffect();
      setColorFromHex(actualHexValue,brightnessTemp);
      break;
    case 3:
      fadeEffect();
      setColorFromHex(hexValueTemp);
      break;
    case 4:
      smoothEffect();
      setColorFromHex(hexValueTemp);
      break;
    case 5:
      breathEffect();
      setColorFromHex(actualHexValue,brightnessTemp);
      break;
    case 6:
      attackEffect();
      setColorFromHex(actualHexValue,brightnessTemp);
      break;
    case 7:
      nightClubEffect();
      setColorFromHex(hexValueTemp);
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
    hexValueTemp = hexValue;
  }
} //flashEffect()

void strobeEffect() {
  int WheelPos = (display_step % 256) / 16;
  if (WheelPos % 2 == 0) {
    brightnessTemp = MAX_BRIGHTNESS;
  }
  else {
    brightnessTemp = 0;
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
  hexValueTemp = hexValue;
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
  hexValueTemp = hexValue;
} //smoothEffect()

void breathEffect() {
  int WheelPos = (display_step % 256);
  int WheelPos2 = WheelPos / 4;
  if (WheelPos > 167) {
    WheelPos = 251 - WheelPos + 42;
  }
  WheelPos = map(WheelPos, 42, 125, 0, MAX_BRIGHTNESS);
  int brightnessAux = mapBrightness(WheelPos);
  if (WheelPos2 % 2 == 0) {
    brightnessTemp = brightnessAux;
  }
} //strobeEffect()

void attackEffect() {
  int WheelPos = (display_step % 64)*4;
  int WheelPos2 = WheelPos / 4;
  WheelPos = map(WheelPos, 42, 125, 100, 0);
  int brightnessAux = mapBrightness(WheelPos);
  if (WheelPos2 % 2 == 0) {
    brightnessTemp = brightnessAux;
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
  hexValueTemp = hexValue;
} //nightClubEffect()

//---------------------------------//

void verifyIRrecieved(String value) {
  if (value == "ON") {
    IoTController::changeStoredValue("dsmd","0");
    IoTController::changeStoredValue("lhbr","100");
  }
  else if (value == "OFF") {
    IoTController::changeStoredValue("dsmd","0");
    IoTController::changeStoredValue("lhbr","0");
  }
  else if (value == "BRILLO_ARRIBA") {
    int actualBrightnessAux = min(MAX_BRIGHTNESS, actualBrightness + 10);
    IoTController::changeStoredValue("lhbr",String(actualBrightnessAux));
  }
  else if (value == "BRILLO_ABAJO") {

    int actualBrightnessAux = max(0, actualBrightness - 10);
    IoTController::changeStoredValue("lhbr",String(actualBrightnessAux));
  }
  else if (value == "FLASH") {
    IoTController::changeStoredValue("dsmd","1");
  }
  else if (value == "STROBE") {
    IoTController::changeStoredValue("dsmd","2");
  }
  else if (value == "FADE") {
    IoTController::changeStoredValue("dsmd","3");
  }
  else if (value == "SMOOTH") {
    IoTController::changeStoredValue("dsmd","4");
  }
  else if (value == "Ninguno") {return;}
  else {
    IoTController::changeStoredValue("dsmd","0");
    IoTController::changeStoredValue("rgbh",value);
  }
}

void checkIRrecieved() {
  if (irrecv.decode(&results)) {
    String valueRecieved = rgbRemote.getIRDecodeValue(results.value);
    Logger::log("New IR value recieved: " + String(valueRecieved), Logger::DEBUG_LOG);
    verifyIRrecieved(valueRecieved);
    irrecv.resume();
  }
}

void IoTConfig() {
  IoTController::addTimer(100, checkIRrecieved);
  IoTController::addTimer(20, stepAnimation);
  IoTController::setup(deviceType, consoleLevel, deviceToken);
  IoTController::createStoredData("rgb_color_hex", "rgbh", 8, "#000000", "String", [](String hexValue) {
    setColorHexFromMqtt(hexValue);
  });
  IoTController::createStoredData("rgb_color_value", "rgbv", 12, "0,0,0", "String", [](String rgbValue) {
    setColorFromRgbString(rgbValue);
  });
  IoTController::createStoredData("actual_brightness", "lhbr", 4, String(actualBrightness), "int", [](String newValue) {
    setActualBrightness(newValue);
  });
  IoTController::createStoredData("display_mode", "dsmd", 2, "0", "String", [](String newValue) {
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
