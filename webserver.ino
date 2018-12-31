ESP8266WebServer webServer(80);

const char HTML[] PROGMEM = 
"<!DOCTYPE HTML> <html lang=ru-RU> <head> <meta charset=\"utf-8\"/> <meta name=viewport content=\"width=device-width, initial-scale=1.0\"> <script>const ESP_WS=\"ws://\"+location.hostname+\":81/\";let suspendTimer;let bAng,gAng;let preAcc;let wsconn;function suspend()\n{if(wsconn.readyState==wsconn.OPEN){wsconn.send('I');console.log('Info request sent');}else{console.log('Wrong state while sending info request: '+wsconn.readyState);}\nwindow.setTimeout(suspend,5000);}\nfunction onAnimPalChange(){let anim=Number(document.getElementById('animSelect').value);let pal=Number(document.getElementById('palSelect').value);let data=(anim<<8)+pal;console.log(data.toString(16));wsconn.send('S'+data.toString(16));if(anim==100){window.addEventListener(\"deviceorientation\",handleOrientation,true);window.addEventListener(\"devicemotion\",handleMotion,true);}else{window.removeEventListener(\"deviceorientation\",handleOrientation,true);window.removeEventListener(\"devicemotion\",handleMotion,true);}}\nfunction onOnBoxChange(){var mainControlsDiv=document.getElementById('mainControls');if(document.getElementById('onBox').checked){mainControlsDiv.style.display='';document.getElementById('animSelect').value=0;onAnimPalChange();}else{mainControlsDiv.style.display='none';wsconn.send('SFF00');}}\nfunction handleOrientation(e){if(e.beta>-45&&e.beta<45){bAng=Math.floor((Number(e.beta)+45)*255/90);gAng=Math.floor((Number(e.gamma)+90)*255/180);let pos=bAng+(gAng<<8);wsconn.send('P '+pos.toString(16));}}\nfunction handleMotion(e){if(preAcc>10&&e.acceleration.y<10){sendBoom();}\npreAcc=e.acceleration.y;}\nfunction sendBoom(){let pos=bAng+(gAng<<8);wsconn.send('PB'+pos.toString(16));console.log('Boom!');}\nfunction connectws(){wsconn=new WebSocket(ESP_WS,['arduino'])\ndocument.getElementById('dscon').innerText='Соединение...';wsconn.onopen=function(){suspend();document.getElementById('dscon').style.display='none';document.getElementById('allc').style.display='';};wsconn.onerror=function(error){console.log('WS error'+error);};wsconn.onmessage=function(e){console.log('Server: ',e.data);if(e.data.charAt(0)=='I'){let ai=parseInt(e.data.substring(1,3),16);let pi=parseInt(e.data.substring(4,6),16);document.getElementById('onBox').checked=(ai!=255);document.getElementById('animSelect').value=ai;document.getElementById('palSelect').value=pi;console.log(\"a=\"+ai+\",p=\"+pi);}};wsconn.onclose=function(){console.log('onclose');document.getElementById('dscon').style.display='';document.getElementById('allc').style.display='none';document.getElementById('dscon').innerText='Нет связи, повтор...';window.setTimeout(connectws,1000);};}\ndocument.addEventListener(\"DOMContentLoaded\",start);function start(){connectws();}</script> <style>body{font-size:200%;font-family:Arial}select{width:100%;font-size:100%;margin:10px 0;border:solid 2px}</style> </head> <body> <div id=dscon>Соединение...</div> <div id=allc style=display:none> <label><input type=checkbox id=onBox onchange=\"onOnBoxChange()\"/>Включить</label> <div id=mainControls> <select id=animSelect onchange=onAnimPalChange()> <option value=0>Начальная</option> <option value=1>Бег</option> <option value=2>Пыльца эльфов</option> <option value=3>Вспышки</option> <option value=4>Случайный цикл</option> <option value=5>Звезды</option> <option value=6>Полосы</option> <option value=7>Полет</option> <option value=100>Магия</option> </select> <select id=palSelect onchange=onAnimPalChange()> <option value=0>RGB</option> <option value=1>Радуга</option> <option value=2>Полосатая радуга</option> <option value=3>Вечеринка</option> <option value=4>Жара</option> <option value=5>Огонь</option> <option value=6>Лёд</option> <option value=7>Рождество</option> </select> </div> </div> </body> </html>";


void webSetup()
{
  //index page
  webServer.on("/", HTTP_GET, []{
      webServer.send(200, "text/html", HTML);
  });

  //sets effect and palette. Inputs two parameters: a=ANIMATION_INDEX&b=PALETTE_INDEX
  //negative animation index turns the strip off
  webServer.on("/set",[](){ webSetEffect(); });
  
  webServer.begin();
}

void webSetEffect() {
    if (!webServer.hasArg("a") || !webServer.hasArg("p")) {
      webServer.send(400, "text/html", "No arg");
    }

    int newAnimInd = webServer.arg("a").toInt();
    animInd = newAnimInd;
    
    int newPalInd = webServer.arg("p").toInt();
    if (newPalInd < 0 && newPalInd >= PALS) {
      char buf[100];
      sprintf(buf, "Wrong p=%d", newPalInd);
      webServer.send(400, "text/html", buf);
    }
    paletteInd = newPalInd;

    anim.setAnim(animInd);
    anim.setPalette(pals[paletteInd]);
    anim.doSetUp();

    webServer.sendHeader("Access-Control-Allow-Origin","*");
    webServer.send(200,"text/html", "");
}

 void webRun()
 {
   webServer.handleClient();
 }
