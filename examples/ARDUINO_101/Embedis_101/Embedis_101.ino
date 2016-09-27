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
#if !defined(ARDUINO_ARCH_ARC32)
  #error "Please use the specific example for your board type this example is for the Arduino101 (Intel Curie) Architecture"
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
    LOG( String() + F("[ Embedis : Arduino101 (Intel Curie) Sketch ]") );
    LOG( String() + F("[ Embedis : select 115200 Baud and 'Both NL & CR' as your line ending ]") );

    /* create the "EEPROM" dictionary (internal EEPROM) */
    setup_EEPROM();

    /* create the "SRAM" dictionary (internal RAM)  */
    setup_SRAM();

    /* Add some useful commands the embedis command line interpreter (CLI */
    setup_commands();
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
