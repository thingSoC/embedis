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


// To configure embedis access to Vcc, call setup_vcc from your
// main setup() function. Optionally, supply the name you want to use.
// e.g. setup_vcc();
//      setup_vcc( F("voltage") );




void setup_vcc() 
{
  // There is only one ADC on the ESP8266. To measure Vcc we must connect
  // Vcc to the ADC using ADC_MODE and leave A0 unconnected. If you wish to
  // use ADC on A0 this file must be removed or the following command
  // commented out.
  // ADC_MODE(ADC_VCC);
  setup_vcc( F("vcc") );
}

void setup_vcc(const String& name) 
{
    Embedis::hardware( name,
    [](Embedis* e) { e->response(read_vcc()); },
    0);
    LOG( String() + F("[ Embedis : Additional hardware installed ]") );    
    LOG( String() + F("[ Embedis : Type 'hardware' to get a listing of new hardware ]") );    

}

String read_vcc() 
{
    float vcc = ESP.getVcc();
    return String(vcc / 19860, 3);
}

