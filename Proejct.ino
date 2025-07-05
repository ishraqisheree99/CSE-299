
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT11   
#define WIFI_SSID "****"
#define WIFI_PASSWORD "****"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
//thinsspeak
#include <ESP8266HTTPClient.h>

WiFiClient client;

String thingSpeakAddress= "http://api.thingspeak.com/update?";
String writeAPIKey;
String tsfield1Name;
String request_string;

HTTPClient http;
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  timeClient.begin();
  dht.begin();
  Serial.println("Humidity and temperature\n\n");
  delay(700);
  timeClient.setTimeOffset(19800);
}

int n = 0;

void loop() {
  
  int sensorValue;
  for(int x = 0 ; x < 500 ; x++) //Start for loop 
  { delay(10);
    sensorValue = sensorValue + analogRead(A0); //Add analog values of sensor 1000 times 
  }
  sensorValue = sensorValue/500.0; //Take average of readings
  
  String s="ppm"+String(n);
  String t;
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  t=String(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  t=t+" "+String(timeStamp);
  Firebase.pushFloat(s,sensorValue);
  Firebase.pushString(s,t);
  n++;
  Serial.print(',');
  Serial.println(sensorValue);
 
  float h = dht.readHumidity();
    float temp = dht.readTemperature();         
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(temp); 
    Serial.println("C  ");
  
  
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }


  if (client.connect("api.thingspeak.com",80)) {
      request_string = thingSpeakAddress;
      request_string += "key=";
      request_string += "V7ZZSZQPH3RBHGFL";
      request_string += "&";
      request_string += "field1";
      request_string += "=";
      request_string += sensorValue;
      request_string += "&";
      request_string += "field2";
      request_string += "=";
      request_string += temp;
      request_string += "&";
      request_string += "field3";
      request_string += "=";
      request_string += h;
      http.begin(request_string);
      http.GET();
      http.end();
  }


  delay(1000);
}
