#include "Gsm.h"
#include <WebSocket.h>

// The following function is to set the debug mode to true to view all the commands that are geting executed one by one

void GsmParser::setDebugMode(bool debugModeU)
{
  debugMode = debugModeU;
}

// The function prints the debug messages
void GsmParser::debugMessages(String MSG)
{

  if (debugMode)
  {
    Serial.println(MSG);
    notifyClientsReceived(MSG);
  }
}

/*
  The fucntion is to set the Serial mode for Establishing
  the connection between MCU and the Module
*/
void GsmParser::setSerial(String seri)
{
  if (seri == "Serial")
  {
    serialm = Serial;
  }
  else if (seri == "Serial2")
  {
    serialm = Serial2;
  }
  else if (seri == "Serial3")
  {
    serialm = Serial2;
  }
  else
  {
    debugMessages("Serial not assigned");
  }
}

void GsmParser::initializeGsmModule()
{
  int GsmModuleCheck = 0;
  echoOff();
  delay(3000);
  while (GsmModuleCheck < 6)
  {
    if (isModuleConnected())
    {
      debugMessages("Successfully connected");
      if (checkSIM())
      {
        debugMessages("SIM found");
        checkSignal();
        break;
        //        mqttParser.mqttBegin();
      }
      else
      {
        debugMessages("SIM not Found");
        debugMessages("Retrying to detect sim in 5 seconds");
        int i = 0;
        while (i < 5)
        {
          i++;
          delay(1000);
          checkSIM();
        }
      }
    }
    else
    {
      debugMessages("Module not connected");
      delay(250); // delay 0.25 Second
      ++GsmModuleCheck;
      //    ESP.restart();
    }
  }
}

// The following function disables the ECHO mode in the module
void GsmParser::echoOff()
{
  debugMessages("ATE0");
  GSMSERIAL.println("ATE0");
  delay(1000);
}

int GsmParser::isModuleConnected()
{
  debugMessages("AT");
  GSMSERIAL.println("AT");
  if (EG21_Response(500, CONNCHECK))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int GsmParser::checkSIM()
{
  debugMessages("AT+CPIN?");
  GSMSERIAL.println("AT+CPIN?");
  if (EG21_Response(2000, SIMCHECK))
  {
    return 1;
  }
  else
  {
    return 0;
  }
  delay(2000);
}
int GsmParser::checkSignal()
{
  debugMessages("AT+QSPN");
  GSMSERIAL.println("AT+QSPN");
  if (EG21_Response(1000, DATASIGNAL))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int GsmParser::EG21_Response(unsigned int timeout, int code)
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
      arr++;
    }
    EC20_response_count = arr;
  }
  if (EC20_response_count > 1)
  {

    switch (code)
    {

    case CONNCHECK: // Basic AT command

      if (EC20_reply[2] == 'O' && EC20_reply[3] == 'K')
      {
        debugMessages("Module recognised");
        return true;
      }
      else
      {
        debugMessages("Module not recognised");
        return false;
      }
      break;

    case SIMCHECK:

      if (EC20_reply[2] == '+' && EC20_reply[3] == 'C' && EC20_reply[4] == 'P' && EC20_reply[7] == ':')
      {

        int j = 0;
        for (int i = 9; i < 14; i++)
        {
          sim_status[j] = EC20_reply[i];
          j++;
        }
        if (!strcmp(sim_status, "READY"))
        {

          return true;
        }
        else
        {

          return false;
        }
      }
      else
      {
        debugMessages("BUG");
        return false;
      }
      break;

    case DATASIGNAL:
      if (EC20_reply[50] == 'O' && EC20_reply[51] == 'K')
      {
        debugMessages("Signal available");
        return true;
      }
      else
      {
        debugMessages("fata" + String(EC20_reply[50]));
        debugMessages("Signal not found");
        return false;
      }
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
