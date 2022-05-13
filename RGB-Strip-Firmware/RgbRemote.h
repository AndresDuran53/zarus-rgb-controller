#ifndef RgbRemote_h
#define RgbRemote_h

#include <Arduino.h>

class RgbRemote {
  private:
    int type = 0;
    uint32_t btnOn;
    uint32_t btnOff;
    uint32_t btnUp;
    uint32_t btnDown;
    
    uint32_t btnFlash;
    uint32_t btnStrobe;
    uint32_t btnFade;
    uint32_t btnSmooth;
    
    uint32_t btnRed;
    uint32_t btnGreen;
    uint32_t btnBlue;
    uint32_t btnWhite;

    uint32_t btnOrange;
    uint32_t btnLightOrange;
    uint32_t btnDarkYellow;
    uint32_t btnYellow;

    uint32_t btnLightGreen;
    uint32_t btnVAgua;
    uint32_t btnDarkVAgua;
    uint32_t btnCeleste;

    uint32_t btnPurple;
    uint32_t btnDarkPurple;
    uint32_t btnDarkPink;
    uint32_t btnPink;

    void fillButtons();
    
  public:
    RgbRemote(int type);
    String getIRDecodeValue(uint32_t value);
    String getValueType(uint32_t value);

};
#endif
