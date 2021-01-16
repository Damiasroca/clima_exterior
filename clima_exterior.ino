#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "NodeMCU_Exterior"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#include <EnvironmentCalculations.h>
#include <BME280I2C.h>
#include <Wire.h>

#define WIFI_SSID "YOUR_WIFI_SSID"

#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define INFLUXDB_URL "http://YOUR_SERVER.COM:8086"

#define INFLUXDB_TOKEN "YOUR_TOKEN"

#define INFLUXDB_ORG "Main_ORG"

#define INFLUXDB_BUCKET "YOUR_BUCKET"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("YOUR_SENSOR");

#include <BME280.h>
#include <EnvironmentCalculations.h>
#include <BME280I2C.h>
#include <Wire.h>
BME280I2C bme;

float pressure;
float temperature_ext;
float humidity;
float dew;

void wifi() {


  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Add tags
  sensor.addTag("device", DEVICE);


  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void setup() {
  Serial.begin(115200);
  wifi();
  Wire.begin();
  while (!Serial) continue;
  bme.begin();

}

void loop() {

  influxdb();

  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  EnvironmentCalculations::TempUnit     envTempUnit =  EnvironmentCalculations::TempUnit_Celsius;

  float dewPoint = EnvironmentCalculations::DewPoint(bme.temp(), bme.hum(), envTempUnit);
  float AbsoluteHumidity = EnvironmentCalculations::AbsoluteHumidity(temperature, humidity, envTempUnit);

  pressure         = pres;
  temperature_ext  = temp;
  humidity         = hum;
  humidity_abs     = AbsoluteHumidity;
  dew              = dewPoint;

  /* BME280 debug

  Serial.print(pressure);
  Serial.print("-------");
  Serial.print(temperature_ext);
  Serial.print("-------");
  Serial.print(humidity);
  Serial.print("-------");
  Serial.print(humidity_abs);
  Serial.print("-------");
  Serial.println(dew);
  */



  delay(1000);

}
