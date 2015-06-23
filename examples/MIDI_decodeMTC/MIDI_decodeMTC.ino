/*
      Generate MIDI Time Code messages by Karg (Timm Schlegelmilch)
      http://karg-music.blogspot.com
      
      Example sketch for my usb_midi library extension for the Teensy 
      microcontroller. This sketch shows how to receive and decode 
      MIDI Time Code (MTC) messages through callback functions.
      Make sure to compile with
      Board: "Teensy XXX"
      USB Type: "MIDI"
      
      licensed under CC BY-SA 4.0
      http://creativecommons.org/licenses/by-sa/4.0/


      Easy-to-understand explanation of the MIDI Time Code:
      http://en.wikipedia.org/wiki/MIDI_timecode

      this sketch uses the TimerOne library:
      https://code.google.com/p/arduino-timerone/

*/


byte mtcType = 255;
byte mtcFPS = 255;
byte mtcFrame = 255;
byte songS = 255;
byte songM = 255;
byte songH = 255;

 
/* the callback routine to decode the MTC */

void OnTimeCodeQuarterFrame(uint16_t data){
  static byte lfbits;      // low bits of the MTC frame number
  static byte lsbits;      // low bits of seconds
  static byte lmbits;      // low bits of minutes
  static byte lhbits;      // low bits of hours
  
  switch (data & 0xF0) {
    case 0x00:
      lfbits = data & 0x0F;
      mtcType = 0;
      break;
    case 0x10:
      mtcFrame = lfbits + ((data & 0x01) << 4);
      mtcType = 1;
      break;
    case 0x20:
      lsbits = data & 0x0F;
      mtcType = 2;
      break;
    case 0x30:
      songS = lsbits + ((data & 0x03) << 4);
      mtcType = 3;
      break;
    case 0x40:
      lmbits = data & 0x0F;
      mtcType = 4;
      break;
    case 0x50:
      songM = lmbits + ((data & 0x03) << 4);
      mtcType = 5;
      break;    
    case 0x60:
      lhbits = data & 0x0F;
      mtcType = 6;
      break;
    case 0x70:
      mtcType = 7;
      mtcFPS = 24;
      songH = lhbits + ((data & 0x01) << 4);
      break;
    case 0x72:
      mtcType = 7;
      mtcFPS = 25;
      songH = data & 0x1;
      break;
    case 0x76:
      mtcType = 7;
      mtcFPS = 30;
      songH = data & 0x1;
      break;
  }
}


/* Setup */

void setup() {
  Serial.begin(115200);
  usbMIDI.setHandleTimeCodeQuarterFrame(OnTimeCodeQuarterFrame);
}


/* Main loop */

void loop() {
  while(usbMIDI.read()){}

  Serial.print("FPS: ");
  Serial.print(mtcFPS);
  Serial.print("    Frame: ");
  Serial.print(mtcFrame);
  Serial.print(".");
  Serial.print(mtcType);
  Serial.print("    Time: ");
  Serial.print(songH);
  Serial.print(":");
  Serial.print(songM);
  Serial.print(":");
  Serial.println(songS);
  delay(5);
}
