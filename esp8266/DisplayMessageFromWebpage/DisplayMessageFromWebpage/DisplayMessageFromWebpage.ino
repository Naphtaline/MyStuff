#include <ESP8266WiFi.h>
#include <string.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h" // this lib has been modified to support 64x48 display

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

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

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

enum e_State { DISCONNECTED = -1, SOFT_AP = 0, RESETING};
int8_t m_State = DISCONNECTED;
WiFiServer m_WebServ(80);

String m_SSID = "";
String m_Pass = "";

void setup()
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.display();
  display.clearDisplay();
  SetupVerticalScroll();

}

void StartWebServer()
{
  m_WebServ.begin();
}

void SoftAP_Setup()
{
  Serial.println();
  Serial.print("Setting soft-AP ... \n");
  
  IPAddress ip(192, 168, 1, 35);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(ip, gateway, subnet);
  boolean l_Result = WiFi.softAP("ESPsoftAP_01", "toto12345");

  if (l_Result)
  {
    Serial.println("Soft_AP Ready");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    m_State = SOFT_AP;
    StartWebServer();
  }
  else
  {
    Serial.println("Soft_AP Failed!");
    m_State = DISCONNECTED;
  }
}

void SoftAP_ServePage(WiFiClient p_Client)
{
  char *l_Page = "<!DOCTYPE html><html><body><h2>Add wifi credentials</h2><form action=\"\"><label for=\"fname\">ssid</label><br><input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"\"><br><label for=\"lname\">password:</label><br><input type=\"text\" id=\"password\" name=\"password\" value=\"\"><br><br><input type=\"submit\" value=\"Submit\"></form></body></html>";

  p_Client.print(l_Page);
}

void SoftAP_HandleWebClient()
{
  WiFiClient l_Client = m_WebServ.available();

  if (!l_Client)
    return;
  SoftAP_ServePage(l_Client);

  // request look like : "GET /?ssid=qwerty&password=12345 HTTP/1.1"
  String l_Request = l_Client.readStringUntil('\r');
  
  char *l_ReqCopy = (char*)malloc(l_Request.length());
  for (int i = 0; i < l_Request.length(); i++)
    l_ReqCopy[i] = l_Request.c_str()[i];

  Serial.print("Request====>");
  Serial.println(l_ReqCopy);
  if (strncmp(l_ReqCopy, "GET /?ssid=", 11) == 0)
  {
    // get ssid
    char *l_SsidPtr = l_ReqCopy;
    while (*l_SsidPtr != '=')
      l_SsidPtr++;
    m_SSID = strtok(l_SsidPtr, "&") + 1;

    // get Password
    char *l_PassPtr = l_SsidPtr + 1;
    while (*l_PassPtr != '=')
      l_PassPtr++;
    m_Pass = strtok(l_PassPtr, " ") + 1;

    Serial.print("ssid====>");
    Serial.println(m_SSID);
    Serial.print("pass====>");
    Serial.println(m_Pass);
    free(l_ReqCopy);
  }
  l_Client.flush();
}


void Reset()
{
  Serial.println("RESETING");
  m_WebServ.stop();
  WiFi.disconnect(true);
  m_SSID = "";
  m_Pass = "";
  m_State = DISCONNECTED;
  Serial.println("DISCONNECTED FROM EVERYWHERE");
}

void loop()
{
  switch (m_State)
  {
    case SOFT_AP:
      SoftAP_HandleWebClient();
      break;
    case DISCONNECTED:
      SoftAP_Setup();
      break;
    case RESETING:
      Reset();
      break;
  }
  DoVerticalScrool(N_image, 24, 25);
}

#define NUMFLAKES 1
#define XPOS 0
#define YPOS 1
#define DELTAY 2

uint8_t icons[NUMFLAKES][3];

void SetupVerticalScroll()
{
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
}

void DoVerticalScrool(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
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
