#ifndef websocket_h
#define websocket_h

#include <Arduino.h>
#include <WebSocketsServer.h>

void wsRun();

void wsSetup();

void wsNotify(int num);

#endif