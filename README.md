# Embedis - Embedded Dictionary Server [![Build Status](https://travis-ci.org/thingSoC/embedis.png?branch=master)](https://travis-ci.org/thingSoC/embedis)

## Installing Embedis 

Use the Arduino Library Installer :

> Sketch -> Include Library -> Manage Libraries

Under "Filter Search", enter "Embedis"
and Click on the "Install" Button


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

[Embedis](https://github.com/thingSoC/embedis) and  [thingSoC](http://www.thingsoc.com) are community supported, you can help by contributing.

<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="hosted_button_id" value="5NPC24C7VQ89L">
<input type="image" src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form>

