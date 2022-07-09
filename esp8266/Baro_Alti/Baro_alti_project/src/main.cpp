#include <string.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>
#include <U8g2lib.h>                           // see https://github.com/olikraus/u8g2/wiki/u8g2reference
#include <heltec.h>
#include <Arduino.h>


////U8G2 area
/**/ U8G2_SSD1306_128X64_NONAME_F_HW_I2C       u8g2(U8G2_R0, 16, 15, 4);         // OLED graphics
/**/
/**/ void PrintDataOnScreen();
/**/ void PrintBattLevelOnScreen();
////end U8G2 area


////BMP area
/**/ Adafruit_BMP3XX bmp;
/**/ #define SEA_LEVEL_PRESSURE_HPA (1023.0f)
/**/
/**/ float g_temperature = 0.0f;
/**/ char g_temperature_str[7];
/**/ float g_current_pressure = 0.0f;
/**/ float g_altitude_m = 0.0f;
/**/ float g_altitude_ft = 0.0f;
/**/ float g_pressure_reference = SEA_LEVEL_PRESSURE_HPA;
/**/
/**/ #define FEET_IN_METER (0.3048f)
/**/ #define FEET_BY_HPA (30)
/**/
/**/bool ReadAndComputeData();
////end BMP area

////Battery area
/**/ //int g_batt_voltage_Pin = A4;  // Wifi Kit 32 shows analog value of voltage in A4
/**/ int g_batt_analog_val = 0;    // variable to store the value read
/**/ int g_batt_percentage = 0;
/**/
/**/ void ReadAndComputeBattLevel();
////end Battery are


void setup()
{
  Serial.begin(115200);
  char lineBuffer[128];

  //Serial.print("CPU Freq: ");
  //Serial.println(getCpuFrequencyMhz()); => default value 240Mhz

  // So we slow down the CPU because we don't need that much power, and the consumption is now  2x less than standard.
  setCpuFrequencyMhz(80);

  // consumption for : 240Mhz => 66.8mA
  // consumption for : 160Mhz => 45.9mA
  // consumption for : 80Mhz  => 33.2mA
  // consumption for : 40Mhz  => 19.88mA
  // consumption for : 20Mhz  => 15.43mA
  // consumption for : 10Mhz  => 13.19mA

  while (!Serial);
  Serial.println("Adafruit BMP388 / BMP390 test");
  Serial.println("Starting up");

  TwoWire *l_wireBMP = new TwoWire(1);
  l_wireBMP->begin(13,12);
  if (!bmp.begin_I2C((uint8_t)0x77, l_wireBMP))
  {   // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // OLED graphics.
  Serial.println("Call u8g2.begin");
  u8g2.begin();

  u8g2.setFont(u8g2_font_ncenB18_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  // Display the title
  u8g2.clearBuffer();
  sprintf(lineBuffer, "%s", "Napht");
  u8g2.drawStr(64 - (u8g2.getStrWidth(lineBuffer) / 2), 10, lineBuffer);
  u8g2.setFont(u8g2_font_ncenR10_tf);
  sprintf(lineBuffer, "%s", "altimeter");
  u8g2.drawStr(64 - (u8g2.getStrWidth(lineBuffer) / 2), 35, lineBuffer);
  u8g2.sendBuffer();
  
  // we need to init the barometer
  ReadAndComputeData();
  delay(500);
  ReadAndComputeData();
  delay(500);
  ReadAndComputeData();
  delay(500);

  // then we assign the reference value
  g_pressure_reference = g_current_pressure;

  delay(1500);

  //analogSetCycles(8);
  //analogSetSamples(1);
  //analogSetClockDiv(1);
  analogSetPinAttenuation(37,ADC_11db);
  //adcAttachPin(37);
}

void Reset()
{
  Serial.println("RESETING");
}

int timer = 0;

void loop()
{
  Serial.println("BEGIN LOOP");

  ReadAndComputeData();
  
  if (timer % 1000 == 0)
  {
    ReadAndComputeBattLevel(); // this take 200ms
    timer = 0;
    delay(50);
  }
  else
    delay(250);

  timer += 250;

  PrintDataOnScreen();

  Serial.println("END\n");
}

void PrintDataOnScreen()
{
// DISPLAY PRINT
  char lineBuffer[128];
  
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_lubB18_tf );
  sprintf(lineBuffer, "%d ft", (int)g_altitude_ft);
  u8g2.drawStr(0, 0, lineBuffer);

  u8g2.setFont(u8g2_font_lubR10_tf);
  sprintf(lineBuffer, "%d m", (int)g_altitude_m);
  u8g2.drawStr(0, 20, lineBuffer);

  sprintf(lineBuffer, "%s C", g_temperature_str);
  u8g2.drawStr(128 - u8g2.getStrWidth(lineBuffer), 52, lineBuffer);
  
  sprintf(lineBuffer, "%d", g_batt_analog_val);
  u8g2.drawStr(128 - u8g2.getStrWidth(lineBuffer), 0, lineBuffer);
  
  sprintf(lineBuffer, "%d\%", g_batt_percentage);
  u8g2.drawStr(128 - u8g2.getStrWidth(lineBuffer), 20, lineBuffer);


  u8g2.sendBuffer();


// SERIAL PRINT
  Serial.print("Ref pressure ");
  Serial.println(g_pressure_reference);
  Serial.print("Read pressure ");
  Serial.println(g_current_pressure);

  Serial.println("");
  Serial.print(g_altitude_m);
  Serial.print(" m");

  Serial.println("");
  Serial.print(g_altitude_ft);
  Serial.println(" ft");

}

bool ReadAndComputeData()
{
  if (! bmp.performReading())
  {
    Serial.println("Failed to perform reading :(");
    return false;
  }

  g_temperature = floorf((bmp.temperature - 2.0f) * 10) / 10;

  dtostrf(g_temperature, 2, 2, g_temperature_str);

  g_current_pressure = bmp.pressure / 100.0f;
  g_altitude_ft = (g_pressure_reference - g_current_pressure) * FEET_BY_HPA;
  g_altitude_m = g_altitude_ft * FEET_IN_METER;
  return true;
}

void ReadAndComputeBattLevel()
{
  int average_value = 0;
  for (int i = 0; i < 20; i++)
  {
    //Serial.printf("Battery power in GPIO 37: ");
    //Serial.println(analogRead(37));
    average_value += analogRead(37);
    delay(10);
  }
  g_batt_analog_val = average_value /20;
  g_batt_percentage = (g_batt_analog_val - 2064) / 2.78;
  Serial.println("Vbat : " + (String)average_value);

}
