#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

ESP8266WebServer server(80);

IPAddress local_IP(192, 168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255, 255, 255, 0);

String texte = "<h1>Le club des BG</h1> <p>ERREUR : Acces Interdit, vous n'etes pas un bg</p>\
<form action=\"/\" method=\"post\">\
Message: <br>\
<input type=\"text\" name=\"message\"><br>\
<input type=\"submit\" value=\"submit\">\
</form>\n";
  
void handleRoot() {

  if (server.args() > 0)
  {
    texte = texte + "<p>" + server.arg(0) + "</p>\n ----- \n";
  }
  server.send(4000, "text/html", texte);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  // put your setup code here, to run once:
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("LeEssePet", "motdepasse");
  server.on("/", handleRoot);
  server.on("/inline", [](){
    server.send(200, "text/html", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
