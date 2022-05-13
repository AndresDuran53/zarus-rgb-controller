#include "RgbRemote.h"

void RgbRemote::fillButtons() {
  switch (this->type) {
    case 0:
      btnOn = 0xF7C03F;
      btnOff = 0xF740BF;
      btnUp = 0xF700FF;
      btnDown = 0xF7807F;

      btnFlash = 0xF7D02F;
      btnStrobe = 0xF7F00F;
      btnFade = 0xF7C837;
      btnSmooth = 0xF7E817;

      btnRed = 0xF720DF;
      btnGreen = 0xF7A05F;
      btnBlue = 0xF7609F;
      btnWhite = 0xF7E01F;

      btnOrange = 0xF710EF;
      btnLightOrange = 0xF730CF;
      btnDarkYellow = 0xF708F7;
      btnYellow = 0xF728D7;

      btnLightGreen = 0xF7906F;
      btnVAgua = 0xF7B04F;
      btnDarkVAgua = 0xF78877;
      btnCeleste = 0xF7A857;

      btnPurple = 0xF750AF;
      btnDarkPurple = 0xF7708F;
      btnDarkPink = 0xF748B7;
      btnPink = 0xF76897;
      break;
  }
}


String RgbRemote::getIRDecodeValue(uint32_t value) {
  if (value == btnOn) {
    return "ON";
  }
  else if (value == btnOff) {
    return "OFF";
  }
  else if (value == btnUp) {
    return "BRILLO_ARRIBA";
  }
  else if (value == btnDown) {
    return "BRILLO_ABAJO";
  }
  else if (value == btnFlash) {
    return "FLASH";
  }
  else if (value == btnStrobe) {
    return "STROBE";
  }
  else if (value == btnFade) {
    return "FADE";
  }
  else if (value == btnSmooth) {
    return "SMOOTH";
  }
  else if (value == btnWhite) {
    return "#FFFFFF";
  }
  else if (value == btnRed) {
    return "#FF0000";
  }
  else if (value == btnGreen) {
    return "#00FF00";
  }
  else if (value == btnBlue) {
    return "#0000FF";
  }
  else if (value == btnOrange) {
    return "#EF4009";
  }
  else if (value == btnLightOrange) {
    return "#F78A2F";
  }
  else if (value == btnDarkYellow) {
    return "#FEAC60";
  }
  else if (value == btnYellow) {
    return "#F6E300";
  }
  else if (value == btnLightGreen) {
    return "#1DB046";
  }
  else if (value == btnVAgua) {
    return "#1FC3C5";
  }
  else if (value == btnDarkVAgua) {
    return "#008D9D";
  }
  else if (value == btnCeleste) {
    return "#055964";
  }
  else if (value == btnPurple) {
    return "#067DD1";
  }
  else if (value == btnDarkPurple) {
    return "#571D46";
  }
  else if (value == btnDarkPink) {
    return "#791C4B";
  }
  else if (value == btnPink) {
    return "#D23D67";
  }
  return "Ninguno";
}

RgbRemote::RgbRemote(int type) {
  this->type = type;
  fillButtons();
}
