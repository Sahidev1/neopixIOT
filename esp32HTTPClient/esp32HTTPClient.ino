
#include "pins_arduino.h"
#include "secrets.h" // Header file containing credentials, remove this or create own secrets.h file
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN D7 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 24 // Popular NeoPixel ring size

#define RED 0x00ff0000
#define GREEN 0x0000ff00
#define BLUE 0x000000ff

const char* ssid = SSID_;
const char* password = PASSWORD_;

const String host_ip = SERVER_IP;
const String host_port = SERVER_PORT;
const String host_FQDN = SERVER_DOMAIN;
boolean firstCon = true;

int polling_time_out;
hw_timer_t *polling_timer;
hw_timer_t *isr_timer;
hw_timer_t *timer;
double t_frequency; //timeout frequency
double x;

int color_Arr[NUMPIXELS];

int rotateIndex;
int currArrIndex;
int pixelIndex;
int rotate_timeout;

boolean rotator_mode;

HTTPClient http;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//frequency f = 1/(T * period_scaler)
//period from frequency T * period_scaler = 1/f
void ARDUINO_ISR_ATTR updateRot (){
    double y = 10;
    double c = 1300;
    t_frequency = y + c*(1 + sin((x/c) + 1.5*PI));
    x = x + 1.0;
}

void setup() {
    //Serial.begin(9600);
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
    isr_timer = timerBegin(1,80,true);
    timer = timerBegin(2,80,true);
    t_frequency = 10.0;
    rotator_mode = false;
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
        
        HTTP_control ();
        timerRestart(polling_timer);
    }
    if (rotator_mode){
        rotator_control ();
    }
}

void HTTP_control (){
    //http.begin("http://" + host_ip + ":" + host_port + "/unocheck" + "?polling=" + polling_time_out);
    http.begin("https://" + host_FQDN + "/unocheck" + "?polling=" + polling_time_out);
    int http_code = http.GET();

    if (http_code > 0 && http_code == HTTP_CODE_OK){
        String payload = http.getString();
        if (payload.startsWith("SETCOLOR")){
            int RGBbits = payload.substring(9).toInt();
            setPIXELcolor (RGBbits);
            if (rotator_mode){
                rotator_mode = false;
                cleanrotate();
            }
        } else if(payload.startsWith("SETPOLLING")) {
            polling_time_out = payload.substring(11).toInt();
        } else if (payload.startsWith("PRESET")){
            if (payload.substring(7).equals("rotate")){
                rotator_mode = true;
                initrotate();
            }
        }
    }
    http.end();
}

void rotator_control (){
    double T = (1 / t_frequency) * 1000000;
    rotate_timeout = (int) T;
    if (timerReadMicros(timer) > rotate_timeout){
        rotateProc();
        timerRestart(timer);
    }
}

void rotateProc(){
    if (pixelIndex >= NUMPIXELS){
        pixelIndex = 0;
        rotateIndex++;
        pixels.show();
        return;
    }
    pixels.setPixelColor(pixelIndex, color_Arr[(rotateIndex+pixelIndex) % NUMPIXELS]);
    pixelIndex++;
}

void initrotate (){
    int randcol;
    for (int i = 0; i < NUMPIXELS; i++){
        if (i < 8) randcol = RED;
        else if (i < 16) randcol = BLUE;
        else randcol = GREEN;

        color_Arr[i] = randcol;
    }
    rotateIndex = 0;
    pixelIndex = 0;
    t_frequency = 200.0;
    x = 1.0;

    timerRestart(isr_timer);
    timerAttachInterrupt(isr_timer, &updateRot, true);  //attach callback
    timerAlarmWrite(isr_timer, 2*1000, true); //set time in us
    timerAlarmEnable(isr_timer);                          
}

void cleanrotate(){
    timerAlarmDisable(isr_timer);
    timerDetachInterrupt(isr_timer);
}

void setPIXELcolor (int RGB_bits){
    pixels.clear();
    int mask = 0x000000ff;
    int RGB[3];
    for (int i = 0; i < 3; i++){
        RGB[i] = RGB_bits & mask;
        RGB_bits = RGB_bits >> 8;
    }

    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(RGB[0], RGB[1], RGB[2]));
        pixels.show();   // Send the updated pixel colors to the hardware.
        delay(50);
    }
}
