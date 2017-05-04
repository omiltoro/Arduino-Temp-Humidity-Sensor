#include <dht.h>
dht DHT;
#define DHT11_PIN 12
#include <Event.h>
#include <Timer.h>


#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"
Timer ti;
//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to start a connection as client.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;

char msg[50];
int numdata;
char inSerial[50];
int i=0;

boolean started=false;
static char postUrl[150];
char *api_key="ee13504e4b14163cb32331dfd013f3af";

void setup()
{
     //Serial connection.
     Serial.begin(9600);
     Serial.println("GSM Shield testing.");
     //Start configuration of shield with baudrate.
     //For http uses is raccomanded to use 4800 or slower.
     if (gsm.begin(2400)) {
          Serial.println("\nstatus=READY");
          started=true;
     } else Serial.println("\nstatus=IDLE");

     if(started) {
          //GPRS attach, put in order APN, username and password.
          //If no needed auth let them blank.
          if (inet.attachGPRS("internet.wind", "", ""))
               Serial.println("status=ATTACHED");
          else Serial.println("status=ERROR");
          delay(1000);

          //Read IP address.
          gsm.SimpleWriteln("AT+CIFSR");
          delay(5000);
          //Read until serial buffer is empty.
          gsm.WhileSimpleRead();

          //TCP Client GET, send a GET request to the server and
          //save the reply.
          
          
     }
};

void loop()
{
  
    //Read for new byte on serial hardware,
     //and write them on NewSoftSerial.
     serialhwread();
    
     //Read for new byte on NewSoftSerial.
     serialswread();

 int chk = DHT.read11(DHT11_PIN);

                  float h = DHT.humidity;
                  float t = DHT.temperature;
                  char tempStr[15];
                  char humidStr[15];
                  dtostrf(t, 4, 2, tempStr);
                  dtostrf(h, 4, 2, humidStr);
          sprintf(postUrl, "/api/data/push?id=4rps21&t=%s&h=%s&api_key=%s",tempStr,humidStr,api_key);
          numdata=inet.httpGET("41.215.34.154", 80,postUrl, msg, 50);
delay(2000);

 
          //Print the results.
          Serial.println("\nNumber of data received:");
          Serial.println(numdata);
          delay(5000);
          Serial.println("\nData received:");
          Serial.println(msg);
          delay(15000);
          
};


void serialhwread()
{
     i=0;
     if (Serial.available() > 0) {
          while (Serial.available() > 0) {
               inSerial[i]=(Serial.read());
               delay(10);
               i++;
          }

          inSerial[i]='\0';
          if(!strcmp(inSerial,"/END")) {
               Serial.println("_");
               inSerial[0]=0x1a;
               inSerial[1]='\0';
               gsm.SimpleWriteln(inSerial);
          }
          //Send a saved AT command using serial port.
          if(!strcmp(inSerial,"TEST")) {
               Serial.println("SIGNAL QUALITY");
               gsm.SimpleWriteln("AT+CSQ");
          }
          //Read last message saved.
          if(!strcmp(inSerial,"MSG")) {
               Serial.println(msg);
          } else {
               Serial.println(inSerial);
               gsm.SimpleWriteln(inSerial);
          }
          inSerial[0]='\0';
     }
}

void serialswread()
{
     gsm.SimpleRead();
}
