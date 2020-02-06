#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

//GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
SoftwareSerial GPS_SoftSerial(D4, D3);
TinyGPSPlus gps;
volatile float minutes, seconds;
volatile int degree, secs, mins;
int latitude1;
int longitude1;



//dht
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11
  
//----------- Enter you Wi-Fi Details---------//
char ssid[] = "lot"; //SSID
char pass[] = "12345678"; // Password
//-------------------------------------------//

const int trigger = D6;
const int echo = D5;
long T;
float distanceCM;
WiFiClient  client;

unsigned long myChannelField = 123456; // Channel ID
const int ChannelField1 = 1; // Which channel to write data
const char * myWriteAPIKey = "xxxxxxxxxxx"; // Your write API Key


//dht
#define dht_dpin D0
DHT dht(dht_dpin, DHTTYPE);
const int ChannelField2 = 2;

//GPS
const int ChannelField3 = 3;
const int ChannelField4 = 4;
const int ChannelField5 = 5;





void setup()
{
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);


  dht.begin();
  Serial.println("Humidity and temperature\n\n");
  delay(700);

  GPS_SoftSerial.begin(9600);
  
 


  
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}
void loop()
{
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
  ThingSpeak.writeField(myChannelField, ChannelField1, distanceCM, myWriteAPIKey);
  delay(1000);




    float h = dht.readHumidity();
    float t = dht.readTemperature();         
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");
    delay(800);

    ThingSpeak.writeField(myChannelField, ChannelField2,dht.readTemperature() , myWriteAPIKey);

       

//gps
smartDelay(1000); /* Generate precise delay of 1ms */
        unsigned long start;
        double lat_val, lng_val, alt_m_val;
        uint8_t hr_val, min_val, sec_val;
        bool loc_valid, alt_valid, time_valid;
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
        alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
        alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
        hr_val = gps.time.hour(); /* Get hour */
        min_val = gps.time.minute();  /* Get minutes */
        sec_val = gps.time.second();  /* Get seconds */
        time_valid = gps.time.isValid();  /* Check if valid time data is available */
        if (!loc_valid)
        {          
          Serial.print("Latitude : ");
          Serial.println("*****");
          Serial.print("Longitude : ");
          Serial.println("*****");
        }
        else
        {
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val, 6);
          Serial.print("Latitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val, 6);
          Serial.print("Longitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
        }
        if (!alt_valid)
        {
          Serial.print("Altitude : ");
          Serial.println("*****");
        }
        else
        {
          Serial.print("Altitude : ");
          Serial.println(alt_m_val, 6);    
        }
        if (!time_valid)
        {
          Serial.print("Time : ");
          Serial.println("*****");
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d/%02d/%02d \n", hr_val, min_val, sec_val);
          Serial.print(time_string);    
        }

        latitude1=lat_val;
        longitude1=lng_val;
         

        ThingSpeak.writeField(myChannelField, ChannelField3, latitude1 , myWriteAPIKey);
        ThingSpeak.writeField(myChannelField, ChannelField4,longitude1 , myWriteAPIKey);
        
        
  
}





static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}







void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{  
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
}













