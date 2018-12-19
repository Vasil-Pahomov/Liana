#include <WebSocketsServer.h>

#include "palette.h"

#include "anim.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


#define ANIMS 7 //number of animations (not including start one) to cycle randomly
#define PALS 8 //number of palettes
#define INTERVAL 30000 //change interval, msec

//#define USE_START_ANIMATION //start animation is used in cycling as well as other animations

#define WIFI_SSID "WiFiKVNR2"
#define WIFI_PASS "R@dm!la V@r0n!ca"

#define WIFI_AP_SSID "Liana"
#define WIFI_AP_PASS "ws2812"

Palette * pals[PALS] = {&PalRgb, &PalRainbow, &PalRainbowStripe, &PalParty, &PalHeat, &PalFire, &PalIceBlue, &PalXMas};

Anim anim = Anim();

unsigned long ms = 10000;//startup animation duration, 10000 for "release" AnimStart

int paletteInd = random(PALS);
int animInd = 0;

ESP8266WebServer server(80);
WebSocketsServer webSocket(81); 

const char HTML[] PROGMEM = 
"<!DOCTYPE HTML><html lang=\"ru-RU\"><head><meta charset=\"utf-8\"/><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><script>const ESP_SRV=\"\";const ESP_WS=\"ws://\" + location.hostname + \":81/\";let suspendTimer;var connection=new WebSocket(ESP_WS, ['arduino']);function suspend(){sendCmd(\"/sus\").then(function(response){return response.json();}).then(function(data){document.body.style.display=\"\";document.getElementById('animSelect').value=data.a;document.getElementById('palSelect').value=data.p;console.log(\"a=\" + data.a + \",p=\" + data.p);document.getElementById('onBox').checked=(data.a >=0);document.getElementById('mainControls').style.display=(data.a >=0) ? '' : 'hidden';});}function onAnimPalChange(){sendCmd(\"/set?a=\" + document.getElementById('animSelect').value + \"&p=\" + document.getElementById('palSelect').value);}function sendCmd(url){if (suspendTimer){clearTimeout(suspendTimer);}return fetch(ESP_SRV + url).then(function(response){if (response.ok){suspendTimer=window.setTimeout(suspend, 5000);return response;}else{throw new Error();}}).catch(function(err){alert('Error communicating!');suspendTimer=window.setTimeout(suspend, 500);throw new Error(err);});;}function onOnBoxChange(){var mainControlsDiv=document.getElementById('mainControls'); if (document.getElementById('onBox').checked){mainControlsDiv.style.display='';document.getElementById('animSelect').value=0;onAnimPalChange();}else{mainControlsDiv.style.display='none';sendCmd(\"/set?a=-1&p=\" + document.getElementById('palSelect').value);}}function handleOrientation(e){if (e.beta > -45 && e.beta < 45){var b=Math.floor((Number(e.beta) + 45)*255/90); var g=Math.floor((Number(e.gamma) + 90)*255/180); var pos=b + (g << 8); var posstr='P' + pos.toString(); console.log('b=' + b + ',g' + g + '=' + posstr); connection.send(posstr);}document.getElementById('accelControls').innerText='a=' + e.alpha + ', b=' + e.beta + ', g=' + e.gamma;}document.addEventListener(\"DOMContentLoaded\", start);function sendMP(){}function start(){window.addEventListener(\"deviceorientation\", handleOrientation, true);connection.onopen=function (){connection.send('Connect ' + new Date());};connection.onerror=function (error){console.log('WebSocket Error ', error);};connection.onmessage=function (e){console.log('Server: ', e.data);};connection.onclose=function (){console.log('WebSocket connection closed');};suspend();}</script><style>body{font-size:200%;font-family: Arial;}select{width: 100%;font-size:100%;margin: 10px 0;border: solid 2px;}</style></head><body style=\"display:none\"><label><input type=\"checkbox\" id=\"onBox\" onchange=\"onOnBoxChange()\"/>Включить</label><div id=\"mainControls\"><select id=\"animSelect\" onchange=\"onAnimPalChange()\"><option value=\"0\">Начальная</option><option value=\"1\">Бег</option><option value=\"2\">Пыльца эльфов</option><option value=\"3\">Вспышки</option><option value=\"4\">Случайный цикл</option><option value=\"5\">Звезды</option><option value=\"6\">Полосы</option><option value=\"7\">Полет</option><option value=\"100\">Магия</option></select><select id=\"palSelect\" onchange=\"onAnimPalChange()\"><option value=\"0\">RGB</option><option value=\"1\">Радуга</option><option value=\"2\">Полосатая радуга</option><option value=\"3\">Вечеринка</option><option value=\"4\">Жара</option><option value=\"5\">Огонь</option><option value=\"6\">Лёд</option><option value=\"7\">Рождество</option></select></div><div id=\"accelControls\"><input class=\"enabled\" id=\"b\" type=\"range\" min=\"-90\" max=\"90\" step=\"1\" oninput=\"sendMP();\" value=\"0\"/><input class=\"enabled\" id=\"g\" type=\"range\" min=\"-90\" max=\"90\" step=\"1\" oninput=\"sendMP();\" value=\"0\"/></div></body></html>";

