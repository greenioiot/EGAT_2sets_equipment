#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <ADS1X15.h>
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
//#include "BluetoothSerial.h"
//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial
//BluetoothSerial SerialBT;

void t1Callback();
void t2Callback();


/**********************************************  WIFI Client 注意编译时要设置此值 *********************************
   wifi client
*/

const char* ssid     = "greenio";
const char* password = "green7650";


//WiFi&OTA 参数
String HOSTNAME = "EGAT_VoltMeter*4+CTsensor*4: ";
#define PASSWORD "green7650" //the password for OTA upgrade, can set it in any char you want

#define WIFI_AP ""
#define WIFI_PASSWORD ""

String deviceToken = ""; //TV4em3w1NA21n6AyOWkO
char thingsboardServer[] = "mqtt.thingcontrol.io";

String json = "";
unsigned long previousMillis = 0;
unsigned long interval = 30000;
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long startTeleMillis;
unsigned long starSendTeletMillis;
unsigned long currentMillis;
const unsigned long periodSendTelemetry = 1;  //the value is a number of milliseconds

unsigned long Closing_time = 0, Opening_time = 0, Charging_time = 0, Time = 0, Time2 = 0, Time3 = 0, Time4 = 0, ms_Task1, ms_Task2, ms_Task3;
int passValue = 1, address1 = 0, i = 0, i2 = 0, i3 = 0, i4 = 0, I_max = 0, Mechanism, Interrupter, Counter = 0, ms, T;

int  S = 0, S2 = 0, S3 = 0;                               //FOR TEST
unsigned long Time_S = 0, Time_S2 = 0, Time_S3 = 0, Time_random1 = 0, Time_random2 = 0;       //FOR TEST

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
int PORT = 8883;

ADS1015 adsV(0x48);     /* Use this for the 12-bit version */
ADS1015 adsA(0x49);     /* Use this for the 12-bit version */
int16_t adc0, adc1, adc2, adc3;
int16_t adc4, adc5, adc6, adc7;
Scheduler runner;

Task t1(1, TASK_FOREVER, &t1Callback);
Task t2(1, TASK_FOREVER, &t2Callback);

uint64_t espChipID = ESP.getEfuseMac();

String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}




