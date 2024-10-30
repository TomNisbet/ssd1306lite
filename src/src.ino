#include "ssd1306lite.h"

// Test image - only needed for the demo
#include "img_nand_128x64.h"

// Declare the global instance of the display
SSD1306Display display;

// 8-byte patterns to draw diagonal lines
static const uint8_t diag1[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static const uint8_t diag2[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

void setup() {
    // Start up the display and clear the screen
    delay(50);
    display.initialize();
    display.clear();
}

void loop() {
    // Display the splash screen and then do some screen inverts to flash it
    splashScreen(0);
    delay(2000);
    flash(3, 300);

    // Clear the screen and then do inverts to flash it
    display.fillScreen(0);
    flash(2, 300);

    // Draw text in both sizes
    textDemo();
    delay(5000);

    // Sample display with updating number values
    numberDemo();
    delay(50);

    // Sample display using the byte fill methods to make simple bar graph meters
    barDemo();
    delay(50);

    // Paint the splash screen again, but put delays between each step to show how
    // it is constructed
    splashScreen(1500);
    delay(2000);

    // Centered text - text can be vertically positioned on pixel boundaries
    centerDemo();
    delay(7000);

    // Use the byte fill methods to draw horizontal lines
    horizontalLinesDemo(40);
    delay(1000);

    // Copy a bitmap image to the full screen
    display.drawImage(0, 0, 8, 128, img_nand_128x64);
    delay(5000);
}


void splashScreen(int wait) {
    display.clear();
    delay(wait);
    // Fill the whole screen with diagonal lines
    display.fillAreaWithBytes(0, 0, 8, 128, diag1, sizeof(diag1));
    delay(wait);
    // Fill the top left quadrant with diagonal lines in the opposite direction
    display.fillAreaWithBytes(0, 64, 4, 64, diag2, sizeof(diag2));
    delay(wait);
    // Also fill the bottom right with the same 
    display.fillAreaWithBytes(4, 0, 4, 64, diag2, sizeof(diag2));
    delay(wait);
    // Display text in the middle of the screen
    display.text2x(3, 20, "SSD1306lite");
    delay(wait);

}

void flash(unsigned count, unsigned wait) {
    while (count--) {
        display.invertScreen(true);
        delay(wait);
        display.invertScreen(false);
        delay(wait);
    }
}

void textDemo() {
    display.text2x(0, 0, "ABCDEFGHIJKLMNOPcliped");
    display.text2x(2, 0, "abcdefghijklmnopcliped");
    display.text(4, 0, "ABCDEFGHIJKLMNOPQRSTUEXTRA_TEXT_CLIPPED");
    display.text(5, 0, "abcdefghijklmnopqrstuEXTRA_TEXT_CLIPPED");
    display.text2x(6, 0, "0123456789+-*/#&");
}


void center(uint8_t row, const char * s) {
    uint8_t col = 64 - (strlen(s) * 6 / 2);
    display.text(row, col, s);
}

void center2x(uint8_t row, const char * s) {
    uint8_t col = 64 - (strlen(s) * 8 / 2);
    display.text2x(row, col, s);
}

void centerDemo() {
    display.clear();
    center2x(0, "AAAAA");
    center2x(2, "BBB");
    center(5, "text must be placed");
    center(6, "on 8 pixel rows, but");
    center(7, "can be on any column");
}

void horizontalLinesDemo(unsigned wait) {
  // draw a horizontal line all the way across the topmost and bottommost row
  // of pixels and then move the lines together until they meet in the middle
    for (uint8_t ix = 0; (ix < 32); ix++) {
        display.fillAreaWithByte(ix >> 3, 0, 1, 128, 1 << (ix & 7));
        display.fillAreaWithByte(7 - (ix >> 3), 0, 1, 128, 0x80 >> (ix & 7));
        delay(wait);
        if ((ix & 7) == 7) {
            // switching to a new display row, so overwrite the current row to
            // all zeroes so that the last line of each row does not remain
            display.fillAreaWithByte(ix >> 3, 0, 1, 128, 0);
            display.fillAreaWithByte(7 - (ix >> 3), 0, 1, 128, 0);
        }
    }
}


void numberDemo() {
    // Draw a screen with updating numbers
    display.clear();
    display.text2x(0, 0, "Speed      RPM");
    display.text2x(2, 0, "  00      2500");
    display.text2x(4, 0, "Temp     Voltage");
    display.text2x(6, 0, " 120       12.4");
    char s[5];
    for (unsigned x = 0; (x <= 85); x += 1 + (x >> 4)) {
        s[0] = '0' + x / 10;
        s[1] = '0' + x % 10;
        s[2] = '\0';
        display.text2x(2, 2*8, s);
        unsigned r = x * 2;
        s[0] = '2';
        s[1] = '5' + r / 100;
        s[2] = '0' + r / 10 % 10;
        s[3] = '0' + r % 10;
        s[4] = '\0';
        display.text2x(2, 10*8, s);
        delay(250);    
    }
}


void drawBar(uint8_t row, uint8_t col, uint8_t value) {
    const uint8_t THRESHOLD = 75;
    if (value > 100)  value = 100;

    // Draw a horizontal bar up to 100 pixels long using bwo bytes of alternating bits to
    // get a grey bar effect.  Note that it doesn't matter that the byte pattern is two
    // bytes but value could be an odd number because the fill method will clip the drawing
    // the appropriated size.
    display.fillAreaWithBytes(row, col, 1, value, "\x55\xaa", 2);
    if (value > THRESHOLD) {
        // If value exceeds the user-defined threshold, overwrite the end of the bar with
        // a solid pixel bar to differentiate from the grey pattern.
        display.fillAreaWithByte(row, col + THRESHOLD, 1, value - THRESHOLD, 0xff);
    }
    // Write all zeros to the unused area at the end of the bar to erase any pixels
    // remaining from a previous value that may have been larger.
    display.fillAreaWithByte(row, col + value, 1, 100 - value, 0x00);
}

void drawBarData(const uint8_t data[], unsigned size) {
    // Draw to bars froma set of value pairs
    for (unsigned ix = 0; (ix < size); ix += 2) {
        drawBar(4, 28, data[ix]);
        drawBar(7, 28, data[ix+1]);
        delay(70);
    }
}

const uint8_t barData1[] = { 50,55, 47,56, 70,73, 60,55, 65,65, 75,88, 100,97, 100,100, 30,22, 5,30 };
const uint8_t barData2[] = { 0,0, 5,7, 7,10, 0,0, 3,4, 0,0};

void barDemo() {
    // Draw the header line
    display.clear();
    display.invertData(true);
    display.fillAreaWithByte(0, 0, 2, 128, 0);
    display.text2x(0, 12, "Signal Levels");
    display.invertData(false);

    // Draw the row labels
    display.text2x(3, 0, "L:");
    display.text2x(6, 0, "R:");

    // Plot a set of data values.  The for loop is used to repeat the data
    // a few times
    for (int count = 0; (count < 6); count++) {
      drawBarData(barData1, sizeof(barData1));
    }
    // Draw a second set of data to break up the repetitive data from the loop above
    drawBarData(barData2, sizeof(barData2));
    drawBarData(barData2, sizeof(barData2));
}