#include <NTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "img.h"
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

struct timespec *tp;


String header = "<head>\
<title>Club des BG</title>\
<meta charset=\"UTF-8\">\
<style>\
img {width : auto; max-height: 200px;}\
#ecriture {text-align: center;}\
.hop, .leon, .poivron, .fleon{ width : 16px; height : 16px; display : inline-block;}\
body { background-color : Lavender; font-size: 150%;}\
.poivron { background-image : url("POIVRON");}\
.hop { background-image : url("HOP");}\
.leon { background-image : url("LEON");}\
.fleon { background-image : url("FLEON");}\
</style>\
</head>";

String texteBase = "<div id=\"ecriture\">\
<h1>&#128023; Le club des BG &#128023; </h1>\
<form action=\"/\">\
<input type=\"submit\" style=\"height:50px;width:100px\" value=\"Rafraichir\" />\
</form>\
<a href=\"/merde\">Merde</a>\
<p>ERREUR : Accès Interdit, vous n'etes pas un bg</p>\
<form action=\"/msg\" method=\"post\">\
URL de l'image : <br>\
<input type=\"url\" maxlength=\"200\" name=\"image\"> <br>\
Message : <br>\
<textarea name=\"message\" maxlength=\"500\" cols=\"60\" rows=\"4\"></textarea><br>\
<input type=\"submit\" value=\"Envoyer\">\
</form></div><hr>";

String adminHTML = "<h1>Controle de BG</h1>\
<a href=\"/\" >Chat</a> <br>\
<a href=\"/merde\"> Merde</a>\
<form action=\"/"ADMIN_URL"\" method=\"post\">\
Merde: <br>\
<textarea name=\"merde\" cols=\"80\" rows=\"40\"></textarea><br>\
<input type=\"submit\" value=\"Envoyer\">\
</form>\
<form action=\"/"ADMIN_URL"\" method=\"post\">\
<button name=\"menage\" value=\"oui\">Nettoyer le chat</button>\
</form>\
<form>Message Admin : <br>\
<textarea name=\"msgAdmin\" cols=\"60\" rows=\"4\"></textarea><br>\
<input type=\"submit\" value=\"Envoyer\">\
</form>\
<form>Epingle : <br>\
<textarea name=\"epingle\" cols=\"60\" rows=\"4\"></textarea><br>\
<input type=\"submit\" value=\"Envoyer\">\
</form>";


String chat;
String merde = "Rien ici... Pour l'instant...";
String epingle;

int nbMsgAff = 0;
int nbMsgReel = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


void handleRoot() {
  server.send(200, "text/html", "<!DOCTYPE html>" + header + "<body>" +
  texteBase + epingle + "<hr>" + chat + "</body></html>");
}

void handleMessage () {
  if (server.args() > 0)
  {
    verifTailleMsgs();
    String msg;
    String imgURL;
    String imgHTML;
    for (int i=0; i < server.args(); ++i)
    {
      if (server.argName(i) == "message")
      {
        if (server.argName(i).length() > 550) 
        {
          tropGros();
          return;
        }
        msg = server.arg(i);
      }
      if (server.argName(i) == "image")
      {
        if (server.argName(i).length() > 220){
          tropGros();
          return;
        }
        imgURL = server.arg(i);
      }
    }
    
    msg = formatMsg (msg);
    imgURL.replace("<", "&lt;");
    imgURL.replace(">", "&gt;");
    imgURL.replace("\"", "&quot;");
    if ((imgURL.length() == 0) && (msg.length() == 0))
    {
      server.send(200, "text/html", "<b>Message trop court...</b>");
      return;
    }
    imgHTML = (imgURL.length() == 0) ? "" : "<img src=\"" +
    imgURL + 
    "\" height=\"200\" width=\"200\"> ";
    //TEMPS
      timeClient.update();
    
    chat = chat + "<b>N*" + nbMsgReel + " " + timeClient.getFormattedTime() + 
    "</b><br>" + imgHTML+ "<pre>" + msg + "</pre> <hr>";    
    ++nbMsgReel;
    ++nbMsgAff;
  }
  
  server.sendHeader("Location","/");
  server.send(303);
}

String formatMsg (String msg) {
    msg.replace("<", "&lt");
    msg.replace(">", "&gt");
    msg.replace(":hop:","<div class=\"hop\"></div>");
    msg.replace(":leon:","<div class=\"leon\"></div>");
    msg.replace(":fleon:","<div class=\"fleon\"></div>");
    msg.replace(":poivron:","<div class=\"poivron\"></div>");
    return msg;
}

void verifTailleMsgs () {
    if (nbMsgAff >= 20)
    {
      nbMsgAff = 0;
      chat = "";
    }
}

void handleMerde () {
  server.send(200, "text/html", merde);
}

void tropGros(){
  server.send(413,"text/html", "Trop gros mon ami");
}

void handleAdmin () {
  if (server.args() > 0)
  {
    if (server.argName(0) == "menage")
    {
      nbMsgAff = 0;
      chat = "<b style=\"color:red;\" >MESSAGES ADMINISTRES</b> <hr>";
    }
    if (server.argName(0) == "merde")
    {
      merde = server.arg(0);
    }
    if (server.argName(0) == "msgAdmin")
    {
      verifTailleMsgs();
      String msg = server.arg(0);
      msg = formatMsg(msg);
      chat = chat + "<b>N*" + nbMsgReel + " " + timeClient.getFormattedTime() + 
    " ADMIN</b><br> <p style=\"color:red;\">" + server.arg(0) + "</p> <hr>";    
      ++nbMsgAff;
      ++nbMsgReel;
    }
    if (server.argName(0) == "epingle")
    {
      epingle = server.arg(0);
    }
  }
  server.send(200, "text/html","<!DOCTYPE html><html>" + header + "<body>" + adminHTML + "</body></html>");
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
  //TEMPS
  timeClient.begin();
  timeClient.setTimeOffset(60*60*2);
  Serial.println(WiFi.localIP());
  server.on("/"ADMIN_URL, handleAdmin);
  server.on("/merde", handleMerde);
  server.on("/", handleRoot);
  server.on("/msg",HTTP_POST, handleMessage);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
