//
// https://bitbucket.org/xoseperez/fauxmoesp/src/master/
#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------


#define ID_PHOTO_FRAME           "photo frame"
#define ID_PHOTO_LAMP            "photo lamp"
#define ID_PHOTO_SPEED             "photo speed"
#define ID_PHOTO_COLOR             "photo color"

// -----------------------------------------------------------------------------
void updateFauxmoLighting()
{
            fauxmo.setState(ID_PHOTO_LAMP, getLightingState(), getLightingBrightness());
            fauxmo.setState(ID_PHOTO_COLOR, getLightingState(), lightingColour);
  
}

void updateFauxmoMotor()
{
            fauxmo.setState(ID_PHOTO_SPEED, getMotorState(), getMotorSpeed());
  
}

void updateFauxmoFrame()
{
  //show the frame ON if either the lighting or motor is ON
  bool frameState = (getLightingState() || getMotorState());
  fauxmo.setState(ID_PHOTO_FRAME, frameState, 0);
}

void setupFauxmo() {

    // Init serial port and clean garbage
    Serial.println();
    Serial.println();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    fauxmo.addDevice(ID_PHOTO_FRAME);
    fauxmo.addDevice(ID_PHOTO_LAMP);
    fauxmo.addDevice(ID_PHOTO_SPEED);
    fauxmo.addDevice(ID_PHOTO_COLOR);

    /* 
      value returns between 0 to 255  
      */
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

        if (strcmp(device_name, ID_PHOTO_FRAME)==0) {
            setFrame(state);
            updateFauxmoLighting();
            updateFauxmoMotor();

        } else if (strcmp(device_name, ID_PHOTO_LAMP)==0) {
            setLighting(state, value);
        } else if (strcmp(device_name, ID_PHOTO_SPEED)==0) {
            setMotor(state, value);
        } else if (strcmp(device_name, ID_PHOTO_COLOR)==0) {
            setColour(state, value);
        }

    });

}


void handleFauxmo() {

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();
    updateFauxmoLighting();
    updateFauxmoMotor();
    updateFauxmoFrame();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);

}
