void setup() {
  // put your setup code here, to run once:
  digitalWrite(12, HIGH); 
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(4, LOW); 
  digitalWrite(5, LOW); 
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW); 
   

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(12, HIGH); /*Timer in progress*/
  /*Relay ON*/
  delay(2000);
  digitalWrite(4, LOW); 
  digitalWrite(5, HIGH); 
  delay(200);
  digitalWrite(0, HIGH); /*LED ON*/
  digitalWrite(4, LOW); 
  digitalWrite(5, LOW); 
  delay(2000);
  /*Relay OFF*/
  digitalWrite(4, HIGH); 
  digitalWrite(5, LOW); 
  delay(200);
  digitalWrite(0, LOW); /*LED OFF*/
  digitalWrite(4, LOW); 
  digitalWrite(5, LOW); 
  digitalWrite(12, LOW); /*Timer Done*/
  delay(2000);
}
