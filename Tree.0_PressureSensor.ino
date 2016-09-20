#include <EEPROM.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>
#include <OSCBundle.h>


EthernetUDP Udp;

//Arduino IP
IPAddress inIP(192, 168, 8, 11);
//defaul host IP
const IPAddress defaulOutIP(192, 168, 8, 2);

//Arduino port
const uint16_t inPort = 49160;
//defualt host port
const uint16_t defaulOutPort = 49161;

//arduino mac addr.
const char mac[] = {
 0x90, 0xA2, 0xDA, 0x10, 0x39, 0x4A
 };


int32_t rawData[4];
int32_t smoothData[4];
int16_t prevSmoothData[4];

const static int sensorPin[4] = {A0, A1, A2, A3};
const static int ledPin = 6;

const static uint8_t defaultBeta = 2;
const static int8_t defaultThreshold = 2;

char oscAddr[21] = "/pressureSens/";   // <--13

typedef struct S_config {
  uint8_t beta[4];
  int8_t threshold[4];
  uint16_t outPort;
  IPAddress outIP;
} T_config;

T_config config = {
  {defaultBeta, defaultBeta, defaultBeta, defaultBeta},
  {defaultThreshold, defaultThreshold, defaultThreshold, defaultThreshold},
  defaulOutPort,
  defaulOutIP
};

//////////eeprom //////////////////////////////////////////////
// ID of the settings block
const static uint8_t CONFIG_VERSION[3] = {22, 44, 66};
#define CONFIG_MEM_START 10

void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_MEM_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_MEM_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_MEM_START + 2) == CONFIG_VERSION[2]) {
    for (unsigned int t = 0; t < sizeof(config); t++) {
      *((char*)&config + t ) = EEPROM.read(CONFIG_MEM_START + 3 + t);
    }
  }
}

void saveConfig() {
  EEPROM.write(CONFIG_MEM_START + 0, CONFIG_VERSION[0]);
  EEPROM.write(CONFIG_MEM_START + 1, CONFIG_VERSION[1]);
  EEPROM.write(CONFIG_MEM_START + 2, CONFIG_VERSION[2]);
  for (unsigned int t = 0; t < sizeof(config); t++) {
    EEPROM.write(CONFIG_MEM_START + 3 + t, *((char*)&config + t));
  }
}

////////////////OSC callback/////////////////////////////
char * numToOSCAddress( int num){
    static char s[10];
    int i = 9;

    s[i--]= '\0';
  do
    {
    s[i] = "0123456789"[num % 10];
                --i;
                num /= 10;
    }
    while(num && i);
    s[i] = '/';
    return &s[i];
}
void setLED(OSCMessage &msg, int patternOffset) {
  if (msg.isInt(0)){
    int val = msg.getInt(0);
    if (val < 0) {val = 0;}
    if (val > 255) {val = 255;}
    analogWrite(ledPin, val);
  }
}

void setBeta(OSCMessage &msg, int patternOffset) {
  for (char j = 0; j < 4; j++) {
    int numMatched = msg.match(numToOSCAddress(j), patternOffset);
    if (numMatched) {
      if (msg.isInt(0)){
        config.beta[j] = msg.getInt(0);
        saveConfig();
      }
    }
  }
}

void setThreshold(OSCMessage &msg, int patternOffset) {
  for (char j = 0; j < 4; j++) {
    int numMatched = msg.match(numToOSCAddress(j), patternOffset);
    if (numMatched) {
      if (msg.isInt(0)){
        config.threshold[j] = msg.getInt(0);
        saveConfig();
      }
    }
  }
}

void setOutPort(OSCMessage &msg, int patternOffset) {
  if (msg.isInt(0)){
    config.outPort = msg.getInt(0);
    saveConfig();
    OSCMessage msgOut("/i/am/alive/");
    Udp.beginPacket(config.outIP, config.outPort);
    msgOut.send(Udp);
    Udp.endPacket();
    msgOut.empty();
  }
}

void setOutIP(OSCMessage &msg, int patternOffset) {
  for (char j = 0; j < 4; j++) {
    int numMatched = msg.match(numToOSCAddress(j), patternOffset);
    if (numMatched) {
      if (msg.isInt(0)){
        config.outIP[j] = msg.getInt(0);
        saveConfig();
        OSCMessage msgOut("/i/am/alive/");
        msgOut.add(inIP[3]);
        msgOut.add(config.outPort);
        Udp.beginPacket(config.outIP, config.outPort);
        msgOut.send(Udp);
        Udp.endPacket();
        msgOut.empty();
      }
    }
  }
}

void setup() {
  //saveConfig();
  loadConfig();
  analogWrite(ledPin, 0);
  // start Ethernet
  Ethernet.begin(mac,inIP);
  Udp.begin(inPort);
  delay(1000);
  OSCMessage msgOut("/i/am/alive/");
  msgOut.add(inIP[3]);
  msgOut.add(config.outPort);
  Udp.beginPacket(config.outIP, config.outPort);
  msgOut.send(Udp);
  Udp.endPacket();
  msgOut.empty();

  oscAddr[14] = '0';
  oscAddr[15] = '1';
  oscAddr[16] = '1';
  oscAddr[17] = '/';
}

void loop() {
  for (int i = 0; i < 4; i++) {
    rawData[i] = analogRead(sensorPin[i]);
    rawData[i] <<= 21;
    smoothData[i] = (smoothData[i] << config.beta[i])-smoothData[i];
    smoothData[i] += rawData[i];
    smoothData[i] >>= config.beta[i];
    smoothData[i] >>= 21;
    if (smoothData[i] - prevSmoothData[i] > config.threshold[i] || prevSmoothData[i] - smoothData[i] > config.threshold[i]){
      prevSmoothData[i] = smoothData[i];
      oscAddr[18] = i+48;
      oscAddr[19] = '\0';
      OSCMessage msgOut(oscAddr);
      msgOut.add(smoothData[i]);
      Udp.beginPacket(config.outIP, config.outPort);
      msgOut.send(Udp);
      Udp.endPacket();
    }
  }

  int size;
  if( (size = Udp.parsePacket())>0) {
    OSCMessage msgIn;
    while(size--) {
      msgIn.fill(Udp.read());
    }
    if(!msgIn.hasError()) {
      msgIn.route("/LED", setLED);
      msgIn.route("/settings/beta", setBeta);
      msgIn.route("/settings/threshold", setThreshold);
      msgIn.route("/settings/outPort", setOutPort);
      msgIn.route("/settings/outIP", setOutIP);
    }
  }

  //delay(5);
}
