#include <WiFi.h>
#include <WiFiUdp.h>
#include "Wire.h"
#include <MPU6050_light.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#define TRIGGER_PIN 0

IPAddress udpAddress;
const int udpPort = 50000;

WiFiUDP udp;
WiFiManager wifiManager;
const char DEVICE_NAME[] = "balance";

MPU6050 mpu(Wire);
unsigned long timer = 0;

void checkButton(){
  // check for button press
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if( digitalRead(TRIGGER_PIN) == LOW ){
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideaa for production
      delay(3000); // reset delay hold
      if( digitalRead(TRIGGER_PIN) == LOW ){
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wifiManager.resetSettings();
        ESP.restart();
      }
      
      // start portal w delay
      Serial.println("Starting config portal");
      wifiManager.setConfigPortalTimeout(120);
      
      if (!wifiManager.startConfigPortal(DEVICE_NAME)) {
        Serial.println("Failed to connect");
        delay(3000);
        // ESP.restart();
      } else {
        //if you get here you have connected to the WiFi
        Serial.println("WiFi connected!");
      }
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIGGER_PIN, INPUT);
  
  //WiFiManager
  bool res = wifiManager.autoConnect(DEVICE_NAME);
  if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
  } else {
      Serial.print(F("WiFi connected! IP address: "));
      Serial.println(WiFi.localIP());
  }
  // use the assigned local IP and change the last byte to .255 to get the broadcast adress for this networks
  // devices listening on the same port will then be able to listen to the data
  udpAddress = WiFi.localIP();
  udpAddress[3]=255;
  Serial.print("Broadcast Address: ");
  Serial.println(udpAddress);
  
  // mDNS setup to be reachable as balanceboard.local
  if(!MDNS.begin(DEVICE_NAME)) {
     Serial.println("Error starting mDNS");
     return;
  }
  
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void loop() {
  mpu.update();

  // store sensed data in an array
  float angles[3] = {mpu.getAngleX(),
                     mpu.getAngleY(),
                     mpu.getAngleZ()};
  // convert data to one unified string
  String x = String(angles[0], 2);
  String y = String(angles[1], 2);
  String z = String(angles[2], 2);
  String angles_str = "x:"+x+",y:"+y+",z:"+z;
  int angles_len = angles_str.length();
  // convert unified string to char array
  char char_buffer[50];
  angles_str.toCharArray(char_buffer, 50);
  // convert char array to bytes (for the udp.write(...) function)
  byte byte_buffer[50];
  for(int i = 0; i < angles_len; i++) {
    byte_buffer[i] = byte(char_buffer[i]);
  }
   
  if((millis()-timer)>100){ // print data every 10ms
    Serial.println(angles_str);
    udp.beginPacket(udpAddress, udpPort);
    udp.write(byte_buffer, angles_len);
    udp.endPacket();
    timer = millis();  
  }
  checkButton();
}
