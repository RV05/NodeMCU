//gps
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

  
//----------- Enter you Wi-Fi Details---------//
char ssid[] = "lot"; //SSID
char pass[] = "12345678"; // Password
//-------------------------------------------//


const int trigger = D4;
const int echo = D3;
long T;
float distanceCM;
const int ChannelField2 = 2;


float xd,yd,zd;
const int ChannelField3 = 3;// Which channel to write data
const int ChannelField4= 4;
const int ChannelField5 = 5;




WiFiClient  client;

unsigned long myChannelField = 944988; // Channel ID
const int ChannelField1 = 1; // Which channel to write data
const char * myWriteAPIKey = "PRPXPKH4RU2AVXBV"; // Your write API Key



//gps
static const int RXPin = D8, TXPin = D7;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
WiFiClient  client;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);




void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  //gps
  ss.begin(GPSBaud);
  

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  if(!accel.begin())
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
    
  }
  accel.setRange(ADXL345_RANGE_4_G);
   WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);





  
}





void loop() {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  


  digitalWrite(trigger, LOW);
  delay(1);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  T = pulseIn(echo, HIGH);
  distanceCM = T * 0.034;
  distanceCM = distanceCM / 2;
  Serial.print("Distance in cm: ");
  Serial.println(distanceCM);
  ThingSpeak.writeField(myChannelField, ChannelField4, distanceCM, myWriteAPIKey);



sensors_event_t event; 
  accel.getEvent(&event);
 
  Serial.print((event.acceleration.x)/9.8); Serial.print(",");
  Serial.print((event.acceleration.y)/9.8); Serial.print(",");
  Serial.println((event.acceleration.z)/9.8);
  delay(10);
  xd=(event.acceleration.x)/9.8;
  yd=(event.acceleration.y)/9.8;
  zd=(event.acceleration.z)/9.8;
  ThingSpeak.writeField(myChannelField, ChannelField1, xd, myWriteAPIKey);
   ThingSpeak.writeField(myChannelField, ChannelField2, yd, myWriteAPIKey);
    ThingSpeak.writeField(myChannelField, ChannelField3, zd, myWriteAPIKey);
  delay(1000);



   while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }


  
}




//GPS
void displayInfo()
{
 // Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());
    
    String latbuf;
    latbuf += (String(latitude, 6));
    Serial.println(latbuf);

    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.println(lonbuf);

    ThingSpeak.setField(5, latbuf);
    ThingSpeak.setField(6, lonbuf);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  
    delay(20000);
    
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();



