void influxdb() {

  sensor.clearFields();


  sensor.addField("Pressió_Ext", pressio);
  sensor.addField("Temperatura_Ext", temperatura);
  sensor.addField("Humitat_Ext", humitat);
  sensor.addField("Humitat_absoluta", hum_absoluta);
  sensor.addField("Punt_de_rosada_Ext", rosada);
  sensor.addField("Basca", basca);



  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }


  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }


  Serial.println("Wait 2500ms");
  delay(2500);
}
