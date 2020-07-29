#include "MS5611.h"
#include <SPI.h>
#include "MeanFilterLib.h"
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <PID_v1.h>

/*Use Ardumega 2560 board*/

//Pressure sensor configuration
#define ADDR 40        // CS (CSB) pin when using SPI (Arduino numbering)              
MS5611 MS5611(ADDR);

//Magnetic sensor configuration
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
sensors_event_t event; 

//Sensor Filters
MeanFilter<uint32_t> pressureFilter(20);
MeanFilter<uint32_t> magFilter(20);

/*HMI pins*/
int sensorPin = A0;    // select the input pin for the potentiometer
int bpValue ;
int bpPin = A4;
int  autoValue ;
int autoPin=A6;
float  capValue ;
int capPin= PF0;
float  altiValue ;
int altiPin = PF2;

double mag_ref_x=0.0;
double mag_ref_y=0.0;
double cap_rad=0.0;
double cap_reference=0.0;
double cap_output=0;
double cap_servo=20;

int mode=1; /*0 manuel  1 regulé*/

Servo servoAlti;
Servo servoCap;

PID capPID(&cap_rad, &cap_output, &cap_reference,2.0,1.0,0.0, DIRECT);

void setup() {
  Serial.begin(250000);

  pinMode(bpPin, INPUT);
  pinMode(autoPin, INPUT);

  servoCap.attach(12); 
  servoAlti.attach(11);

  /*Init PID*/
  capPID.SetMode(AUTOMATIC);
  capPID.SetOutputLimits(-10.0,10.0);

  /* Initialise the pressure sensor */
  SPI.begin();
  MS5611.init();
  //First pressure read to init pressureFilter
  int result = MS5611.read(12);// 8 - 12 bit oversampling;
  if (result != 0)
  {
      Serial.print("Error in read: ");
      Serial.println(result);
  }
  else
  {
    pressureFilter.AddValue(MS5611.getPressure());
  }

  /* Initialise the magnetic sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    //Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }else
  {
    //Serial.println("HMC5883 detected ...");
    //First magnetic read
    mag.getEvent(&event);
    cap_rad = atan2(event.magnetic.y, event.magnetic.x);
  }
  


  
}

void lectureCap(void)
{
  double cap_raw=0.0;
  /*Read magnetic sensor*/
  mag.getEvent(&event);
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
//  Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
//  Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
//  Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
 
  // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  cap_raw = atan2(event.magnetic.y, event.magnetic.x);
  if( (cap_rad - cap_raw)>PI)
  {
    cap_rad = cap_raw + 2*PI;
  }else if ( (cap_rad - cap_raw <(-PI)))
  {
    cap_rad = cap_raw - 2*PI;
  }else
  {
    cap_rad = cap_raw;
  }

  // Check for wrap due to addition of declination.
  //if(cap_rad > 2*PI)
    //cap_rad -= 2*PI;
  
  //cap_rad = (mag_ref_x - event.magnetic.x);//+(mag_ref_y - event.magnetic.y);
   
  // Correct for when signs are reversed.
  //if(cap_rad < 0)
    //cap_rad += 2*PI;
    
  // Check for wrap due to addition of declination.
  //if(cap_rad > 2*PI)
    //cap_rad -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = cap_rad * 180/M_PI; 

  //Affichage CAP
  //Serial.print("Cap degre:"); Serial.println(headingDegrees);
  //Serial.println();
  //Serial.print("Cap rad:"); Serial.println(heading);
  //Serial.println();
}

void lectureAlti(void)
{
    /*Read pressure sensor*/
  int result = MS5611.read(12);// 8 - 12 bit oversampling;
  if (result != 0)
  {
      Serial.print("Error in read: ");
      Serial.println(result);
  }
  else
  {
     pressureFilter.AddValue(MS5611.getPressure());
     //Affichage pression
//      Serial.print("\t\tPf:\t");
//      Serial.print(pressureFilter.GetFiltered());    
//      Serial.print("\t\tPr:\t");
//      Serial.print(MS5611.getPressure());   
//      Serial.println();
  }
}

void loop()
{

  /*Lecture des boutons et potentiomètres*/
  bpValue = digitalRead(bpPin);
  autoValue = digitalRead(autoPin);
  capValue = analogRead(capPin);
  altiValue = analogRead(altiPin);

  /*Mode manuel*/
  if(mode==0)
  {
      capValue = map(capValue, 0, 1023, 20, 160);     // scale it to use it with the servo (value between 0 and 180)
      servoCap.write(capValue);                  // sets the servo position according to the scaled value

      //Serial.print("Cap value: ");
      //Serial.println(capValue);
      altiValue = map(altiValue, 0, 1023, 20, 160);     // scale it to use it with the servo (value between 0 and 180)
      servoAlti.write(altiValue);                  // sets the servo position according to the scaled value
  }else
  {
    lectureCap();
    lectureAlti();  
    capPID.Compute();
    cap_servo = map_double(cap_output,-10.0,10.0,20.0,160.0);     // scale it to use it with the servo [20;160]
    servoCap.write(cap_servo);                  // sets the servo position according to the scaled value
    Serial.print("in:"); Serial.print(cap_rad);
    Serial.print(" out:"); Serial.print(cap_output);
    Serial.println();
  }
  delay(10);
}


double map_double(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
