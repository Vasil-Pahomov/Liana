WebSocketsServer webSocket(81); 

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
  wsNotify(num);
  if (animInd > 0) {
    ms = millis() + INTERVAL; //"reset" duration for any animation except start
  }
}


void wsRun()
{
  webSocket.loop();
}

void wsSetup()
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

//notifies websocket connection num of current status
//if num is -1, notifies all active connections
//Sent data is as follows:
//1st char = 'I'
//2-3 char - HEX byte, current animation index, -1 if strip is off
//4-5 char - HEX byte, current palette index
void wsNotify(int num) {
  char buf[100];
  sprintf(buf, "I%02X%02X", animInd, paletteInd);
  if (num >= 0) {
    webSocket.sendTXT(num, buf, 0);
  } else {
    webSocket.broadcastTXT(buf, 0);
  }
}
