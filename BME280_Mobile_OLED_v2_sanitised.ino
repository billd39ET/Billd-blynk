/*
 * Created by Bill Donnelly Sep 2020
 * 
 * *******
 * ******* MUST use ESP8266 Board Manager v2.6.3 otherwise the OLED display will not work 
 * ******* AND must use modified version of Adafruit_SSD1306.h to allow for 128x64 display 
 * *******
 * 
 * 
 * Hardware: Wemos D1 Mini
 * MAC:      NN:NN:NN:NN:NN:NN
 * TFT:      SSD1306 128x64 I2C Oled LCD
 * 
 *
 * Based on Examples | Adafruit SSD1306 Library | WeMosD1_mini_I2C_Oled_Shield.ino
 * I2C
 * WeMosD1_mini_I2C_Oled_Shield.ino
 * 
 * 
 * 29 Sep 2020
 *  Added ArduinoOTA.setHostname("BME280_Mobile_OLED_v2");
 * 
 * 
 * 27 May 2020
 *  Removed Adafruit logo on startup . . . details https://forums.adafruit.com/viewtopic.php?f=31&t=63503
 *  
 * 
 * 23 May 2020
 *  Based on: BME280_Mobile_v1.ino
 *            Garage_Temp_BME280_TFT_Blynk_v1.ino
 *  All the same BME280 code, just added the TFT printing code          
 *  
 *  
 * 
 * 
 * 26 Dec 2019
 *  BME280_Mobile_NoDeepSleep_v1.ino
 *  Copied from BME280_Mobile_DeepSleep_v3.ino 
 *   Remove: 
 *    Deep sleep function, I want it to be powered from USB only
 *   Add:
 *    OTA (didn't work with deep sleep)
 *    Numerical entry to be able to adjust the alarm level from the app (copied from Blynk_OTA_Starter_Sketch_v3.ino)
 *    Reset Button Widget to reset Max Temperature from app
 *    Blynk.syncAll(); // Synchronise all widget values in the app to the server
 *   
 * 02 Nov 2019
 *  Add alarm limit, send Push plus Email
 *  Add Numeric Input Widget to adjust alarm threshold
 *  
 * 
 * 26 Oct 2019
 * 
 *  Add code to allow connect to any SSID, act as an Access Point to enter details
 *  Code copied from Blynk_Auto_Connect_Wifi_v1.ino
 *  
 *  Working well, auto connect when required, deep sleep, sends data every 60 secs
 *  
 *  
 * 
 * 25 Aug 2019
 * 
 *  Copied from BME280_Mobile_DeepSleep_v1.ino
 *  Experimenting with code to connect to multiple known SSIDs in Blynk
 *  
 *  Use examples in ESP8266WiFi | WifiMulti and WifiScan
 *  
 *  Stopped experimenting . . . the footy was too good ;-)
 * 
 * 3 Aug 2019
 * 
 *  Arduino sketch:  BME280_Mobile_DeepSleep_v1.ino
 *  Blynk project:   BME280 Data, tab Mobile
 *  Blynk device:    WeMos D1 (WiFi) - Mobile Wemos D1 BME280
 *  Blynk auth:      
 * 
 * Copied from: BME280_Starter_Sketch_v1.ino and Yard_Temp_BME280_Blynk_DeepSleep_v1.ino
 * Removed fron setup(): 
 *   timer.setInterval(3000L, wifiIP);
 *   timer.setInterval(3000L, wifiMAC);
 *   timer.setInterval(3000L, printValuesBlynk); 
 * Added to setup():
 *   wifiIP();
 *   wifiMAC();
 *   printValuesBlynk();
 *   ESP.deepSleep(20e6);          // 20 seconds
 *   //ESP.deepSleep(900e6);          // 15 minutes
 *   
 * This way, the ESP will wake up every 15 minutes, the setup() will run, which will run each of the functions, then sleep again.
 * Removed all OTA code
 * 
 * Had to add a brief delay after the Blynk.virtualWrite(V12, pressure); so that the function could actually
 *  send the pressure before the ESP went to deep sleep.
 *  
 * Pins: 
 *   testOTA         V39
 *   IP              V1
 *   MAC             V2
 *   currentTime     V5
 *   currentDate     V7
 *   temperature     V10
 *   humidity        V11
 *   pressure        V12
 *   temperatureMax  V14 reset by button widget
 *   currentTime     V15
 *   VCC             3.3V
 *   GND             GND
 *   SCL             D1        
 *   SDA             D2
 *   
 */

int testOTA = 393939;

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>             // is this needed?
#include <WiFiManager.h>           
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <TimeLib.h>                 // rtc
#include <WidgetRTC.h>               // rtc
#include <ArduinoOTA.h>              // OTA
#include <Wire.h>
#include <Adafruit_Sensor.h>         // BME280
#include <Adafruit_BME280.h>         // BME280
#define SEALEVELPRESSURE_HPA (1013.25)

// start OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET -1  // -1 for ESP8266
Adafruit_SSD1306 display(OLED_RESET);
 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

char auth[] = "nnn";  // insert your Blynk device auth code

const char* ssid;  // auto connect - enter these on first boot up 
const char* pass;  // auto connect

BlynkTimer timer;
IPAddress strWiFiIp;
WidgetRTC rtc;                       // define rtc widget

Adafruit_BME280 bme;                 // BME I2C
float temperature;
float humidity;
float pressure;
float temperature_old;
float humidity_old;
float pressure_old;
float temperatureMax = 0;
char currentTime[9];                 // rtc time
char currentDate[11];                // rtc date
int alarmThreshold = 100;


