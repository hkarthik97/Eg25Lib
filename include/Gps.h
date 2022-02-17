#ifndef __GPS_H_
#define __GPS_H_

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#define GSMSERIAL Serial2

#define GPSMODE 104
#define NMEAMODE 105
#define GPSSIGNAL 106
#define GPSGGA 107
#define GPSRMC 108


class GPSparser{
  private:

 

  public:

  float _latitude;
  float _longitude;
  float _speed;
  float _altitude;
  float _direction;
  unsigned int _satelliteCount;
  float _degree;

  bool debugMode;
  int rmcCount = 0;
  int ggaCount = 0;
  int gpsLocCount = 0;
  
  char sim_status[10] = { 0 };
  char MQTT_DATA[200] = { 0 };
     
  bool sim_stauts = false;
  
  unsigned int EC20_response_count = 0;

  //GPS initializer functions
  void setDebugMode(bool debugModeU);
  void debugMessages(String MSG);
  int initializeNmeaMode();
  void initializeGpsModes();
  int initializeGpsMode();
  int EG21_Response(unsigned int timeout, int code);
  int parseLocData(char arr[]);
  int checkGpsSignal();
  //GPS Co-Ordinates Setter function
  void setLatitude(float lat,String ld);
  void setLongitude(float lon, String ld);
  void setSpeed(float speed);
  void setAltitude(float altitude);
  void setDirection(float direction);
  void setSatelliteCount(int satelliteCount);
  void setDegrees(float degreess);
  float getLatitude();
  float getLongitude();
  float getSpeed();
  float getAltitude();
  float getDirection();
  int getSatelliteCount();
  float getSpeedMph();
  float getSpeedKmh();
  float getDegree();
};

#endif
