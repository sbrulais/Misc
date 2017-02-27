#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>


#define TFT_CS     10
#define TFT_RST    9  
#define TFT_DC     8

#define TFT_SCLK 13   
#define TFT_MOSI 11 

#define TFT_LED 4

#define SWITCH_PIN 6

#define RELAY_PIN 2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup(void) {
  tft.initR(INITR_BLACKTAB);  // Need to do this in every sketch
  tft.fillScreen(ST7735_BLACK); 
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  
  pinMode(SWITCH_PIN,INPUT_PULLUP);
  
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.setCursor(10,20);
  tft.println("19.3 C");
  tft.setTextColor(ST7735_RED); 
  tft.setTextSize(5);
  tft.setCursor(50,80);
  tft.println("*"); 
  
  delay(1000);
  
}

void loop() {
  if(digitalRead(SWITCH_PIN))
  {
    tft.setTextColor(ST7735_RED); 
    digitalWrite(RELAY_PIN, HIGH);
  }else
  {
    tft.setTextColor(ST7735_BLUE); 
    digitalWrite(RELAY_PIN, LOW);
  }
  tft.setCursor(50,80);
  tft.println("*"); 
  delay(200);  
}
