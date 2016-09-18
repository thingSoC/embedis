/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 PatternAgents, LLC

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

#include <Embedis.h>

/* Test for Arduino101 platform - need special examples for those */
#if defined(ARDUINO_ARCH_ARC32)
  #include "CurieEEPROM.h"
#else
  #error "Please use the specific example for your board type this example is for the Arduino101/Curie Architecture"
#endif

// Embedis will run on the Serial port. Use the Arduino
// serial monitor and send "COMMANDS" to get started.
// Make sure "No line ending" is -not- selected. All others work.
Embedis embedis(Serial);

void setup() 
{
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB (Leo, Teensy, etc)
    }
    /* We use "LOG" instead of "serial.println", to create a LOG channel */
    /* Use SUBSCRIBE LOG to get these messages                           */
    LOG( String() + F(" ") );
    LOG( String() + F("[ Embedis : ESP8266 Sketch ]") );
    LOG( String() + F("[ Embedis : select 115200 Baud and 'Both NL & CR' as your line ending ]") );
    LOG( String() + F("[ Embedis : Note Bene! CurieEEPROM write function not working yet (we're working on it) ]") );

    // Create a key-value Dictionary in EEPROM
    Embedis::dictionary( 
        "EEPROM",
        1024,
        [](size_t pos) -> char { return EEPROM.read8(pos); },
        [](size_t pos, char value) { EEPROM.write8(pos, value); }
    );
    LOG( String() + F("[ Embedis : EEPROM dictionary installed ]") );
    LOG( String() + F("[ Embedis : EEPROM dictionary selected ]") );
   
    // Add pinMode command to mirror Arduino's
    Embedis::command( F("pinMode"), [](Embedis* e) {
        if (e->argc != 3) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        String argv3(e->argv[2]);
        argv3.toUpperCase();
        int mode;
        if (argv3 == "INPUT") mode = INPUT;
        else if (argv3 == "OUTPUT") mode = OUTPUT;
        else if (argv3 == "INPUT_PULLUP") mode = INPUT_PULLUP;
        else return e->response(Embedis::ARGS_ERROR);
        pinMode(pin, mode);
        e->response(Embedis::OK);
    });

    // Add digitalWrite command to mirror Arduino's
    Embedis::command( F("digitalWrite"), [](Embedis* e) {
        if (e->argc != 3) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        String argv3(e->argv[2]);
        argv3.toUpperCase();
        int mode;
        if (argv3 == "HIGH") mode = HIGH;
        else if (argv3 == "LOW") mode = LOW;
        else mode = argv3.toInt();
        digitalWrite(pin, mode);
        e->response(Embedis::OK);
    });

    // Add digitalRead command to mirror Arduino's
    Embedis::command( F("digitalRead"), [](Embedis* e) {
        if (e->argc != 2) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        if (digitalRead(pin)) {
            e->response(F("HIGH"));
        } else {
            e->response(F("LOW"));
        }
    });

    // Add analogRead command to mirror Arduino's
    Embedis::command( F("analogRead"), [](Embedis* e) {
        if (e->argc != 2) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        e->response(':', analogRead(pin));
    });

    /* okay, done setting up new dictionary and commands... */
    LOG( String() + F("[ Embedis : Type 'commands' to get a listing of commands ]") );
}

void loop() 
{
    embedis.process();
    /* give delay - for any internal RTOS to switch context */
    delay(20);
}

// This will log to an embedis channel called "log".
// Use SUBSCRIBE LOG to get these messages.
// Logs are also printed to Serial until an empty message is received.
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
