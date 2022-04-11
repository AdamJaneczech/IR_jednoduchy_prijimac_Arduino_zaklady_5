#define IR_PIN_PRIJIMACE 2
#define NEOPIXEL_PIN 3
#define POCET_LED 24

#include <Arduino.h>
#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

Adafruit_NeoPixel pasek(POCET_LED, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);	//definice nového LED pásku

boolean zapnuto;
byte mod = 0; //0 - jas, 1 - R, 2 - G, 3 - B
byte moznosti[3];

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

void problikni(){
  byte indikator[3] = {0,0,0};
  indikator[mod] = 255;
  for(byte opakovani = 0; opakovani < 3; opakovani++){
    pasek.setPixelColor(0, indikator[0], indikator[1], indikator[2]);
    pasek.show();
    delay(100);
    pasek.setPixelColor(0, 0, 0, 0);
    pasek.show();
    delay(100);
  }
}

void setup() {
  //zahájit sériovou komunikaci (115200 baud/s)
  Serial.begin(115200);
  //zahájení činnosti Neopixel LED pásku
  pasek.begin();   
  //začátek relace přijímače
  IrReceiver.begin(IR_PIN_PRIJIMACE);  
  //vynulovat všechny hodnoty proměnné moznosti pomocí cyklu 
  for(byte moz = 0; moz < 3; moz++){
    moznosti[moz] = 255;
  }   
  mod = 0; 
  pasek.clear();
  problikni();
  zapnuto = false;
  Serial.println("Pripraven!");
}

void loop() {
  if(IrReceiver.decode()){
    IrReceiver.resume();
    if(IrReceiver.decodedIRData.flags == 0){  //v případě, že flags = 0, nebyl vyslán opakovací signál -> nebude neustále dokola docházet k opakování příkazu
      //IrReceiver.printIRResultShort(&Serial);
      switch(IrReceiver.decodedIRData.command){
        case 0x16:  //reset
          setup();
          break;
        case 0x13:  //zap./vyp. - tlačítko OK
          zapnuto = !zapnuto;
          if(!zapnuto){
            pasek.clear();
          }
          else{
            //nastavit jas a barvu LEDek podle proměnných
            for(byte ledka = 0; ledka < POCET_LED; ledka++){
              pasek.setPixelColor(ledka, moznosti[0], moznosti[1], moznosti[2]);
            }
            pasek.show();
          }
          break;
        case 0x1B:
          for(byte index = 0; index < 3; index++){
            EEPROM.write(index, moznosti[index]);
          }
          pasek.clear();
          for(byte ledka = 0; ledka < POCET_LED; ledka++){
            pasek.setPixelColor(ledka, moznosti[0], moznosti[1], moznosti[2]);
            pasek.show();
            delay(30);
          }  
          break;
        case 0x41:
          for(byte index = 0; index < 3; index++){
            moznosti[index] = EEPROM.read(index);
          }
          pasek.clear();
          for(int ledka = POCET_LED - 1; ledka >= 0; ledka--){
            pasek.setPixelColor(ledka, moznosti[0], moznosti[1], moznosti[2]);
            pasek.show();
            delay(30);
          }
          zapnuto = true;
          break;
        case 0xE:  //funkce - přepínání
          switch(mod){
            case 2:
              mod = 0;
              break;
            default:
              mod++;
              break;
          }
          Serial.print("MOD: ");
          Serial.println(mod);
          problikni();
          if(zapnuto){
            pasek.setPixelColor(0, moznosti[0], moznosti[1], moznosti[2]);
            pasek.show();
          }
          break;
      }
    }
    switch(IrReceiver.decodedIRData.command){
      case 0x11:  //+
        if(moznosti[mod] <= 255 - krok + 1){
          moznosti[mod] += krok;
        }
        //nastavit novou barvu/jas dle aktuální hodnoty v poli proměnných pomocí cyklu
        for(byte ledka = 0; ledka < POCET_LED; ledka++){
          pasek.setPixelColor(ledka, moznosti[0], moznosti[1], moznosti[2]);
        }
        break;
      case 0x15:  //-
        if(moznosti[mod] >= 0 + krok - 1){
          moznosti[mod] -= krok;
        }
        for(byte ledka = 0; ledka < POCET_LED; ledka++){
          pasek.setPixelColor(ledka, moznosti[0], moznosti[1], moznosti[2]);
        }
        break;
    }
    pasek.show();   
  }
}