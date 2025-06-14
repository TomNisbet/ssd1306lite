# SSD1306lite

![display splash screen](images/display-splash.jpg)

SSD1306lite is a very lightweight Arduino hardware driver with text display methods for readily-available SSD1306-based I2C 128x64 OLED displays.

This code works with 128x64 I2C OLED displays and supports text and very basic bitmap drawing.  It does not support scrolling or arbitrary drawing functions.  It uses minimal RAM and does not require any support libraries.  A hello world sketch only requires a few bytes of RAM.

This code does not use the Arduino Wire library and requires no buffer space.  The low-level I2C code is based on SSD1306xLED from the [ATtiny85 tinusaur project by Neven Boyanov](https://bitbucket.org/tinusaur/ssd1306xled), which was itself inspired by [IIC_wtihout_ACK](http://www.14blog.com/archives/1358).

## Hardware connections

SSD1306 displays are native 3.3V devices, but many are advertised as being 5V compatible.  They seem to work fine using 5V for the Vcc, SCL, and SDA signals although this may shorten the life of the display.  Some displays will start to flicker if the Vcc is right at or slightly above 5V.

The SSD1306Lite library supports two modes for the displays.  In the simplest configurations, the display's Vcc is connected to the Arduino's 5V pin and the SCL and SDA signals are driven high and low by the Arduino's A5 and A4 pins.

A better configuration is to connect the display's Vcc to the Arduino's 3.3V pin. SCL
and SDA are still connected to A5 and A4, but a 4.7K pullup resistor is added from SCL to 3.3V and another from SDA to 3.3V from the Arduino.  In this configuration, the Arduino does not drive the output pins to get a high signal and the pullups provide the 3.3V needed.  This 3.3V configuration is a better match for the display and it is also more compatible with the I2C bus.

**If the pullup resistors are not used, uncomment the NO_PULLUPS define in the ssd1306lite.cpp file.**

## Demo sketch

A demo sketch is included to show off the features of the code.  Wire the SDA, SCL, and power pins to an Arduino.  Then open the src.ino file in the Arduino IDE to see a set of sample screens and animations.  This should run on an Uno or Nano with no code changes or configurations required.

This has been tested on the Arduino Uno and Nano using the default A4 and A5 ports for SDA and SCL.  It will work with other Arduinos as well, but the port definitions will need to be changed to match the hardware.  Instructions to do this are at the top of the ssd1306.cpp file.

## Features and limitations

Features:

* Text drawing with single and double-height fonts, 6x8 and 8x16
* Bitmap display from Arduino PROGMEM (EEPROM) storage
* Basic fill functions to draw some lines and shapes
* Extremely lightweight - no external libraries required

Limitations:

* Only supports SSD1306-based displays using i2C - no support for SPI displays
* All text and drawing is constrained to 8-bit boundaries in the vertical direction

## Using the code in a project

The code is not distributed as a library.  To incorporate it, follow these steps:

* Add the ssd1306lite cpp and h files to your project's directory along with the two font header files.  
* Include the main header file in your sketch file
* Declare an SSD1306Display object
* Initialize the display
* Call the needed display functions for your application

## Sample sketch

Below is the minimal hello world sketch

```C++
#include "ssd1306lite.h"

// Declare the global instance of the display
SSD1306Display display;

void setup() {
    // Initialize the display
    delay(50);
    display.initialize();

    // clear the screen and display text
    display.clear();
    display.text2x(0, 0, "Hello, world");
}

void loop() {
}
```

## Display coordinates

All of the calls use row and column addressing. There are eight rows, each containing eight vertical pixels (64 pixels total).  There are 128 one-pixel wide columns.

This matches the Display RAM in the SSD1306 controller.  Display RAM is changed by writing bytes to the controller, where each byte represents 8 vertical pixels.  The LSB of each byte is the top pixel in the row of eight.

This same format is used in the font and bitmap image files.  A set of 128 bytes writes 8 full lines of the display.  A set of 1024 bytes are needed to write all 128 columns by 64 lines.

The SSD1306lite text and fill methods write data immediately to the controller's Display RAM.  This allows it to use very little memory because it has no local buffer.  

The downside to this approach is that it restricts text to defined rows with 8 pixels boundaries. A text character can be drawn on the eight display rows 0..7 or 8..15 or up to 56..63, but it cannot be drawn on 3..10, for example.

If arbitrary text placement or more advanced graphics are needed, a different code base should be used that supports a local graphics buffer in the Arduino RAM.

## Sample Images

![display numbers](images/display-numbers.jpg)
![display bar graph](images/display-bars.jpg)
![display image](images/display-image.jpg)