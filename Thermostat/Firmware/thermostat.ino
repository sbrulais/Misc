#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>


#define TFT_CS     10
#define TFT_RST    9  
#define TFT_DC     8

#define TFT_SCLK 13   
#define TFT_MOSI 11   

int relay = 2;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup(void) {
  tft.initR(INITR_BLACKTAB);  // You will need to do this in every sketch
  tft.fillScreen(ST7735_BLACK); 
  pinMode(relay, OUTPUT);

  //tft print function!
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
  tft.setTextColor(ST7735_RED); 
  tft.setCursor(50,80);
  tft.println("*"); 
  digitalWrite(relay, HIGH);
  delay(5000);  
  tft.setTextColor(ST7735_BLUE); 
  tft.setCursor(50,80);
  tft.println("*"); 
  digitalWrite(relay, LOW);
  delay(5000);  
}
