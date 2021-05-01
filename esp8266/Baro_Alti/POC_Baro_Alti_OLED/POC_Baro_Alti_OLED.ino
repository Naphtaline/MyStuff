#include <ESP8266WiFi.h>
#include <string.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h" // this lib has been modified to support 64x48 display
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define NUMFLAKES 1
#define XPOS 0
#define YPOS 1
#define DELTAY 2

uint8_t icons[NUMFLAKES][3];

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

enum e_State { DISCONNECTED = -1, SOFT_AP = 0, RESETING};
int8_t m_State = DISCONNECTED;
WiFiServer m_WebServ(80);

Adafruit_BMP3XX bmp;
#define SEA_LEVEL_PRESSURE_HPA (1015.0f)

float g_temperature = 0.0f;
float g_current_pressure = 0.0f;
float g_altitude = 0.0f;
float g_pressure_reference = SEA_LEVEL_PRESSURE_HPA;

#define FEET_IN_METER (0.3048f)
#define FEET_BY_HPA (30)

void setup()
{
  Serial.begin(115200);

  while (!Serial);
  Serial.println("Adafruit BMP388 / BMP390 test");
  Serial.println("Starting up");

  if (!bmp.begin_I2C())
  {   // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

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
  char final_array[120];
  memset(final_array, 0, 120);

  String final_string = "<!DOCTYPE html><html><body><h2>Data sensor values</h2><h3>Temp : ";
  String temp = String(13);
  String middle = "</h3><h3>Altitude : ";
  String prssure = String(1005);
  String middle2 = "</h3><h3>Altitude : ";

  final_string = final_string + temp + middle + prssure + middle2;
  if (final_string.length() <= 120)
    final_string.toCharArray(final_array, final_string.length());

  p_Client.print(final_array);
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

  l_Client.flush();
}


void Reset()
{
  Serial.println("RESETING");
  m_WebServ.stop();
  WiFi.disconnect(true);
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
  }

  ReadAndComputeData();
  PrintDataOnScreen();
  delay(1000);
}

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

void PrintDataOnScreen()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.print(g_temperature, 1);
  display.println(" C");

  display.println();
  display.print(g_current_pressure, 1);
  display.println(" hpa");

  display.println();
  display.print(g_altitude);
  display.println("m");

  Serial.println("alt: ");
  Serial.print(g_altitude);
  Serial.print(" m");

  display.display();
}

void ReadAndComputeData()
{
  if (! bmp.performReading())
  {
    Serial.println("Failed to perform reading :(");
    return;
  }
  g_temperature = bmp.temperature;
  g_current_pressure = bmp.pressure / 100.0, 1;
  Serial.println();
  Serial.println(g_pressure_reference);
  Serial.println(g_current_pressure);
  Serial.println();
  g_altitude = (g_pressure_reference - g_current_pressure) * FEET_BY_HPA * FEET_IN_METER;
}

void DoVerticalScrool(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
  // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++)
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++)
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height())
      {
        icons[f][XPOS] = random(display.width() - (w / 2));
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
    //delay(2000);
}
