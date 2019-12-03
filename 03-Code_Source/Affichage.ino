#include <Arduino.h>

// for LCD
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  P.begin();
  P.displayText("Test!", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

void loop() {
  // put your main code here, to run repeatedly:
  P.displayAnimate();
  
  return;
}
