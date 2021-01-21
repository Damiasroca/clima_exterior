
void post_windy() {

  unsigned long currentTime = millis();

  if (currentTime - previousTime >= eventInterval) {
    BearSSL::WiFiClientSecure client;;


    Serial.print("connecting to ");
    Serial.println(host);

    //Serial.printf("Using fingerprint '%s'\n", fingerprint);
    client.setInsecure();
    //client.setFingerprint(fingerprint);

    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      return;
    }
    String presi, tempi, humi, rosi;

    presi = pressio;
    tempi = temperatura;
    humi = humitat;
    rosi = rosada;

    String getData = "?temp=" + tempi + "&mbar=" + presi + "&dewpoint=" + rosi + "&humidity=" + humi;

    
    
    String  url = "/pws/update/++++++YOUR_WINDY_API_KEY++++++" + getData;					//EDIT
                             
    Serial.print("requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    String line = client.readStringUntil('\n');
    if (line.startsWith("{\"state\":\"SUCCESS\"")) {
      Serial.println("Sent data successfull!");
    } else {
      Serial.println("Sent data SUCCESSED");
    }
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");

    previousTime = currentTime;

  }

}
