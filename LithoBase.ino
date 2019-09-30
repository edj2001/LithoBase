//Device settings:
// Wemos D1 mini
// Flash Size: 4M (1m spiffs)
// Debug Port: Disabled
// Debug Level: None
// lwIP variant:  V1.4 Higher Bandwidth  !important! from fauxmo readme https://bitbucket.org/xoseperez/fauxmoesp/src/master/
// VTables: Flash
// CPU Frequency: 80 MHz
// Exceptions: Disabled
// Erase Flash:  Only Sketch


//boards versions:
// esp8266 2.4.2

//library versions:
// fauxmo 3.1.0
// wifimanager 0.14.0
// NeoPixelBus by Makuna 2.5.0

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "OTA.h"

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include "fauxmoPrototypes.h"
#include "Lighting.h"
#include "motor.h"
#include "frame.h"

#include "fauxmo.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

void tick()
{
  //toggle state
  int state = digitalRead(LED_BUILTIN);  // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);     // set pin to the opposite state

    animations.UpdateAnimations();
    strip.Show();
  
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.05, tick);
  setLightsWIFIConfigMode();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.05, tick);

  //Lighting setup
  setupLighting();
  setLightsConnectingToWIFI();

  //Motor setup
  setupMotor();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration

    if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(LED_BUILTIN, LOW);

  setLightsDisplayMode();
  
  setupOTA();
  
  setupFauxmo();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();

  handleLights();
  handleMotor();
  handleFauxmo();

}
