#include <WiFi.h>
#include <WiFiUdp.h>
#include "Wire.h"
#include <MPU6050_light.h>

char * ssid = "SSID";
char * pwd = "PASSWORD";

IPAddress udpAddress;
const int udpPort = 50000;

WiFiUDP udp;

MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);

  //Connect to the WiFi network
  WiFi.begin(ssid, pwd);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // use the assigned local IP and change the last byte to .255 to get the broadcast adress for this networks
  // devices listening on the same port will then be able to listen to the data
  udpAddress = WiFi.localIP();
  udpAddress[3]=255;
  Serial.print("Broadcast Address: ");
  Serial.println(udpAddress);
  
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
}