#ifndef __MQTT_H_
#define __MQTT_H_

#define GSMSERIAL Serial2
#define MQTTMODE 109
#define MQTTSUBSCRIBE 110
#define MQTTPUBLISH 111
#define MQTTRECEIVE 112
#define MQTTPORT 113
#define MQTTCONN 114

class MQTT
{

private:
  // Mqtt Username and password variable
  String _username;
  String _password;

public:
  String mqttHost;
  char mqttSubTopic[50] = "test";
  char mqttPubTopic[50] = "/zed/gpsdata";
  unsigned int mqttPortNum = 1883;
  unsigned int sizeOfMQTTMsq;
  bool mqttMsgSendBit;
  bool mqttMsgSendAck;
  unsigned int EC20_response_count = 0;
  char sim_status[10] = {0};
  char MQTT_DATA[200] = {0};
  bool configFlag = true;
  bool debugMode;

  void mqttBegin();
  bool mqttConnect();
  bool mqttPub(String mqttMsg, String topic);
  bool mqttSub();
  bool mqttSend(String mqttMsg);
  void setMqttHost(String hostname, unsigned int port);
  void setUsername(String username, String password);
  //  void setMqttSub(char mqttsub[]);
  //  void setMqttPub(char mqttPub[]);

  void setDebugMode(bool debugModeU);
  void debugMessages(String MSG);

  int EG21_Response_Mqtt(unsigned int timeout, int code);
  int EC20_MQTT_RECV(unsigned int timeout);
};

#endif //__MQTT_H_
