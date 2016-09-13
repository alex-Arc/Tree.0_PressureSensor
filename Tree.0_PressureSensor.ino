#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>
#include <OSCBundle.h>
#include <OSCTiming.h>

EthernetUDP Udp;

//Arduino IP
IPAddress inIP(192,168,1,200);
//host IP
IPAddress outIP(192,168,1,2);

//Arduino port
uint16_t inPort = 49160;
//defualt host port
uint16_t outPort = 49155;

//arduino mac addr.
const char mac[] = {
 0xD90, 0xA2, 0xDA, 0x10, 0x38, 0xE9 
 };


const static uint8_t defaultBeta[4] = {1, 4, 4, 4};
const static int8_t defaultThreshold[4] = {1, 1, 1, 1};

uint8_t Beta[4];
int8_t threshold[4];
int32_t rawData[4];
int32_t smoothData[4];
int16_t prevSmoothData[4];
int16_t diff[4];
const int sensorPin[4]  = {A0, A1, A2, A3};

boolean newDara = false;

void setup() {
  /////////DEBUG/////////
  Serial.begin(9600);
  delay(500);
  Serial.println("begin");

  
  Ethernet.begin(mac,inIP);
  Udp.begin(inPort);
  
  for (int i = 0; i < 4; i++) {
    Beta[i] = defaultBeta[i];
    threshold[i] = defaultThreshold[i];
  }
}

void loop() {
  OSCBundle bnl;
  for (int i = 0; i < 4; i++) {
    rawData[i] = analogRead(sensorPin[i]);
    rawData[i] <<= 21;
    smoothData[i] = (smoothData[i] << Beta[i])-smoothData[i];
    smoothData[i] += rawData[i];
    smoothData[i] >>= Beta[i];
    smoothData[i] >>= 21;
    diff[i] = smoothData[i]-prevSmoothData[i];
    if (diff[i] > threshold[i] || diff < 0-threshold[i]){
      prevSmoothData[i] = smoothData[i];
      char oscAddr[16] = "/pressureSens/";
      oscAddr[14] = i+48;
      oscAddr[15] = '\0';
      //Serial.println(oscAddr);
      OSCMessage msg(oscAddr);
      msg.add(smoothData[i]);
      Udp.beginPacket(outIP, outPort);
      msg.send(Udp);
      Udp.endPacket();
    }
  }

 /* if (newDara == true) {
    Udp.beginPacket(outIP, outPort);
    bnl.send(Udp);
    Udp.endPacket();
    bnl.empty();
  }*/
  delay(5);


}
