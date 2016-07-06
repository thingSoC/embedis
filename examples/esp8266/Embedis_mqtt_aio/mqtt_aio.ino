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
// MQTT Helper Routines for using the AdaFruit.IO MQTT System
//
// e.g. setup_mqtt_aio();  // call once from setup();
//      loop_mqtt_aio();   // call each pass through loop();

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Store the MQTT server, username, and password in flash memory.
// Unfortunately, this is required for using the Adafruit MQTT library.
// It is set at compile time, so we can't easily change it using Embedis.
// While the keys mqtt_server, etc. can be set and changed in Embedis,
// Unfortunately, they have no effect on the Adafruit MQTT libraries at this time.
// Other Embedis keys (e.g. mqtt_key, wifi_ssid, wifi_passwd, led_pin, etc.) 
// function normally, it is only the Adafruit MQTT keys 
// e.g. the MQTT_SERVER,  MQTT_NAME, and MQTT_KEY keys
// that are fixed in "PROGMEM" and not easily changed.
const char MQTT_SERVER[]   PROGMEM = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed called 'Analog0' for publishing the ADC reading.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char ADC0_FEED[] PROGMEM = AIO_USERNAME "/feeds/adc0";
Adafruit_MQTT_Publish adc0 = Adafruit_MQTT_Publish(&mqtt, ADC0_FEED);

// Setup a feed called 'mqtt_key' for publishing a value from the Embedis EEPROM keystore.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char MQTT_EMBEDIS_FEED[] PROGMEM = AIO_USERNAME "/feeds/mqtt_embedis";
Adafruit_MQTT_Publish mqtt_embedis = Adafruit_MQTT_Publish(&mqtt, MQTT_EMBEDIS_FEED);

// Setup a feed called 'LED0' for subscribing to changes in the onboard LED.
const char LED0_FEED[] PROGMEM = AIO_USERNAME "/feeds/led0";
Adafruit_MQTT_Subscribe led0 = Adafruit_MQTT_Subscribe(&mqtt, LED0_FEED);

static uint8_t led;      /* a blinky LED (GPIO output)     */

typedef union{
    struct __attribute__((__packed__)){  // packed data
        char charBuf[32];
    }buf;
    uint8_t raw[sizeof(buf)];
} mqtt_buf_t;
mqtt_buf_t mqtt_buf;

void setup_mqtt_aio() 
{
// Setup MQTT subscription for feeds.
  mqtt.subscribe(&led0);
  LOG(String() + F("MQTT: Subscriptions Added!"));
  MQTT_connect();
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  /* reconnect to MQTT Server */
  LOG(String() + F("MQTT: Connecting to MQTT Server... "));
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       LOG( String() + mqtt.connectErrorString(ret) );
       LOG( String() + F("MQTT: Retrying connection in 5 seconds..."));
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  LOG( String() + F("MQTT: Connected to MQTT Server!"));
}

int loop_mqtt_aio() 
{
  int return_status = 0;
  // check that the MQTT server is alive before we do anything...   
  MQTT_connect(); 

  // wait for incoming MQTT subscription packets and process them
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &led0) {
      // MQTT is going to control the onboard led
      // setup the LED pin GPIO number based on the Embedis key "led_pin"
      String led_pin_number = setting_led_pin();
      led = (uint8_t) led_pin_number.toInt();
      pinMode(led, OUTPUT);
      LOG( String() + F("MQTT: led0 pin_number: ") +  led_pin_number + F(" led_pin: ") + led);
      if (strncmp((char *)led0.lastread, "ON", led0.datalen ) == 0) {
        digitalWrite(led, LOW);
        LOG( String() + F("MQTT: Subscribed led0 = ON"));
      } else if (strncmp((char *)led0.lastread, "OFF", led0.datalen ) == 0) {
        digitalWrite(led, HIGH);
        LOG( String() + F("MQTT: Subscribed led0 = OFF"));
      }     
    }
  }

  // publish outgoing feeds
  int adc_value = analogRead(A0);
  //adc_value = analogRead(A0);
  if (! adc0.publish(adc_value)) {
    LOG( String() + F("MQTT: Publish adc0 Failed! adc0 = ") + adc_value);
    return_status = 1;
  } else {
    LOG( String() + F("MQTT: Publish adc0 Succeeded! adc0 = ") + adc_value);
  }

  // publish Embedis "mqtt_embedis" value
  String mqtt_embedis_value = setting_mqtt_embedis();
  mqtt_embedis_value.toCharArray(mqtt_buf.buf.charBuf, sizeof(mqtt_buf_t));
  //int val = mqtt_embedis_value.toInt();
  if (! mqtt_embedis.publish(mqtt_buf.raw, sizeof(mqtt_buf_t))) {
    LOG( String() + F("MQTT: Publish mqtt_embedis Failed! value = ") + mqtt_embedis_value);
    return_status = 1;
  } else {
    LOG( String() + F("MQTT: Publish mqtt_embedis Succeeded! value = ") + mqtt_embedis_value);
  }
 
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  
  return return_status;
}
