#if defined(ESP8266)
#include <ESP8266WebServer.h>
ESP8266WebServer webServer(80);
#elif defined(ESP32)
#include <WebServer.h>
WebServer webServer(80);
#else
#error "Unsupported board class"
#endif
#include <EspHtmlTemplateProcessor.h>
#include <ezTime.h>
#include "config.h"
#include "ota.h"

extern Timezone tz;

EspHtmlTemplateProcessor templateProcessor(&webServer);

static constexpr const char *redirectStr = "<script>window.location='/'</script>Not found. <a href='/'>Home</a>";

//escapes characters that are special in HTML text and attribute values
static String htmlEscape(const String& s) {
  String out;
  out.reserve(s.length());
  for (unsigned int i = 0; i < s.length(); i++) {
    char c = s[i];
    switch (c) {
      case '&':  out += F("&amp;"); break;
      case '<':  out += F("&lt;"); break;
      case '>':  out += F("&gt;"); break;
      case '"':  out += F("&quot;"); break;
      case '\'': out += F("&#39;"); break;
      default:   out += c;
    }
  }
  return out;
}

//resolves {{key}} placeholders in .htm templates: query argument first, then config value
String indexKeyProcessor(String& key)
{
  if (webServer.hasArg(key)) {
    return htmlEscape(webServer.arg(key));
  }
  if (currentConfig.configJsonDoc.containsKey(key)) {
    return htmlEscape(currentConfig.configJsonDoc[key].as<String>());
  }
  return " ";
}

//Base code from https://github.com/r-downing/SPIFFSReadServer/blob/master/SPIFFSReadServer.h
bool webFileRead(String path) {
  if (path.endsWith("/")) path += "index.htm";
  //the config file holds the MQTT password, never serve it
  if (path == CONFIG_FILE_NAME) return false;
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

void webRedirectHomeWithRestartMessage() {
  webServer.sendHeader("Cache-Control", " max-age=172800");
  webServer.send(302, "text/html", "<script>window.location='/?restartmsg=block'</script>Not found. <a href='/'>Home</a>");
}

void webOnOTA(){
  otaBegin();
  // webServer.sendHeader("Cache-Control", " max-age=172800");
  webServer.send(200, "text/html", "OTA is beginning <a href='/'>Home</a>");
  // webRedirectHomeWithRestartMessage();
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
  webRedirectHomeWithRestartMessage();
}

void webOnMqttSettings() {
  currentConfig.mqttHost = webServer.arg("mqttHost");
  currentConfig.mqttPort = webServer.arg("mqttPort").toInt();
  currentConfig.mqttClientId = webServer.arg("mqttClientId");
  currentConfig.mqttLogin = webServer.arg("mqttLogin");
  currentConfig.mqttPass = webServer.arg("mqttPass");
  currentConfig.mqttTopic = webServer.arg("mqttTopic");
  Serial.printf("Setup save mqtt host=%s, port=%d\n", currentConfig.mqttHost.c_str(), currentConfig.mqttPort);
  currentConfig.configSave();
  webRedirectHomeWithRestartMessage();
}

//diagnostics: local time as the night mode sees it
void webOnCurrentTime() {
  String resp = tz.dateTime();
  resp += (timeStatus() == timeSet) ? F(" (time synced)") : F(" (time NOT synced)");
  webServer.send(200, "text/plain", resp);
}


void webSetup() {
  Serial.println("WebServer is setuping");
  webServer.onNotFound([&]() {
      Serial.println("NF: " + webServer.uri());
      if (!webFileRead(webServer.uri())) {
        webServer.sendHeader("Cache-Control", " max-age=172800");
        webServer.send(302, "text/html", redirectStr);
      }    
  });

  //todo: it's not clear you should add the handler for each settings page here, think of it
  webServer.on("/setup", HTTP_POST, webOnSetup);
  webServer.on("/mqtt", HTTP_POST, webOnMqttSettings);
  webServer.on("/ota", HTTP_POST, webOnOTA);
  webServer.on("/time", HTTP_GET, webOnCurrentTime);

  webServer.begin();
  Serial.println("WebServer setuped");
}
