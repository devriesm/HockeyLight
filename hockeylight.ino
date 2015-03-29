/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/*************************************************** 
  This sketch is for an RF controlled hockey light.
  
  Parts Used:
  http://www.adafruit.com/products/50
  http://www.adafruit.com/product/1788
  http://www.adafruit.com/product/1096
  http://www.adafruit.com/products/1391
  http://www.adafruit.com/products/268
  http://www.adafruit.com/product/85
  http://www.ebay.com/.../2-Police-Light.../200935299700

  Written by Mark DeVries.
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


////



// Analog threshold used to determine when FOB buttons are pressed.
const byte BUTTON_THRESHOLD = 50;


void setup() {
  Serial.begin(9600);
  Serial.println("Start Up!");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
    
    
  // pin 5 is for the powerswitch tail.
  // pin 5 is not used by the mp3 shield
  // therefore, we can use it to control the powerswitch tail
  pinMode(5, OUTPUT); 
}

void loop() {
  
  //get analog pin 3 reading - button a
  byte a = analogRead(3);
  
  //check if button a was pressed
  if ( a > BUTTON_THRESHOLD ) {
    
    //turn the light on
    digitalWrite(5, HIGH);
    
    // Start playing a file, then we can do stuff while waiting for it to finish
    if (! musicPlayer.startPlayingFile("chihorn.mp3")) {
      Serial.println("Could not open file chihorn.mp3");
      while (1);
    }
    Serial.println(F("Started playing"));
    
    byte b;
  
    while (musicPlayer.playingMusic) {
      // music is now playing
      
      // get analog pin 2 - button b
      b = analogRead(2);
      
      // check if button b was pressed
      if ( b > BUTTON_THRESHOLD ) {
        // stop playing the audio
        musicPlayer.stopPlaying();
      } else {
        delay(100);
      }
    }
    // turn off the light because audio stopped playing
    digitalWrite(5, LOW);
    Serial.println("Done playing music");
  }
  
}
