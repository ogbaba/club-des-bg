#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include "mdp.h"

ESP8266WebServer server(80);
/*
IPAddress local_IP(192, 168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255, 255, 255, 0);
*/
//#define ADMIN_URL "url"
//#define THEWIFISSID "ssid"
//#define THEWIFIPWD "mdp"

String texteBase = "<style>\
img {width : auto; max-height: 200px;}\
</style>\
<h1>&#128023; Le club des BG &#128023; </h1>\
<form action=\"/\">\
<input type=\"submit\" style=\"height:50px;width:100px\" value=\"Rafraichir\" />\
</form>\
<a href=\"/merde\">Merde</a>\
<p>ERREUR : Acces Interdit, vous n'etes pas un bg</p>\
<form action=\"/msg\" method=\"post\">\
URL de l'image : <br>\
<input type=\"url\" name=\"image\"> <br>\
Message : <br>\
<textarea name=\"message\" cols=\"60\" rows=\"4\"></textarea><br>\
<input type=\"submit\" value=\"submit\">\
</form><hr>";

String adminHTML = "<h1>Controle de BG</h1>\
<a href=\"/\" >Chat</a> <br>\
<a href=\"/merde\"> Merde</a>\
<form action=\"/"ADMIN_URL"\" method=\"post\">\
Merde: <br>\
<textarea name=\"merde\" cols=\"80\" rows=\"40\"></textarea><br>\
<input type=\"submit\" value=\"submit\">\
</form>\
<form action=\"/"ADMIN_URL"\" method=\"post\">\
<button name=\"menage\" value=\"oui\">Nettoyer le chat</button>\
</form>";

String chat;
String merde = "Rien ici... Pour l'instant...";

int nbMsg = 0;

void handleRoot() {

  server.send(200, "text/html", texteBase + chat);
}

void handleMessage () {
  if (server.args() > 0)
  {
    ++nbMsg;
    if (nbMsg % 20 == 0)
    {
      chat = "";
    }
    String msg;
    String imgURL;
    String imgHTML;
    for (int i=0; i < server.args(); ++i)
    {
      if (server.argName(i) == "message")
      {
        msg = server.arg(i);
      }
      if (server.argName(i) == "image")
      {
        imgURL = server.arg(i);
      }
    }
    
    msg.replace("<", "&lt");
    msg.replace(">", "&gt");
    imgURL.replace("<", "&lt;");
    imgURL.replace(">", "&gt;");
    imgURL.replace("\"", "&quot;");
    imgHTML = (imgURL.length() == 0) ? "" : "<img src=\"" +
    imgURL + 
    "\" height=\"200\" width=\"200\"> ";
    chat = chat + "<b>N*" + nbMsg + "</b><br>" + imgHTML+ "<pre>" + msg + "</pre> <hr>";
  }
  server.sendHeader("Location","/");
  server.send(303);
}

void handleMerde () {
  server.send(200, "text/html", merde);
}

void handleAdmin () {
  if (server.args() > 0)
  {
    if (server.argName(0) == "menage")
    {
      chat = "";
      nbMsg = 0;
    }
    if (server.argName(0) == "merde")
    {
      merde = server.arg(0);
    }
  }
  server.send(200, "text/html", adminHTML);
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
  /*
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("ClubDesBG");
  */
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  // CHANGEME 
  WiFi.begin(THEWIFISSID, THEWIFIPWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  server.on("/"ADMIN_URL, handleAdmin);
  server.on("/merde", handleMerde);
  server.on("/", handleRoot);
  server.on("/msg", handleMessage);
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
