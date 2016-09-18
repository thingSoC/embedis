# Embedis - Embedded Dictionary Server [![Build Status](https://travis-ci.org/thingSoC/embedis.png?branch=master)](https://travis-ci.org/thingSoC/embedis)

Embedis is an open source library for creating simple embedded dictionary servers.
Embedis is used for embedding key-value dictionaries into the
SRAM, FLASH, FRAM, NVSRAM, EEPROM, or SDcard memory systems
of small, limited resource, embedded computing platforms,
such as the Arduino(AVR), Teensy(Cortex-M4), ESP8266, and others.

We needed to support several different persistent memory store types,
for a number of "Internet of Things", and small embedded system projects.

These types include the internal processor/SoC based SRAM, FLASH, EEPROM,
and Scratchpad SRAM memories, as well as externally attached FLASH, EEPROM,
FRAM, NVSRAM and SDCard memories.

Embedis was developed to provide a simple, consistent command line interface, and APIs
for storing and retrieving data from pins, sensors, interfaces, and other I/O devices,
across a number of different embedded platforms and IDEs.

## Installing Embedis 

Use the Arduino Library Installer :
> Sketch -> Include Library -> Manage Libraries

[![Install](http://thingsoc.github.io/img/projects/embedis/library_manager.png?raw=true)  
*Library Manager*](http://thingsoc.github.io/embedis/wiki)

Under "Filter Search", enter "embedis" :

[![LibMgr](http://thingsoc.github.io/img/projects/embedis/libraries.png?raw=true)  
*Search for Embedis*](http://thingsoc.github.io/embedis/wiki)


and Click on the "Install" Button :

[![LibMgr](http://thingsoc.github.io/img/projects/embedis/library_search.png?raw=true)  
*Install*](http://thingsoc.github.io/embedis/wiki)

Now you can explore the Embedis examples under :

> Files -> Examples -> Embedis -> Embedis

Note Bene : The default example uses EEPROM only, and will only function
on boards with support for internal EEPROM (i.e. Arduino Uno/Leo/Mega/etc.),
or boards with support for emulated EEPROM, such as the ESP8266.
There are specific examples for boards with different capabilities, such as the Arduino Due,
that do NOT include EEPROM internal to the microprocessor.

## Using Embedis

The Embedis command line interface (CLI) uses familiar SET/GET/DEL commands
for storing, retriving, and deleting key-value pairs in the persistant memory stores.
For example :

    ```
    set mykey somevalue
    +OK
    get mykey
    +somevalue
    del mykey
    :1
    ```

For more information on uisng Embedis, please see the [-> WIKI <-](https://github.com/thingSoC/embedis/wiki/)

## Embedis Development Environment

The git repository includes a submodule for the testing framework and a
branch ['gh-pages'](http://thingSoC.github.io/embedis) for the development generated documentation. 
The recommended commands to use are as follows:

```
git clone {origin_url} embedis
cd embedis
git submodule init
git submodule update
git clone {origin_url} html --branch gh-pages
```

You'll need a few tools to build the tests and documentation. These are
common tools available for all operating systems so you shouldn't have
too much trouble getting them installed.

 * [CMake](http://www.cmake.org)
 * [Doxygen](http://www.doxygen.org)

CMake can create a typical Makefile as well as project files for Xcode,
Visual Studio, and many others. Here's a quick start for Makefile users:

```
$ cmake .
$ make && ctest
$ make doc
```

## Contributors

The folks who make this project possible:

 * PatternAgents, LLC
 * David Turnbull
 * Tom Moxon
 

## Community Supported

[Embedis](https://github.com/thingSoC/embedis) and  [thingSoC](http://www.thingsoc.com) are community supported, you can help by donating to support this work.

<span class="badge-paypal"><a href="https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&amp;hosted_button_id=5NPC24C7VQ89L" title="Donate to this project using Paypal"><img src="https://img.shields.io/badge/paypal-donate-yellow.svg" alt="PayPal donate button" /></a></span>

-------------------------------------------------------
