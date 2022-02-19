#define IR_PIN_PRIJIMACE 2
#define NEOPIXEL_PIN 3
#define INDIKATOR_PIN 4
#define RELE_PIN 5
#define POCET_LED 24

#include <Arduino.h>
#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pasek(POCET_LED, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);	//definice nového LED pásku
Adafruit_NeoPixel indikator(1, INDIKATOR_PIN, NEO_GRB + NEO_KHZ800);	//definice nového LED pásku

boolean zapnuto = false;
byte mod = 0; //0 - jas, 1 - R, 2 - G, 3 - B
byte moznosti[4] = {255, 255, 255, 255};

const byte krok = 32;

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

void setup() {
  //zahájit sériovou komunikaci (115200 baud/s)
  Serial.begin(115200);
  //zahájení činnosti Neopixel LED pásku
  pasek.begin();   
  //začátek relace přijímače
  IrReceiver.begin(IR_PIN_PRIJIMACE);
  //nastavit pin relé jako výstup
  pinMode(NEOPIXEL_PIN, OUTPUT);        
  Serial.println("Pripraven!");
}

void loop() {
  if(IrReceiver.decode()){
    //Vytisknout na sériovém monitoru
    IrReceiver.printIRResultShort(&Serial);
    switch(IrReceiver.decodedIRData.command){
      case 0x00:  //zap./vyp.
        zapnuto ^= zapnuto;
        digitalWrite(RELE_PIN, zapnuto);
        break;
      case 0x01:  //funkce
        switch(mod){
          case 3:
            mod = 0;
            break;
          default:
            mod++;
            break;
        }
        //promyslet pointery
        moznosti[0] = pasek.getBrightness();
        moznosti[1] = pasek.getPixelColor(1);
        break;
      case 0x02:  //+
        if(moznosti[mod] <= 255 - krok){
          moznosti[mod] += krok;
        }
        break;
      case 0x03:  //-
        if(moznosti[mod] >= 0 + krok){
          moznosti[mod] -= krok;
        }
    }
    pasek.setBrightness(moznosti[0]);
    pasek.setPixelColor(1, moznosti[1], moznosti[2], moznosti[3]);
    pasek.show();
  }
}