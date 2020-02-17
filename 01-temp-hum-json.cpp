#include "ESPHelper.h"
#include "Chrono.h"
#include <ArduinoJson.h>
#include "DHT.h"

Chrono myChrono(Chrono::SECONDS);

const char* svrtopic = "domoticz/in";

#define DHTPIN 0
#define DHTTYPE DHT22

String LastKnownT = "19.0";
String LastKnownH = "35.00";
float hum = 35.00;
String HumStat = "0";
float HumStatOneA = 40.10;
float HumStatOneB = 44.90;
float HumStatOneC = 55.10;
float HumStatOneD = 59.90;
float HumStatZeroA = 45.00;
float HumStatZeroB = 55.00;
float HumStatTwo = 40.00;
float HumStatThree = 60.00;

float TCal = 1.0;
float HCal = 4.0;

DHT dht(DHTPIN, DHTTYPE);

netInfo homeNet = {  .mqttHost = "***.***.***.***",     //can be blank if not using MQTT
          .mqttUser = "",   //can be blank
          .mqttPass = "",   //can be blank
          .mqttPort = 1883,         //default port for MQTT is 1883 - only chance if needed.
          .ssid = "YourSSID", 
          .pass = "YourSSIDPass"};

ESPHelper myESP(&homeNet);

void setup() {
  delay(100);
  Serial.begin(115200); //start the serial line
  delay(100);
  Serial.println("Starting Up, Please Wait...");
  myESP.OTA_disable();
  myESP.addSubscription(svrtopic);
  Serial.print("MQTT Subscirption added: ");
  Serial.println(svrtopic);
  myESP.begin();
  myESP.setMQTTCallback(callback);
  Serial.println("MQTT Started");
  dht.begin();
  delay(100);
  myChrono.restart();
  Serial.println("Initialization Finished.");  
}

void loop(){
  myESP.loop();  //run the loop() method as often as possible - this keeps the network services running
  unsigned long elapsed = myChrono.elapsed();
  if (myChrono.hasPassed(60,true)) {
      float h = dht.readHumidity();
      float hum = h - HCal; 
      if (hum>=HumStatThree){
        String HumStat = "3";
      }
      else if ((hum>=HumStatZeroA)&&(hum<=HumStatZeroB)){
        String HumStat = "0";
      }
      else if (hum<=HumStatTwo){
        String HumStat = "2";
      }
      else if ((hum>=HumStatOneA)&&(hum<=HumStatOneB)){
        String HumStat = "1";
      }
      else if ((hum>=HumStatOneC)&&(hum<=HumStatOneD)){
        String HumStat = "1";
      }
      String mystring1;
      mystring1 = String(hum);
      Serial.print("Hum: ");
      Serial.println(hum);
      float t = dht.readTemperature();
      float tmp = t - TCal;
      String mystring2;
      mystring2 = String(tmp);
      Serial.print("Temp: ");
      Serial.println(t);
      if (mystring1 != mystring2){
        LastKnownT = mystring2;
        LastKnownH = mystring1;
      }
      else if (mystring1 == mystring2){
        Serial.println("Sensor Error");
        mystring2 = LastKnownT;
        mystring1 = LastKnownH;
      }
      StaticJsonBuffer<300> JSONbuffer;
      JsonObject& JSONencoder = JSONbuffer.createObject();
      JSONencoder["idx"] = 11;
      JSONencoder["nvalue"] = 0;
      JSONencoder["svalue"] = LastKnownT+";"+LastKnownH+";"+HumStat;
      //JSONencoder["HUM"] = h;
      char JSONmessageBuffer[100];
      JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      myESP.publish(svrtopic,JSONmessageBuffer,false);
      Serial.print("JSON: ");
      Serial.println(JSONmessageBuffer);
  }
  delay(10);   
  yield();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = topic;
  if(payload[0] == '0'){
    Serial.print("Received from Topic: ");
    Serial.println(topic);
    Serial.println("Payload Received: ");
     
  }
  else if (payload[0] == '1'){
    Serial.println("Payload Received 1");
 
  }  
}
