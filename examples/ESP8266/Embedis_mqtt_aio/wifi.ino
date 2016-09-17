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
#include <ESP8266mDNS.h>
#include <DNSServer.h>

MDNSResponder mdns;
DNSServer dnsServer;

static const unsigned int TIMEOUT = 15000; // ms

/************************* WiFi Connection Manager ***************************/
// WiFi connection manager. To use:
//
// Call setup_wifi();  from your inititialization setup() function
// Call loop_wifi();   from your main loop() function.
//
// setup_wifi() - wifi connection initialization
int setup_wifi() {
  int connected = 0;
  while (connected == 0) {
    connected = loop_wifi();
    delay(100);
  }
  delay(100);
}

// loop_wifi() - wifi connection state machine
//
// ssid : access point to connect to in station mode
// passphrase : for access point ssid (optional)
// hostname : mDNS name, default esp8266
//
// ap_ssid : for becoming an access point, default esp8266
// ap_passphrase : for securing this access point (optional)
//
// Returns: (can be used for status LED)
//  0 while connecting
//  1 when connected
//  2 as access point
int loop_wifi() {
    static unsigned int loop_wifi_state = 0;
    static unsigned int timeout;

    if (loop_wifi_state == 0) {
        String mode = setting_wifi_mode();
        if (mode == "ap") loop_wifi_state = 1;
        else if (mode == "sta") loop_wifi_state = 3;
        else loop_wifi_state = 6;
        return 0;
    }
    if (loop_wifi_state == 1) {
        String ap_ssid = setting_ap_ssid();
        String ap_passphrase = setting_ap_passphrase();
        LOG( String() + F("WIFI: Started Access Point (AP_SSID) : ") + ap_ssid);
        if (ap_passphrase == setting_default_passphrase()) {
            LOG( String() + F("WIFI: Access Point Passphrase (AP_PASS) : ") + ap_passphrase);
        }
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid.c_str(), ap_passphrase.c_str());
        dnsServer.start(53, "*", WiFi.softAPIP());
        LOG( String() + F("WIFI: Access Point IP Address : ") + WiFi.softAPIP().toString() );
        LOG(" "); // End setup logging
        ++loop_wifi_state;
        return 0;
    }
    if (loop_wifi_state == 2) {
        dnsServer.processNextRequest();
        return 2;
    }
    if (loop_wifi_state == 3 || loop_wifi_state == 6) {
        String ssid = setting_sta_ssid();
        String passphrase = setting_sta_passphrase();
        if (!ssid.length() && loop_wifi_state == 6) {
            loop_wifi_state = 1; 
            return 0;
        }
        WiFi.mode(WIFI_STA);
        WiFi.begin ( ssid.c_str(), passphrase.c_str() );
        if (WiFi.status() == WL_IDLE_STATUS) {
            WiFi.disconnect();
        } else {
            LOG(String() + F("WIFI: connecting to: ") + ssid);
            timeout = millis() + TIMEOUT;
            ++loop_wifi_state;
        }
        return 0;
    }
    if (loop_wifi_state == 7) {
        if (WiFi.status() == WL_CONNECTED) {
            loop_wifi_state = 4;
        }
        else if (millis() > timeout) {
            LOG(F("WiFi: Unable to connect."));
            loop_wifi_state = 1;
        }
        return 0;
    }
    if (loop_wifi_state == 4) {
        if (WiFi.status() == WL_CONNECTED) {
            LOG( String() + F("WIFI: connected with IP: ") + WiFi.localIP().toString() );
            String hostname = setting_mdns_hostname();
            if (hostname.length()) {
                if(mdns.begin ( hostname.c_str(), WiFi.localIP() ) ) {
                    LOG( String() + F("MDNS: ") +  hostname + F(".local"));
                }
            }
            ++loop_wifi_state;
            return 1;
        }
        return 0;
    }
    if (loop_wifi_state == 5) {
        if (WiFi.status() != WL_CONNECTED) return 0;
        mdns.update();
        return 1;
    }
    // should never get here
    return 0;
}
