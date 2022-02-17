#include <Arduino.h>
#include "Mqtt.h"
#include <WebSocket.h>

void MQTT::setUsername(String username, String password)
{
  _username = username;
  _password = password;
}

void MQTT::setDebugMode(bool debugModeU)
{
  debugMode = debugModeU;
}

void MQTT::debugMessages(String MSG)
{

  if (debugMode)
  {
    Serial.println(MSG);
    notifyClientsReceived(MSG);
  }
}

void MQTT::mqttBegin()
{

  if (MQTT::mqttConnect())
  {
    debugMessages("MQTT Connection Initiated");
    MQTT::mqttSub();
  }
  else
  {
    debugMessages("MQTT Connection failed");
    //    ESP.restart();
  }
}

bool MQTT::mqttConnect()
{
  if (configFlag)
  {
    debugMessages("AT+QMTCFG=\"recv/mode\",0,0,1");
    GSMSERIAL.println("AT+QMTCFG=\"recv/mode\",0,0,1");
  }
  if (MQTT::EG21_Response_Mqtt(2000, MQTTMODE))
  {
    //    return 1;
    debugMessages("Success");
    debugMessages("AT+QMTOPEN=0,\"");
    debugMessages(mqttHost);
    debugMessages("\",");
    debugMessages(String(mqttPortNum));
    GSMSERIAL.print("AT+QMTOPEN=0,\"");
    GSMSERIAL.print(mqttHost);
    GSMSERIAL.print("\",");
    GSMSERIAL.println(mqttPortNum);

    if (MQTT::EG21_Response_Mqtt(1000, MQTTPORT))
    {
    retryConn:
      debugMessages("Success2");
      debugMessages("AT+QMTCONN=0,\"3\",\"" + _username + "\",\"" + _password + "\"");
      GSMSERIAL.println("AT+QMTCONN=0,\"3\",\"" + _username + "\",\"" + _password + "\"");

      debugMessages("retry mqtt connection");
      if (MQTT::EG21_Response_Mqtt(1000, MQTTCONN))
      {
        debugMessages("Successful");
        return true;
      }
      else
      {
        debugMessages("Failedd");
        goto retryConn;
        return false;
      }
    }
    else
    {
      debugMessages("Failed");
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool MQTT::mqttSub()
{
retrySub:
  debugMessages("AT+QMTSUB=0,1,\"");
  debugMessages(mqttSubTopic);
  debugMessages("\",0");
  GSMSERIAL.print("AT+QMTSUB=0,1,\"");
  GSMSERIAL.print(mqttSubTopic);
  GSMSERIAL.println("\",0");

  if (EG21_Response_Mqtt(2000, MQTTSUBSCRIBE))
  {
    Serial.println("Subscription Success");
    return true;
  }
  else
  {
    Serial.println("Subscription failed");
    goto retrySub;
    return false;
  }
}

bool MQTT::mqttPub(String mqttMsg, String topic)
{
  //  mqttMsg = "This is test data, hello MQTT";
  int lengthofstr = mqttMsg.length();
  debugMessages("AT+QMTPUBEX=0,0,0,0,\"" + topic + "\"," + String(lengthofstr));
  GSMSERIAL.println("AT+QMTPUBEX=0,0,0,0,\"" + topic + "\"," + String(lengthofstr));

  MQTT::EG21_Response_Mqtt(1000, MQTTPUBLISH);
  GSMSERIAL.println(String(mqttMsg));
  if (MQTT::EG21_Response_Mqtt(1000, MQTTPUBLISH))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int MQTT::EG21_Response_Mqtt(unsigned int timeout, int code)
{
  int arr = 0;
  unsigned long current_time = millis();
  char EC20_reply_char = '0';
  char EC20_reply[5000] = {
      0};
  memset(EC20_reply, 0, sizeof(EC20_reply));

  while ((millis() - current_time) < timeout)
  {
    if (GSMSERIAL.available())
    {
      EC20_reply_char = GSMSERIAL.read();
      EC20_reply[arr] = EC20_reply_char;
      //      debugMessages(String(EC20_reply_char));
      arr++;
    }
    EC20_response_count = arr;
  }

  if (EC20_response_count > 1)
  {
    // debugMessages("All response");
    //     for (int a = 0; a < EC20_response_count; a++)
    //           {
    //             debugMessages(EC20_reply[a]);
    //           }

    switch (code)
    {
    case MQTTMODE:
      debugMessages("Inside MQTT Mode");
      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        return true;
      }
      else
      {
        return false;
      }
      break;

    case MQTTPORT:
      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    case MQTTCONN:
      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    case MQTTSUBSCRIBE:
      //          debugMessages("EC20 response:");
      //          for (int a = 0; a < EC20_response_count; a++)
      //          {
      //            debugMessages(EC20_reply[a]);
      //          }

      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        return true;
      }
      else
      {
        return false;
      }

      break;
    case MQTTPUBLISH:
      //              debugMessages("EC20 response:");
      //          for (int a = 0; a < EC20_response_count; a++)
      //          {
      //            debugMessages(EC20_reply[a]);
      //          debugMessages(a);
      //          }
      //
      if (EC20_reply[9] == 'Q' && EC20_reply[12] == 'P' && EC20_reply[14] == 'B' && EC20_reply[19] == '0' && EC20_reply[21] == '0' && EC20_reply[23] == '0')
      {
        return 1;
      }
      else
      {
        return 0;
      }

      break;

    default:

      break;
    }
  }
}
//
void MQTT::setMqttHost(String hostname, unsigned int port)
{
  mqttHost = hostname;
  mqttPortNum = port;
}

// void MQTT::setMqttSub(char* mqttsub)
//{
//   mqttSubTopic = mqttsub;
// }

// void MQTT::setMqttPub(char* mqttPub)
//{
//   mqttPubTopic = mqttPub;
// }
