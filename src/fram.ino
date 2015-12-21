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
/**
  ******************************************************************************
  *
  * @file        fram.ino
  * @copyright   PatternAgents, LLC
  * @brief       The Embedis Dictionary, support for External SPI FRAM
  *
  ******************************************************************************
  */

/**
  ******************************************************************************
  *
  *  Support for External SPI FRAM. 
  *  To use, change the "#if 0" to "#if 1" and add the following line to
  *  the embedis_dictionaries[] in the config.ino file
  *  {"FRAM", &embedis_ram_commands, (void*)&arduino_fram_access},
  *
  ******************************************************************************
  */

#if 1

typedef enum opcodes_e
{
  OPCODE_WREN   = 0b0110,     /* Write Enable Latch */
  OPCODE_WRDI   = 0b0100,     /* Reset Write Enable Latch */
  OPCODE_RDSR   = 0b0101,     /* Read Status Register */
  OPCODE_WRSR   = 0b0001,     /* Write Status Register */
  OPCODE_READ   = 0b0011,     /* Read Memory */
  OPCODE_WRITE  = 0b0010,     /* Write Memory */
  OPCODE_RDID   = 0b10011111  /* Read Device ID */
} opcodes_t;


const uint8_t _cs = 20;
const uint8_t _clk = 13;
const uint8_t _miso = 12;
const uint8_t _mosi = 11;


size_t arduino_fram_size() {
    // SPI Setup
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    pinMode(_clk, OUTPUT);
    pinMode(_mosi, OUTPUT);
    pinMode(_miso, INPUT);
    // fixed size for now, interogate part later...
    return (32768);
}

char arduino_fram_fetch(size_t pos) {
     // delay(1);
     digitalWrite(_cs, LOW);
     SPItransfer(OPCODE_READ);
     SPItransfer((uint8_t)(pos >> 8));
     SPItransfer((uint8_t)(pos & 0xFF));
     uint8_t x = SPItransfer(0);
     digitalWrite(_cs, HIGH);
     return x;
}

void arduino_fram_store(size_t pos, char value) {
  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_WREN);
 
  digitalWrite(_cs, HIGH);
  
  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_WRITE);
  SPItransfer((uint8_t)(pos >> 8));
  SPItransfer((uint8_t)(pos & 0xFF));
  SPItransfer(value);
  /* CS on the rising edge commits the WRITE */
  digitalWrite(_cs, HIGH);
  //SPItransfer(OPCODE_WRDI);
}

const embedis_ram_access arduino_fram_access = {
    arduino_fram_size,
    arduino_fram_fetch,
    arduino_fram_store,
    0
};

uint8_t SPItransfer(uint8_t x) {
    uint8_t reply = 0;
    for (int i=7; i>=0; i--) {
      reply <<= 1;
      digitalWrite(_clk, LOW);
      digitalWrite(_mosi, x & (1<<i));
      digitalWrite(_clk, HIGH);
      if (digitalRead(_miso)) 
	reply |= 1;
    }
    return reply;
}

#endif
