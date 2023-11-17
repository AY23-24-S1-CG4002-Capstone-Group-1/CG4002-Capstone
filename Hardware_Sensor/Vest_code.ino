//https://forum.arduino.cc/t/how-to-create-a-38-khz-pulse-with-arduino-using-timer-or-pwm/100217/7

// VEST
#define player 1 // Orange (50)
//#define player 2 // BLUE (20)

#include <Adafruit_NeoPixel.h>
#define neoPixelPin A2
#define numPixels 10
Adafruit_NeoPixel NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

int livesNum = 10; // number of HP (1 live, 10 HP)
int HP = 100;

#define RECV_PIN A3 // not pin 2
unsigned long triggertime = 0; 
unsigned long serialtime = 0;
unsigned long receivetime = 0;
unsigned long pulsebuffer = 0;

// int lightuptime = 1000;
int lightuptime = 400;
int hitbywhichplayer = 0; // to send to internal comms
bool receiverstate = 0;
int pulsecheck = 0;

int pulsedelay;


void setup()
{
  Serial.begin(9600);
  NeoPixel.begin(); 
  NeoPixel.setBrightness(50);

   if (player == 1) {
      pulsedelay = 50; // align with player 2 gun
  } else if (player == 2) {
      pulsedelay = 20;   // align with player 1 gun
  }
}
 
void health() {
  HP -= 10;
}

void playerShot() {
  
  livesNum--;
  NeoPixel.clear();
  for(int x = 0; x < livesNum; x++) { // neopixel
      if (player ==  1) {
        NeoPixel.setPixelColor(x, NeoPixel.Color(242, 133, 0)); // orange
      } else if (player == 2) {
        NeoPixel.setPixelColor(x, NeoPixel.Color(0, 0, 255)); // blue
      }
    }
    NeoPixel.show(); 
    delay(10);

  if(livesNum <= 0) {
    NeoPixel.clear();
    NeoPixel.show(); 
    delay(500);
    livesNum = 10;
  }
}

void loop(){  

  //Serial.println(digitalRead(2)); // active LOW
  NeoPixel.clear();
  if(livesNum == numPixels) {
     for(int x = 0; x < livesNum; x++) { // neopixel
        if (player ==  1) {
        NeoPixel.setPixelColor(x, NeoPixel.Color(242, 133, 0)); // orange
      } else if (player == 2) {
        NeoPixel.setPixelColor(x, NeoPixel.Color(0, 0, 255)); // blue
      }
    }
    NeoPixel.show(); 
  }
  
  // Receiver
  if(millis()>=pulsebuffer+pulsedelay)
  {
    pulsebuffer = millis();
    Serial.println(pulsebuffer);
    if(receiverstate!=digitalRead(RECV_PIN))
    {
      Serial.println(" DataFlipped");
      pulsecheck++;
      if(pulsecheck==4)
      {
        hitbywhichplayer = 1; // internal comms
        playerShot();
        health();
        receivetime=millis();
        Serial.println("PulseConfirmed");
      }
    }
    else
    {
      pulsecheck = 0;
    }
    receiverstate = digitalRead(RECV_PIN);    

    if(hitbywhichplayer==1)
    {
      if(millis()<receivetime+lightuptime)
      {
        Serial.println("HitReceived");
      }
      else
      {
        hitbywhichplayer=0;
      }
    }
  }
}
