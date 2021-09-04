//sensor
#include "MAX30100_PulseOximeter.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESP8266WebServer.h"
#include <Wire.h>
#include <SimpleTimer.h>
  #define REPORTING_PERIOD_MS 10000

SimpleTimer timer;



#include "FirebaseESP8266.h"
#include "esp8266wifi.h" // diubah tanda kurang dari dan lebih dari sebelum esp dan sesudah .h

#define FIREBASE_HOST "https://projekiot-8b4d3-default-rtdb.asia-southeast1.firebasedatabase.app/" // diisi hostname kalian
#define FIREBASE_AUTH "dvXloOD7EATHijvOze2vSwyR99bYPtzYmsCLxlOZ" // diisi fire auth
#define WIFI_SSID "Za_Fa" // ssid wifi kalian
#define WIFI_PASSWORD "pikirendewe" //password wifi
  String path = "/Sensor";
  String jsonStr;
FirebaseData firebaseData;

PulseOximeter pox;

uint32_t tsLastReport = 0;
volatile boolean heartBeatDetected = false;
void setup() {
  Serial.begin(115200);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
      while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
              }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());
    Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

   if (!pox.begin())
    {
      Serial.println("FAILED");
      for(;;);
    }
    else
    {
      Serial.println("SUCCESS");
     
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);

    pox.setOnBeatDetectedCallback(onBeatDetected);
    timer.setInterval(3000L, getSendData);
}

void onBeatDetected()
{
  heartBeatDetected = true;
    Serial.println("Beat!");
}


void loop() {
      pox.update();

  if (millis() - tsLastReport >= REPORTING_PERIOD_MS) {
pox.shutdown();
getSendData();
pox.resume();

          tsLastReport = millis();
    
    }  
  }

  void getSendData() {
        float bpm = pox.getHeartRate();
        float spO2 = pox.getSpO2();
        if(heartBeatDetected && bpm!=0){
          if(spO2>0){
              Firebase.setInt(firebaseData, "denyut nadi",pox.getHeartRate());
              Firebase.setInt(firebaseData, "Oksigen",pox.getSpO2());
                      Serial.println("Diupload");
          }
        }

  }
