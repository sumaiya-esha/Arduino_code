#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"


#include <OneWire.h>
#include <DallasTemperature.h>

#define REPORTING_PERIOD_MS     1000

// Data wire is connected to GPIO 2,(D4)
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature tempSensor(&oneWire);




PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected()
{
    Serial.println("Beat");
}



WiFiClient client;

long myChannelNumber=1426454;
const char myWriteAPIKey[]="WI6H9W83D4NDKAAO";  //write data to thingspeak



void setup() {
   Serial.begin(9600);





   
   WiFi.begin("PRADIP","winteriscoming@westeros");
   while(WiFi.status() !=WL_CONNECTED)
   {
    Serial.print("..");
    delay(200);
    }
    Serial.println();
    Serial.println("Connected.");
    //Serial.println(WiFi.localIP()); //print the ip of connected wifi.
    
    
    tempSensor.begin();    // Start up the DS18B20 library

     // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } 
    else {
        Serial.println("SUCCESS");
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);

    ThingSpeak.begin(client);

}


void loop() {
     
      tempSensor.requestTemperatures();
      int tempC = tempSensor.getTempCByIndex(0);
 
        if (tempC == -127.00) {
                  Serial.println("Unable to read from sensor");
        } 
         else {
                 Serial.print("Temp. in Celsius: ");
                 Serial.println(tempC);
         }


         pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        int heartrate = pox.getHeartRate();
        int spo2 = pox.getSpO2();
        
        Serial.print(heartrate);
        Serial.print("bpm / SpO2:");
        Serial.print(spo2);
        Serial.println("%");}
        delay(15000);
}
 
    





      ThingSpeak.writeField(myChannelNumber, 1, tempC, myWriteAPIKey);  // values will be sent to sp. channel,field under this API.
      ThingSpeak.writeField(myChannelNumber, 2, heartrate, myWriteAPIKey);
      ThingSpeak.writeField(myChannelNumber, 3, spo2, myWriteAPIKey);
      delay(15000); // update data evey 15 sec.
}
