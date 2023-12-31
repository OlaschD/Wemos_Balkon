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
 Version      : V1.0.3 (20230917)
 Einsatz      : 
              : 
 Hinweis      : Board in PlatformIO: d1_mini_pro
              : 
              : 
 ToDo         : BME280 und Code aufräumen

History 20230815 begin
  V1.0.0
  LED und Taster funktionieren aber Upload nur über USB 
  V1.0.1  
  DS18B20 eingebunden und mit millims definiert
  V1.0.2
  einbinden OTA Function 
  V1.0.3
  MQTT einrichten Funktioniert
*/


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include<PubSubClient.h>

#define wifi_ssid "klncqwoez283431pidd1ic9r32f8931"
#define wifi_password "h785fh6108b01"
#define mqtt_server "192.168.178.65"
#define mqtt_user "mqtt_olaf"
#define mqtt_password "Kastor1956"
#define EspHostname "Wemos1"
String clientID = "Wemos-1";
const char *WemosDSTemperatur = "wemos/temperatur";
const char *WemosName = "wemos/name";
#define WemosStatus "wemos/status"
 
float DS_Temperatur;
String WTemp;
int timeout;
char ds18B20_Cstring[6];
WiFiClient espClient;
PubSubClient client(espClient);

const int LED =         13;  // D7 = SPI MOSI = GPIO13
const int Taster =      15;  // D8 = SPI CS = GPIO15 


// RX = GPIO3 = RXD0
// TX = GPIO1 = TXD0
// D1 = GPIO5 = SCL
// D2 = GPIO4 = SDA

int Taster_Status =     0;



const int oneWireBus = 12; // D6 = GPIO12 = SPI SCLK
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
// float WTemp;
DeviceAddress sensor_address;
unsigned long previousMillis = 0;					// vorheriger Milliwert
const long interval = 25000;							// welche Zeit

// meine Function hier declarations:
void Setup_wifi();
// void Sensor_auslesen();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void printAddress(DeviceAddress deviceAddress);


float getTemperatur() {
  float DStemp;
  // Serial.println("Messe Temperatur ...");
  timeout=30;
 do {
   sensors.requestTemperatures();
   DStemp = sensors.getTempCByIndex(0);
   delay(100);
   timeout--;
   if (timeout<0) DStemp=99.9;  									// Wenn Sensor defekt
 } while (DStemp == 85.0 || DStemp == -127.0) ;
     WTemp = String(ds18B20_Cstring);                                           // WTemp = DS_Temperatur;   //dtostrf(temperatureC, 5,2,DS_Temperatur);
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
 return DStemp;
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  // sensors.getAddress(sensor_address, 0);
  Serial.print("Sensor Adresse: ");
  printAddress(sensor_address);
  Serial.println();
  Setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback); 
  ArduinoOTA.setHostname("Wemos1");
  ArduinoOTA.begin();
  pinMode(LED, OUTPUT);
  pinMode(Taster, INPUT);
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite (LED, LOW);
  // Serial.println("Setup beendet . . .");
 }

void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
    if (digitalRead(Taster) == HIGH) {
         digitalWrite(LED, HIGH);
       } else {
         digitalWrite(LED, LOW);
     } 
  unsigned long currentMillis = millis();			// aktuelle Millis an currentMillis(Variable übergeben)
  if (currentMillis - previousMillis >= interval) { // aktuellen Millis prüfen, ob schon mit vorherigen Wert abgelaufen ist 
    previousMillis = currentMillis; 				// vorheriger Wert an currentMillis übergeben 
    // Sensor_auslesen();  // hier die Function die aufgerufen werden soll
      client.publish(WemosName, EspHostname);
      DS_Temperatur = getTemperatur();
      dtostrf(DS_Temperatur, 3, 2, ds18B20_Cstring);
      client.publish(WemosDSTemperatur, ds18B20_Cstring);
      Serial.println(ds18B20_Cstring);                                  //String(DS_Temperatur).c_str());
      client.publish(WemosStatus, "online");
  }
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
/*
void Sensor_auslesen() {
    sensors.requestTemperatures(); 
    DS_Temperatur = sensors.getTempCByIndex(0);   
    
    Serial.print(String(DS_Temperatur).c_str());
    Serial.println(" °C"); 
    ds18B20_Cstring = String(DS_Temperatur);
    // Serial.print(String(ds18B20_Cstring));
    WTemp = String(ds18B20_Cstring);                                           // WTemp = DS_Temperatur;   //dtostrf(temperatureC, 5,2,DS_Temperatur);
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
}*/

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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Nachricht zum Thema gekommen: ");
  Serial.print(topic);
  Serial.print(". Nachricht: ");
  String messageTemp;
for (unsigned i = 0; i <= length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();  
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
      DS_Temperatur = getTemperatur();
      dtostrf(DS_Temperatur, 3, 2, ds18B20_Cstring);
      client.publish(WemosDSTemperatur, ds18B20_Cstring);
      // dtostrf(WTemp, 5,2,DS_Temperatur);
      Serial.println(ds18B20_Cstring);                                  //String(DS_Temperatur).c_str());
      client.publish(WemosStatus, "online");
// ... and resubscribe
      
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" nochmal in 5 seconds");
// Wait 5 seconds before retrying
        delay(5000);
      }
   }
} 


