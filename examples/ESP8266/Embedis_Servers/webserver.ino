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

// CAUTION: HTTP Basic Auth isn't secure.
//          Credentials are sent in clear text.

// HTTP server which serves static files from SPIFFS.
// Call setup_webserver() from your main setup() function.
// Call loop_webserver() from your main loop() function.

#include <ESP8266WebServer.h>
#include <FS.h>

ESP8266WebServer server80( 80 );

class LogRequestHandler : public RequestHandler {
public:
    virtual bool canHandle(HTTPMethod method, String uri) {
        String ls("[ Embedis : Webserver ");
        String ps(" ] ");
        if (method == HTTP_GET) ls += "GET ";
        if (method == HTTP_POST) ls += "POST ";
        if (method == HTTP_PUT) ls += "PUT ";
        if (method == HTTP_PATCH) ls += "PATCH  ";
        if (method == HTTP_DELETE) ls += "DELETE ";
        if (method == HTTP_OPTIONS) ls += "OPTIONS ";
        LOG(ls + uri + ps);
        return false;
    }
};

void setup_webserver() 
{
    setup_webserver("");
}

// You can serve from a subdirectory by supplying a root.
// e.g. setup_webserver(F("/path/to/files"))
void setup_webserver(const String& root) 
{
    // SPIFFS.begin() can crash so we log something before trying.
    // If crashing or failing here you probably didn't upload the filesystem.
    // https://github.com/esp8266/arduino-esp8266fs-plugin/releases
    LOG(F("[ Embedis : Mounting SPIFFS Filesystem ]"));
    if (SPIFFS.begin()) LOG(F("[ Embedis : SPIFFS Filesystem Ready ]"));
    else LOG(F("[ Embedis : SPIFFS Filesystem Mount Failure! ] "));

    // handlers added after LogRequestHandler will be logged
    server80.addHandler(new LogRequestHandler);
    server80.on ("/embedis", HTTP_POST, webserver_embedis);
    server80.serveStatic("", SPIFFS, root.c_str(), "");

    server80.begin();
    String ws = F("[ Embedis : Started HTTP Server ]");
    if (root.length()) ws = ws + F(" serving ") + root;
    LOG(ws);
    String passphrase = setting_login_passphrase();
    if (passphrase == setting_default_passphrase()) {
        LOG(String("[ Embedis : HTTP Server Authentication (User/Pass) : ") + setting_login_name() + " / " + passphrase + F(" ] "));
    }
}

void loop_webserver() 
{
    server80.handleClient();
}


class EmbedisStringStream : public Stream {
public:
    String* in;
    unsigned int pos;
    String pubout;
    String cmdout;
    bool puboverflow;
    bool cmdoverflow;
    const unsigned int outsize;
    String concat;
    EmbedisStringStream(unsigned int size) : in(0), outsize(size) {
        pubout.reserve(size);
        cmdout.reserve(size);
        flush();
    }
    virtual size_t write(uint8_t c) {
        if (in) {
            if (cmdout.length() >= outsize) cmdoverflow = true;
            if (cmdoverflow) return 0;
            cmdout += (char)c;
        } else {
            if (pubout.length() >= outsize) puboverflow = true;
            if (puboverflow) return 0;
            pubout += (char)c;
        }
        return 1;
    }
    virtual size_t write(const uint8_t *buffer, size_t size) {
        size_t count = 0;
        for (size_t i = 0; i < size; i++) {
            count += write(buffer[i]);
        }
        return count;
    }
    virtual int available() {
        if (!in) return 0;
        return in->length() - pos;
    }
    virtual int read() {
        if (!in || pos >= in->length()) return -1;
        return (*in)[pos++];
    }
    virtual int peek() {
        if (!in || pos >= in->length()) return -1;
        return (*in)[pos];
    }
    virtual void flush() {
        pubout.remove(0);
        puboverflow = false;
    }
    void setCommand(String* s) {
        in = s;
        pos = 0;
        cmdout.remove(0);
        cmdoverflow = false;
    }
};

EmbedisStringStream ess80(2048);
Embedis embedis80(ess80);

bool webserver_embedis()
{
    String passphrase = setting_login_passphrase();
    bool authenticated;
    if (!passphrase.length()) {
        authenticated = true;
    } else {
        authenticated = server80.authenticate(setting_login_name().c_str(), passphrase.c_str());
    }
    // ESP8266WebServer won't send a content length for an empty string.
    // Where this can happen the length must be explicitly set.
    // Otherwise XHR can be slow. Extremely slow.
    String cmd = server80.arg("cmd");
    if (cmd.length()) {
        if (!authenticated) {
            server80.requestAuthentication();
            return true;
        }
        embedis80.reset();
        cmd += "\r\n";
        ess80.setCommand(&cmd);
        embedis80.process();
        if (ess80.cmdoverflow) {
            server80.send(200, "text/plain", "-ERROR result overflow\r\n");
        } else {
            server80.setContentLength(ess80.cmdout.length());
            server80.send(200, "text/plain", ess80.cmdout);
        }
        ess80.setCommand(0);
    } else {
        if (!authenticated) {
            server80.setContentLength(0);
            server80.send(200, "text/plain", "");
            return true;
        }
        if (ess80.puboverflow) ess80.flush();
        server80.setContentLength(ess80.pubout.length());
        server80.send(200, "text/plain", ess80.pubout);
        ess80.flush();
    }
}
