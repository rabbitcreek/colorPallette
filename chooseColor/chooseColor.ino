#include <Wire.h>
#include "Adafruit_TCS34725.h"
//#include <Adafruit_NeoPixel.h>
    #include <FastLED.h>
    #define NUM_LEDS 40
    #define DATA_PIN 33

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, 33, NEO_GRB + NEO_KHZ800);
CRGB leds[NUM_LEDS];
float r, g, b;
// our RGB -> eye-recognized gamma color
byte gammatable[256];
 uint16_t clear, red, green, blue;
bool checker = 0;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
  Serial.println("Color View Test!");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  //strip.begin();
  //
  //strip.show(); // Initialize all pixels to 'off'
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }
  
  for (int i=0; i<3; i++){ //this sequence flashes the first pixel three times as a countdown to the color reading.
    
    leds[0] = CRGB::Red; 
      FastLED.show();
      delay(1000);
      
      // Set the first led back to black for 1 second
      leds[0] = CRGB::Black;
      FastLED.show();
      delay(1000);
  }
  
 

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED
  
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  //sum += clear; // clear contains RGB already so no need to re-add it
  
  
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  
  Serial.print("\t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
  Serial.println();

  Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
  


// Fill the dots one after the other with a color

  for(uint16_t i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
    leds[i].maximizeBrightness();
    //FastLED.setBrightness(75);
  }
  FastLED.show();
}

void loop() {
  
  
    if(touchRead(2) < 42 && checker == 0) {
      Serial.println("ON");
      Serial.println(touchRead(2));
      checker = 1;
      recheck();
      delay(1000);
    }
      else if(touchRead(2) < 42 && checker == 1) {
        FastLED.clear();
        FastLED.show();
        tcs.setInterrupt(true);
        Serial.println(touchRead(2));
        checker = 0;
        delay(1000);
      }
    Serial.println(touchRead(2));
     
     FastLED.show();
}
void recheck(){
  FastLED.clear();
  FastLED.show();
  tcs.setInterrupt(false);      // turn on LED

  for (int i=0; i<3; i++){ //this sequence flashes the first pixel three times as a countdown to the color reading.
   for (int j = NUM_LEDS; j > 0; j--) {
    leds[j] = CRGB::Blue;
    FastLED.show();
    delay(10);
    leds[j] = CRGB::Black;
    delay(10);
     
   }
   
  }
  fill_solid( leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
   //delay(1000);
  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED
  
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  //sum += clear; // clear contains RGB already so no need to re-add it
  
  
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  
  Serial.print("\t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
  Serial.println();

  Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
  


// Fill the dots one after the other with a color

  for(uint16_t i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
    leds[i].maximizeBrightness();
  }
  FastLED.show();
}
