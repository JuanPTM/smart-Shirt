#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "FS.h"

// Set these to run example.
#define FIREBASE_HOST "testw-8b1b6.firebaseio.com"
#define FIREBASE_AUTH "uRwL0OL0ICuAPBw7HKn0rgyMmCQnRrrp3MKnCfau"
#define WIFI_SSID "proyectos1"
#define WIFI_PASSWORD "proyectos"

#define analogIn A0
#define B D2
#define A D1
#define strobe D5

#define TESTFILE "/values.txt"

//#define DEBUG       --- UNCOMENT FOR DEBUG PORPUSE

#ifdef DEBUG
 #define DEBUG_PRINT(x)     Serial.print(x)
 #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x)
#endif 

bool    spiffsActive = false;
bool connFirebase = false;
int readValue[] = {
  0,0,0,0};  

// digital values to control 4 inputs
int b_bin[]={LOW,LOW,HIGH,HIGH};
int a_bin[]={LOW,HIGH,LOW,HIGH};

//aux
int contador=0;
int entrada=0;
int b_val=0;
int a_val=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(strobe, OUTPUT);

  Serial.begin(9600);
  if (SPIFFS.begin()) {
    DEBUG_PRINTLN("SPIFFS Active");
    spiffsActive = true;
  }  
}

void loop() {
  
  for(entrada=0;entrada<=3;entrada++) {

    //select mux input
    a_val=a_bin[entrada];
    b_val=b_bin[entrada];

    digitalWrite(A,a_val);
    digitalWrite(B,b_val);

    //strobe LOW to read
    digitalWrite(strobe,LOW);

    //read value
    readValue[entrada] = analogRead(analogIn);  // read input value

    //strobe HIGH to avoid jitters
    digitalWrite(strobe,HIGH);
  }
  
  DEBUG_PRINT("Valores leidos: ");
  DEBUG_PRINT(readValue[0]);
  DEBUG_PRINT(" ");
  DEBUG_PRINT(readValue[1]);
  DEBUG_PRINT(" ");
  DEBUG_PRINT(readValue[2]);
  DEBUG_PRINTLN(" .");

  if (readValue[0]<850 || readValue[1]<850 || readValue[2]<850)
  {
    DEBUG_PRINTLN("LLEGA AL WIFI");
    if( !(WiFi.status() == WL_CONNECTED))
    {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      contador = 0;
      while(contador < 20)
      {
        delay(500);
        DEBUG_PRINT(".");
        contador++;
      }
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      DEBUG_PRINT("connected: ");
      DEBUG_PRINTLN(WiFi.localIP());
      if (!connFirebase){
          Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
          connFirebase = true;
      }
      Firebase.pushString("",String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
      DEBUG_PRINTLN("Enviado valor a FIREBASE");
      if (Firebase.failed()) 
      {
        DEBUG_PRINT("setting /message failed:");
        DEBUG_PRINTLN(Firebase.error());  
        return;
      }
      delay(100);

      if (spiffsActive)
      {
        if (SPIFFS.exists(TESTFILE)) {
          File f = SPIFFS.open(TESTFILE, "r");
          if (f) {
            String key;
            String value;
            while (f.position()<f.size())
            {
              key=f.readStringUntil('/');
              key.trim();
              value=f.readStringUntil('\n');
              value.trim();
              DEBUG_PRINTLN(key+value);
              yield();
              Firebase.pushString("",String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
              DEBUG_PRINTLN("Enviado valor a FIREBASE");
              if (Firebase.failed()) 
              {
                DEBUG_PRINT("setting /message failed:");
                DEBUG_PRINTLN(Firebase.error());  
                return;
              }
              delay(100);
              yield();
            } 
            f.close();
            SPIFFS.remove(TESTFILE);
          }else
          {
            DEBUG_PRINTLN("Error al abrir el fichero.");
          }
        }
      }
    }else 
    {
      if (spiffsActive)
      {
        if (SPIFFS.exists(TESTFILE)) {
          File f = SPIFFS.open(TESTFILE, "a");
          if (f) {
            f.println(String(millis())+"/"+String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
            DEBUG_PRINTLN("Escrito en fichero= "+ String(millis())+"/"+String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
          }else
          {
            DEBUG_PRINTLN("Error al abrir el fichero.");
          }
        }else{
          DEBUG_PRINTLN("No echiste,creando");
          File f = SPIFFS.open(TESTFILE, "w");
          if (f) {
            f.println(String(millis())+"/"+String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
            DEBUG_PRINTLN("Escrito en fichero= "+ String(millis())+"/"+String(readValue[0])+"/"+String(readValue[1])+"/"+String(readValue[2]));
          }else
          {
            DEBUG_PRINTLN("Error al abrir el fichero.");
          }
        }
      }else
      {
        DEBUG_PRINTLN("SPIFFS NO ESTA FUNCIONANDO");
      }
    }
  }
}
