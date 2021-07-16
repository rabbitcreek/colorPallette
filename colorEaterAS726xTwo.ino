/***************************************************************************
  This is a library for the Adafruit AS7262 6-Channel Visible Light Sensor

  This sketch reads the sensor

  Designed specifically to work with the Adafruit AS7262 breakout
  ----> http://www.adafruit.com/products/3779
  
  These sensors use I2C to communicate. The device's I2C address is 0x49
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  
  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include "Adafruit_AS726x.h"
#include <FastLED.h>
#include <TFT_eSPI.h>
#include <SPI.h>
int j = 0;
int pos = 0;
int fixedColors[] = { TFT_VIOLET, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_ORANGE, TFT_RED};
TFT_eSPI tft = TFT_eSPI();
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
 int maxValueLight = 0;
 int totalValue = 0;
 int finalLight = 0;
 int change = 1;
 bool ding = 0;
//create the object
Adafruit_AS726x ams;
int constantLight = 0;
//buffer to hold raw values
//uint16_t sensorValues[AS726x_NUM_CHANNELS];
float lightValue[6];
float fixedValue[] = { 200, 160, 96, 64, 32, 1};
String colors[] = { "violet","Blue",  "green","yellow", " orange", "red"};
//buffer to hold calibrated values (not used by default in this example)
float calibratedValues[AS726x_NUM_CHANNELS];
uint16_t sensorValues[AS726x_NUM_CHANNELS];
int lightOrder[6];
void setup() {
  Serial.begin(9600);
  while(!Serial);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  pinMode(0,INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  // initialize digital pin LED_BUILTIN as an output.
 // pinMode(LED_BUILTIN, OUTPUT);
pinMode(0, INPUT_PULLUP);
pinMode(35, INPUT_PULLUP);
  //begin and make sure we can talk to the sensor
  if(!ams.begin()){
    Serial.println("could not connect to sensor! Please check your wiring.");
    while(1);
  }
  
}

void loop() {
  change = 1;
  //ding = 1;
 
   while(!digitalRead(0) || !digitalRead(35)){
    ding = 1;
  tft.fillRect(0,j-1,135,5,TFT_BLACK); 
  tft.fillRect(0,j,135,5,TFT_WHITE);
  delay(25);
  tft.fillRect(0,j,135,5,TFT_BLACK);
 
   for(int i=0; i < 6; i++){
    int z = lightValue[maxValueLight];
    int p = map(lightValue[i], 0,z, 0,16); 
 linearMeter(p, 0, (10 + i * 40), 5, 30, 3, 15, fixedColors[i]);
 
  }
  if(!digitalRead(35)) change = -1;
  j = j + change;
  if(j > 240 ) j=1;
  if(j < 1) j = 240;
  pos = map(j, 0,240, 255, 1);
  Serial.print("pos");
  Serial.println(pos);
  Serial.print("j");
  Serial.println(j);
  tft.fillRect(0,j-1,135,5,TFT_WHITE);
  for(int q = 0 ; q < NUM_LEDS; q++){
     leds[q].setHSV(pos, 175,255); 
    if(q%3 == 0){
      leds[q]. setHSV(224, 0, 255);
    }
  }
  FastLED.show();
   }
 if(touchRead(2) < 10) {
    Serial.print("touchread");
    Serial.println(touchRead(2));
    ding = 0;
    tft.fillScreen(TFT_BLACK);
  }
  if(!ding){
    FastLED.clear();
     for (int i=30; i>1; i--){ //this sequence flashes the first pixel three times as a countdown to the color reading.
    
    leds[i] = CRGB::Red; 
      FastLED.show();
      delay(30);
      
      // Set the first led back to black for 1 second
      leds[i] = CRGB::Black;
      FastLED.show();
      delay(30);
  }
totalValue = 0;
  //read the device temperature
  uint8_t temp = ams.readTemperature();
  
  ams.drvOn(); //uncomment this if you want to use the driver LED for readings
  ams.startMeasurement(); //begin a measurement
  
  //wait till data is available
  bool rdy = false;
  while(!rdy){
    delay(5);
    rdy = ams.dataReady();
  }
   ams.drvOff(); //uncomment this if you want to use the driver LED for readings

  //read the values!
 
  ams.readRawValues(sensorValues);
  //ams.readCalibratedValues(calibratedValues);

  lightValue[0] = sensorValues[AS726x_VIOLET];

  lightValue[1] = sensorValues[AS726x_BLUE];
  lightValue[2] = sensorValues[AS726x_GREEN];
  lightValue[3] = sensorValues[AS726x_YELLOW];
  lightValue[4] = sensorValues[AS726x_ORANGE];
  lightValue[5] = sensorValues[AS726x_RED];
  for(int i=0; i<6; i++){
   
    totalValue = totalValue + lightValue[i];
  }
 
    maxValue();
    constantLight = fixedValue[maxValueLight];
    
    for(int j = 0; j < 6; j++){
      Serial.print( colors[j]); Serial.println(lightValue[j]);
    }
  

 
  Serial.print("maxValueLight"); Serial.println(maxValueLight);
  Serial.print("finalLight"); Serial.println(finalLight);
  Serial.print("constantLight"); Serial.println(constantLight);
  Serial.print("totalValue"); Serial.println(totalValue);
  
   for(int led = 0; led < NUM_LEDS; led++){
    leds[led].setHSV(finalLight, 255,255); 
    if(led%3 == 0){
      leds[led]. setHSV(224, 0, 255);
    }
  }
  for(int i=0; i < 6; i++){
    int z = lightValue[maxValueLight];
    int p = map(lightValue[i], 0,z, 0,16); 
 linearMeter(p, 0, (10 + i * 40), 5, 30, 3, 15, fixedColors[i]);
 
  }
   int ypos = map(finalLight,0,250,220,3);
  tft.fillRect(0,ypos,135,5,TFT_RED);
  FastLED.show();
  delay(2000);
   //
   ding = 1;
  }
}


void maxValue(){
  float  maxValue = lightValue[0];
  maxValueLight = 0;
  for (int i = 1; i<6; i++){
   
    if( lightValue[i] >= maxValue){
      maxValue = lightValue[i];
      maxValueLight = i;
    }
  }
  finalLight = fixedValue[maxValueLight];
  if(maxValueLight  < 5 && maxValueLight > 0){
   finalLight = finalLight + ((lightValue[maxValueLight - 1] / totalValue) * abs((fixedValue[maxValueLight - 1] - constantLight)));
  
    finalLight =  finalLight - ((lightValue[maxValueLight + 1] / totalValue) * abs((fixedValue[maxValueLight + 1] - constantLight)));
  } 
  else if(maxValueLight == 0){
     finalLight =  finalLight - ((lightValue[maxValueLight + 1] / totalValue) * abs((fixedValue[maxValueLight + 1] - constantLight)));
  }
  else{
    finalLight = finalLight + ((lightValue[maxValueLight - 1] / totalValue) * abs((fixedValue[maxValueLight - 1] - constantLight)));
  }
}

// #########################################################################
//  Draw the linear meter
// #########################################################################
// val =  reading to show (range is 0 to n)
// x, y = position of top left corner
// w, h = width and height of a single bar
// g    = pixel gap to next bar (can be 0)
// n    = number of segments
// s    = colour scheme
void linearMeter(int val, int x, int y, int w, int h, int g, int n, int s)
{
  // Variable to save "value" text colour from scheme and set default
  int colour = TFT_BLUE;
   
  // Draw n colour blocks
  for (int b = 1; b <= n; b++) {
    if (val > 0 && b <= val) { // Fill in coloured blocks
      colour = TFT_BLUE;
      tft.fillRect(x + b*(w+g), y, w, h, s);
    }
    else // Fill in blank segments
    {
      tft.fillRect(x + b*(w+g), y, w, h, TFT_BLACK);
    }
    //tft.fillRect(135,10,5,35,TFT_WHITE);
  }
}
void changeLight(){
  tft.fillRect(0,j-1,135,5,TFT_BLACK); 
  tft.fillRect(0,j,135,5,TFT_WHITE);
  delay(25);
  tft.fillRect(0,j,135,5,TFT_BLACK);
 
   for(int i=0; i < 6; i++){
    int z = lightValue[maxValueLight];
    int p = map(lightValue[i], 0,z, 0,16); 
 linearMeter(p, 0, (10 + i * 35), 5, 30, 3, 15, fixedColors[i]);
 
  }
  if(!digitalRead(35)) change = -1;
  j = j + change;
  if(j > 230 ) j=0;
  if(j < 1) j = 230;

tft.fillRect(0,j-1,135,5,TFT_WHITE);
}
