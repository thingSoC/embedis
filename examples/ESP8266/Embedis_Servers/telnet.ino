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

// CAUTION: Password auth isn't secure.
//          Credentials are sent in clear text.

// Telnet server which connects to Embedis.
// Call setup_telnet() from your main setup() function.
// Call loop_telnet() from your main loop() function.

#include <WiFiServer.h>
#include <WiFiClient.h>
#include "Embedis.h"

WiFiServer server23(23);
WiFiClient server23Client;
Embedis embedis23(server23Client);

void setup_telnet() 
{
    server23 = WiFiServer(23);
    server23.begin();
    server23.setNoDelay(true);
    LOG( String() + F("[ Embedis : Started Telnet Server ]") );    
}

String temp_telnet_passphrase;

void loop_telnet() 
{
    static int eat = 0;
    static int auth = 0;

    // new connections
    if (server23.hasClient()) {
        if (!server23Client.connected()) {
            server23Client.stop();
            server23Client = server23.available();
            embedis23.reset(true);
            eat = 0;
            auth = -2;
        } else {
            server23.available().stop();
        }
    }

    int ch;

    // discard negotiation from the client
    while (eat >= 0 || auth >= 0) {
        int peek = server23Client.peek();
        if (peek < 0) break;
        if (peek == 255) {
            server23Client.read();
            eat = 2;
            continue;
        }
        if (eat > 0 && eat <= 3) {
            ch = server23Client.read();
            if (--eat==1) {
                if (ch == 250) eat = 250; // SB
                if (ch == 240) eat = 0;   // SE
            }
            continue;
        }
        if (eat == 250 || peek == 0 || peek == 10) {
            server23Client.read();
            continue;
        }
        eat = -1;
        break;
    }

    switch(auth) {
    case -99:
        // Logged in
        if (eat < 0) embedis23.process();
        break;
    case -2:
        server23Client.write(255); // IAB
        server23Client.write(253); // DO
        server23Client.write(34);  // LINEMODE
        server23Client.write(255); // IAB
        server23Client.write(250); // SB
        server23Client.write(34);  // LINEMODE
        server23Client.write(1);   // MODE: EDIT
        server23Client.write(3);   // DEFAULT MASK
        server23Client.write(255); // IAB
        server23Client.write(240); // SE
        server23Client.write(255); // IAB
        server23Client.write(251); // WILL
        server23Client.write(1);   // ECHO
        //nobreak
    case -1:
        server23Client.print("Password:");
        temp_telnet_passphrase = setting_login_passphrase();
        auth = 0;
        return;
    default:
        if (eat >= 0) return;
        ch = server23Client.read();
        if (ch < 0) break;
        if (ch == 13) {
            server23Client.println("");
            if (auth == temp_telnet_passphrase.length()) {
                server23Client.write(255); // IAB
                server23Client.write(252); // WONT
                server23Client.write(1);   // ECHO
                auth = -99;
                temp_telnet_passphrase = "";
                server23Client.println("Logged in.");
            } else {
                auth = -1;
            }
            eat = 0;
            break;
        }
        if (auth >= 0 && temp_telnet_passphrase[auth] == ch) {
            auth++;
            break;
        }
        // Failed password. Stay in default until CR.
        auth = -3;
        break;
    }

}
