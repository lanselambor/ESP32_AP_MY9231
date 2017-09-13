
#include <WiFi.h>
#include "my9291.h"
#include <stdlib.h>

#define MY9291_DI_PIN       13
#define MY9291_DCKI_PIN     15
#define RAINBOW_DELAY       10

// RGB LED references
char colors[12];
char *s;
String currentLine = "";
uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;
bool rainbow_on = false;
const int pirSensor = 5;
my9291 _my9291 = my9291(MY9291_DI_PIN, MY9291_DCKI_PIN, MY9291_COMMAND_DEFAULT, 3);


// WiFi references
const char *ssid = "MyESP32AP";
const char *passwd = "12345678";
WiFiServer server(80);


void rainbow(unsigned char index) {

    if (index < 85) {
        _my9291.setColor((my9291_color_t) { (unsigned int) index * 3, (unsigned int) 255 - index * 3, 0, 0, 0 });
    } else if (index < 170) {
        index -= 85;
        _my9291.setColor((my9291_color_t) { (unsigned int) 255 - index * 3, 0, (unsigned int) index * 3, 0, 0 });
    } else {
        index -= 170;
        _my9291.setColor((my9291_color_t) { 0, (unsigned int) index * 3, (unsigned int) 255 - index * 3, 0, 0 });
    }

}

void rainbow_loop(){
    static unsigned char count = 0;
    static unsigned long last = millis();
    if (millis() - last > RAINBOW_DELAY) {
        last = millis();
        rainbow(count++);
    }
}

void handleLEDColor(uint8_t r, uint8_t g, uint8_t b) {
    _my9291.setColor((my9291_color_t) { r, g, b, 0, 0 });
}

uint8_t getPIRState(void) {
    return HIGH == digitalRead(pirSensor) ? 1 : 0;
}

void pirDetectLoop() {
    static unsigned long pirDetectTStart = millis();
    static unsigned long lightTStart;
    static uint8_t state = 0;

    if(state == 0 ) {
        if (millis() - pirDetectTStart > 100) {
            pirDetectTStart = millis();
            state =  getPIRState();            
            if(state == 1) {
                Serial.println("PIR Sensor trigled...");
                // Handle pirDetection
                handleLEDColor(100,100,100);
                lightTStart = millis();
            }            
        }
    } else {            
        if(millis() - lightTStart > 5000) {
            state= 0;
            // Handle pirDetection
            Serial.println("Close LED...");
            handleLEDColor(0,0,0);
        }
    }    
}

void cleanBuffer(char *buf, int num) {
    for(int i = 0; i < num; i++) {
        buf[i] = '\0';
    }
}


void setup() {
    Serial.begin(115200);
    
    // MY9291
    _my9291.setState(true);
    
    // PIR sendor 
    pinMode(pirSensor, INPUT);

    WiFi.softAP(ssid, passwd);
    Serial.println(WiFi.softAPIP());
    server.begin();
}


void loop() {
    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {                            // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port    
    while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
            char c = client.read();             // read a byte, then
            Serial.write(c);                    // print it out the serial monitor
            if (c == '@') {                    // if the byte is a newline character
                // Set color string format: @xxx,xxx,xxx#
                currentLine = "";
            } else if (c == '#') {  // if you got anything else but a carriage return character,
                // Stop Receive data and set color                
                strcpy(colors, currentLine.c_str());
                Serial.print("Color: ");
                Serial.println(colors);
                if( currentLine == "clear" || currentLine == "CLEAR") {
                    rainbow_on = false;
                    handleLEDColor(0,0,0);
                } else if (currentLine == "rainbow" || currentLine == "RAINBOW") {
                    rainbow_on = true;
                } else {
                    rainbow_on = false;
                    s = strtok(colors, ",");
                    r = atoi(s);
                    s = strtok(NULL, ",");
                    g = atoi(s);
                    s = strtok(NULL, ",");
                    b = atoi(s);
                    handleLEDColor(r,g,b);
                }
                cleanBuffer(colors, 12);
            } else {
                currentLine += c;
            }
        }
        else {
            if(rainbow_on) {
                rainbow_loop();
            }
            pirDetectLoop();
        }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    }

    // RGB LED rainbow
    if(rainbow_on) {
        rainbow_loop();
    }

    // Loop read pir sensor state
    pirDetectLoop();
}