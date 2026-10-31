#if defined(ESP8266)
#include <ESP8266WebServer.h>
ESP8266WebServer webServer(80);
#elif defined(ESP32)
#include <WebServer.h>
WebServer webServer(80);
#else
#error "Unsupported board class"
#endif
#include <ezTime.h>
#include <ArduinoJson.h>
#include "fs_compat.h"
#include "web.h"
#include "config.h"
#include "settings.h"
#include "ota.h"
#include "api.h"

extern Timezone tz;

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

//value of a {{key}} placeholder: query argument first, then a setting; "" when unknown
static String templateValue(const String& key) {
  if (webServer.hasArg(key)) {
    return htmlEscape(webServer.arg(key));
  }
  return htmlEscape(settings.get(key));
}

//streams an HTML file to the client replacing {{key}} placeholders (chunked response)
static void sendTemplate(File& file) {
  webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webServer.send(200, "text/html", "");

  String out;
  out.reserve(1024);
  String key;
  enum { TEXT, OPEN, KEY, CLOSE } state = TEXT;

  while (file.available()) {
    char c = (char)file.read();
    switch (state) {
      case TEXT:
        if (c == '{') state = OPEN; else out += c;
        break;
      case OPEN:
        if (c == '{') { state = KEY; key = ""; }
        else { out += '{'; out += c; state = TEXT; }
        break;
      case KEY:
        if (c == '}') state = CLOSE; else key += c;
        break;
      case CLOSE:
        if (c == '}') { out += templateValue(key); state = TEXT; }
        else { key += '}'; key += c; state = KEY; }
        break;
    }
    if (out.length() >= 1024) {
      webServer.sendContent(out);
      out = "";
    }
  }
  //unterminated placeholder at the end of file is sent as is
  if (state == OPEN) out += '{';
  else if (state == KEY || state == CLOSE) { out += "{{"; out += key; if (state == CLOSE) out += '}'; }
  if (out.length() > 0) webServer.sendContent(out);
  webServer.sendContent("");
}

static String contentTypeFor(const String& path) {
  if (path.endsWith(".htm") || path.endsWith(".html")) return "text/html";
  if (path.endsWith(".css")) return "text/css";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".png")) return "image/png";
  if (path.endsWith(".gif")) return "image/gif";
  if (path.endsWith(".jpg")) return "image/jpeg";
  if (path.endsWith(".ico")) return "image/x-icon";
  if (path.endsWith(".xml")) return "text/xml";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

//serves a file from the file system; .htm files go through the template processor
bool webFileRead(String path) {
  if (path.endsWith("/")) path += "index.htm";
  //the config file holds the MQTT password, never serve it
  if (path == CONFIG_FILE_NAME) return false;

  bool useTemplate = path.endsWith(".htm") || path.endsWith(".html");
  String contentType = contentTypeFor(path);

  //a minified version is preferred when present (myscript.min.js)
  int lastPeriod = path.lastIndexOf('.');
  if (lastPeriod >= 0) {
    String minPath = path.substring(0, lastPeriod) + ".min" + path.substring(lastPeriod);
    if (LIANA_FS.exists(minPath)) path = minPath;
  }

  if (!LIANA_FS.exists(path)) {
    Serial.printf("Not found: '%s'\n", path.c_str());
    return false;
  }

  if (webServer.uri().indexOf("nocache") < 0)
    webServer.sendHeader("Cache-Control", " max-age=172800");

  File file = LIANA_FS.open(path, "r");
  if (useTemplate) {
    sendTemplate(file);
  } else {
    webServer.streamFile(file, contentType);
  }
  file.close();
  return true;
}

void webRedirectHomeWithRestartMessage() {
  webServer.sendHeader("Cache-Control", " max-age=172800");
  webServer.send(302, "text/html", "<script>window.location='/?restartmsg=block'</script>Not found. <a href='/'>Home</a>");
}

static void sendJson(void (*fill)(JsonObject)) {
  JsonDocument doc;
  fill(doc.to<JsonObject>());
  String out;
  serializeJson(doc, out);
  webServer.sendHeader("Cache-Control", "no-cache");
  webServer.send(200, "application/json", out);
}

void webOnOTA(){
  otaBegin();
  webServer.send(200, "text/html", "OTA is beginning <a href='/'>Home</a>");
}

void webOnSetup() {
  settings.leds = webServer.arg("leds").toInt();
  settings.brightness = webServer.arg("brightness").toInt();
  settings.neofeature = webServer.arg("neofeature").toInt();
  settings.nightEnabled = webServer.hasArg("nightEnabled");
  settings.nightStartHour = webServer.arg("nightStartHour").toInt();
  settings.nightEndHour = webServer.arg("nightEndHour").toInt();
  settings.nightAnim = webServer.arg("nightAnim").toInt();
  settings.nightBrightness = webServer.arg("nightBrightness").toInt();
  settings.save();
  Serial.printf("Setup saved: leds=%d, brightness=%d, neofeature=%d, night=%d\n",
    settings.leds, settings.brightness, settings.neofeature, settings.nightEnabled);
  webRedirectHomeWithRestartMessage();
}

void webOnMqttSettings() {
  settings.mqttHost = webServer.arg("mqttHost");
  settings.mqttPort = webServer.arg("mqttPort").toInt();
  settings.mqttClientId = webServer.arg("mqttClientId");
  settings.mqttLogin = webServer.arg("mqttLogin");
  settings.mqttPass = webServer.arg("mqttPass");
  settings.mqttTopic = webServer.arg("mqttTopic");
  settings.save();
  Serial.printf("MQTT settings saved: host=%s, port=%d\n", settings.mqttHost.c_str(), settings.mqttPort);
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

  webServer.on("/setup", HTTP_POST, webOnSetup);
  webServer.on("/mqtt", HTTP_POST, webOnMqttSettings);
  webServer.on("/ota", HTTP_POST, webOnOTA);
  webServer.on("/time", HTTP_GET, webOnCurrentTime);
  webServer.on("/api/state", HTTP_GET, []() { sendJson(stateToJson); });
  webServer.on("/api/catalog", HTTP_GET, []() { sendJson(catalogToJson); });

  webServer.begin();
  Serial.println("WebServer setuped");
}
