/*
The following library is developed for communicating with the GSM moodule
Supported Modules : EG25-G , EG21-G
*/

// testing git
// Testing ErrorHandling
#include <Gsm.h>
#include <Gps.h>
#include <Mqtt.h>
#include <WebSocket.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

#define MQTTHOST ""
#define MQTTPORT 1883
#define MQTTSUBTOPIC ""
#define LOGTOPIC ""
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
String GPSTOPIC = "";
String BLEREQUEST = "";
// object to access Eg25 library
int gpsFlag = 0;
int buzzer = 4;
GsmParser gsm;
GPSparser gps;
MQTT mqtt;

// Debug Mode has been set to true to view the debug messages
//   gsm.setDebugMode(true);
String DeviceMac = WiFi.macAddress();

String errorLog(String logs)
{

  const size_t capacity = JSON_OBJECT_SIZE(10);
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject &List_0 = jsonBuffer.createObject();
  List_0["Error"] = logs;
  List_0["Status"] = 4;
  List_0["Deviceid"] = DeviceMac;

  char JSONmessageBuffer[450];
  List_0.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  return String(JSONmessageBuffer);
}

void setup()
{
  GSMSERIAL.begin(115200);
  Serial.begin(115200);
  Serial.print("program started");
  WiFi.begin("Zed", "Wireless4U!");
  pinMode(buzzer, OUTPUT);
  int wifiCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Waiting for WiFi");
    wifiCounter++;
    if (wifiCounter > 30)
    {
      Serial.println("Exiting while loop and entering into main loop");
      break;
    }
  }
  if (WiFi.SSID() == "Zed")
  {
    initWebSocket();
    if (!MDNS.begin("zigs"))
    {
      Serial.println("Error starting mDNS");
      return;
    }
  }

  delay(5000);

  // The following are GSM Functions

  gsm.setDebugMode(true);
  gsm.setSerial("Serial2");
  gsm.initializeGsmModule();

  // The following are GPS functions
  gps.setDebugMode(true);
  gps.initializeGpsModes();

  // The following are mqtt functions
  mqtt.setDebugMode(true);
  //(mqtt.zig-web.com, 1883)
  mqtt.setMqttHost(MQTTHOST, MQTTPORT);
  //(subscription topic)
  //  mqtt.setMqttSub(MQTTSUBTOPIC);
  mqtt.setUsername(MQTT_USERNAME, MQTT_PASSWORD);
  mqtt.mqttBegin();
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0)
  {
    String str;
    str = Serial.readStringUntil('\1');

    int str_len = str.length() + 1;

    // Prepare the character array (the buffer)
    char char_array[str_len];

    // Copy it over
    str.toCharArray(char_array, str_len);
    Serial.println(char_array);

    // stop requested
    if (strcmp(char_array, "9") == 0)
    {
      digitalWrite(buzzer, HIGH);
      delay(5000);
      digitalWrite(buzzer, LOW);
    }
    // sos requested
    if (strcmp(char_array, "7") == 0)
    {
      digitalWrite(buzzer, HIGH);
      delay(10000);
      digitalWrite(buzzer, LOW);
    }

    if (mqtt.mqttPub(String(char_array), BLEREQUEST))
    {
      Serial.println("Successfully Published");
      //        digitalWrite(buzzer,HIGH);
      //        delay(5000);
      //        digitalWrite(buzzer,LOW);
    }
    else
    {
      Serial.println("Failed Published");
    }
  }
  clientscleanup();
  if (gps.checkGpsSignal())
  {
    if (gpsFlag < 1)
    {

      String jsonErroLog = errorLog("GPS signal available");
      if (mqtt.mqttPub(jsonErroLog, LOGTOPIC))
      {
        Serial.println("Successfully Published");
      }
      else
      {
        Serial.println("Failed Published");
      }
      gpsFlag++;
    }
    float Latitude = gps.getLatitude();
    float Longitude = gps.getLongitude();
    float speed = gps.getSpeedMph();
    float speed2 = gps.getSpeedKmh();
    int satelliteCount = gps.getSatelliteCount();
    float Altitude = gps.getAltitude();
    float degreess = gps.getDirection();

    Serial.println(Latitude, 6);
    Serial.println(Longitude, 6);
    Serial.println(speed, 2);
    Serial.println(speed2, 2);
    Serial.println(satelliteCount);
    Serial.println(Altitude);
    const size_t capacity = JSON_OBJECT_SIZE(20);
    DynamicJsonBuffer jsonBuffer(capacity);

    JsonObject &List_0 = jsonBuffer.createObject();

    List_0["Latitude"] = Latitude;
    List_0["Longitude"] = Longitude;
    List_0["Speed"] = speed;
    List_0["Direction"] = degreess;
    List_0["Voltage"] = 3.28;
    List_0["Altitude"] = Altitude;
    List_0["Token"] = "3Y1QwEDfikGni1PPouV7aw==";
    List_0["Deviceid"] = DeviceMac;
    List_0["Wifisignal"] = -50;
    List_0["Statellitecount"] = satelliteCount;

    char JSONmessageBuffer[450];
    List_0.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    if (mqtt.mqttPub(String(JSONmessageBuffer), GPSTOPIC))
    {
      Serial.println("Successfully Published");
    }
    else
    {
      Serial.println("Failed Published");
    }
    notifyClientsReceived(String(Latitude, 6));
    notifyClientsReceived(String(Longitude, 6));
    notifyClientsReceived(String(speed2, 2));
    notifyClientsReceived(String(satelliteCount));
    notifyClientsReceived(String(Altitude, 3));
  }
  else
  {
    Serial.println("Waiting for signal");
    String jsonErroLog = errorLog("Waiting for GPS signal");
    if (mqtt.mqttPub(jsonErroLog, LOGTOPIC))
    {
      Serial.println("Successfully Published");
    }
    else
    {
      Serial.println("Failed Published");
    }
  }
}
