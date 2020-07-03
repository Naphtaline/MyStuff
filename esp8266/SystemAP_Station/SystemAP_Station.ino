#include <ESP8266WiFi.h>

enum e_State { DISCONNECTED = -1, SOFT_AP = 0, CONNECTED, RESETING};
byte m_State = DISCONNECTED;
WiFiServer m_WebServ(80);

void SetupSoftAp()
{
  Serial.println();
  Serial.print("Setting soft-AP ... \n");
  
  boolean l_Result = WiFi.softAP("ESPsoftAP_01", "toto12345");
  
  if(l_Result)
  {
    Serial.println("Soft_AP Ready");
    m_State = SOFT_AP;
    m_WebServ.begin();
  }
  else
  {
    Serial.println("Soft_AP Failed!");
    m_State = DISCONNECTED;
  }
}

void setup()
{
    Serial.begin(115200);

// MODE SOFT AP == STARTUP
  SetupSoftAp();
}

void SoftAP_ServePage(WiFiClient p_Client)
{
  char *l_Page = "<!DOCTYPE html><html><body><h2>HTML Forms</h2><form action=\"/action_page.php\"><label for=\"fname\">First name:</label><br><input type=\"text\" id=\"fname\" name=\"fname\" value=\"John\"><br><input type=\"submit\" value=\"Submit\"></form><p>If you click the \"Submit\" button, the form-data will be sent to a page called \"/action_page.php\".</p></body></html>";


  p_Client.print(l_Page); 
}

void SoftAP_HandleWebClient()
{
    WiFiClient l_Client = m_WebServ.available();
        Serial.println("la !");

    if (!l_Client)
      return;
    Serial.println("ici !");
    SoftAP_ServePage(l_Client);
}

void loop()
{
  switch (m_State)
  {
    case SOFT_AP:
      SoftAP_HandleWebClient();
      Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
      Serial.println(WiFi.localIP());
      delay(3000);
      break;
  }

}
