/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2016 PatternAgents, LLC

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

#include "main.h"
#include "Embedis.h"


class MockStream : public Stream {
public:
    std::string in;
    std::string out;
    virtual size_t write(uint8_t c) {
        out += (char)c;
        return 1;
    }
    virtual size_t write(const uint8_t *buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            out += (char)buffer[i];
        }
        return size;
    }
    virtual int available() {
        return in.length();
    }
    virtual int read() {
        if (in.length() == 0) return -1;
        int r = in[0];
        in = in.substr(1);
        return r;
    }
    virtual int peek() {
        if (in.length() == 0) return -1;
        return in[0];
    }
    virtual void flush() {
        in = "";
        out = "";
    }
};


MockStream ms0;
Embedis em0(ms0, BUF_LENGTH, ARGV_LENGTH);

MockStream ms1;
Embedis em1(ms1, BUF_LENGTH, ARGV_LENGTH);

MockStream* ms = &ms0;
Embedis* em = &em0;


std::string embedis_test(std::string cmd)
{
    if (!cmd.empty()) {
        char last1 = 0, last2 = 0;
        for (size_t i = 0; i < cmd.length(); i++) {
            ms->in += cmd[i];
            last1 = last2;
            last2 = cmd[i];
        }
        if (last1 != '\r' && last2 != '\n') {
            ms->in += "\r\n";
        }
    }
    em->process();
    std::string ret = ms->out;
    ms->out = "";
    return ret;
}


void embedis_test_interface(int i)
{
    switch(i) {
    case 0:
        ms = &ms0;
        em = &em0;
        break;
    case 1:
        ms = &ms1;
        em = &em1;
        break;
    default:
        throw "unknown interface";
    }

}


void embedis_test_init()
{
    embedis_test_interface(0);
    em0.reset(true);
    em1.reset(true);
    ms0.flush();
    ms1.flush();
    for (auto& d : kvs_data) d = 0xff;
}


static bool rom_get(const String& key, String& value)
{
    if (key == F("vendor")) {
        value = F("PatternAgents");
        return true;
    }
    return false;
}


static void rom_keys(Embedis* e)
{
    e->response('*', 1);
    e->response("vendor");
}


std::vector<char> kvs_data(64);


static char ram_kvs_fetch(size_t pos)
{
    return kvs_data.at(pos);
}


static void ram_kvs_store(size_t pos, char value)
{
    kvs_data.at(pos)=value;
}


int main()
{
    Embedis::dictionary("rom", rom_get, 0, 0, rom_keys, 0);
    Embedis::dictionary("ram", kvs_data.size(), ram_kvs_fetch, ram_kvs_store, 0);

    testing::reporter(new testing::DefaultReporter);
    return testing::run();
}