void setup() {
  Serial.begin(9600); 
// OLED Setup---------------------------------------------------------------------------
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  //display.display();                        // Adafruit spash screen in .cpp buffer
  display.clearDisplay();
  // text display on Setup
  display.setRotation(0); 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("       BME280");
  display.println("  you@youremail.com");
  display.display();                          // this is required to write the previous lines to the display
  delay(2000);
  //display.clearDisplay();
       
  WiFiManager wifiManager;
  //wifiManager.resetSettings();                  // This will clear known APs after each reset
  wifiManager.autoConnect("AutoConnectAP");
  ssid = WiFi.SSID().c_str();
  pass = WiFi.psk().c_str();
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V39, testOTA);               // write testOTA to Blynk
  
  display.println("SSID");
  display.print(WiFi.SSID());
  display.display();
  delay(2000);
  
  bool status;           //BME
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  Blynk.run();
  wifiIP();
  wifiMAC();
  printValuesBlynk();

  timer.setInterval(10000L, wifiIP);                       // check IP address every 10 seconds, print to serial monitor and Blynk
  timer.setInterval(10000L, wifiMAC);                      // check MAC address every 10 seconds, print to serial monitor and Blynk
  timer.setInterval(3000L, wifiSSID);                      // check SSID every 3 seconds, print to serial monitor
  timer.setInterval(3000L, printValuesBlynk);              // check BME rate every 3 sec
  timer.setInterval(5000L, printValuesOLED);                // send data to TFT
  
  ArduinoOTA.setHostname("BME280_Mobile_OLED_v2");
  ArduinoOTA.begin();                                      // OTA last line of setup()
}


void loop() {
  ArduinoOTA.handle();                                     // OTA first line of loop()
  Blynk.run();
  timer.run();
}


BLYNK_CONNECTED() {
  rtc.begin();                        // Synchronize time on connection
  Blynk.syncAll();                    // Synchronise all widget values in the app to the server
}


// Get IP address: V1
void wifiIP() {                                           // gets local IP address, prints to serial monitor
  IPAddress strWiFiIp = WiFi.localIP();
  Serial.print("ESP8266 IP:   ");
  Serial.print(strWiFiIp); 
  Serial.println("");             
  Blynk.virtualWrite(V1, WiFi.localIP().toString());      // write local IP to Blynk 
}


// Get MAC: V2
void wifiMAC() {                                          // write IP address to Blynk and serial monitor
  Serial.print("ESP8266 MAC:  ");
  Serial.print(WiFi.macAddress());
  Serial.println("");
  Blynk.virtualWrite(V2, WiFi.macAddress());
}

// Get SSID
void wifiSSID() {
  Serial.print("ESP8266 SSID:  ");
  Serial.println(WiFi.SSID());
  Serial.println("");
  Blynk.virtualWrite(V7, WiFi.SSID());
}


void printValuesBlynk() {
    sprintf(currentTime, "%02d:%02d:%02d", hour(), minute(), second());  // set time of new tempMax with leading zero
    sprintf(currentDate, "%02d/%02d/%04d", day(), month(), year());      // rtc set date of door open leading zero
    Serial.println(currentTime);
    Serial.println(currentDate);
    Serial.println("");
    
    temperature = bme.readTemperature();
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print("alarmThreshold = ");
    Serial.println(alarmThreshold);
    Blynk.virtualWrite(V13, alarmThreshold);
    Blynk.virtualWrite(V10, temperature);    // send temp to blynk
    if (temperature > temperatureMax) {
      temperatureMax = temperature;
      Blynk.virtualWrite(V14, temperatureMax);
      Blynk.virtualWrite(V15, currentTime);
      //Blynk.virtualWrite(V7, currentDate);
    }

    if (temperature > alarmThreshold){
      Blynk.notify("Mobile BME Over Temperature!!!");                    // Blynk notification
      Blynk.email("billd01@gmail.com", "Mobile BME Over Temperature!!!", "Mobile BME Over Temperature!!!");
    }  
    
    Serial.print("TemperatureMax = ");
    Serial.print(temperatureMax);
    Serial.println(" *C");
        
    humidity = bme.readHumidity();
    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    Blynk.virtualWrite(V11, humidity);       // send humidity to blynk
    
    pressure = (bme.readPressure() / 100.0F);
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" hPa");
    Serial.println("");
    Blynk.virtualWrite(V12, pressure);    // send pressure to blynk

    delay(100);
}

void printValuesOLED() {
 // read the data
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  
  // Clear the buffer.
  display.clearDisplay();
 
  // text display tests
  display.setRotation(0); 
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("T ");
  display.print(temperature);
  display.print(" ");
  display.print((char)247);
  display.println("C");
  //display.println();
  display.print("H ");
  display.print(humidity);
  display.print(" ");
  display.println("%H");
  
  display.display();
  delay(2000);
  display.clearDisplay();
 }


BLYNK_WRITE(V16){                      // reset temperatureMax with button widget in PUSH mode on virtual pin 16
  int resetdata = param.asInt();
  if(resetdata == 1){
    temperatureMax = 0;
    Blynk.virtualWrite(V14, temperatureMax);
    Blynk.virtualWrite(V15, currentTime);
    //Blynk.virtualWrite(V7, currentDate);
  }
}

BLYNK_WRITE(V9){                   // numerical value input widget
 alarmThreshold = param.asInt();
 Blynk.virtualWrite(V13, alarmThreshold);
}
