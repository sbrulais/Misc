/* 
 *  Projet Marco Pilot: asservissement du cap et de l'altitude
 *  
 *  Pour programmer la carte choisir:
 *    - la cible Ardumega 2560
 *    - la vitesse du port serie 250000 bauds
*/

/*Importation des librairies*/
#include "MS5611.h"
#include <SPI.h>
#include "MeanFilterLib.h"
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <PID_v1.h>


/*Configuration du barometre pour l'altitude*/
#define ADDR 40        /*Adresse*/             
MS5611 MS5611(ADDR); /*Instanciation du capteur*/

/*Configuration du magnetometre pour le cap*/
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
sensors_event_t event; 

/*Filtre a moyenne glissante pour les mesures de pression et magnetique*/
MeanFilter<uint32_t> pressureFilter(20);
MeanFilter<uint32_t> magFilter(20);

/*Valeur du bouton poussoir pour enregistrer le cap*/
int bpValue ;
/*Pin du bouton poussoir*/
int bpPin = A4;

/*Valeur de l'interrupteur auto/manuel*/
int  autoValue ;
/*Pin de l'interrupteur*/
int autoPin=A6;

/*Valeur du potentiometre cap*/
float  capValue ;
/*Pin du potentiometre cap*/
int capPin= PF0;

/*Valeur du potentiometre altitude*/
float  altiValue ;
/*Pin du potentiometre altitude*/
int altiPin = PF2;

/*Valeur du cap en radiant*/
double cap_rad=0.0;
/*Valeur de reference du cap*/
double cap_reference=0.0;
/*Valeur de sortie du PID du cap*/
double cap_output=0;
/*Valeur du servo cap*/
double cap_servo=20;

/*Instance de servo altimetre*/
Servo servoAlti;
/*Butees du servo d'altitude*/
#define MIN_SERVO_ALTI 20
#define MAX_SERVO_ALTI 160
/*Instance de servo cap*/
Servo servoCap;
/*Butees du servo de cap*/
#define MIN_SERVO_CAP 20
#define MAX_SERVO_CAP 160



/* PID du cap*/
PID capPID(&cap_rad, &cap_output, &cap_reference,2.0,1.0,0.0, DIRECT);

void setup() {

  /*Initialisation du port serie*/
  Serial.begin(250000);

  /*Initialisation des boutons*/
  pinMode(bpPin, INPUT);
  pinMode(autoPin, INPUT);

  /*Configuration des pins des servos*/
  servoCap.attach(12); 
  servoAlti.attach(11);

  /*Initialisation du PID*/
  capPID.SetMode(AUTOMATIC);
  /*Butees du PID*/
  capPID.SetOutputLimits(-10.0,10.0);

  /* Initialisation du capteur de pression */
  SPI.begin();
  MS5611.init();
  /*Premiere lecture de pression*/
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

  /* Initialisation de capteur magnetique */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    while(1);
  }else
  {
    /*Premiere mesure magnetique*/
    mag.getEvent(&event);
    cap_rad = atan2(event.magnetic.y, event.magnetic.x);
  }
 
}

/*Fonction de lecture du cap*/
void lectureCap(void)
{
  /*Valeur brute du cap*/
  double cap_raw=0.0;
  /*Lecture de la valeur brute du magnetometre*/
  mag.getEvent(&event);

  /*Calcul du vecteur magnetique entre les axes X et Y*/
  cap_raw = atan2(event.magnetic.y, event.magnetic.x);
  /*Gestion des dépassement autour des valeurs -pi et pi*/
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

  // Conversion du resultat en degree*/
  float headingDegrees = cap_rad * 180/M_PI; 

  //Affichage CAP
  //Serial.print("Cap degre:"); Serial.println(headingDegrees);
  //Serial.println();
  //Serial.print("Cap rad:"); Serial.println(heading);
  //Serial.println();
}

/*Fonction de lecture de l'altitude*/
void lectureAlti(void)
{
  /*Lecture du capteur de pression*/
  int result = MS5611.read(12);// mode 8 - 12 bit oversampling;
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

/*Boucle principale du programme*/
void loop()
{
  /*Lecture des boutons et potentiomètres*/
  bpValue = digitalRead(bpPin);
  autoValue = digitalRead(autoPin);
  capValue = analogRead(capPin);
  altiValue = analogRead(altiPin);

  /*Mode manuel*/
  if(autoValue==0)
  {
      capValue = map(capValue, 0, 1023, MIN_SERVO_CAP,MAX_SERVO_CAP);     // scale it to use it with the servo (value between 0 and 180)
      servoCap.write(capValue);                  // sets the servo position according to the scaled value

      //Serial.print("Cap value: ");
      //Serial.println(capValue);
      altiValue = map(altiValue, 0, 1023, MIN_SERVO_ALTI, MAX_SERVO_ALTI);     // scale it to use it with the servo (value between 0 and 180)
      servoAlti.write(altiValue);                  // sets the servo position according to the scaled value
  }else /*Mode automatique*/
  {
    lectureCap();
    lectureAlti();  
    /*Lors d'un appui sur le bouton, on remet la consigne du PID au cap actuel*/
    if(bpValue==1)
    {
      cap_reference=cap_rad;
    }
    /*execution du PID*/
    capPID.Compute();
    /*Mise a l'echelle de la sortie du PID pour contraindre l'excursion du servo*/
    cap_servo = map_double(cap_output,-10.0,10.0,MIN_SERVO_CAP,MAX_SERVO_CAP);     
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
