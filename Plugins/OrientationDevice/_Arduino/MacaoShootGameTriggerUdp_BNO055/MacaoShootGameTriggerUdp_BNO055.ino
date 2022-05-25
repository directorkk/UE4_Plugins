#include "AK09918.h"
#include "ICM20600.h"
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

// wifi, tcp
ESP8266WiFiMulti WiFiMulti;
const char* gWifiSSID = "Phalanity_ShootTrigger";
const char* gWifiPassword = "42817785";
WiFiClient gTcpClient;
WiFiUDP gUdp;
const char* gUdpHost = "192.168.3.26";
const uint16_t gUdpPort = 20258;
const char* gPattern = "DATA_START";
int gPatternLen = String(gPattern).length();
int gByteSentCount = 0;
int gTimeCheckServerAlive = millis();
int gBoardID = -1;

// trigger
bool gTriggerBtnIsPressed = false;
bool gCalibrateBtnIsPressed = false;

void setup()
{
  // *
  // initialize sensor
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
  Serial.begin(9600);

  
  /* Initialise BNO055 */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);

  // *
  // initialize wifi and tcp connection
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(gWifiSSID, gWifiPassword);

  gBoardID = getBoardID(WiFi.macAddress());
  Serial.println("BoardId: " + String(gBoardID));
  Serial.print("Wait for WiFi... ");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // *
  // initialize button trigger
  pinMode(D0, INPUT);
  pinMode(D5, INPUT);
}


void loop()
{
  /*
  sensors_event_t event; 
  bno.getEvent(&event);
  sendMessage("ORIENTATION|" + String(event.orientation.x) + "," + String(event.orientation.y) + "," + String(event.orientation.z));
  */
  
  imu::Quaternion quat = bno.getQuat();  
  sendMessage("QUAT|" + String(gBoardID) + ";" + String(quat.w()) + "," + String(quat.x()) + "," + String(quat.y()) + "," + String(quat.z()));

  if (digitalRead(D5) == 1 && !gCalibrateBtnIsPressed)
  {
    gCalibrateBtnIsPressed = true;
    //sendMessage("CALIBRATE|" + String(event.orientation.x) + "," + String(event.orientation.y) + "," + String(event.orientation.z));
    sendMessage("CALIBRATE|" + String(gBoardID) + ";" + String(quat.w()) + "," + String(quat.x()) + "," + String(quat.y()) + "," + String(quat.z()));
  }
  if (digitalRead(D5) != 1 && gCalibrateBtnIsPressed)
  {
    gCalibrateBtnIsPressed = false;
  }
  
  if (digitalRead(D0) == 1 && !gTriggerBtnIsPressed)
  {
    gTriggerBtnIsPressed = true;
    sendMessage("PRESS|" + String(gBoardID) + ";");
  }
  if (digitalRead(D0) != 1 && gTriggerBtnIsPressed)
  {
    gTriggerBtnIsPressed = false;
    sendMessage("RELEASE|" + String(gBoardID) + ";");
  }

  /*Serial.print("X: ");
  Serial.print(event.orientation.x, 2);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 2);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 2);
  Serial.println("");*/
  Serial.print("W: ");
  Serial.print(quat.w(), 2);
  Serial.print("\tX: ");
  Serial.print(quat.x(), 2);
  Serial.print("\tY: ");
  Serial.print(quat.y(), 2);
  Serial.print("\tZ: ");
  Serial.print(quat.z(), 2);
  Serial.println("");

  delay(50);

}


int sendMessage(String Message)
{
  int byteCount = 0;

  Message = Message + " ";

  //if(gTcpClient.connected())
  //{
  int messageLen = Message.length();
  char messageInBytes[messageLen];
  Message.toCharArray(messageInBytes, messageLen);
  int totalLen = gPatternLen + 8 + messageLen;
  byte packedMessage[totalLen];
  for (int i = 0 ; i < gPatternLen ; i++)
  {
    packedMessage[i] = gPattern[i];
  }
  packedMessage[gPatternLen + 0] = (byte)(messageLen >> 0);
  packedMessage[gPatternLen + 1] = (byte)(messageLen >> 8);
  packedMessage[gPatternLen + 2] = (byte)(messageLen >> 16);
  packedMessage[gPatternLen + 3] = (byte)(messageLen >> 24);
  packedMessage[gPatternLen + 4] = 0;
  packedMessage[gPatternLen + 5] = 0;
  packedMessage[gPatternLen + 6] = 0;
  packedMessage[gPatternLen + 7] = 0;
  for (int i = 0 ; i < messageLen ; i++)
  {
    packedMessage[gPatternLen + 8 + i] = (byte)messageInBytes[i];
  }


  gUdp.beginPacket(gUdpHost, gUdpPort);
  byteCount = gUdp.write(packedMessage, totalLen);
  gUdp.endPacket();
  //byteCount = gTcpClient.write(packedMessage, totalLen);
  //}

  return byteCount;
}

int getBoardID(String MacAddress)
{
  String mac0 = getValue(MacAddress, ':', 0);
  String mac4 = getValue(MacAddress, ':', 4);
  String mac5 = getValue(MacAddress, ':', 5);

  /*Serial.println(hexToDec(mac0));
  Serial.println(hexToDec(mac4));
  Serial.println(hexToDec(mac5));*/
  int rtn = hexToDec(mac0) * hexToDec(mac4) * hexToDec(mac5);

  return rtn;
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int hexToDec(String Hex)
{
  int stringLen = Hex.length()+1;
  
  char hexInChar[stringLen];
  Hex.toCharArray(hexInChar, stringLen);

  return strtol(hexInChar, 0, 16);
}
