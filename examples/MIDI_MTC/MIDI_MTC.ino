/*

      Based on code from Grg38
      http://forum.arduino.cc/index.php?topic=57636.0

      Easy-to-understand explanation of the MIDI Time Code:
      http://en.wikipedia.org/wiki/MIDI_timecode

      this sketch uses the TimerOne library:
      https://code.google.com/p/arduino-timerone/

*/


/* configuration */

// uncomment to use with Arduino and Teensy USB Mode: Serial:
//#define MIDI_OVER_SERIAL

// the button pin
#define BUTTON_PIN 23

// the frames per second for the MTC
#define FPS 25         // possible FPS: 24, 25, 30
                       // FPS 29,97 was not implemented            
                          


/* some includes and variables */

#include <TimerOne.h>

byte mtcFPSBits = 1<<1; // use 25 FPS as standard setting
float mtcInterval = (1000000.0 / (FPS * 4)); // timer for midi time code messages, i.e. FPS times per second

volatile byte mtcValue; // value of that message
volatile byte mtcType = 0; // type of the MIDI Time Code Message

volatile byte mtcF; // counter for the MIDI Time Code Quarter Frame
volatile byte songH, songM, songS; // variables to keep track of the time the current song is playing

boolean playing = false;
int buttonState = 0;



/* the routine to generate the MTC */

void sendMTC() {
  switch(mtcType){
    case 0: 
      mtcValue=mtcF&0xF;         
      break;
    case 1: 
      mtcValue=(mtcF&0xF0)/16;   
      break;
    case 2:
      mtcValue=songS&0xF;         
      break;
    case 3: 
      mtcValue=(songS&0xF0)/16;   
      break;
    case 4:
      mtcValue=songM&0xF;         
      break;
    case 5:
      mtcValue=(songM&0xF0)/16;   
      break;
    case 6: 
      mtcValue=songH&0xF;         
      break;
    case 7: 
      mtcValue=((songH&0xF0)/16)+mtcFPSBits;  
      break;
  }

#if defined (MIDI_OVER_SERIAL)
  Serial.write(0xF1);
  Serial.write((mtcType<<4)+mtcValue);
#else
  usbMIDI.sendTimeCodeQuarterFrame(mtcType, mtcValue);
#endif

  if (++mtcType>7){
    mtcType=0;
    mtcF+=2;
    if (mtcF>FPS-1){
      songS++;
      mtcF-=FPS;
    }
    if (songS>59){
      songM++;
      songS-=60;
    }
    if (songM>59){
      songH++;
      songM-=60;
    }
  }
}



/* Setup */

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);   
  switch (FPS){             
    case 24:
      mtcFPSBits = 0<<1;
      break;
    case 25:
      mtcFPSBits = 1<<1;
      break;
    case 30:
      mtcFPSBits = 2<<1;
      break;
  }
  Timer1.initialize(mtcInterval);
  Timer1.disablePwm(BUTTON_PIN);  
}


/* Main loop */

void loop() {
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {  // my button is low when pressed
    playing = !playing;
    if (playing){ 
      Timer1.attachInterrupt(sendMTC);
    }else{  
      Timer1.detachInterrupt();
      songS = songM = songH = mtcF = 0;
    } 
  }
}


