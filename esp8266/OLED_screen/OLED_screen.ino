#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h" // this lib has been modified to support 64x48 display

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 1
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

// 24 x 25
const unsigned char N_image [] = {
B00001100, B00110000, B00000000,
B00001100, B00110000, B00000000,
B00110011, B00111100, B00000000,
B00110011, B00111100, B00000000,
B11000000, B11110011, B00000000,
B11000000, B11110011, B00000000,
B11000000, B00110000, B11000000,
B11000000, B00110000, B11000000,
B11000000, B00000000, B11000000,
B11000011, B00000000, B11000000,
B11000011, B00000000, B11000000,
B11000011, B11000000, B11000000,
B11000011, B11000000, B11000000,
B11000011, B00110000, B11000000,
B11000011, B00110000, B11000000,
B11000011, B00110000, B11000000,
B11000011, B00110000, B11000000,
B11000011, B00110000, B11000000,
B11000011, B00110000, B11000000,
B00110011, B00110011, B00000000,
B00110011, B00110011, B00000000,
B00001111, B00111100, B00000000,
B00001111, B00111100, B00000000,
B00000011, B00110000, B00000000,
B00000011, B00110000, B00000000
};

// 32 x 14
const unsigned char apth_image [] = {
B00000000, B00000000, B00010000, B01000000,
B00000000, B00000000, B00110000, B11000000,
B00000000, B00000000, B01110001, B11000000,
B00111000, B00011100, B00110000, B11001100,
B01111100, B00101110, B01111100, B11011110,
B10001100, B01100111, B00110000, B11100110,
B00111100, B11100011, B00110000, B11000110,
B01001100, B01100011, B00110000, B11000110,
B11001100, B01110011, B00110100, B11000110,
B11001110, B01111010, B00111000, B11000111,
B01111100, B01101100, B00110000, B11000110,
B00000000, B01100000, B00000000, B00000000,
B00000000, B01110000, B00000000, B00000000,
B00000000, B01100000, B00000000, B00000000
};


#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  display.clearDisplay();
  display.drawBitmap(5, 12,  N_image, 24, 25, 1);
  display.drawBitmap(26, 17,  apth_image, 32, 14, 1);
  display.display();
  delay(2000);
  display.clearDisplay();

  // Clear the buffer.
/*
  // draw a single pixel
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
  display.clearDisplay();

  // miniature bitmap display
  display.display();
  delay(1);

  // invert the display
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
  display.clearDisplay();
  */
  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(N_image, 24, 25);
}


void loop() {

}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width() - (w / 2));
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
  delay(1);
}

void testscrolltext(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
  delay(1);

  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}
