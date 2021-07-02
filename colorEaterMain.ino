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
 while( !digitalRead(0)){
    for(int j = 0; j < 255;j++){
    for(int led = 0; led < NUM_LEDS; led++){
    leds[led].setHSV(j, 255,255); 
    if(led%3 == 0){
      leds[led]. setHSV(224, 0, 255);
    }
  }
delay(20);
  FastLED.show();
    }
  
  
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
    //lightValue[i]= constrain(lightValue[i], 0,20000);
    //lightValue[i] = map(lightValue[i], 0, 20000, 0,10);
    totalValue = totalValue + lightValue[i];
  }
 
    maxValue();
    constantLight = fixedValue[maxValueLight];
    
    for(int j = 0; j < 6; j++){
      Serial.print( colors[j]); Serial.println(lightValue[j]);
    }
  
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" Violet: "); Serial.print(calibratedValues[AS726x_VIOLET]);
  Serial.print(" Blue: "); Serial.print(calibratedValues[AS726x_BLUE]);
  Serial.print(" Green: "); Serial.print(calibratedValues[AS726x_GREEN]);
  Serial.print(" Yellow: "); Serial.print(calibratedValues[AS726x_YELLOW]);
  Serial.print(" Orange: "); Serial.print(calibratedValues[AS726x_ORANGE]);
  Serial.print(" Red: "); Serial.print(calibratedValues[AS726x_RED]);
  Serial.println();
  Serial.println();
 
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
 
  
  FastLED.show();
  delay(1000);
  
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
