
#include "Gps.h"
#include <WebSocket.h>

void GPSparser::setDebugMode(bool debugModeU)
{
  debugMode = debugModeU;
}

void GPSparser::debugMessages(String MSG)
{

  if (debugMode)
  {
    Serial.println(MSG);
    notifyClientsReceived(MSG);
  }
}
void GPSparser::setLatitude(float lat, String ld)
{
  float minutes = fmod(lat, 100.0);
  float totalMinutes = minutes / 60;
  int degrees = int(lat / 100);
  // Serial.println(degrees);
  // Serial.println(totalMinutes,6);
  if (ld == "N")
  {
    lat = degrees + totalMinutes;
    Serial.println("--------------------------------Lat set");
    Serial.println(lat, 6);
    _latitude = lat;
  }
  else if (ld == "S")
  {
    lat = -abs(degrees + totalMinutes);
    _latitude = lat;
  }
}

void GPSparser::setLongitude(float lon, String ld)
{
  float minutes = fmod(lon, 100);
  float totalMinutes = minutes / 60;
  int degrees = int(lon / 100);
  //  lon = degrees + totalMinutes;
  //  Serial.println(-abs(lon));
  if (ld == "E")
  {
    lon = degrees + totalMinutes;
    _longitude = lon;
  }
  else if (ld == "W")
  {
    lon = -abs(degrees + totalMinutes);
    _longitude = lon;
  }
}

void GPSparser::setSpeed(float speed)
{
  _speed = speed;
}

void GPSparser::setAltitude(float altitude)
{
  _altitude = altitude;
}

void GPSparser::setDirection(float direction)
{
  _direction = direction;
}

void GPSparser::setSatelliteCount(int satelliteCount)
{
  _satelliteCount = satelliteCount;
}

void GPSparser::setDegrees(float degreess)
{
  _degree = degreess;
}

float GPSparser::getLatitude()
{

  return _latitude;
}

float GPSparser::getLongitude()
{
  return _longitude;
}

float GPSparser::getSpeed()
{
  return _speed;
}

float GPSparser::getAltitude()
{
  return _altitude;
}

float GPSparser::getDirection()
{
  return _direction;
}

int GPSparser::getSatelliteCount()
{
  return _satelliteCount;
}

float GPSparser::getSpeedMph()
{
  return _speed * 1.852;
}

float GPSparser::getSpeedKmh()
{
  return _speed * 1.150779;
}

float GPSparser::getDegree()
{
  return _degree;
}

// Initializes all the GPS required modes

