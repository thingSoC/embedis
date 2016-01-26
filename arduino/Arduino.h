/*
 Arduino.h - Mocks to make some Arduino code run for testing
 Copyright (C) 2016 PatternAgents, LLC

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Arduino_h
#define Arduino_h

// With these mock implementations you can use WString, Stream, and Print
// on platforms which aren't Arduino. These aren't complete implementations
// but are sufficient for running automated tests.

#include <cstdio>
#include <assert.h>

#define PSTR(s) s
#define PGM_P const char*
#define strcpy_P strcpy
#define strlen_P strlen
#define pgm_read_byte(p) p[0]

inline void itoa(int val, char * s, int radix) {
    assert(radix==10);
    sprintf(s, "%d", val);
}

inline void utoa(unsigned int val, char * s, int radix) {
    assert(radix==10);
    sprintf(s, "%u", val);
}

inline void ltoa(long val, char * s, int radix) {
    assert(radix==10);
    sprintf(s, "%ld", val);
}

inline void ultoa(unsigned long val, char * s, int radix) {
    assert(radix==10);
    sprintf(s, "%lu", val);
}


inline char * dtostrf(double val, signed char width, unsigned char prec, char * s) {
    assert(prec==2);
    sprintf(s, "%.2f", val);
    return s;
}

inline unsigned int millis() {
    unsigned int m = 0;
    return ++m;
}


#endif /* Arduino_h */