void setup() {
  Serial.begin(115200);
  Serial.println("Entering setup");
  randomSeed(analogRead(0)*analogRead(1));
  anim.setAnim(animInd);
  anim.setPeriod(20);
  anim.setPalette(pals[0]);
  anim.doSetUp();

  setupWiFi();
  
  setupHttpServer();

  setupSocket();

  Serial.println("Setup done");
}

void setupWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);

 // ждем соединения:
  while (WiFi.status() != WL_CONNECTED && millis() < 10000) {
    delay(500);
    Serial.print(".");
  }
  
  if (millis() >= 10000) {
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  }

  Serial.print("IP address: ");Serial.println(WiFi.localIP());
}

void setupHttpServer()
{
  //index page
  server.on("/", HTTP_GET, []{
      server.send(200, "text/html", HTML);
  });

  //sustains current effect for next INTERVAL. Returns JSON with current animation index and palette index:
  // {'a':ANIMATION_INDEX,'b':'PALETTE_INDEX'}
  // negative animation index means the strip is powered off
  server.on("/sus",[](){ sustainEffect(); });

  //sets effect and palette. Inputs two parameters: a=ANIMATION_INDEX&b=PALETTE_INDEX
  //negative animation index turns the strip off
  server.on("/set",[](){ setEffect(); });
  
  server.begin();
}

void setupSocket()
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}


void loop() {
  yield();
  
  /* this piece of code checks for looping while trying to find different colors
  for (int pi=0;pi<PALS;pi++) {
    int c = 0;
    
    Serial.print(F("pi="));Serial.print(pi);
    Color c1 = pals[pi]->getPalColor((float)rngb()/256);
    Color c2 = c1;
    while (c1.isCloseTo(c2)) {
      c = c + 1;
      c2 = pals[pi]->getPalColor((float)rngb()/256);
    }
    Serial.print(F(" c="));Serial.println(c);
  }
  /**/

  anim.run();
  
  yield();
  
  if (millis() > ms && animInd >= 0) {// non-negative animind is for turned off strip - it never ends automatically
    ms = millis() + INTERVAL; 
    switch ( (animInd < 0) ? 0 : random(1)) {
      case 0: 
      {
        Serial.print(F("anim->"));
        int prevAnimInd = animInd;
#ifdef USE_START_ANIMATION
        while (prevAnimInd == animInd) animInd = random(ANIMS+1);
        if (animInd == 0) ms = millis() + 10000;//startup animation has fixed 10 seconds length
#else
        while (prevAnimInd == animInd) animInd = random(ANIMS) + 1;
#endif        
        anim.setAnim(animInd);
        anim.setPeriod(random(20, 40));
        anim.setPalette(pals[paletteInd]);
        anim.doSetUp();
        break;
      }
      case 1:
      {
        Serial.print(F("pal->"));
        int prevPalInd = paletteInd;
        while (prevPalInd == paletteInd) paletteInd = random(PALS);
        anim.setPalette(pals[paletteInd]);
        Serial.print(paletteInd);
        break;
      }
    }
    Serial.println();
  }
  /**/

  yield();

  server.handleClient();

  yield();

  webSocket.loop();
}

void sustainEffect() {
    char buf[100];
    sprintf(buf, "{\"a\":%d,\"p\":%d}", animInd, paletteInd);
    if (animInd > 0) {
      ms = millis() + INTERVAL; //"reset" duration for any animation except start
    }
    server.sendHeader("Access-Control-Allow-Origin","*");
    server.send(200,"text/html", buf);
}

void setEffect() {
    if (!server.hasArg("a") || !server.hasArg("p")) {
      server.send(400, "text/html", "No arg");
    }

    int newAnimInd = server.arg("a").toInt();
/*    if (newAnimInd > ANIMS) {
      char buf[100];
      sprintf(buf, "Wrong a=%d", newAnimInd);
      server.send(400, "text/html", buf);
    }*/
    animInd = newAnimInd;
    
    int newPalInd = server.arg("p").toInt();
    if (newPalInd < 0 && newPalInd >= PALS) {
      char buf[100];
      sprintf(buf, "Wrong p=%d", newPalInd);
      server.send(400, "text/html", buf);
    }
    paletteInd = newPalInd;

    anim.setAnim(animInd);
    anim.setPalette(pals[paletteInd]);
    anim.doSetUp();

    server.sendHeader("Access-Control-Allow-Origin","*");
    server.send(200,"text/html", "");
 }
 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { 
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      if (animInd == 100 && (payload[0] == 'P') || payload[0] == 'B') {            
        uint16_t pos = (uint16_t) atoi((const char *) &payload[1]);
        int spotPos = (pos & 0xFF)*LEDS >> 8;
        byte spotColor = (pos >> 8) & 0xFF;
        anim.setMagicParams(num, spotPos, spotColor, payload[0] == 'B');
      }      
      break;
  }
}
