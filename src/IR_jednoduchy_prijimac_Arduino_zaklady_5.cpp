#define IR_PIN_PRIJIMACE 2

#include <Arduino.h>
#include <IRremote.hpp>

//definice pro dekódování ovladačů od různých výrobců
#define DECODE_DENON
#define DECODE_JVC
#define DECODE_KASEIKYO
#define DECODE_PANASONIC
#define DECODE_LG
#define DECODE_NEC
#define DECODE_SAMSUNG
#define DECODE_SONY
#define DECODE_RC5
#define DECODE_RC6

#define DECODE_BOSEWAVE
#define DECODE_LEGO_PF
#define DECODE_MAGIQUEST
#define DECODE_WHYNTER

//pole pro uložení 8-bitových hodnot dat z ovladače
uint8_t infraData[110];  

void setup() {
  //zahájit sériovou komunikaci (115200 baud/s)
  Serial.begin(115200);   
  //začátek relace přijímače
  IrReceiver.begin(IR_PIN_PRIJIMACE);        
  Serial.println("Pripraven!");
}

void loop() {
  if(IrReceiver.decode()){
      //Vytisknout na sériovém monitoru
      IrReceiver.printIRResultShort(&Serial);
      //Uložit výsledky čtení do pole
      IrReceiver.compensateAndStoreIRResultInArray(infraData);
  }
}