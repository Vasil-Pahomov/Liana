ESP8266WebServer webServer(80);
EspHtmlTemplateProcessor templateProcessor(&webServer);

static constexpr const char *redirectStr = "<script>window.location='/'</script>Not found. <a href='/'>Home</a>";

void webSetup() {
    webServer.onNotFound([&]() {
      Serial.println("NF: " + webServer.uri());
      if (!webFileRead(webServer.uri())) {
        webServer.sendHeader("Cache-Control", " max-age=172800");
        webServer.send(302, "text/html", redirectStr);
      }    
  });

  webServer.on("/setup", HTTP_POST, webOnSetup);

  webServer.begin();
}

String indexKeyProcessor(String& key)
{
  if (webServer.hasArg(key)) {
    return webServer.arg(key);
  }
  if (currentConfig.configJsonDoc.containsKey(key)) {
    return currentConfig.configJsonDoc[key];
  }
  return " ";
}

//Base code from https://github.com/r-downing/SPIFFSReadServer/blob/master/SPIFFSReadServer.h
bool webFileRead(String path) {
  if (path.endsWith("/")) path += "index.htm";
  String contentType;
  bool useTemplate = false;
  if (path.endsWith(".htm") || path.endsWith(".html")) { contentType = "text/html"; useTemplate = true; }
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".png")) contentType = "image/png";
  else if (path.endsWith(".gif")) contentType = "image/gif";
  else if (path.endsWith(".jpg")) contentType = "image/jpeg";
  else if (path.endsWith(".ico")) contentType = "image/x-icon";
  else if (path.endsWith(".xml")) contentType = "text/xml";
  else if (path.endsWith(".pdf")) contentType = "application/x-pdf";
  else if (path.endsWith(".zip")) contentType = "application/x-zip";
  else if (path.endsWith(".gz")) contentType = "application/x-gzip";
  else if (path.endsWith(".json")) contentType = "application/json";
  else contentType = "text/plain";

  //split filepath and extension
  String prefix = path, ext = "";
  int lastPeriod = path.lastIndexOf('.');
  if (lastPeriod >= 0) {
    prefix = path.substring(0, lastPeriod);
    ext = path.substring(lastPeriod);
  }

  //look for smaller versions of file
  //minified file, good (myscript.min.js)
  if (SPIFFS.exists(prefix + ".min" + ext)) path = prefix + ".min" + ext;
  //gzipped file, better (myscript.js.gz)
  //if (SPIFFS.exists(prefix + ext + ".gz")) path = prefix + ext + ".gz";
  //min and gzipped file, best (myscript.min.js.gz)
  //if (SPIFFS.exists(prefix + ".min" + ext + ".gz")) path = prefix + ".min" + ext + ".gz";

  if (SPIFFS.exists(path)) {
    if (webServer.hasArg("download"))
      webServer.sendHeader("Content-Disposition", " attachment;");
    if (webServer.uri().indexOf("nocache") < 0)
      webServer.sendHeader("Cache-Control", " max-age=172800");

    //optional alt arg (encoded url), server sends redirect to file on the web
    if (WiFi.status() == WL_CONNECTED && webServer.hasArg("alt")) {
      webServer.sendHeader("Location", webServer.arg("alt"), true);
      webServer.send ( 302, "text/plain", "");
    } else {
      //server sends file
      if (useTemplate)
      {
        currentConfig.configLoad();
        templateProcessor.processAndSend(path, (GetKeyValueCallback)indexKeyProcessor);
      } else {
        File file = SPIFFS.open(path, "r");
        webServer.streamFile(file, contentType);
        file.close();
      }
    }
    return true;
  } //if SPIFFS.exists
  else{
    Serial.printf("Not found: '%s'\n",path.c_str());
  }
  return false;
} 

void webOnSetup() {
  int leds = webServer.arg("leds").toInt();
  int brightness = webServer.arg("brightness").toInt();
  int neofeature = webServer.arg("neofeature").toInt();
  Serial.printf("Setup save leds=%d , brightness=%d, neofeature=%d\n", leds, brightness, neofeature);
  currentConfig.leds = leds;
  currentConfig.brightness = brightness;
  currentConfig.neofeature = neofeature;
  currentConfig.configSave();
  webServer.sendHeader("Cache-Control", " max-age=172800");
  webServer.send(302, "text/html", "<script>window.location='/?restartmsg=block'</script>Not found. <a href='/'>Home</a>");
}
