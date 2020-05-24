# ESPBase
---

## Dependencies
- [ESP8266 Core for Android](https://github.com/esp8266/Arduino) (Arduino core for ESP8266 WiFi chip)
- [WifiManager](https://github.com/tzapu/WiFiManager) (ESP8266 WiFi Connection manager with fallback web configuration portal)

Make sure you have installed the dependencies according to their installation instructions for your environment


## How it works

### Connecting to wifi
As described in the WifiManager projects documentation:

- When your ESP starts up, it sets it up in Station mode and tries to connect to a previously saved Access Point
- if this is unsuccessful (or no previous network saved) it moves the ESP into Access Point mode and spins up a DNS and WebServer (default ip 192.168.4.1)
- using any wifi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point
- because of the Captive Portal and the DNS server you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal
- choose one of the access points scanned, enter password, click save
- ESP will try to connect. If successful, it relinquishes control back to your app. If not, reconnect to AP and reconfigure.
- There are options to change this behavior or manually start the configportal and webportal independantly as well as run them in non blocking mode.


### Configure the Web server
The project comes with a simple web page that displays a list of buttons.

The buttons are generated from a javascript array of strings.
The `void handleJavascript()` function has an example with a single button "Test" and a link to a non-existent enpoint called `http://{your_ESP_IP}/led1`
The html code for the demo page is available in `index.html` in this repository.
This has been minimized and before it was added to the `void handleRoot()` function as a string.

### Demo page
The buttons are generated from a simple array of strings where the strings contain the text and link seperated with a pipe symbol as shown here:
```
var buttonData = ['Test|./led1']
``` 

![Demo Page with a singe button](https://i.imgur.com/Unsgcz8.png)