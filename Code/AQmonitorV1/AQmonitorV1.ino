//-----------------------------------------------
//This sketch is combined from Adafruit DHT sensor and tdicola for dht.h library
// https://learn.adafruit.com/dht/overview
// https://gist.github.com/teos0009/acad7d1e54b97f4b2a88
//other Authors Arduino and associated Google Script:
//Aditya Riska Putra
//Ahmed Reza Rafsanzani
//Ryan Eko Saputro
//See Also:
//http://jarkomdityaz.appspot.com/
//
//ELINS UGM
//
//Modified for Hackster.io project for the MKR1000 
//by Stephen Borsay(Portland, OR, USA)
//Since Arduino can't https, we need to use Pushingbox API (uses http)to run 
//the Google Script (uses https). Alternatly use Ivan's SecureWifi encryption
//Modified By Sam Salin for the GBRL
//Arduino Actually Can HTTPS, hoping to modify the code to use it
//PIN INFORMATION
//  PPM   -> MKR_0
//  Ozone -> A1
//  CO    -> A3
//  SHT31 -> iic (SCL/SDA)
#include <WiFi101.h>
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "v9606769D2CC718F"; //device ID on Pushingbox for our Scenario

const char* MY_SSID = "PSU"; //does not currently seem to want to connect to PSU network, can connect at home fine
const char* MY_PWD =  ""; //wifi password

//Define analog input and values for for Mics 03 sensor:
const int OzoneMPin=A1; 
int OzoneMSensorValue=0;

//Set Up digital pin for Shinyei PM sensor
int pin = 0;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 1000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

//Define analog input and values for EC4-500-CO sensor:
const int COPin=A3;
int COSensorValue=0;
int CO=0;


int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("serial conection initialized");

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network: ");
    Serial.println(MY_SSID);
    //Connect to WPA/WPA2 network.Change this line if using open/WEP network
    //status = WiFi.begin(MY_SSID, MY_PWD); //connect to secured wifi
    status = WiFi.begin(MY_SSID); //connect to open wifi
    delay(10000);  // wait 10 seconds for connection:
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
  }
  Serial.println("found SHT31");
}

void loop() {

   // Wait between measurements.
  delay(10000);

  //Data Assignments
  //Use float for higher resolution
  
  //read temp
  int humidityData = 0;// sht31.readTemperature(); 
  //read humidity
  int celData = 0;//sht31.readHumidity();
 
  //Read Particle Count from PM sensor

  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  if ((millis()-starttime) > sampletime_ms){
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    //Serial.print(lowpulseoccupancy);
    //Serial.print(",");
    //Serial.print(ratio);
    //Serial.print(",");
    //Serial.println(concentration);
    lowpulseoccupancy = 0;
    starttime = millis();
  }

  int fehrData = concentration;
  //read O3
  int hifData = analogRead(OzoneMPin); //Read value from ozone pin
  //read CO
  int hicData = analogRead(COPin);

  Serial.print("Temperature: ");
  Serial.print(humidityData);
  Serial.print(" *F\n");
  Serial.print("Humidity: ");
  Serial.print(celData);
  Serial.print("%\n");
  Serial.print("PPM:  ");
  Serial.print(fehrData);
  Serial.print("\n");
  Serial.print("Ozone Concentration: ");
  Serial.print(hicData);
  Serial.print("\n");
  Serial.print("CO Concentration: ");
  Serial.print(hifData);
  Serial.println("\n");

  Serial.println("\nSending Data to Server..."); 
    // if you get a connection, report back via serial:
  WiFiClient client;  //Instantiate WiFi object, can scope from here or Globally

    //API service using WiFi Client through PushingBox then relayed to Google
    if (client.connect(WEBSITE, 80)){ 
         client.print("GET /pushingbox?devid=" + devid
       + "&humidityData=" + (String) humidityData
       + "&celData="      + (String) celData
       + "&fehrData="     + (String) fehrData
       + "&hicData="      + (String) hicData
       + "&hifData="      + (String) hifData
         );

      // HTTP 1.1 provides a persistent connection, allowing batched requests
      // or pipelined to an output buffer
      client.println(" HTTP/1.1"); 
      client.print("Host: ");
      client.println(WEBSITE);
      client.println("User-Agent: MKR1000/1.0");
      //for MKR1000, unlike esp8266, do not close connection
      client.println();
      Serial.println("\nData Sent"); 
    }
}

