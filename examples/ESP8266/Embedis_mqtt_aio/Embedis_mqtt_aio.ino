/*
  ======================================================================================
  Embedis for Adafruit MQTT Library ESP8266 Example

  This example sketch demonstrates using the Adafruit.IO MQTT system with 
  the Embedis keystore for holding configuration and other data.
  Embedis is used to store credentials like the Wi-Fi SSID to connect with,
  and other configuration parameters that you can change without recompiling
  your program (sketch). This example publishes an ADC feed, and subscribes to
  and LED feed for remote control of the on-board led. The LED can be reassigned
  to different I/O pins using the Embedis keystore to change the I/O pin setting.
  Hardware changes like that, or the Wi-Fi SSID and Password, will require a reset
  of a reboot to take effect.

  This example sketch also publishes via MQTT an Embedis Key named "mqtt_embedis",
  and whatever string (32 characters maximum) you set in the Embedis keystore will
  get published to the MQTT server as the feed ../feeds/mqtt_embedis

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino/

  Must use Embedis Dictionary Library from:
     https://github.com/thingsoc/embedis/

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  MQTT FEEDS :
  http://io.adafruit.com/mqtt_user/feeds/adc0
  http://io.adafruit.com/mqtt_user/feeds/led0
  http://io.adafruit.com/mqtt_user/feeds/mqtt_embedis
  
  ADAFRUIT.IO DASHBOARD EXAMPLE :
  https://io.adafruit.com/patternagents/embedis-mqtt-aio
  
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
/*  Embedis - Embedded Dictionary Server
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
#include <ESP8266WiFi.h>
#include "Embedis.h"

Embedis embedis(Serial); /* the embedis console (Serial Monitor) */

/************************* Adafruit.io Setup *********************************/
/* 
   Unfortunately the MQTT server, port, username, and key are required to 
   be stored in FLASH Memory (PROGMEM) for using the Adafruit MQTT library.
   This is set at compile time, so we can't easily change it using Embedis.
   While these particular mqtt keys can be set and changed in Embedis,
   they have no effect on the Adafruit MQTT libraries.
   Change the values here for the your Adadfruit User and Password and recompile...        
*/
#define AIO_SERVER      "io.adafruit.com"            // Adafruit.IO MQTT Server
#define AIO_SERVERPORT  1883                         // use 8883 for SSL
#define AIO_USERNAME    "...your AIO username..."    // see https://accounts.adafruit.com
#define AIO_KEY         "...your AIO key..."         // see https://io.adafruit.com

void setup() {
  setup_log();       /* setup the serial LOG first, to get LOG messages         */
  setup_EEPROM();    /* this sets up the dictionary and should be called second */
  setup_wifi();      /* make the initial Wi-Fi connection                       */
  setup_mqtt_aio();  /* setup the Adafruit.IO MQTT feeds and subscriptions      */
  
  LOG("Embedis: ready!");
  LOG("");           /* End setup logging by passing an empty string to disable */
                     /* You can reenable logging using "subscribe log" commnand */
}

void loop() {
  embedis.process(); /* process the Embedis Command Line */
  loop_wifi();       /* maintain the Wi-Fi connection, if lost */
  loop_mqtt_aio();   /* maintain the MQTT feeds */
}


