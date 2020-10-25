



/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <stdint.h>



#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16 
const static unsigned char logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000};

 /* RELATED TO HX117*/
/*float const a1=-3.6652E-05;// To get the simulation temperature
float const b1=614.76;*/

float const a1=-3.6385E-05;// To get the simulation temperature
float const b1=610.031;

//float const a2=1.0114E-04;  // To get the temperature from PT1000
//float const b2=-260.84;     //changed at 20200331
float const a2=1.0218E-04;  // To get the temperature from PT1000
float const b2=-263.11;     //changed at 20200415

float const a3=3.6473E-5;    // To get the TUBE Temperature 
float const b3=0.3075;

/*
float const a4=1.1584E-4;   // to get the sensor resistance  
float const b4=-14.735;     // 2020-03-30 after calibration: y= 2.6042x-14.735
                            // new parameter SHOULD BE: a4=2.6042 *4.4483E-5,,, b4=-14.735*/
//*****************************************/
float const a4=1.1457E-4;   // to get the sensor resistance  
//float const b4=-15.059;  
float const b4=-12.69; 

int AD_SCK1 = 13;  // clk is connected to PIN9 as output
int AD_DATA1 = 12;    // data in is connected to digital pin 10 as input
int AD_SCK2 = 8;  // clk is connected to PIN9 as output
int AD_DATA2 = 9;    // data in is connected to digital pin 10 as input
int DI=10;  // test mode.. 1-- measure TUBE, 0- disp simulation
int DO=11;  // current source control- 0 just input voltage, 1- voltage + C*r

unsigned long Count1;
unsigned long Count;


void setup()   {                
 
 Serial.begin(9600);

 /*Ini. HX711*/
  pinMode(AD_SCK1 , OUTPUT);  // sets the digital pin 13 as output
  pinMode(AD_DATA1, INPUT);    // sets the digital pin 7 as input
  pinMode(AD_SCK2, OUTPUT);  // sets the digital pin 13 as output
  pinMode(AD_DATA2, INPUT);    // sets the digital pin 7 as input
  
  pinMode(DI, INPUT);  
  pinMode(DO, INPUT);
  digitalWrite(DO,LOW);
 
  digitalWrite(AD_SCK1, 1);
  delay (10);              // reset the chip HX711
  digitalWrite(AD_SCK1, 0);
  
  digitalWrite(AD_SCK2, 1);
  delay (10);              // reset the chip HX711
  digitalWrite(AD_SCK2, 0);
  delay(500);
  
  delay(500);


/* hx711 ini. ends*/

/*ini. oled */
  display.begin(SSD1306_SWITCHCAPVCC,0x3c);  // initialize with the I2C addr 0x3d (for the 128x64)
  display.clearDisplay();   // clears the screen and buffer
// init done
  
  // /*text display format*/
  
    /*Row one*/  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("T:"); display.println(String(100.0,2));

  /*Row Two*/
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,16);
  display.print("CJT:"); display.println(String(22.50,2));
  display.display();    
 
}
 

void loop() {
 if (digitalRead(DI)==HIGH)
 {
digitalWrite(DO, LOW);
delay(100);

OneStep(AD_SCK1, AD_DATA1);

  long y_temperary;
  y_temperary=long(Count1&(0x00ffffff));
  if((y_temperary&0x800000)!=0){y_temperary=-16777216+y_temperary;}
  float CJT=a2*float(y_temperary)+b2;

  OneStep(AD_SCK2, AD_DATA2);
  y_temperary=long(Count&(0x00ffffff));
  if((y_temperary&0x800000)!=0){y_temperary=-16777216+y_temperary;}
  float Sim_T1=float(y_temperary);
  float Sim_T=a3*Sim_T1+b3+CJT;

  digitalWrite(DO, HIGH);
  delay(100);
  OneStep(AD_SCK2, AD_DATA2 );
  y_temperary=long(Count&(0x00ffffff));
  if((y_temperary&0x800000)!=0){y_temperary=-16777216+y_temperary;}
  float Resistance=a4*(float(y_temperary)-Sim_T1)+b4;

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("TUB:"); display.println(String(Sim_T,2));
  display.setCursor(0,16);
  display.print("R:"); display.println(String(Resistance,2));
  display.display();  
    }//measure
  
  else{
  Serial.println("Simulation mode");  
  
OneStep(AD_SCK1, AD_DATA1 );
float CJT=a2*float(Count1)+b2;
float Sim_T=a1*float(Count)+b1+CJT;

Serial.print("CJT=");
Serial.println(Count1,DEC);
Serial.print("Simu=");
Serial.println(Count,DEC);

display.clearDisplay();
  display.setCursor(0,0);
  display.print("T:"); display.println(String(Sim_T,2));
  display.setCursor(0,16);
  display.print("CJT:"); display.println(String(CJT,2));
  display.display();
 
  }//DISP:CJT and simulation 
  delay(300);
  
}



/**************************************/
/**************************************/
;
;
;
;
/***********************************/
/************************************/
void OneStep(int AD_SCK, int AD_DATA ){

unsigned char i; 
digitalWrite(AD_SCK, 0);
Count1=0;
while(digitalRead(AD_DATA)){}; //wait uitill AD_DATA becomes low
for (i=0;i<24;i++){
digitalWrite(AD_SCK,1);
Count1=Count1<<1;
digitalWrite(AD_SCK,0);
if(digitalRead(AD_DATA)) {Count1++;}
}

digitalWrite(AD_SCK,1); // cha, gain 128,for the next turn, not the current one
digitalWrite(AD_SCK,0);

Count=0;
while(digitalRead(AD_DATA)){}; //wait uitill AD_DATA becomes low
for (i=0;i<24;i++){
digitalWrite(AD_SCK,1);
Count=Count<<1;
digitalWrite(AD_SCK,0);
if(digitalRead(AD_DATA)) {Count++;}
}
digitalWrite(AD_SCK,1); // chb, gain 
digitalWrite(AD_SCK,0);

digitalWrite(AD_SCK,1);
digitalWrite(AD_SCK,0);
  }
/*****************************'''''''''/
/************************************/ 
