#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "FS.h"

// Set these to run example.
#define FIREBASE_HOST "testw-8b1b6.firebaseio.com"
#define FIREBASE_AUTH "uRwL0OL0ICuAPBw7HKn0rgyMmCQnRrrp3MKnCfau"
#define WIFI_SSID "pods"
#define WIFI_PASSWORD "ef96da80a978"

#define analogIn A0
#define B D2
#define A D1
#define strobe D5

#define TESTFILE "/values.txt"

int readValue[] = {0,0,0};
bool    spiffsActive = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  Serial.begin(9600);
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Active");
    spiffsActive = true;
  }
}

void loop() {
  
  // put your main code here, to run repeatedly:
  Serial.println("---------------------------");
  Serial.println("Iniciando lectura sensores. C1");
  digitalWrite(A,LOW);
  digitalWrite(B,HIGH);
  digitalWrite(strobe,LOW);
  readValue[0]=analogRead(analogIn);
  digitalWrite(strobe,HIGH);
  Serial.println(readValue[0]);
  delay(100);
  
  Serial.println("---------------------------");
  Serial.println("Iniciando lectura sensores. C3");
  //digitalWrite(A,LOW); //JUST INFO
  digitalWrite(B,LOW);
  digitalWrite(strobe,LOW);
  readValue[1]=analogRead(analogIn);
  digitalWrite(strobe,HIGH);
  Serial.println(readValue[1]);
  delay(100);
  
  Serial.println("---------------------------");
  Serial.println("Iniciando lectura sensores. C2");  
  digitalWrite(A,HIGH);
  //digitalWrite(B,LOW);
  digitalWrite(strobe,LOW);
  readValue[2]=analogRead(analogIn); 
  digitalWrite(strobe,HIGH);
  Serial.println(readValue[2]);
  delay(100);


  if (readValue[0]< 950 or readValue[1]<950 or readValue[2]<950)
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("connected: ");
      Serial.println(WiFi.localIP());
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      Firebase.setString(String(millis()),String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
      if (Firebase.failed()) 
      {
        Serial.print("setting /message failed:");
        Serial.println(Firebase.error());  
        return;
      }
      //TODO AÑADIR VOLCADO DEL FICHERO A BBDD y borrar fichero
    }else
    {
      if (spiffsActive)
      {
        if (SPIFFS.exists(TESTFILE)) {
          File f = SPIFFS.open(TESTFILE, "a");
          if (f) {
            f.println(String(millis())+"/"+String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
          }else
          {
            Serial.println("Error al abrir el fichero.");
          }
        }
      }else
      {
        Serial.println("SPIFFS NO ESTA FUNCIONANDO");
      }
    }
  }
}