int GPSparser::checkGpsSignal()
{
  debugMessages("AT+QGPSLOC?");
  GSMSERIAL.println("AT+QGPSLOC?");
  if (EG21_Response(1000, GPSSIGNAL))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
void GPSparser::initializeGpsModes()
{
  if (initializeGpsMode())
  {
    debugMessages("GPS mode Successfully initialized");
    if (initializeNmeaMode())
    {
      debugMessages("GPS NMEA mode Initialized");
    }
    else
    {
      debugMessages("GPS NMEA mode Initialization failed");
    }
  }
  else
  {
    debugMessages("GPS mode initialization failed");
  }
}

// Initializes GPS mode
int GPSparser::initializeGpsMode()
{
  debugMessages("AT+QGPS=1");
  GSMSERIAL.println("AT+QGPS=1");
  if (EG21_Response(1000, GPSMODE))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// Initializes NMEA mode in GPS
// Note : Mandatory for GPS
int GPSparser::initializeNmeaMode()
{
  debugMessages("AT+QGPSCFG=\"nmeasrc\",1");
  GSMSERIAL.println("AT+QGPSCFG=\"nmeasrc\",1");
  if (EG21_Response(1000, NMEAMODE))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// GPS data parser

int GPSparser ::parseLocData(char arr[])
{
  char *token = strtok(arr, ",");
  char *LocArray[17];
  while (token != NULL)
  {
    LocArray[gpsLocCount] = token;
    token = strtok(NULL, ",");
    gpsLocCount++;
  }

  gpsLocCount = 0;

  String dLat = String(LocArray[1]);
  String dLon = String(LocArray[2]);
  String Speed = String(LocArray[8]);
  String degreess = String(LocArray[3]);
  String Satellites = String(LocArray[10]);
  String Altitude = String(LocArray[4]);

  int latLength = dLat.length();
  int lonLength = dLon.length();
  //
  String lanDirection = String(dLat.charAt(latLength - 1));
  String lonDirection = String(dLon.charAt(lonLength - 1));

  dLat.remove(latLength - 1);
  dLon.remove(lonLength - 1);

  setLatitude(dLat.toFloat(), lanDirection);
  setLongitude(dLon.toFloat(), lonDirection);
  setSpeed(Speed.toFloat());

  setSatelliteCount(Satellites.toInt());
  setAltitude(Altitude.toFloat());

  debugMessages(lanDirection);
  debugMessages(lonDirection);
  debugMessages(Speed);
  debugMessages(degreess);
  debugMessages(Satellites);
}

int GPSparser::EG21_Response(unsigned int timeout, int code)
{
  int arr = 0;
  unsigned long current_time = millis();
  char EC20_reply_char = '0';
  char EC20_reply[5000] = {0};
  memset(EC20_reply, 0, sizeof(EC20_reply));

  while ((millis() - current_time) < timeout)
  {
    if (GSMSERIAL.available())
    {
      EC20_reply_char = GSMSERIAL.read();
      EC20_reply[arr] = EC20_reply_char;
      arr++;
    }

    EC20_response_count = arr;
  }

  if (EC20_response_count > 1)
  {
    switch (code)
    {
    case GPSMODE:

      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        debugMessages("GPS activated");
        return true;
      }
      else if (EC20_reply[3] == 'C' && EC20_reply[4] == 'M' && EC20_reply[5] == 'E')
      {
        String ErCode = String(EC20_reply[14]) + String(EC20_reply[15]) + String(EC20_reply[16]);
        int ErrorCode = ErCode.toInt();
        // Switching between the error codes that are generated when the GPS mode is enabled
        switch (ErrorCode)
        {
        case 504:
          debugMessages("Session is ongoning");
          return true;
          break;

        default:
          debugMessages("Reached Default");
          return false;
        }
      }
      else
      {
        debugMessages("Failed to initialize GPS");
        return false;
      }

      break;

    case NMEAMODE:

      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        return true;
      }
      else
      {
        return false;
      }

      break;

    case GPSSIGNAL:
      //                             ESP32.print("EC20 response:");
      //                             for (int a = 0; a < EC20_response_count; a++)
      //                             {
      //                               ESP32.print(EC20_reply[a]);
      //                             }

      if (EC20_reply[3] == 'C' && EC20_reply[4] == 'M' && EC20_reply[5] == 'E')
      {
        String ErCode = String(EC20_reply[14]) + String(EC20_reply[15]) + String(EC20_reply[16]);
        int ErrorCode = ErCode.toInt();
        // Switching between the error codes that are generated when the GPS mode is enabled
        switch (ErrorCode)
        {
        case 516:
          debugMessages("Not fixed now");
          return false;
          break;
        default:
          debugMessages("Reached Default");
          return true;
        }
      }
      else
      {
        parseLocData(EC20_reply);
        return true;
      }

      break;

    case GPSRMC:
      //      parseRmcData(EC20_reply);

      //      ESP32.print("EC20 response:");
      //      for (int a = 0; a < EC20_response_count; a++) {
      //        ESP32.print(EC20_reply[a]);
      //      }

      break;

    case GPSGGA:
      //      parseGgaData(EC20_reply);
      break;
    default:
      debugMessages("Default Switch");
    }
  }
  else
  {
    debugMessages("No response from the module");
    return 0;
  }
}
