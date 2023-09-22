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
 Hardwareinfo : Taster an D2(GPIO4) über10k gegen Masse, LED an D1(GPIO5) gegen Masse, DS18B20 an D5(GPIO14)
              : 
 Status       : OK
 Version      : V1.0.2 (20230917)
 Einsatz      : 
              : 
 Hinweis      : Board in PlatformIO: d1_mini_pro
              : 
              : 
 ToDo         :

History 20230815 begin
  V1.0.0
  LED und Taster funktionieren aber Upload nur über USB 
  V1.0.1  
  DS18B20 eingebunden und mit millims definiert
  V1.0.2
  einbinden OTA Function

*/


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define wifi_ssid "klncqwoez283431pidd1ic9r32f8931"
#define wifi_password "h785fh6108b01"
// #define mqtt_server "192.168.178.65"
// #define mqtt_user "mqtt_olaf"
// #define mqtt_password "Kastor1956"
// #define EspHostname "Wemos1"
// String clientID = "Wemos-1";
// #define WemosTaster "wemos/taster"
// #define WemosLED "wemos/led"
// #define WemosName "wemos/name"
// #define WemosStatus "wemos/status"
 
// WiFiClient espClient;
// PubSubClient client(espClient);

const int LED =         5; // D1 = GPIO5
const int Taster =      4; // D2 = GPIO4
int Taster_Status =     0;

const int oneWireBus =  14; // D5 = GPIO14
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor_address;
unsigned long previousMillis = 0;					// vorheriger Milliwert
const long interval = 5000;							// welche Zeit

// meine Function hier declarations:
void Setup_wifi();
void Sensor_auslesen();
// void callback(char* topic, byte* message, unsigned int length);
// void reconnect();
// void printAddress(DeviceAddress deviceAddress);


void setup() {
  Serial.begin(115200);
  sensors.begin();
  // sensors.getAddress(sensor_address, 0);
  Serial.print("Sensor Adresse: ");
  // printAddress(sensor_address);
  Serial.println();
  Setup_wifi();
  // client.setServer(mqtt_server,1883);
  // client.setCallback(callback); 
  // ArduinoOTA.setHostname(EspHostname);
  // ArduinoOTA.begin();
  pinMode(LED, OUTPUT);
  pinMode(Taster, INPUT);
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite (LED, LOW);
  Serial.println("Setup beendet . . .");
 }

void loop() {
    if (digitalRead(Taster) == HIGH) {
         digitalWrite(LED, HIGH);
       } else {
         digitalWrite(LED, LOW);
     } 
  unsigned long currentMillis = millis();			// aktuelle Millis an currentMillis(Variable übergeben)
  if (currentMillis - previousMillis >= interval) { // aktuellen Millis prüfen, ob schon mit vorherigen Wert abgelaufen ist 
    previousMillis = currentMillis; 				// vorheriger Wert an currentMillis übergeben 
    Sensor_auslesen();  // hier die Function die aufgerufen werden soll
  }
}

void Sensor_auslesen() {
    sensors.requestTemperatures(); 
    float temperatureC = sensors.getTempCByIndex(0);   
    Serial.print(temperatureC);
    Serial.println("ºC"); 
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);

}

/*
void printAddress(DeviceAddress deviceAddress){
for (uint8_t i = 0; i < 8; i++) {
    Serial.print("0x");
if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}   */

void Setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Verbunden mit: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi verbunden mit IP: ");
  // Serial.print("IP Adresse: ");
  Serial.println(WiFi.localIP());
}
/*
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
}   */