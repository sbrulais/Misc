/*Home made connected thermostat S.Brulais*/
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "wifi_info.h"

#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

#define RELAY_L1_PIN 5
#define RELAY_L2_PIN 4

#define RELAY_OFF 0
#define RELAY_ON 1

#define SLEEP_PIN 12

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PWD;

const char* host = WIFI_HOST;

void setup() {
  
  
  digitalWrite(SLEEP_PIN, HIGH);
  pinMode(SLEEP_PIN, OUTPUT);
  digitalWrite(SLEEP_PIN, HIGH);
  pinMode(RELAY_L1_PIN, OUTPUT);
  digitalWrite(RELAY_L1_PIN, LOW);
  pinMode(RELAY_L2_PIN, OUTPUT);
  digitalWrite(RELAY_L2_PIN, LOW);
  Serial.begin(115200);
  delay(100);

  dht.begin();
  Serial.println("DHT setup...");


}

void loop() {
  int i;
  float h=0;
  float t=0;
  
  t=0;
  h=0;
  float h_read;
  float t_read;
  for(i=0;i<10;i++)
  {
    // Reading temperature and humidity
    h_read = dht.readHumidity();
    h += h_read;
    // Read temperature as Celsius
    t_read = dht.readTemperature();
    t += t_read;
    delay(100);
  }
  h/=10;
  t/=10;
   // Display data
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C ");


  wifi_connect();

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    /*Goto sleep*/
    digitalWrite(SLEEP_PIN, LOW);
    delay(20000);
  }
  
  // This will send the request to the server
  client.print(String("GET /thermostat/show_schedule.php?reqtmp=") + t + "&reqhum=" + h + " HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" + 
              "Connection: close\r\n\r\n");
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();
  if ( client.connect(WIFI_HOST, httpPort) == 1) {
    //Serial.println("connected");
    client.println("GET /thermostat/get_state_auto.php HTTP/1.1"); //download text
    String clientHost = "Host: ";
    clientHost.concat( WIFI_HOST );
    client.println(clientHost);
    client.println("Connection: close");  //close 1.1 persistent connection  
    client.println(); //end of get request
    delay(100);

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      /*Searching answer starting with 'O'*/
      int indexO = line.lastIndexOf('O');
      /*Getting ON or OFF command*/
      if(indexO>0)
      {
        String substr=line.substring(indexO);
        if(substr.startsWith("ON")==true)
        {
          Serial.println("ON!");
          Relay(RELAY_ON);
        }
        if (substr.startsWith("OFF")==true)
        {
          Serial.println("OFF!");
          Relay(RELAY_OFF);
        }
      }
    }
   client.stop();
   Serial.println();
  }
  /*Goto sleep*/
  digitalWrite(SLEEP_PIN, LOW);
  delay(20000);
}


void Relay(int OnOff)
{
  digitalWrite(RELAY_L1_PIN, LOW);
  digitalWrite(RELAY_L2_PIN, LOW);
  delay(100);
  if (OnOff==RELAY_OFF)
  {
    digitalWrite(RELAY_L1_PIN, HIGH);
  }else if (OnOff==RELAY_ON)
  {
    digitalWrite(RELAY_L2_PIN, HIGH);
  }
  delay(200);
  digitalWrite(RELAY_L1_PIN, LOW);
  digitalWrite(RELAY_L2_PIN, LOW);

}

void software_Reboot()
{
  wdt_enable(WDTO_15MS);

  while(1)
  {

  }
}

void wifi_connect()
{
  int retry =0;
    // Connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  IPAddress ip(192, 168, 0, 70); 
  IPAddress gateway(192, 168, 0, 254); 
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  IPAddress subnet(255, 255, 255, 0); 
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  
  while ( (WiFi.status() != WL_CONNECTED) && (retry < 40) ) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  if (retry>=40)
  {
    /*Goto sleep*/
    digitalWrite(SLEEP_PIN, LOW);
    delay(20000);
    //software_Reboot();
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

