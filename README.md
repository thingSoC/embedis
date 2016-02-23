# Embedis - Embedded Dictionary Server [![Build Status](https://travis-ci.org/thingSoC/embedis.png?branch=master)](https://travis-ci.org/thingSoC/embedis)

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

<span class="badge-paypal"><a href="https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&amp;hosted_button_id=5NPC24C7VQ89L" title="Donate to this project using Paypal"><img src="https://img.shields.io/badge/paypal-donate-yellow.svg" alt="PayPal donate button" /></a></span>

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
 * Tom Moxon
 * David Turnbull

## Community Supported

[Embedis](https://github.com/thingSoC/embedis) and  [thingSoC](http://www.thingsoc.com) are community supported, you can help by donating to support this work.

<span class="badge-paypal"><a href="https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&amp;hosted_button_id=5NPC24C7VQ89L" title="Donate to this project using Paypal"><img src="https://img.shields.io/badge/paypal-donate-yellow.svg" alt="PayPal donate button" /></a></span>