void setupOTA()
{

  ArduinoOTA.setHostname(HOSTNAME.c_str());
  ArduinoOTA.setPassword(PASSWORD);
  ArduinoOTA.onStart([]()
  {
    Serial.println("Start Updating....");
    Serial.printf("Start Updating....Type:%s\n", (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem");

  });

  ArduinoOTA.onEnd([]()
  {
    Serial.println("Update Complete!");
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    String pro = String(progress / (total / 100)) + "%";
    int progressbar = (progress / (total / 100));
    Serial.print("Progress : ");
    Serial.println((progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    String info = "Error Info:";
    switch (error)
    {
      case OTA_AUTH_ERROR:
        info += "Auth Failed";
        Serial.println("Auth Failed");
        break;

      case OTA_BEGIN_ERROR:
        info += "Begin Failed";
        Serial.println("Begin Failed");
        break;

      case OTA_CONNECT_ERROR:
        info += "Connect Failed";
        Serial.println("Connect Failed");
        break;

      case OTA_RECEIVE_ERROR:
        info += "Receive Failed";
        Serial.println("Receive Failed");
        break;

      case OTA_END_ERROR:
        info += "End Failed";
        Serial.println("End Failed");
        break;
    }
    Serial.println(info);
    ESP.restart();
  });
  ArduinoOTA.begin();
}



//---------------------------------------------------------------------------------------------------------------------------------

void t1Callback() {

  C_time();

  if (millis() - ms_Task1 > 1000) {
    Serial.print("V1:" + String(adc0) + "    " + "V2:" + String(adc1) + "    " + "V3:" + String(adc2) + "    " + "V4:" + String(adc3) + "    " + "Charg_T:" + String(Charging_time) + "ms." + "    " + "I_max:" + String(I_max)  + "    ");
    Serial.print("ClosE_T:" + String(Closing_time) + "    " + "Open_time:" + String(Opening_time) + "    " + "Mechanism:" + String(Mechanism) + "    " + "Interrupter:" + String(Interrupter) + "    " + "Coun:" + String(Counter) + "    ");
    Serial.println("Ia;" + String(adc5) + "    " + "Ib;" + String(adc6) + "    " + "Ic;" + String(adc7));
    Serial.print("adc0 : "); Serial.print(adc0); Serial.print("   "); Serial.print("adc1 : "); Serial.print(adc1); Serial.print("   "); Serial.print("adc2 : "); Serial.print(adc2); Serial.print("   "); Serial.print("adc3 : "); Serial.print(adc3); Serial.print("   "); Serial.print("adc4 : "); Serial.print(adc4); Serial.print("   ");
    Serial.print("adc5 : "); Serial.print(adc5); Serial.print("   "); Serial.print("adc6 : "); Serial.print(adc6); Serial.print("   "); Serial.print("adc7 : "); Serial.print(adc7); Serial.println("   ");
    ms_Task1 = millis();
  }
}

void t2Callback() {
  adc0 = adsV.readADC(0);
  adc1 = adsV.readADC(1);
  adc2 = adsV.readADC(2);
  adc3 = adsV.readADC(3);
  adc4 = adsA.readADC(0);
  adc5 = adsA.readADC(1);
  adc6 = adsA.readADC(2);
  adc7 = adsA.readADC(3);

  O_time();
  I_time();
  State_Check ();


  if ( status == WL_CONNECTED)
  {
    if ( !client.connected() )
    {
      reconnectMqtt();
    }
  }

  if (Serial.available() > 0) {
    char key = Serial.read();
    Serial.println(key);
    if (key == '0')  {
      Counter = 0 ;
      Mem_C ();
    }
  }
  if ( millis() - ms_Task3 > 60000) {
    sendState();
    ms_Task3 = millis();
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------    Test Functions


//void t1Callback() {
//  C_time();
//  if (millis() - ms_Task1 > 1000) {
//    Serial.print("V1:" + String(adc0) + "    " + "V2:" + String(adc1) + "    " + "V3:" + String(adc2) + "    " + "V4:" + String(adc3) + "    " + "Charg_T:" + String(Charging_time) + "ms." + "    " + "I_max:" + String(I_max)  + "    ");
//    Serial.print("ClosE_T:" + String(Closing_time) + "    " + "Open_time:" + String(Opening_time) + "    " + "Mechanism:" + String(Mechanism) + "    " + "Interrupter:" + String(Interrupter) + "    " + "Coun:" + String(Counter) + "    ");
//    Serial.println("Ia;" + String(adc5) + "    " + "Ib;" + String(adc6) + "    " + "Ic;" + String(adc7));
//    ms_Task1 = millis();
//  }
//}
//
//void t2Callback() {
//  sim_Close_open();
//  sim_I_motor();
//  sim_Ia_Ib_Ic() ;
//  O_time();
//  State_Check ();
//  I_time() ;
//
//  if (ms_Task2 % 60000 == 0) {
//    status = WiFi.status();
//    if ( status == WL_CONNECTED)
//    {
//      if ( !client.connected() )
//      {
//        reconnectMqtt();
//      }
//    }
//  }
//
//  if (Serial.available() > 0) {
//    char key = Serial.read();
//    Serial.println(key);
//    if (key == '0')  {    // Reset Counter
//      Counter = 0 ;
//      Mem_C ();
//    }
//    if (key == '1')  {   // sim_Close_open();
//      S = 0 ;
//    }
//    if (key == '2')  {  // sim_I_motor();
//      S2 = 0 ;
//    }
//    if (key == '3')  {  // sim_Ia_Ib_Ic() *** Toggle ;
//      if (S3 == 0) {
//        S3 = 1;
//      }
//      else if (S3 == 1) {
//        S3 = 0;
//      }
//    }
//  }
//
//  if ( millis() - ms_Task2 > 60000) {
//    sendState();
//    ms_Task2 = millis();
//  }
//}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void setup() {
  Serial.begin(115200); // Open serial connection to report values to host
  Serial.println(F("Starting... Ambient Temperature/Humidity Monitor"));
  Serial.println();
  Serial.println(F("***********************************"));

  EEPROM.begin(1000);
  Counter = EEPROM.readUInt(Counter);

  deviceToken = mac2String((byte*) &espChipID);
  HOSTNAME.concat(deviceToken);

  Serial.print("MAC address : ");
  Serial.println(deviceToken);

  WiFi.begin(ssid, password);
  reconnectWIFI();

  client.setServer( thingsboardServer, PORT );
  reconnectMqtt();

  Serial.print("Start.....");
  startMillis = millis();  //initial start time




  setupOTA();
  //SerialBT.begin(hex);

  Serial.println("Initialize...OTA");

  Serial.println("Initialize...ADS");
  adsV.begin();

  adsV.setDataRate(7);
  adsV.setWireClock(400000);
  adsA.begin();
  adsA.setDataRate(7);
  adsA.setWireClock(400000);



  runner.init();

  runner.addTask(t1);
  runner.addTask(t2);
  ms_Task1 =  millis();
  ms_Task2 =  millis();
  t1.enable();
  Serial.println("Enabled Task1");
  t2.enable();
  Serial.println("Enabled Task2");

}

void loop()
{
  status = WiFi.status();
  if (millis() - Time4 >= 3000)  //test whether the period has elapsed
  {
    setupOTA();
    reconnectWIFI();
    Time4 = millis();  //IMPORTANT to save the start time of the current LED state.
  }
  ArduinoOTA.handle();
  runner.execute();
  client.loop();
}

void reconnectMqtt()
{
  if ( client.connect("EGAT.SubStation", deviceToken.c_str(), deviceToken.c_str()) )
  {
    Serial.println( F("Connect MQTT Success."));
    client.subscribe("v1/devices/me/rpc/request/+");
  } else {
    Serial.println( F("Connect MQTT fail."));
  }
}

void processTele(char jsonTele[])
{
  char *aString = jsonTele;
  Serial.println("OK");
  Serial.print(F("+:topic v1/devices/me/ , "));
  Serial.println(aString);
  client.publish( "v1/devices/me/telemetry", aString);
}


void Mem_C () {
  EEPROM.writeUInt(address1, Counter);
  EEPROM.commit();
}



//---------------------------------------------------------------------------------------------------------------Detec from ADC 0-7 functions--------------------------------------------


void C_time() {
  if ((adc0 > 200) && (adc1 > 50) && (i != 1)) {
    Time = millis();
    i = 1;
  }
  if ((adc0 > 200) && (adc1 <= 0) && (i == 1)) {
    Closing_time = millis() - Time;
    sendClosing_time();
    i = 0;
    Serial.print("i:"); Serial.println(i);
  }
  if (adc1 > 200 && i4 == 0) {
    i4 = 1;
  }
  else if (adc1 <= 0 && i4 == 1 ) {
    Counter++;
    Mem_C ();
    sendCounter();
    i4 = 0;
  }
}

void O_time() {
  if (adc2 > 200 && adc3 > 0 && i2 != 1) {
    Time2 = millis();
    i2 = 1;
  }
  if (adc2 > 200 && adc3 <= 0 &&  i2 == 1) {
    Opening_time = millis() - Time2;
    sendOpening_time();
    i2 = 0;
  }
}

void I_time () {
  if (adc4 > 144 && adc4 > I_max) {
    I_max = adc4;
  }
  if (adc4 > 144 && i3 == 0) {
    Time3 = millis();
    i3 = 1;
  }
  if (adc4 <= 144 && i3 == 1) {
    Charging_time = millis() - Time3;
    sendCharging_time_I_max();
    i3 = 0;
    I_max = 0;
  }
}

void State_Check () {
  if (adc1 == 0) {
    Mechanism = 0 ;
  } else if (adc3 == 0) {
    Mechanism = 1 ;
  }
  if (adc5 > 50 && adc6 > 50 && adc7 > 50) {
    Interrupter = 0 ;
  } else if (adc5 <= 0 && adc6 <= 0 && adc7 <= 0) {
    Interrupter = 1 ;
  }
}

//---------------------------------------------------------------------------------------------------------------Sent telemetry functions--------------------------------------------



void sendOpening_time() {  // OT =  Opening_time
  String json = "";
  json.concat("{\"OT\":");
  json.concat(Opening_time);
  json.concat("}");
  Serial.println(json);
  client.publish( "v1/devices/me/telemetry",  json.c_str());
}

void sendClosing_time() {         // CT =  Closing_time
  String json = "";
  json.concat("{\"CT\":");
  json.concat(Closing_time);
  json.concat("}");
  Serial.println(json);
  client.publish( "v1/devices/me/telemetry",  json.c_str());
}

void sendCharging_time_I_max() {   //  ChT =  Charging_time  , IM = I_max ของ มอเตอร์
  String json = "";
  json.concat("{\"ChT\":");
  json.concat(Charging_time);
  json.concat(",\"IM\":");
  json.concat(I_max);
  json.concat("}");
  Serial.println(json);
  client.publish( "v1/devices/me/telemetry",  json.c_str());
}

void sendState() {  // M = Mechanism   ,  I = Interrupter
  String json = "";
  json.concat("{\"M\":");
  json.concat(Mechanism);
  json.concat(",\"I\":");
  json.concat(Interrupter);
  json.concat(",\"V1\":");
  json.concat(adc0);
  json.concat(",\"V2\":");
  json.concat(adc1);
  json.concat(",\"V3\":");
  json.concat(adc2);
  json.concat(",\"V4\":");
  json.concat(adc3);
  json.concat(",\"Imotor\":");
  json.concat(adc4);
  json.concat(",\"Ia\":");
  json.concat(adc5);
  json.concat(",\"Ib\":");
  json.concat(adc6);
  json.concat(",\"Ic\":");
  json.concat(adc7);
  json.concat("}");
  Serial.println(json);
  client.publish( "v1/devices/me/telemetry",  json.c_str());
}

void sendCounter() {   // C = Counter
  String json = "";
  json.concat("{\"C\":");
  json.concat(Counter);
  json.concat("}");
  Serial.println(json);
  client.publish( "v1/devices/me/telemetry",  json.c_str());

}

//-----------------------------------------------------------------------------SIMULATIONS CODE---------------------------------------------------------------------------------------
//
//void sim_Close_open() {
//  if (S == 0 ) {
//    ;
//    adc0 = 0 ;
//    adc1 = 11 ;
//    adc2 = 0 ;
//    adc3 = 11 ;
//    S = 1 ;
//  }
//  if (S == 1 ) {
//    adc0 = 11 ;
//    adc1 = 11 ;
//    adc2 = 11 ;
//    adc3 = 11 ;
//    S = 2 ;
//    Time_random1 = random(10, 40);
//    Time_S = millis();
//  }
//  if (S == 2 && (millis() - Time_S > Time_random1)) {
//    adc0 = 11 ;
//    adc1 = 0 ;
//    adc2 = 11 ;
//    adc3 = 0 ;
//    Time_S = millis();
//    S = 3;
//  }
//}
//
//void sim_I_motor() {
//  if (S2 == 0 ) {
//    Time_S2 = millis();
//    Time_S3 = millis();
//    Time_random2 = random(8000, 15000);
//    S2 = 1 ;
//  }
//  if (S2 == 1  && (millis() - Time_S2 < Time_random2)) {
//    if (millis() - Time_S3 > 500) {
//      adc4 = random(11, 254);
//      Time_S3 = millis();
//    }
//  }
//  if (S2 == 1 && (millis() - Time_S2 > Time_random2)) {
//    adc4 = 0;
//    Time_S2 = millis();
//    S2 = 3;
//  }
//}
//
//void sim_Ia_Ib_Ic() {
//  if (S3 == 0 ) {
//    adc5 = 0;
//    adc6 = 0;
//    adc7 = 0;
//  }
//  if (S3 == 15 ) {
//    adc5 = 15;
//    adc6 = 15;
//    adc7 = 15;
//  }
//}


void reconnectWIFI()
{
  while (WiFi.status() != WL_CONNECTED) {
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis >= interval)) {
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      previousMillis = currentMillis;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      setupOTA();
    }
  }
}
