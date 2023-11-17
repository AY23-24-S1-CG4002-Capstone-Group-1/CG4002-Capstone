      
// Pulsegun
/* Code to pulse pin 3 with a modulated signal
* Can be used to drive an IR LED to keep a TSOP IR reciever 
* This allows you to use a modulated reciever and a continious beam detector
* By Mike Cook Nov 2011 - Released under the Open Source licence
*/
//https://forum.arduino.cc/t/how-to-create-a-38-khz-pulse-with-arduino-using-timer-or-pwm/100217/7
//https://arduinogetstarted.com/tutorials/arduino-neopixel-led-strip
// https://arduinogetstarted.com/tutorials/arduino-button-debounce
#include <Adafruit_NeoPixel.h>
int lastButtonState = HIGH;
int currentButtonState;

int shotNum = -1;
#define neoPixelPin 2
#define numPixels 6
Adafruit_NeoPixel NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);
#define ledPin A0 


volatile byte pulse = 0;
#define NPN_BASE_PIN 4
#define NPN_SIGNAL_PIN 3 // must be PWM pin!
#define PB_PIN 5
unsigned long triggerTime = 0;
unsigned long serialtime = 0;
unsigned long receivetime = 0;
unsigned long pulsetime = 0;
bool activatepulse = 0;
bool pulsestate = 0;
bool enableLED = 1;
int i = 0;
bool shotFired = 0;

//int pulsedelay = 20; // change to sync player vest & gun Player 1 (gun) = 50, Player 2 (gun) = 20
int pulsedelay = 50;

int shoottime = 400; // emitter change

// Communications
bool target_is_hit = 0; // to send to internal comms
int reload = 0;

ISR(TIMER2_COMPB_vect){  // Interrupt service routine to pulse the modulated pin 3
    pulse++;
  if(pulse >= 8) { // change number for number of modulation cycles in a pulse
    pulse =0;
    TCCR2A ^= _BV(COM2B1); // toggle pin 3 enable, turning the pin on and off
  }
}

void setIrModOutput(){  // sets pin 3 going at the IR modulation rate
  pinMode(NPN_SIGNAL_PIN, OUTPUT);
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); // Just enable output on Pin 3 
  TCCR2B = _BV(WGM22) | _BV(CS22);
  OCR2A = 51; // defines the frequency 51 = 38.4 KHz, 54 = 36.2 KHz, 58 = 34 KHz, 62 = 32 KHz
  OCR2B = 26;  // deines the duty cycle - Half the OCR2A value for 50%
  TCCR2B = TCCR2B & 0b00111000 | 0x2; // select a prescale value of 8:1 of the system clock
}

void LEDControl() {
   shotNum += 1;
    for(int x = 0; x <= shotNum; x++) { // neopixel
      NeoPixel.setPixelColor(x, NeoPixel.Color(24, 0, 0));
    }
    NeoPixel.show(); 
    digitalWrite(ledPin, HIGH); // led
    delay(100);
    digitalWrite(ledPin, LOW);

    if(shotNum == 5 || reload == 1) {
      NeoPixel.clear();
      delay(200);
      NeoPixel.show(); 
      shotNum = -1;
      reload = 0;
    }
}

void setup(){
  Serial.begin(9600);
  NeoPixel.begin();
  pinMode(PB_PIN, INPUT_PULLUP);
  pinMode(NPN_BASE_PIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  setIrModOutput();
  TIMSK2 = _BV(OCIE2B); // Output Compare Match B Interrupt Enable
}

void loop(){  

  if(shotNum < 0) {
    NeoPixel.clear();
    NeoPixel.show();
  }
    currentButtonState = digitalRead(PB_PIN);
  
  // Gun - Trigger is pressed
    if(lastButtonState == LOW && currentButtonState == HIGH) { // button pressed
    activatepulse = 1;
    triggerTime = millis();
    LEDControl();
    shotFired = 1;
    //struct Data_Packet data_packet;

    //data_packet = computeDataPacketResponse(1);
    //Serial.write((uint8_t*) &data_packet, sizeof(data_packet));
    delay(10);
    
  }
  else if(lastButtonState == HIGH && currentButtonState == LOW) {
  }
  // save the last state
  lastButtonState = currentButtonState;
  
  // Gun - Cease shooting after shoottime has passed
  if(millis()>=triggerTime+shoottime)
  {
    activatepulse = 0;
  }

  // Gun - Activate pulse signal with pulsetime delay
  if(activatepulse==1)
  {
    if(millis()>=pulsetime+pulsedelay)
    {
      digitalWrite(NPN_BASE_PIN, pulsestate);
      pulsestate = !pulsestate;
      pulsetime = millis();
    }
  }

  if(activatepulse==0)
  {
    digitalWrite(NPN_BASE_PIN, LOW);
    enableLED=1;
  }
  
}
