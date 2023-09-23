#include "Sensor_BME280.cpp"
// Bibliotheken die in der .h und .cpp Dateien benötigt werden
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Variablen deklarieren
#define WemosStatus "wemos/status"
const char *WemosBMEtemp = "wemos/bme280/temp";
const char *WemosBMEhum = "wemos/bme280/hum";
const char *WemosBMEpres = "wemos/bme280/pres";
float temp(NAN), hum(NAN), pres(NAN);
// BME280
float h, t, p;
unsigned int l;
char temperatureCString[8];
char humidityCString[7];
char pressureCString[8];
String bme_temp_str = "";
String bme_hum_str = "";
String bme_pre_str = "";
#define SEALEVELPRESSURE_HPA (1010.25)
Adafruit_BME280 bme; // I²C
unsigned long delayTime;

//  Funktionen deklarieren