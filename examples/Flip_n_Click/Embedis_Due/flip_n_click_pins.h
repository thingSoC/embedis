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
/*
 * Pin Out Definition for Mikroe Flip-n-Click
 * ==========================================
 *
 * Pin Assignment for Mikroe Flip-n-Click Board Revision 1.0
 * 
 * We'll use the schematic node names as our pin names where it makes sense.
 * (this should really be put into a varient of arduino_pins.h after testing)
 * 
 * http://www.mikroe.com/download/eng/documents/development-tools/accessory-boards/click/flip-n-click/flip-n-click-v100.manual.web.pdf
 * 
 */

/* Pin Out for Bottom Arduino Shield Connector 
 */ 
#define RX_USB    0   /* PA8  : RX        : Arduino Due D0   : FTDI USB Serial (Don't use for Input/Output) */
#define TX_USB    1   /* PA9  : TX:       : Arduino Due D1   : FTDI USB Serial (Don't use for Input/Output) */
#define PWM2      2   /* PB25 : PWM2      : Arduino Due D2   : */
#define PWM3      3   /* PC28 : PWM3      : Arduino Due D3   : */
#define PWM4      4   /* PA29 : PWM4      : Arduino Due D4   : Due also connects PC26? test which one */
#define PWM5      5   /* PC25 : PWM5      : Arduino Due D5   : */
#define PWM6      6   /* PC24 : PWM6      : Arduino Due D6   : */
#define PWM7      7   /* PC23 : PWM7      : Arduino Due D7   : */
#define PWM8      8   /* PC22 : PWM8      : Arduino Due D8   : */
#define PWM9      9   /* PC21 : PWM9      : Arduino Due D9   : */
#define PWM10    10   /* PC29 : PWM10     : Arduino Due D10  : Due also connects PA28? */
#define PWM11    11   /* PD7  : PWM11     : Arduino Due D11  : */
#define PWM12    12   /* PD8  : PWM12     : Arduino Due D12  : */
#define PWM13    13   /* PB27 : PWM13     : Arduino Due D13  : LED_BULITIN */
#define SDA0     70   /* PA17 : I2C0_SDA  : Arduino Due SDA  : */
#define SCL0     71   /* PA18 : I2C0_SCL  : Arduino Due SCL  : */
#define AREF     -1   /* NC   : NC        : Arduino Due AREF : AREF is floating? */
#define AD0      54   /* PA16 : AD0       : Arduino Due A0   : */
#define AD1      55   /* PA24 : AD1       : Arduino Due A1   : */
#define AD2      56   /* PA23 : AD2       : Arduino Due A2   : */
#define AD3      57   /* PA22 : AD3       : Arduino Due A3   : */
#define AD4      58   /* PA6  : AD4       : Arduino Due A4   : */
#define AD5      59   /* PA4  : AD5       : Arduino Due A5   : */

/* Pinout for Click Socket A
 */
#define LEDA     38   /* PC6  : LEDA      : Arduino Due D38  : */
#define SDAA     70   /* PA17 : I2C0_SDA  : Arduino Due SDA  : */
#define SCLA     71   /* PA18 : I2C0_SCL  : Arduino Due SCL  : */
#define TXDA     18   /* PA11 : TXD0      : Arduino Due TX1  : Serial1 */
#define RXDA     19   /* PA10 : RXD0      : Arduino Due RX1  : Serial1 */
#define PWMA      6   /* PC24 : PWM6      : Arduino Due D6   : */
#define ANAA     54   /* PA16 : AD0       : Arduino Due A0   : */
#define RSTA     33   /* PC1  : RSTA      : Arduino Due D33  : */
#define CSNA     77   /* PA28 : SPI0_CS0  : Arduino Due CS0  : */ 
#define SCKA     76   /* PA27 : SPI0_SCK  : Arduino Due SCK  : */
#define MISOA    74   /* PA25 : SPI0_MISO : Arduino Due MISO : */
#define MOSIA    75   /* PA26 : SPI0_MOSI : Arduino Due MOSI : */ 
  
/* Pinout for Click Socket B
 */
#define LEDB     37   /* PC5  : LEDB      : Arduino Due D37  : */
#define SDAB     70   /* PA17 : I2C0_SDA  : Arduino Due SDA  : */
#define SCLB     71   /* PA18 : I2C0_SCL  : Arduino Due SCL  : */
#define TXDB     16   /* PA13 : TXD2      : Arduino Due TX2  : Serial2 */
#define RXDB     17   /* PA12 : RXD2      : Arduino Due RX2  : Serial2 */
#define PWMB      7   /* PC23 : PWM7      : Arduino Due D7   : */
#define ANAB     55   /* PA24 : AD1       : Arduino Due A1   : */
#define RSTB     34   /* PC2  : RSTB      : Arduino Due D34  : */
#define CSNB     79   /* PA29 : SPI0_CS1  : Arduino Due ???  : N.B. this is not mapped for Due? */ 
#define SCKB     76   /* PA27 : SPI0_SCK  : Arduino Due SCK  : */
#define MISOB    74   /* PA25 : SPI0_MISO : Arduino Due MISO : */
#define MOSIB    75   /* PA26 : SPI0_MOSI : Arduino Due MOSI : */ 
 
/* Pinout for Click Socket C
 */
#define LEDC     39   /* PC7  : LEDC      : Arduino Due D39  : */
#define SDAC     20   /* PB12 : I2C1_SDA  : Arduino Due SDA1 :  */
#define SCLC     21   /* PB13 : I2C1_SCL  : Arduino Due SCL1 :  */
#define TXDC     14   /* PD4  : TXD3      : Arduino Due TX3  : Serial3 */
#define RXDC     15   /* PD5  : RXD3      : Arduino Due RX3  : Serial3 */
#define PWMC      8   /* PC22 : PWM8      : Arduino Due D8   : */
#define ANA      56   /* PA23 : AD2       : Arduino Due A2   : */
#define RSTSC    35   /* PC3  : RSTC      : Arduino Due D35  : */
#define CSNC     52   /* PB21 : SPI0_CS2  : Arduino Due D52  : */ 
#define SCKC     76   /* PA27 : SPI0_SCK  : Arduino Due SCK  : */
#define MISOC    74   /* PA25 : SPI0_MISO : Arduino Due MISO : */
#define MOSIC    75   /* PA26 : SPI0_MOSI : Arduino Due MOSI : */ 

/* Pinout for Click Socket D
 */
#define LEDD     40   /* PC8  : LEDD      : Arduino Due D40  : */
#define SDAD     20   /* PB12 : I2C1_SDA  : Arduino Due SDA1 : */
#define SCLD     21   /* PB13 : I2C1_SCL  : Arduino Due SDA1 : */
#define TXDD     14   /* PD4  : TXD3      : Arduino Due TX3  : */
#define RXDD     15   /* PD5  : RXD3      : Arduino Due RX3  : */
#define PWMD      9   /* PC21 : PWM9      : Arduino Due D9   : */
#define ANAD     57   /* PA22 : AD3       : Arduino Due A3   : */
#define RSTD     36   /* PC4  : RSTD      : Arduino Due D36  : */
#define CSND     78   /* PB23 : SPI0_CS3  : Arduino Due D78  : */ 
#define SCKD     76   /* PA27 : SPI0_SCK  : Arduino Due SCK  : */
#define MISOD    74   /* PA25 : SPI0_MISO : Arduino Due MISO : */
#define MOSID    75   /* PA26 : SPI0_MOSI : Arduino Due MOSI : */ 
