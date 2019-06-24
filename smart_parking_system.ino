 #include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
const int trigpin1= D8;  
const int echopin1= D7; 
const int trigpin2= D4;  
const int echopin2= D5; 
const int led1= D0; 
const int led2= D1;

Servo myservo;
int duration1;
int duration2;
int distance1;  
int distance2; 
 
//-------- Customise these values -----------
const char* ssid = "MLRITM-PLACEMENT";
const char* password = "Mlritm@123";

 
#define ORG "7jp8h1"
#define DEVICE_TYPE "parking"
#define DEVICE_ID "1234"
#define TOKEN "12345678"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

void setup() {
  pinMode(trigpin1,OUTPUT);
  pinMode(trigpin2,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);  
  pinMode(echopin1,INPUT);
  pinMode(echopin2,INPUT);  
   myservo.attach(D2);
 Serial.begin(115200);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
}
 
void loop() {
  digitalWrite(trigpin1,HIGH);  
  delay(1000);  
  digitalWrite(trigpin1,LOW);
  duration1=pulseIn(echopin1,HIGH);  
  distance1= duration1*0.034/2;
  Serial.print("distance1 : ");
  Serial.println(distance1);
  digitalWrite(trigpin2,HIGH);  
  delay(1000);  
  digitalWrite(trigpin2,LOW);
  duration2=pulseIn(echopin2,HIGH);  
  distance2= duration2*0.034/2;
  Serial.print("distance2 : ");
  Serial.println(distance2); 
  int pos;
  if(distance1<=10)
  {
    Serial.println("Open gate");
    digitalWrite(led1,HIGH);
    digitalWrite(led2,LOW);
    myservo.write(90);              
    delay(4000);
    myservo.write(0);
    Serial.println("Close gate");                      
  }
  if(distance2 >=11)
  {
      Serial.println("slot is empty you can park the vechile");
      digitalWrite(led2,HIGH);
      digitalWrite(led1,LOW);              
      delay(50);                      
  }
  if(distance2 <=10)
  {
    Serial.println("slot is full you cannot  park the vechile");
    digitalWrite(led2,LOW);
    digitalWrite(led1,LOW);
  }
  if(distance1>=11)
  {
    Serial.println("Close gate");
    digitalWrite(led2,LOW);
    digitalWrite(led1,LOW);
  }
  delay(1500);


PublishData(distance1,distance2);
 if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}
void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(topic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void PublishData(int distance1, int distance2){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"distance1\":";
  payload += distance1;
  payload+="," "\"distance2\":";
  payload += distance2;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
