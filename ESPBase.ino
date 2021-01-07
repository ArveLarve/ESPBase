#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <WiFiManager.h>

// Set web server port number to 80
ESP8266WebServer server(80);


void initWifi(){
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information
  // wifiManager.resetSettings();

  wifiManager.setAPCallback(configModeCallback);
  // wifiManager.setConfigPortalTimeout(180);

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void initOTA(){

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

}

// -----------------------  Web Server Section ----------------------------------------------------------------------

void initWebServer() {

  server.on("/", HTTP_GET, handleRoot);
  server.on("/myscripts.js", HTTP_GET, handleJavascript);

  server.onNotFound(handleNotFound);
  server.begin();  
}

void handleRoot() {
  server.send(200, "text/html", "<html lang='en'><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <meta http-equiv='X-UA-Compatible' content='ie=edge'> <meta name='mobile-web-app-capable' content='yes'> <title>ESP</title> <script src='myscripts.js'></script></head><style>#buttons{max-width: 500px; padding-inline-start: 0; margin: 10px;}#buttons li{list-style: none; text-align: center; background-color: #60759b; margin-bottom: 20px; padding: 20px; font-size: 2em; box-shadow: 1vw 1vw 3vw 1px #ccc;}.header{font-size: 2em; margin: 1em;}#buttons li a{text-decoration: none; color: #FFFFFF; display: block;}#buttons li a:hover{text-decoration: none; color: #FFF; font-weight: bold;}</style><body> <div style='font-family:sans-serif'> <div class='header'>Title</div><ul id='buttons'> </ul> </div><script>var lis=''; if(buttonData){for (var i=0; i < buttonData.length; i++){var parts=buttonData[i].split('|'); lis +='<li><a href=\"' + parts[1] + '\">' + parts[0] + '</a></li>';}document.getElementById('buttons').innerHTML=lis;}</script></body></html>");
}

void handleJavascript() {
  server.send(200, "application/javascript", "var buttonData = ['Test LED 1|./led1']");   // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
// ---------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  initWifi();
  initOTA();
  initWebServer();

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  ArduinoOTA.handle();

  server.handleClient();
}
