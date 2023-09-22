/*
 Titel        : Wemos-Test
 Info         : Test 
 Beschreibung : Wemos mit dem MQTT  verbinden und über HA abrufen
              : 
 Arduino      : D1 Mini ESP8266-12F v3
 Modul ID     : 
 Projekt-Nr.  : 
 Datum        : 
 Schaltung in : 
 Hardwareinfo : Taster an D1 über10k gegen Masse, LED an D2 gegen Masse
              : 
 Status       : iA
 Version      : V1.0 (20230915)
 Einsatz      : 
              : 
 Hinweis      : d1_mini
              : 
              : 
 ToDo         :

*/
/* History 20230815 begin

  V1.0.1 

*/


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define wifi_ssid "klncqwoez283431pidd1ic9r32f8931"
#define wifi_password "h785fh6108b01"
#define mqtt_server "192.168.178.65"
#define mqtt_user "mqtt_olaf"
#define mqtt_password "Kastor1956"
#define EspHostname "Wemos1"
String clientID = "Wemos-1";
#define WemosTaster "wemos/taster"
#define WemosLED "wemos/led"
#define WemosName "wemos/name"
#define WemosStatus "wemos/status"
 
WiFiClient espClient;
PubSubClient client(espClient);

const int LED =         4; // D2 = GPIO4
const int Taster =      5; // D1 = GPIO5
int Taster_Status =     0;
const int oneWireBus =  14; // D5 = GPIO14
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor_address;


// put function declarations here:
void Setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void printAddress(DeviceAddress deviceAddress);


void setup() {
  Serial.begin(115200);
  sensors.begin();
  // sensors.getAddress(sensor_address, 0);
  Serial.print("Sensor Adresse: ");
  // printAddress(sensor_address);
  Serial.println();
  Setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback); 
  ArduinoOTA.setHostname(EspHostname);
  ArduinoOTA.begin();
  pinMode(LED,OUTPUT);
  pinMode(Taster, INPUT);
 }

void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();

  if (digitalRead(Taster) == HIGH) {
    if (LED == LOW) {
      client.publish(WemosTaster, "ON");
      client.publish(WemosLED, "HIGH");
      digitalWrite(LED, HIGH);
      Serial.print("LED ist: ");
      Serial.println(LED);
    } 
  }
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);   
  Serial.print(temperatureC);
  Serial.println("ºC");
  delay(5000);
}


void printAddress(DeviceAddress deviceAddress){
for (uint8_t i = 0; i < 8; i++) {
    Serial.print("0x");
if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}

void Setup_wifi() {
  delay(10);
// We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Verbunden mit: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi verbunden");
  Serial.println("IP Adresse: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
for (unsigned i = 0; i <= length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
if (String(topic) == WemosLED) {
if(messageTemp == "ON"){
      Serial.print("Wechselt LED zu an\n");
      digitalWrite(LED, HIGH);   //Invertiertes Signal
      client.publish(WemosLED, "ON");
      delay(200);
    }
else if(messageTemp == "OFF"){
      Serial.print("Changing LED to OFF\n");
      digitalWrite(LED, LOW);  //Invertiertes Signal
      client.publish(WemosLED, "OFF"); 
      delay(200);
    }
  }
}
void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
    Serial.print("Verbinde mit MQTT Server...");
// Create a random client ID
    clientID += String(random(0xffff), HEX);
// Attempt to connect
if (client.connect(clientID.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("verbunden");
// Once connected, publish an announcement...
      client.publish(WemosName, EspHostname);
// ... and resubscribe
      client.subscribe(WemosLED);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying
        delay(5000);
      }
   }
}