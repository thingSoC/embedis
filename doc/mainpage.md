\brief		Embedis : Embedded Dictionary Server Documentation
\details	A library for embedding a key-value dictionary in Flash, FRAM, NVSRAM, and EEPROM. 
\author		Dave Turnbull, Tom Moxon
\copyright	PatternAgents, LLC 2015, 2016
\mainpage	Embedis Documentation
\section	main_intro Introduction
\par
Embedis is an open source library for creating simple embedded dictionary servers. \n
Embedis is used for embedding key-value dictionaries into the \n
SRAM, FLASH, FRAM, NVSRAM, EEPROM, or SDcard memory systems \n
of small, limited resource, embedded computing platforms, \n
such as the Arduino(AVR), Teensy(Cortex-M4),\n
ESP8266, and others. \n
\par
\section	main_background Background
\par
We needed to support several different persistant memory store types, \n
for a number of "Internet of Things", and small embedded system projects. \n
These types include the internal processor/SoC based SRAM, FLASH, EEPROM, \n
and Scratchpad SRAM memories, as well as externally attached FLASH, EEPROM, \n
FRAM, NVSRAM and SDCard memories. \n
\par
Embedis was developed to provide a simple, consistant command line interface, and APIs \n
for storing and retrieving data from pins, sensors, interfaces, and other I/O devices, \n
across a number of different embedded platforms and IDEs. \n
\par
\section	main_overview Embedis Overview 
\par
Embedis is a simple, easily extensible framework for adding persistant, nonvolatile storage to your projects.
It is designed to be be lightweight (using only a few kilobytes of program space), and to efficiently support very small storage
devices, such as EEPROM or FRAM data storage only a few kilobytes. Embedis implements a Command Line Interface, similar to
a subset of the Redis data structure server.
\par
\section	main_overview Embedis Command Line Interface 
\par
The Embedis command line interface (CLI) uses familiar SET/GET/DEL commands \n
for storing, retriving, and deleting key-value pairs in the persistant memory stores. \n
For example :\n
> set mykey somevalue\n
> +OK\n
> get mykey\n
> +somevalue\n
> del mykey\n
> :1\n

\par
The different persistent memory stores are called "dictionaries", \n
and you can query what dictionary types are available on your system with
the DICTIONARIES command, for example:
> DICTIONARIES \n
> *3 \n
> +FRAM\n
> +FLASH\n
> +EEPROM \n

\par
Would indicate that EEPROM, FLASH, and FRAM are available on that system.
New "dictionaries", or persistent memory stores, are easily added to the Embedis server
by adding a handler with READ, WRITE, and COMMIT methods to your sketch.
\par
The "select" command is used to switch between different storage devices
Different memory types are selected using their device name, for example:
> select EEPROM\n
> +OK free = 8190\n

Would select an internal EEPROM for storage, and inform you that it has 8190 bytes of storage remaining.
(The current version of Embedis supports a maximum of 64K Keys, and uses two bytes of the 8196 bytes of the EEPROM
to indicate the key pointer (in this case, no or zero keys).

\par
A list of keys can be retrieved using the KEYS command for the selected memory :
> select EEPROM \n
> +OK free = 8190 \n
> keys \n
> *0 \n

In this case, we show zero keys, or an empty dictionary.

\par
Embedis starts writing from the high memory address
of the device, and writes "down" in addressing, from high to low memory addresses.
The reason for this is to be able to coexist with other uses of the same EEPROM, such
as the DeviceTree information for the auto-configuration of Socks, Hats, Capes, and Shields.

\par
Access to hardware resources are accomplished similarly using the READ and WRITE commands, for example: \n
> HARDWARE\n
> *4 \n
> +wifi \n
> +vcc \n
> +temp \n
> +blte \n
> \n
> READ VCC \n
> +3.335 \n
> \n
> READ TEMP \n
> +37.542 \n

/par
Several custom commands are shown in the Embedis examples for the Arduino, including pinMode, DigitalWrite, DigitalRead, and AnalogRead,
allowing you to read sensors and toggle I/O pins directly from the Embedis Command Line Interface. For example, to toggle the LED on and OFF
on an Arduino\n
> pinMode 13 OUTPUT \n
> +OK \n
> digitalWrite 13 HIGH \n
> +OK \n
> digitalWrite 13 LOW \n
> +OK \n

\par
For more information, see the Embedis Wiki at https://github.com/thingSoC/embedis/wiki
\par

