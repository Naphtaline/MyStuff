#include <ESP8266WiFi.h>
#include <string.h>

enum e_State { DISCONNECTED = -1, SOFT_AP = 0, CONNECTING_TO_WIFI, CONNECTED_TO_WIFI, RESETING};
int8_t m_State = DISCONNECTED;
WiFiServer m_WebServ(80);

String m_SSID = "";
String m_Pass = "";

void setup()
{
  Serial.begin(115200);
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

void WifiConnectection_Setup()
{
  Serial.print("Trying connect to ");
  Serial.println(m_SSID);
  
  WiFi.hostname("JeanMich");
  WiFi.begin(m_SSID, m_Pass);

  Serial.println();
  bool l_Reseting = false;
  for (int i = 0; i <= 30; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
      break;
    if (i >= 30)
      l_Reseting = true;
    Serial.print(".");
    delay(500);
  }
  if (l_Reseting)
  {
    m_State = RESETING;
    return;
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(m_SSID);

  StartWebServer();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  m_State = CONNECTED_TO_WIFI;
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
      //Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
      //Serial.println(WiFi.softAPIP());
      if (m_SSID != "")
        m_State = CONNECTING_TO_WIFI;
      //delay(3000);
      break;
    case CONNECTING_TO_WIFI:
      m_WebServ.stop();
      WifiConnectection_Setup();
      break;
    case CONNECTED_TO_WIFI:
      break;
    case DISCONNECTED:
      SoftAP_Setup();
      break;
    case RESETING:
      Reset();
      break;
  }
}
