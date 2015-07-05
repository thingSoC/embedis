 
\brief		Embedis : Embedded Dictionary Server Documentation
\details	A library for embedding a key-value dictionary in Flash, FRAM, NVSRAM, and EEPROM. 
\author		Dave Turnbull
\date		2015-06-29
\copyright	PatternAgents, LLC
\mainpage	Embedis Documentation
\section	main_intro Introduction
\par
Embedis is an open source library for creating simple embedded dictionary servers. \n
Embedis is used for embedding key-value dictionaries into the \n
SRAM, FLASH, FRAM, NVSRAM, EEPROM, or SDcard memory systems \n
of small, limited resource, embedded computing platforms, \n
such as the Arduino(AVR), Teensy(Cortex-M4), PSoC(Cortex-M0/3), \n
and others. \n
\par
\section	main_background Background
\par
I needed to support several different persistant memory store types, \n
for a number of "Internet of Things", and small embedded system projects. \n
These types include the internal processor/SoC based SRAM, FLASH, EEPROM, \n
and Scratchpad SRAM memories, as well as externally attached FLASH, EEPROM, \n
FRAM, NVSRAM and SDCard memories. \n
\par
Embedis was developed to provide a simple interface for storing and retrieving data \n
from pins, sensors, interfaces, and other I/O devices, \n
across a number of different embedded platforms and IDEs. \n
\par
\section	main_theory Embedis CLI Overview 
\par
The Embedis command line interface (CLI) uses familiar SET/GET/DEL commands \n
for storing, retriving, and deleting key-value pairs in the persistant memory stores. \n
For example :\n
> \>set mykey somevalue\n
> OK\n
> \>get mykey\n
> "somevalue"\n
> \>del mykey\n
> OK\n

\par
Different memory types are selected using their device name, for example :\n
> \>select EEPROM\n
> OK\n
> \>set mykey somevalue (written to the internal EEPROM Memory)\n
> OK\n
> \>select FLASH\n
> OK\n
> \>set mykey somevalue (written to the internal FLASH Memory)\n
> OK\n
> \>select S25FL127S\n
> OK\n
> \>set mykey somevalue (written to external S25FL127S FLASH Memory)\n
> OK\n

\par
A list of keys can be retrieved using the KEYS command for the selected memory :
> \>select EEPROM\n
> OK\n
> \>keys\n
> $5\n
> mykey\n
> \>get mykey\n
> "somevalue"\n

\par
Access to hardware drivers is accomplished similarly using the READ/WRITE commands \n
> \> write /led/0 0\n
> OK\n
> \> read /led/0\n
> +0\n
> \> write /led/0 1\n
> OK\n


\par
\defgroup main_api API Definitions
@{
@}


