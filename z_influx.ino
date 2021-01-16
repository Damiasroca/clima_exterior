void influxdb() {
  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();

  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("Pressió_Ext", pressure);
  sensor.addField("Temperatura_Ext", temperature_ext);
  sensor.addField("Humitat_Ext", humidity);
  sensor.addField("Punt_de_rosada_Ext", dew);
  sensor.addField("Humitat_Absoluta", humidity_abs);


  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // If no Wifi signal, try to reconnect it
  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  //Wait 10s
  Serial.println("Wait 1s");
  delay(1000);
}
