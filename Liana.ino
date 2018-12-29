#include <WebSocketsServer.h>
#include "palette.h"
#include "anim.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASS"

#define ANIMS 7 //number of animations (not including start one) to cycle randomly
#define PALS 8 //number of palettes
#define INTERVAL 30000 //change interval, msec

//#define USE_START_ANIMATION //start animation is used in cycling as well as other animations

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
"<!DOCTYPE HTML> <html lang=ru-RU> <head> <meta charset=\"utf-8\"/> <meta name=viewport content=\"width=device-width, initial-scale=1.0\"> <script>const ESP_WS=\"ws://\"+location.hostname+\":81/\";let suspendTimer;let bAng,gAng;let preAcc;let wsconn;function suspend()\n{if(wsconn.readyState==wsconn.OPEN){wsconn.send('I');console.log('Info request sent');}else{console.log('Wrong state while sending info request: '+wsconn.readyState);}\nwindow.setTimeout(suspend,5000);}\nfunction onAnimPalChange(){let anim=Number(document.getElementById('animSelect').value);let pal=Number(document.getElementById('palSelect').value);let data=(anim<<8)+pal;console.log(data.toString(16));wsconn.send('S'+data.toString(16));if(anim==100){window.addEventListener(\"deviceorientation\",handleOrientation,true);window.addEventListener(\"devicemotion\",handleMotion,true);}else{window.removeEventListener(\"deviceorientation\",handleOrientation,true);window.removeEventListener(\"devicemotion\",handleMotion,true);}}\nfunction onOnBoxChange(){var mainControlsDiv=document.getElementById('mainControls');if(document.getElementById('onBox').checked){mainControlsDiv.style.display='';document.getElementById('animSelect').value=0;onAnimPalChange();}else{mainControlsDiv.style.display='none';wsconn.send('SFF00');}}\nfunction handleOrientation(e){if(e.beta>-45&&e.beta<45){bAng=Math.floor((Number(e.beta)+45)*255/90);gAng=Math.floor((Number(e.gamma)+90)*255/180);let pos=bAng+(gAng<<8);wsconn.send('P '+pos.toString(16));}}\nfunction handleMotion(e){if(preAcc>10&&e.acceleration.y<10){sendBoom();}\npreAcc=e.acceleration.y;}\nfunction sendBoom(){let pos=bAng+(gAng<<8);wsconn.send('PB'+pos.toString(16));console.log('Boom!');}\nfunction connectws(){wsconn=new WebSocket(ESP_WS,['arduino'])\ndocument.getElementById('dscon').innerText='Соединение...';wsconn.onopen=function(){suspend();document.getElementById('dscon').style.display='none';document.getElementById('allc').style.display='';};wsconn.onerror=function(error){console.log('WS error'+error);};wsconn.onmessage=function(e){console.log('Server: ',e.data);if(e.data.charAt(0)=='I'){let ai=parseInt(e.data.substring(1,3),16);let pi=parseInt(e.data.substring(4,6),16);document.getElementById('onBox').checked=(ai!=255);document.getElementById('animSelect').value=ai;document.getElementById('palSelect').value=pi;console.log(\"a=\"+ai+\",p=\"+pi);}};wsconn.onclose=function(){console.log('onclose');document.getElementById('dscon').style.display='';document.getElementById('allc').style.display='none';document.getElementById('dscon').innerText='Нет связи, повтор...';window.setTimeout(connectws,1000);};}\ndocument.addEventListener(\"DOMContentLoaded\",start);function start(){connectws();}</script> <style>body{font-size:200%;font-family:Arial}select{width:100%;font-size:100%;margin:10px 0;border:solid 2px}</style> </head> <body> <div id=dscon>Соединение...</div> <div id=allc style=display:none> <label><input type=checkbox id=onBox onchange=\"onOnBoxChange()\"/>Включить</label> <div id=mainControls> <select id=animSelect onchange=onAnimPalChange()> <option value=0>Начальная</option> <option value=1>Бег</option> <option value=2>Пыльца эльфов</option> <option value=3>Вспышки</option> <option value=4>Случайный цикл</option> <option value=5>Звезды</option> <option value=6>Полосы</option> <option value=7>Полет</option> <option value=100>Магия</option> </select> <select id=palSelect onchange=onAnimPalChange()> <option value=0>RGB</option> <option value=1>Радуга</option> <option value=2>Полосатая радуга</option> <option value=3>Вечеринка</option> <option value=4>Жара</option> <option value=5>Огонь</option> <option value=6>Лёд</option> <option value=7>Рождество</option> </select> </div> </div> </body> </html>";

