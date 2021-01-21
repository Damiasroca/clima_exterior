//Escrit íntegrament per Damià Sintes Roca a 18/01/2021.
//Llegir comentaris per Migjorn o Fornells.
//MIRAR CALIBRACiÓ PRESSiÓ ATMOSFÈRICA ABANS DE FLASHAR!!

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#define DEVICE "NAME _YOUR_DEVICE"  									//EDIT
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

//Fornells
#define WIFI_SSID "+++YOUR_SSID+++"									//EDIT

#define WIFI_PASSWORD "+++YOUR_WIFI_PASS+++"								//EDIT

#define INFLUXDB_URL "http://YOUR_SERVER.com:8086"							//EDIT

#define INFLUXDB_TOKEN "+++YOUR_INFLUX_TOKEN+++"							//EDIT

#define INFLUXDB_ORG "+++YOUR_ORG+++"									//EDIT

#define INFLUXDB_BUCKET "+++YOUR_BUCKET+++"								//EDIT


  #define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

  InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

  Point sensor("NAME_YOUR_SENSOR"); 									//EDIT


  const uint8_t fingerprint[20] = {0xdf, 0xc2, 0x44, 0xa7, 0x73, 0x4c, 0x51, 0x9b, 0x8d0, 0x12, 0x55, 0xd2, 0xee, 0x19, 0xcc, 0x98, 0xf9, 0xe8, 0x26, 0xb1};
  const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
  const char *host = "stations.windy.com";

  const unsigned long eventInterval = 400000UL;
  unsigned long previousTime = 0;

  #include <BME280.h>
  #include <EnvironmentCalculations.h>
  #include <BME280I2C.h>
  #include <Wire.h>
  BME280I2C bme;

  float pressio, temperatura, basca, hum_absoluta, humitat, rosada;

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


  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");


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


  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  EnvironmentCalculations::TempUnit     envTempUnit =  EnvironmentCalculations::TempUnit_Celsius;

  float dewPoint         = EnvironmentCalculations::DewPoint(temp, hum, envTempUnit);
  float AbsoluteHumidity = EnvironmentCalculations::AbsoluteHumidity(temp, hum, envTempUnit);
  float HeatIndex        = EnvironmentCalculations::HeatIndex(temp, hum, envTempUnit);

  pressio      = pres; //Calibrar
  temperatura  = temp; //Calibrar
  humitat      = hum;
  hum_absoluta = AbsoluteHumidity;
  rosada       = dewPoint;
  basca        = HeatIndex;

  /*Serial.print(pressio);
  Serial.print("-------");
  Serial.print(temperatura);
  Serial.print("-------");
  Serial.print(humitat);
  Serial.print("-------");
  Serial.print(hum_absoluta);
  Serial.print("-------");
  Serial.print(rosada);
  Serial.print("-------");
  Serial.println(basca);*/

  influxdb();
  post_windy();

  delay(1000);

  }
