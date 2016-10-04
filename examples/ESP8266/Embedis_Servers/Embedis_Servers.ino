/*  
    Embedis - Embedded Dictionary Server
    Copyright (C) 2015, 2016 PatternAgents, LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 *  embedis_servers : embedis, web, and telnet servers for ESP8266
    --------------------------------------------------------------
    This example Arduino IDE sketch for the ESP8266 platform
    (or the Arduino Platform + WiFI Shield Configuration) 
    is used to provide three (3) simultaineous server applications, namely :
    1) Embedis Command Line Interface on Serial (Hardware Serial Monitor)
    2) Web Based Embedis Command Line Interface on Port 80
    3) Telnet Based Embedis Command Line Interface on Port 23

    Any of those Servers can be used to get/set keys in the EEPROM database,
    such as the the WiFi SSID and Passphrase. You no longer need to embed 
    that "data" into your "program"! The EEPROM is persistant, and can be
    used across multiple projects to configure your program setting for
    the specific hardware and network configuration you are using. 
    
    Now, you don't need to recompile your program and reflash your device
    in order to change the device settings anymore. If your ESP8266 WiFi
    doesn't connect, it reverts to an Access Point with a Web Server to allow
    you to change the configuration settings and join another WiFi network.

    We think that this is a much better mechanism for storing settings between projects.
    Once you start using Embedis for your projects, you'll see how quick and easy
    it is to move and reconfigure your devices without needing to recompile 
    and reflash your device firmware just to change a configuration setting.
    Now settings can be changed "on-the-fly" with just a web browser.
    (some settings changes however, will require a restart/reboot to take effect.)

   ====================================================================================== 
    SECURITY CAUTION: (WARNING!)
    These examples demonstrate only simple authentication methods.
    These are not intended to be secure, they are teaching examples only.

    SIMPLE AUTHENTICATION:
    The authentication scheme used here is very, very simple, where the
    default device password is a lowercased base64 function of that 
    devices access point Media Access Control, or Ethernet Hardware Address.
    
    The default device password will be printed out on the Serial Monitor during bootup,
    and should be unique for each device. This is important in a classroom situation
    to keep each student from (accidentally...) accessing/changing their neighbors device.
    
    It is not intended for use in a production environment, where we suggest that 
    encryted keys and values be used, however that is beyond of the scope of this example.
    Embedis itself is transparent, so a more secure implementation would encrypt
    the values (and even the keys themselves) prior to calling Embedis. 
    In this example, all of the keys and values are in unencrypted plain-text, 
    for demonstration, teaching, and debugging purposes.
   ======================================================================================
*/

#if !defined(ARDUINO_ARCH_ESP8266)
#error "This Sketch is for the ESP8266/ESP32 Platform only..., untested on others..."
#endif

#include <ESP8266WiFi.h>
#include "Embedis.h"
Embedis embedis(Serial); /* the LOG/console Serial monitor */

static uint8_t led;      /* a blinky LED (GPIO output)     */

void setup() 
{
    /* start the LOG/console channel first, to log everything else... */   
    Serial.begin(115200);
    delay(50);
    LOG( String() + F(" ") );
    LOG( String() + F("[ ==================================================== ]") );
    LOG( String() + F("[ Embedis : WWW/Telnet/CLI Servers Sketch for ESP8266! ]") );
    LOG( String() + F("[ ==================================================== ]") );
    
    setup_EEPROM();   // keep this second, the configuration settings are loaded here
    setup_vcc();
    setup_commands();
    setup_webserver();
    setup_telnet();

    
    // setup the LED pin based on the Embedis key "led_pin"
    String led_pin_number = setting_led_pin();
    led = (uint8_t) led_pin_number.toInt();
    pinMode(led, OUTPUT);
    LOG( String() + F("[ Embedis : Platform led_pin_number: ") +  led_pin_number + F(" led_pin: ") + led + F(" ]"));
}

void loop() 
{
    embedis.process();    // process the Embedis Command Line
    blink( loop_wifi() ); // blink the LED
    loop_webserver();     // process the web server
    loop_telnet();        // process the telnet server
}


// Blink out a number. More than 2 may be hard to count.
// Using 0 blinks fast and steady.
void blink(int num) {
    static unsigned long heartbeat = 0;
    static int beatcount = 0;
    //static uint8_t led = 0;

    unsigned long now = millis();
    if (now > heartbeat) {
        if (digitalRead(led)) {
            digitalWrite (led, 0);
            if (!num) heartbeat = now + 250;
            else heartbeat = now + 1;
        } else {
            digitalWrite (led, 1);
            if (!num) heartbeat = now + 250;
            else {
                if (beatcount) {
                    --beatcount;
                    heartbeat = now + 175;
                } else {
                    led++;
                    if (led > 32) led = 30;
                    beatcount = num - 1;
                    heartbeat = now + 999;
                }
            }
        }
    }
}


// This will log to an embedis channel called "log".
// Use SUBSCRIBE LOG to get these messages.
// Logs are also printed to Serial until an empty message is received.
// Success with WiFi sends an empty message.
void LOG(const String& message) {
    static bool inSetup = true;
    if (inSetup) {
        if (!message.length()) {
            inSetup = false;
            return;
        }
        SERIAL_PORT_MONITOR.println(message);
    }
    Embedis::publish("log", message);
}
