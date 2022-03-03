#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

int relay = 1;

ESP8266WebServer server(80);
 

void handleRootPath() {
  server.send(200, "text/plain", "Dead simple http server");
}

void handleOnPath() {
  digitalWrite(relay, HIGH);
  server.send(200, "text/plain", "on");
}

void handleOffPath() {
  digitalWrite(relay, LOW);
  server.send(200, "text/plain", "off");
}

void setup() {
 
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  WiFi.begin("Bbox-A259B8D5", "rantanplan");  //Connect to the WiFi network
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
 
    delay(500);
    Serial.println("Waiting to connect…");
 
  }
 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP
 
  server.on("/other", []() {   //Define the handling function for the path
 
    server.send(200, "text / plain", "Other URL");
 
  });
 
  server.on("/", handleRootPath);    //Associate the handler function to the path
  server.on("/relayon", handleOnPath);    //Associate the handler function to the path
  server.on("/relayoff", handleOffPath);    //Associate the handler function to the path
  server.begin();                    //Start the server
  Serial.println("Server listening");
 
}
 
void loop() {
  server.handleClient();         //Handling of incoming requests
}
