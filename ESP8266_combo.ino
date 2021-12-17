// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

String str;
// WiFi network info.
char ssid[] = "Hotspot";
char wifiPassword[] = "123456789";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "6fc57070-0654-11e8-b797-8356759ccb0f";
char password[] = "406f456b6f50d36f36882e2543cb69f30c6cc62b";
char clientID[] = "500a5780-069c-11e8-a2eb-7d0b45aada7b";

int tar_temp,analogValue,celsius;
float millivolts;
//channel 0 -> Simple counter for checking 
//channel 1 -> Room Temp
//channel 2 -> Target temperature from dashboard slider 
//channel 3 -> Gas Leakage : Emergency Scene ON 


void setup() {
	Serial.begin(9600);
 Serial1.begin(115200);
 pinMode(5,INPUT);
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  tar_temp=26;
}

void loop() {
 // int l;
	Cayenne.loop();
// if(millis()-l >20)
// {
//  l=millis();
// Cayenne.virtualWrite(0, millis());
//}
if(digitalRead(5)==HIGH)
{
  Serial.println("GAS LEAKAGE DETECTED");
  Cayenne.virtualWrite(3,1);
}
else
{
Cayenne.virtualWrite(3,0);
}
analogValue = analogRead(A0);
millivolts = (analogValue/1024.0) * 3300; 
celsius = millivolts/10;
//celsius = celsius - 2 ;
Serial.print("in DegreeC=   ");
Serial.println(celsius);
Cayenne.celsiusWrite(1, celsius);
Serial.println(tar_temp);

  // str =String(tar_temp);
  //Serial1.println(str);
}

// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
CAYENNE_OUT_DEFAULT()
{
	// Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
	Cayenne.virtualWrite(0, millis());
	// Some examples of other functions you can use to send data.
	//Cayenne.celsiusWrite(1, 22.0);
	//Cayenne.luxWrite(2, 700);
	//Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
	CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
	//Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}

CAYENNE_IN(2)
{
  CAYENNE_LOG("Got a value: %s", getValue.asStr());
  // You can also use:
    tar_temp = getValue.asInt();
  // double d = getValue.asDouble()
}


