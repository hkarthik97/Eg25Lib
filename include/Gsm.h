#ifndef __GSM_H_
#define __GSM_H_

#if (ARDUINO >= 100)
#include "Arduino.h"
#else

#include "WProgram.h"

#endif

#define GSMSERIAL Serial2
#define CONNCHECK 101
#define SIMCHECK 102
#define DATASIGNAL 103

class GsmParser
{
private:
public:
     //     GPSparser gps;
     int rmcCount = 0;
     int ggaCount = 0;
     int gpsLocCount = 0;

     char sim_status[10] = {0};
     char MQTT_DATA[200] = {0};
     bool sim_stauts = false;

     bool debugMode = false;
     unsigned int EC20_response_count = 0;

     Stream &serialm = Serial2;
     void setDebugMode(bool debugMode);
     void debugMessages(String MSG);
     void setSerial(String serial);
     void initializeGsmModule();
     void echoOff();
     int isModuleConnected();
     int checkSIM();
     int checkSignal();
     //     void initializeGpsModes();
     void resetModule();
     int EG21_Response(unsigned int timeout, int code);

     //Following Function are GPS mode Functions

     //     int initializeGpsMode();
     //     int initializeNmeaMode();
};

#endif
