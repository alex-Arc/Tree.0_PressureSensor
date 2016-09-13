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
 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED 
 };


const static uint8_t defaultBeta[4] = {1, 4, 4, 4};

uint8_t Beta[4];
uint32_t rawData[4];
uint32_t smoothData[4];
uint16_t prevSmoothData[4];

const int sensorPin[4]  = {A0, A1, A2, A3};

void setup() {
  /////////DEBUG/////////
  Serial.begin(9600);
  delay(500);
  Serial.println("begin");

  for (int i = 0; i < 4; i++) {
    Beta[i] = defaultBeta[i];
  }
}

void loop() {

  for (int i = 0; i < 4; i++) {
    rawData[i] = analogRead(sensorPin[i]);
    rawData[i] <<= 22;
    smoothData[i] = (smoothData[i] << Beta[i])-smoothData[i];
    smoothData[i] += rawData[i];
    smoothData[i] >>= Beta[i];
    smoothData[i] >>= 22;
  }
    /*Serial.print(smoothData[0]);
    Serial.print(" ");
    Serial.print(prevSmoothData[0]);
    Serial.print(" ");
    int16_t diff = smoothData[0]-prevSmoothData[0];
    Serial.println(diff);*/
    int16_t diff = smoothData[0]-prevSmoothData[0];
    if (diff > 1 || diff < -1){
       Serial.println(smoothData[0]);
       prevSmoothData[0] = smoothData[0];
    }

  
  delay(5);
  //analogRead
  //smooth                    or filter so mutch it only triggers true
  // if dif > thres
    //send OSC


}
