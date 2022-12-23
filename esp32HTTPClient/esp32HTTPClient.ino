
#include "pins_arduino.h"
#include "secrets.h" // Header file containing credentials, remove this or create own secrets.h file
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <stdint.h>


// Which pin on the Arduino is connected to the NeoPixels?
#define PIN D7 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 24 // Popular NeoPixel ring size


const char* ssid = SSID_;
const char* password = PASSWORD_;

const String host_ip = SERVER_IP;
const String host_port = SERVER_PORT;
const String host_FQDN = SERVER_DOMAIN;
boolean firstCon = true;

int polling_time_out;
hw_timer_t *polling_timer;

HTTPClient http;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.println("starting");
    pixels.begin();
    pixels.setBrightness(255);
    int col = 0x0023c510;
    setPIXELcolor(col);
    delay(1000);
    pixels.clear();
    pixels.show();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    polling_time_out = 2000;
    http.setReuse(true);
    polling_timer = timerBegin(0,80,true);
}

void loop() {
    if(WiFi.status() == WL_CONNECTED && timerReadMilis(polling_timer) >= polling_time_out) {
        if (firstCon){
            int col = 0x00a17733;
            setPIXELcolor(col);
            delay(250);
            pixels.clear();
            pixels.show();
            firstCon = false;
        }
        
        Serial.print("connected");

        Serial.print("HTTP GET STARTED");

        http.begin("http://" + host_ip + ":" + host_port + "/unocheck" + "?polling=" + polling_time_out);
        //http.begin("https://" + host_FQDN + "/unocheck" + "?polling=" + polling_time_out);
        Serial.println("HTTP GET CALLING");

        int http_code = http.GET();

        if (http_code > 0){
            if (http_code == HTTP_CODE_OK){
                String payload = http.getString();
                Serial.println(payload);
                if (payload.startsWith("SETCOLOR")){
                    int RGBbits = payload.substring(9).toInt();
                    setPIXELcolor (RGBbits);
                } else if(payload.startsWith("SETPOLLING")) {
                    polling_time_out = payload.substring(11).toInt();
                    Serial.println("SETPOLLING=");
                    Serial.print(payload.substring(11).toInt());
                }
            }
        }
        timerRestart(polling_timer);
        http.end();
    }
    else {
        
    }
}

void setPIXELcolor (int RGB_bits){
    pixels.clear();
    int mask = 0x000000ff;
    int RGB[3];
    Serial.println(RGB_bits);
    for (int i = 0; i < 3; i++){
        RGB[i] = RGB_bits & mask;
        Serial.println(RGB[i]);
        RGB_bits = RGB_bits >> 8;
    }

    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(RGB[0], RGB[1], RGB[2]));
        
        pixels.show();   // Send the updated pixel colors to the hardware.
        delay(50);
    }
}
