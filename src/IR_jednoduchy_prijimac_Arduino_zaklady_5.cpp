#define IR_PIN_PRIJIMACE 2
#define NEOPIXEL_PIN 3
#define INDIKATOR_PIN 4
#define POCET_LED 24

#include <Arduino.h>
#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pasek(POCET_LED, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);	//definice nového LED pásku
Adafruit_NeoPixel indikator(1, INDIKATOR_PIN, NEO_GRB + NEO_KHZ800);	//definice nového LED pásku

boolean zapnuto;
byte mod = 0; //0 - jas, 1 - R, 2 - G, 3 - B
byte moznosti[4];

const byte krok = 17;

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
  for(byte moz = 0; moz < 4; moz++){
    moznosti[moz] = 255;
  }    
  pasek.clear();
  zapnuto = false;
  Serial.println("Pripraven!");
}

void loop() {
  if(IrReceiver.decode()){
    //Vytisknout na sériovém monitoru
    IrReceiver.resume();
    if(IrReceiver.decodedIRData.flags == 0){
      //IrReceiver.printIRResultShort(&Serial);
      switch(IrReceiver.decodedIRData.command){
        case 0x13:  //zap./vyp. - tlačítko OK
          zapnuto = !zapnuto;
          if(!zapnuto){
            pasek.clear();
          }
          else{
            for(byte ledka = 0; ledka < POCET_LED; ledka++){
              pasek.setPixelColor(ledka, moznosti[1], moznosti[2], moznosti[3]);
            }
            pasek.setBrightness(moznosti[0]);
            pasek.show();
          }
          Serial.print("STAV: ");
          Serial.println(zapnuto);
          break;
        case 0xE:  //funkce
          switch(mod){
            case 3:
              mod = 0;
              break;
            default:
              mod++;
              break;
          }
          Serial.print("MOD: ");
          Serial.println(mod);
          break;
        case 0x16:  //reset
          setup();
      }    
    }
    switch(IrReceiver.decodedIRData.command){
      case 0x11:  //+
        if(moznosti[mod] <= 255 - krok + 1){
          moznosti[mod] += krok;
        }
        pasek.setBrightness(moznosti[0]);
        for(byte ledka = 0; ledka < POCET_LED; ledka++){
          pasek.setPixelColor(ledka, moznosti[1], moznosti[2], moznosti[3]);
        }
        break;
      case 0x15:  //-
        if(moznosti[mod] >= 0 + krok - 1){
          moznosti[mod] -= krok;
        }
        pasek.setBrightness(moznosti[0]);
        for(byte ledka = 0; ledka < POCET_LED; ledka++){
          pasek.setPixelColor(ledka, moznosti[1], moznosti[2], moznosti[3]);
        }
        break;
      default:
        break;
    }

    pasek.show();   
  }
}