#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include "LittleFS.h"

void initWifi();
void configModeCallback(WiFiManager *myWiFiManager);
void initOTA();
void initFS();
void initWebServer();
void toggleLed();
void handleJavascript();
void handleNotFound();


// Set web server port number to 80
ESP8266WebServer server(80);


void initWifi() {
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

void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void initOTA() {

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

void initFS() {
  delay(500);

  // Uncomment and run it once, if you want to erase all the stored data in the filesystem
  // LittleFS.format();

 
  Serial.println(F("Inizializing FS..."));
  if (LittleFS.begin()){
    Serial.println(F("done."));
  }else{
    Serial.println(F("fail."));
  }
  
  // Open root folder
  Dir dir = LittleFS.openDir("/");
  // List all files
  while (dir.next()) {
    // Print the filename
    Serial.print(dir.fileName());
    Serial.print(" - ");
    // Print the size of the file
    if(dir.fileSize()) {
      File f = dir.openFile("r");
      Serial.println(f.size());
      f.close();
    }else{
      Serial.println("0");
    }
  }
}

// -----------------------  Web Server Section ----------------------------------------------------------------------

void initWebServer() {

  server.serveStatic("/", LittleFS, "/index.html");
  server.serveStatic("/index.html", LittleFS, "/index.html");
  server.on("/myscripts.js", HTTP_GET, handleJavascript);

  server.on("/led1", HTTP_GET, toggleLed);

  server.onNotFound(handleNotFound);
  server.begin();  
}


void toggleLed() {

  int newLedStatus = digitalRead(LED_BUILTIN);
  char returnVal[100];
  sprintf(returnVal, "{\"ledStatus\":%d}", newLedStatus);

  digitalWrite(LED_BUILTIN, !newLedStatus);
  server.send(200,"application/json", returnVal);
}

void handleJavascript() {
  // Send HTTP status 200 (Ok) and send some text to the browser/client
  server.send(200, "application/javascript", "var buttonData = ['Toggle LED|/led1'];");   
}

void handleNotFound() {
  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  server.send(404, "text/plain", "404: Not found"); }
// ---------------------------------------------------------------------------------------------------------------

void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);
  // Turn on LED (the LED has a pull-up resistor, so the logic is inverted)
  digitalWrite(LED_BUILTIN, !HIGH);

  Serial.begin(115200);
  Serial.println("Booting");

  initFS();
  initWifi();
  initOTA();
  initWebServer();

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Turn off LED when connected to Wifi and ready.
  digitalWrite(LED_BUILTIN, !LOW);

}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}