void setup() { 
  Serial.begin(115200);
  Serial.println("Entering setup");

  setupWiFi();
  
  setupHttpServer();

  setupSocket();

  randomSeed(analogRead(0)*analogRead(1));
  anim.setAnim(animInd);
  anim.setPeriod(20);
  anim.setPalette(pals[0]);
  anim.doSetUp();


  Serial.println("Setup done");
}

void setupWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);

 // ждем соединения:
  while (WiFi.status() != WL_CONNECTED && millis() < 30000) {
    delay(500);
    Serial.print(".");
  }
  
  if (millis() >= 30000) {
    WiFi.disconnect();
    Serial.println("Fallback to Access Point");
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  }

  Serial.print("IP address: ");Serial.println(WiFi.localIP());

   if (MDNS.begin("liana")) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
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
  server.on("/sus",[](){ webSustainEffect(); });

  //sets effect and palette. Inputs two parameters: a=ANIMATION_INDEX&b=PALETTE_INDEX
  //negative animation index turns the strip off
  server.on("/set",[](){ webSetEffect(); });
  
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
  
  if (millis() > ms && animInd != 255) {// animind == 255 is for turned off strip - it never ends automatically
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
        setAnimPal();
        notify(-1);
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

//sets animation and palette to values specified in animInd and paletteInd, 
//and notifies all WebSocket connections of the change
void setAnimPal() {
  anim.setAnim(animInd);
  anim.setPeriod(random(20, 40));
  anim.setPalette(pals[paletteInd]);
  anim.doSetUp();
}

//notifies websocket connection num of current status
//if num is -1, notifies all active connections
//Sent data is as follows:
//1st char = 'I'
//2-3 char - HEX byte, current animation index, -1 if strip is off
//4-5 char - HEX byte, current palette index
void notify(int num) {
  char buf[100];
  sprintf(buf, "I%02X%02X", animInd, paletteInd);
  if (num >= 0) {
    webSocket.sendTXT(num, buf, 0);
  } else {
    webSocket.broadcastTXT(buf, 0);
  }
}

void webSustainEffect() {
    char buf[100];
    sprintf(buf, "{\"a\":%d,\"p\":%d}", animInd, paletteInd);
    if (animInd > 0) {
      ms = millis() + INTERVAL; //"reset" duration for any animation except start
    }
    server.sendHeader("Access-Control-Allow-Origin","*");
    server.send(200,"text/html", buf);
}

void webSetEffect() {
    if (!server.hasArg("a") || !server.hasArg("p")) {
      server.send(400, "text/html", "No arg");
    }

    int newAnimInd = server.arg("a").toInt();
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
 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) { 
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
      switch (payload[0]) {
        case 'P':
          wsPoint(num, payload, length);
          break;
        case 'S':
          wsSet(num, payload, length);
          break;
        case 'I':
          wsInfo(num, payload, length);
          break;
        default:
          Serial.printf("Unknown WS command: %s", payload);
          break;
      }
      break;
  }
}

/* Point command
 * Sent by web when device is admittedly pointed at the strip
 * 1st char - space or 'B', latter means that "Boom" gesture was made
 * 2-3 char - HEX byte, number of LED to which device points
 * 4-5 char - HEX byte, color index (roll inclination of the device)
 */
void wsPoint(uint8_t num, uint8_t * payload, size_t length) {
  if (animInd == 100) {            
    uint32_t data = (uint32_t) strtol((const char *) &payload[2], NULL, 16);
    int spotPos = (data & 0xFF)*LEDS >> 8;
    byte spotColor = (data >> 8) & 0xFF;
    anim.setMagicParams(num, spotPos, spotColor, payload[1] == 'B');
  }      
}

/* Set command
 * Sets animation and palette
 * 1-2 char - HEX byte, animation index, 255 means power off
 * 3-4 char - HEX byte, palette index
 */
void wsSet(uint8_t num, uint8_t * payload, size_t length) {
  uint32_t data = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
  animInd = (data >> 8) & 0xFF;
  paletteInd = data & 0xFF;
  if (paletteInd >= PALS) { paletteInd = 0; };
  Serial.printf("Text: %s, data:%d, a:%d, p:%d", payload, data, animInd, paletteInd); 

  setAnimPal();  
}

/* Info command
 * Triggers sending current data
 * No input parameters
 */
void wsInfo(uint8_t num, uint8_t * payload, size_t length) {
  notify(num);
  if (animInd > 0) {
    ms = millis() + INTERVAL; //"reset" duration for any animation except start
  }
}
