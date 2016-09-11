#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OSCBundle.h>
#include <OSCTiming.h>

EthernetUDP Udp;

//Arduino IP
IPAddress inIP(192.168.1.200);
//host IP
IPAddress outIP(192.168.1.2);

//Arduino port
uint16_t inPort = 49160;
//defualt host port
uint16_t outPort = 49155;

//arduino mac addr.
const char mac[] = {
 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

 const int sensorPin[4]  = {A0, A1, A2, A3};
 float curentVal[4]  = {};
 float activVal[4]   = {};

void setup() {
  /////////DEBUG/////////
  Serial.Begin(9600);
  delay(500);
  Serial.println("begin");


}

void loop() {

  //analogRead
  //smooth                    or filter so mutch it only triggers true
  // if dif > thres
    //send OSC


}
