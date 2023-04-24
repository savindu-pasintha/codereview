#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>
#include <Keypad.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <PubSubClient.h>

#define lednum 60
#define master 1
#define slave 0
#define button 13
#define buzzer 4
#define IRthresh 1
#define mComm 22

bool scanCheck = 0;
bool pingCheck = 0;
unsigned long startTimeM = 0;
int i = 1;

String serverResponse = "";
String orderStatusJson = "";
String trayStatusJson = "";
String prevTrayStatusJson = "";
int totalTrays = 1;
String events = "";
String prevEvent = "";
String buttonPress = "";
String errors = "";
String  orderID = "";
String  currentOrderID = "";
String trayType = "slave"; 
String trayResponse = "";
bool completedCheck = false;
int x=0;
int y=0;
int z=0;
int b=0;
int config = 0;
String command;
String sMsg = "?";
classes
Preferences preeEEPROM; 

int trayMode;
String slaveAddress = "";
String slaveAddressLog = "";
String processedTransactionLog = "";
String orderCompletedLog = "";
String orderAllocation = "";
bool trayLight = false;
int transactionLogIndex = 1;
String trayStatusLog = "";
String trayNumber = "";
String orderStatus = "";
String transactionID = "";
String idTransaction = "";
String trayStatusData = "";
String buttonState = "";
String prevOrderStatusJson = "";
String prevMobiData = "";
bool allocate = false;
bool process = false;
int address;
String sameLog;
String S_address;
String trayLog="";
int feedBack=0;
int totalSlave = 0;
int trayBrightness = 255;

uint32_t white = 0xFFFFFF;
uint32_t magenta = 0xFF00FF;
uint32_t teal = 0x00FFFF;
uint32_t red = 0xFF0000;
uint32_t green = 0x1BB541;
uint32_t purple = 0x800080;

LED Strip Pins
int ledA = 15;
int ledB = 18;
int ledC = 5;
int ledD = 14;

int readA;
int readB;
int readC;
int readD;
String state2 = "";
Sensor Pins
int sensA = 34;
int sensB = 35;
int sensC = 32;
int sensD = 33;
Variable for led Strip
int ledCheck = 0;
button Pins
int eButt  = 27;
int eLed  = 19;


MQTT Broker
const char *mqtt_broker = "164.90.146.205";
const char *topic = "Customers/Data";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
Create a WiFi client object
WiFiClient trayMaster;
Create an MQTT client object, using the WiFi client object as its network client
PubSubClient client(trayMaster);

#define ON true
#define OFF false

Scanner variables 
String scanner =""; 
String scannedData ="";
String prev; 
AsyncWebServer server(80);
const char* soft_ap_ssid = "esp32-ap"; 
const char* soft_ap_password = "password";  
const char* mobiPOSurl = "http://164.90.146.205:1880/MobiPOSdata";
const char* orderJsonUrl = "http://164.90.146.205:1880/orderJson";
const char* ssid = "LabAP";
const char* password = "preelabs12345679"; 
long wifiTimeOut = 2000;
String mobiData = "";
String orderStatusJSON = "";
String currentOrderStatus = "";
to see if the cell was ocupied

String buttonsBuffer = "";
String eventsBuffer = "";

int buttonStateV = 0;
int lastMillis = 0;

String mInstruction;
String sameInstruction;
String rMsg = "";
String orderInfo = "";

////////slave variables
String mCombo = "";
String trayResponseJson = "";
String previousInstruction = "";
String newInstruction = "";
String masterPing = "";
String instructionBuffer ="";
bool orderBarcodeFound = false;
bool orderPinFound = false;
bool scanFound = false;
int orderStatusSize = 0;
/////////////////////NEW VARIABLes end

String thisTraysAddress = "1";
String serverResponse = "";
String orderStatusJson = "";
String trayStatusJson = "";
String slaveResponse = "";
without trayallocation:
String testServerResponse =  "{\"status\":\"success\",\"result\":[{\"idTransaction\":\"60\",\"orderStatus\":\"scanned\",\"linkToken\":\"7254\"},{\"idTransaction\":\"62\",\"orderStatus\":\"created\",\"linkToken\":\"9640\"},{\"idTransaction\":\"64\",\"orderStatus\":\"staged\",\"linkToken\":\"9541\"}]}";
with tray allocation:
String testServerResponse = "{\"status\":\"success\",\"result\":[{\"idTransaction\":\"60\",\"orderStatus\":\"Scanned\",\"linkToken\":\"7254\",\"traysAllocated\":[\"2\",\"3\"]},{\"idTransaction\":\"62\",\"orderStatus\":\"created\",\"linkToken\":\"9640\",\"traysAllocated\":[\"0\"]},{\"idTransaction\":\"61\",\"orderStatus\":\"created\",\"linkToken\":\"4621\",\"traysAllocated\":[\"1\"]}]}";
/*
{
    "status": "success",
    "result": [
        {
            "idTransaction": "60",
            "orderStatus": "scanned",
            "linkToken": "7254"
        },
        {
            "idTransaction": "62",
            "orderStatus": "created",
            "linkToken": "9640"
        },
        {
            "idTransaction": "64",
            "orderStatus": "staged",
            "linkToken": "9541"
        }
    ]
}
*/
String testOrderStatusJson = "{\"Orders\":[{\"OrderID\":\"38\",\"orderStatus\":\"created\",\"traysAllocated\":[\"1\"]},{\"OrderID\":\"4\",\"orderStatus\":\"Scanned\",\"traysAllocated\":[\"2\",\"3\"]}]}";
/*
{
  "Orders": [
    {
      "OrderID": "3",
      "orderStatus": "created",
      "traysAllocated": [
        "1"
      ]
    },
    {
      "OrderID": "4",
      "orderStatus": "Scanned",
 "traysAllocated": [
        "2",
        "3"
      ]
    }
  ]
}
*/
String testTrayStatusJson = "{\"Trays\":[{\"Addr\":\"1\",\"status\":\"allocated\",\"trayInstruction\":\"place\",\"orderID\":\"3\"},{\"Addr\":\"2\",\"status\":\"occupied\",\"trayInstruction\":\"keep\",\"orderID\":\"4\"}]}";

/* //check this
{
  "Trays": [
{  
  {
      {
      "Addr": "1",
      "status": "allocated",
	"trayInstruction": "place",
      "orderID": "3"
    },
    {
      "Addr": "2",
      "status": "occupied",
	"trayInstruction": "keep"
      "orderID": "4"
    }
  ]
}
*/

String testSlaveResponse = "{\"addr\":\"0\",\"orderID\":\"3\",\"state\":\"placing\",\"events\":\"itemDetected\",\"button\":\"pressed\",\"errors\":\"na\"}";

 /* check this

    {
      "addr": "0",
      "orderID": "3",
      "state": "placing",
      "events": "itemDetected",
      "button": "pressed",
      "errors": "na"
    }

 */


////////////////////NEW VARIABLes end



mqtt

void serialPrintS(String _label, String _value, bool _enable)
{
  if(_enable)
  {
    Serial.println(_label);
    Serial.println(_value);
  }
}

void MQTTinit(const char *mqtt_broker, const char *topic, const char *mqtt_username, const char *mqtt_password, const int mqtt_port, bool _debug) {
  Connect to the MQTT broker
  client.setServer(mqtt_broker, mqtt_port); // Set the MQTT broker IP address and port
   Set the MQTT callback function
  client.setCallback(callback);
  Wait until the client is connected to the MQTT broker
  while (!client.connected()) 
  { 
    String client_id = "Preelabs-Limited-";
    Generate a unique client ID using the MAC address of the device
    client_id += String(WiFi.macAddress()); 
    Serial.printf("The client %s connects to the mosquitto mqtt broker\n", client_id.c_str());
    Connect to the MQTT broker with the generated client ID, username, and password
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) 
    { 
      if (_debug) 
      {
        Serial.println("Successfully connected to Mosquitto Broker");
      }
    }
    else 
    {
      Serial.print("Failed to connect to the Mosquitto Broker with state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  Subscribe to the specified MQTT topic
  client.subscribe(topic); 
  if (_debug) 
  {
    Serial.print("Subscribed to MQTT topic: ");
    Serial.println(topic);
  }
}

int ledControl(int _ledPin, int _totalLed, int _start, int _stop, uint32_t _color, bool _on, int _flicker, String _action,int _brightness)
{
  Adafruit_NeoPixel cell(_totalLed, _ledPin, NEO_GRB + NEO_KHZ800);

  if (_on == true)
  {
    if (_flicker == 0)
    {
      for (int i = _start; i < _stop; i++)
      {
        cell.setBrightness(_brightness);
        cell.setPixelColor(i, _color);
        cell.show();
      }
      _flicker = 1;
    }
    if (_action.equals("blink"))
    {
      delay(100);
      for (int i = _start; i < _stop; i++)
      {
        cell.setBrightness(0);
        cell.setPixelColor(i, _color);
        cell.show();
      }
      delay(100);
      for (int i = _start; i < _stop; i++)
      {
        cell.setBrightness(_brightness);
        cell.setPixelColor(i, _color);
        cell.show();
      }
      delay(100);
      for (int i = _start; i < _stop; i++)
      {
        cell.setBrightness(0);
        cell.setPixelColor(i, _color);
        cell.show();
      }
      delay(100);
      for (int i = _start; i < _stop; i++)
      {
        cell.setBrightness(_brightness);
        cell.setPixelColor(i, _color);
        cell.show();
      }
    }
    else if (_action.equals("fade"))
    {
      for (int j = 250; j >= 50; j -= 3)
      {
        for (int i = _start; i < _stop; i++)
        {
          cell.setPixelColor(i, _color);
          cell.setBrightness(j);
          cell.show();
        }
      }
      for (int j = 50; j < 251; j += 3)
      {
        for (int i = _start; i < _stop; i++)
        {
          cell.setPixelColor(i, _color);
          cell.setBrightness(j);
          cell.show();
        }
      }
    }
  }
  else
  {
    if (_flicker == 0)
    {
      for (int i = 0; i < _totalLed; i++)
      {
        if (i < _start || i > _stop - 1)
        {
          cell.setBrightness(_brightness);
          cell.setPixelColor(i, _color);
          cell.show();
        }
      }
      _flicker = 1;
    }
  }
  return _flicker;
}
String readSerial1(long _timeOut,bool _debug)
{
  String _rMsg;
  int _breakState  = 0;
  save received start time 
  unsigned long _startTime = millis();
  unsigned long _earlyTimeout = 50; 
  bool _characterReceived = 0;
  String _r;


  
  While waitime is greater than current time 
  while((Serial1.available()) || (millis() - startTime = timeOut))
  while (millis() < _startTime + _timeOut) 
  { 
    If data available, store data 
  
    if (Serial1.available()) 
    { 
      _characterReceived = 1;
      char c = Serial1.read(); 
      _rMsg += c; 
      If end indcator is recieved, end loop 
      if(c == '}') 
      { 
        _breakState = 1;
         Serial.println(String(millis() - _startTime));
         break; 
        /*_r += c;
        if (_r.equals("}}"))
        {
          break; 
        }
      } */
      }
    }
    if(millis() >= _earlyTimeout + _startTime )
    {
      Serial.println("no initial msg received in time, early timeout");
      break;
    } 
  }
  if (_debug == true)
  {
    

    Serial.println("Received: " );
    Serial.println(_rMsg);
    Serial.println("................................................................................................." );
    if(_breakState == 0)
    {
      Serial.println("broke via timeout" );
    }
    else
    {
      Serial.println("broke via bracket" );
    }
  }
  return _rMsg;
}
void writeEEPROM(String _fileName, String _data, bool _debug)
{
  if (_debug == true)
  {
    Serial.println("Storing " + _fileName + " to EEPROM");
  }
  Open the storage location in eeprom
  preeEEPROM.begin("my-app", false);
  Store data
  preeEEPROM.putString(_fileName.c_str(), _data.c_str());
  close location
  preeEEPROM.end();
}
String readEEPROM(String _fileName, bool _debug)
{
  preeEEPROM.begin("my-app", false);
  String _data = preeEEPROM.getString(_fileName.c_str());
  /*if (_data.length() < 2)
  {
    _data = "0";
  }*/
  preeEEPROM.end();
  if (_debug == true)
  {
    Serial.println("Function \"readEEPROM\":\n");
    Serial.println("Reading " + _fileName + " from EEPROM");
    Serial.println(_data);
    Serial.println("................................................................................................." );
  }
  return _data;
}
/*
bool preePost(String _postEndPoint)
{
  if (WiFi.status() == WL_CONNECTED)
  {
      HTTPClient http;
      http.begin(_postEndPoint);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(preePostPayload);

      if (httpResponseCode == 200)
      {
          postResponse = http.getString(); //Get the response to the request
          Serial.println("\r\n--Response from server--");
          Serial.println(httpResponseCode); //Print return code
          Serial.println(postResponse);    //Print request answer
          Serial.println("--End of Response--\r\n");
          http.end();
          _parameterCount = 0;
          return true;
      }
      else
      {
          cledBlink(FASTBLINK, 3); //connect
          Serial.print("\r\n**ERROR: Post Request Failed: ");
          Serial.println(httpResponseCode);
          http.end();
          _parameterCount = 0;
          return false;
      }
  }
  else
  {
    cledBlink(FASTBLINK, 6);
    Serial.println("\r\n**ERROR: WiFi not connected");
    _parameterCount = 0;
    return false;
  }
  preePostPayload = "";
}
*/
String getMobiPOSdata(const char* _serverName,bool _debug) 
{
  HTTPClient http;
  Your IP address with path or Domain name with URL path 
  http.begin(_serverName);

  If you need Node-RED/server authentication, insert user and password below
  http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

  Send HTTP POST request
  int httpResponseCode = http.GET();
  Create String to store HTTP request
  String payload = "{}"; 
  If HTTP Response code is greater than zero do the following 
  if (httpResponseCode>0) 
  {
    Save HTTP request data
    payload = http.getString();
    If _debug is true print HTTP Response code and payload
    if(_debug == true)
    {
      Serial.println("Function \"getMobiPOSdata\":");
      Serial.println();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Payload: ");
      Serial.println(payload);
      Serial.println("................................................................................................." );
    }
  }
  If HTTP Response code is lesser than zero do the following 
  else 
  {
    If _debug is true print HTTP Response code
    if(_debug == true)
    {
      Serial.println("Function \"getMobiPOSdata\":");
      Serial.println();
      Serial.print("HTTP Response error code: ");
      Serial.println(httpResponseCode);
      Serial.println("................................................................................................." );
    }
  }
  Free resources
  http.end();
  Return payload
  return payload;
}
String updatePayload(String sPayload, String _serverName,bool _debug = false) 
{
  If debug is enable do the following 
  if(_debug)
  {
    Print the function name
    Serial.println("Function \"updatePayload\":");
    Serial.println();
  }
  Variable to store received payload
  String rPayload = "";
  Create an HTTP client object
  HTTPClient http;
  Set http request IP address or Domain name with URL path 
  http.begin(_serverName);
  Set the request headers (optional)
  http.addHeader("Content-Type", "application/json");
  Send the POST request with the payload
  int httpSendResponseCode = http.POST(sPayload);
  Check if POST request was successful
  if (httpSendResponseCode == HTTP_CODE_OK) 
  {
    Save HTTP request data
    rPayload = http.getString(); 
    End HTTP client object
    http.end();
    If debug is enable do the following
    if(_debug)
    {
      Print sent payload
      Serial.print("Payload sent successfully: ");
      Serial.println(sPayload);
      Check if Get request was successful
      if (rPayload.length() > 0) 
      {
        Print receieved payload
        Serial.print("New Payload received: ");
        Serial.println(rPayload);
      }
      Check if Get request failed
      else 
      {
        Print response code
        Serial.print("Error receiving new payload");
      }
      Serial.println("................................................................................................." );
    }
  } 
  Check if POST request failed
  else 
  {
    If debug is enable do the following
    if(_debug)
    {
      Print response code
      Serial.print("Error sending payload: ");
      Serial.println(httpSendResponseCode);
      Serial.println("................................................................................................." );
    }
  }
  return rPayload;
}
bool initWiFi(const char* _ssid, const char* _password,bool _debug) 
{
  Set WiFi mode to both Station and access point
  WiFi.mode(WIFI_AP_STA);
  Try connecting to WiFi
  WiFi.begin(_ssid, _password);
  If _debug is true display the following
  if(_debug == true)
  {
    Serial.println("Function \"initWiFi\":");
    Serial.println();
    Serial.print("WiFi Connecting to \"");
    Serial.print(_ssid);
    Serial.print("\" with passward \"");
    Serial.print(_password);
    Serial.print("\"");
  }
  Repeat if WiFi status is connecting
  while (WiFi.status() != WL_CONNECTED) 
  {
    If _debug is true display the following
    if(_debug == true)
    {
      Serial.print('.');
      delay(50);
    }
  }
  If _debug is true display the following
  if(_debug == true)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nWiFi Connected");
    }
    else
    {
      Serial.println("\nWiFi Connection failed");
    }
    Serial.println("................................................................................................." );
  }
  return WiFi.status();
}
int getPathSize(String _input,bool _debug)
{
  int _output = 0;
  DynamicJsonDocument objectBuffer(5024);
  deserializeJson(objectBuffer, _input);
  _output = objectBuffer.size();
  if (_debug == true)
  {
    Serial.print("Input: ");
    Serial.println(_input);
    Serial.print("Output: ");
    Serial.println(_output); // Serialize the Json document and save in "output"
  }
  return _output;
}
String extractNestedArray(String jsonArray,bool _debug) 
{
  String result = "";
  Parse the JSON array string
  String userDataString;
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, jsonArray);
  Get the user_data array from the result object
  JsonArray userDataArray = doc["result"];
  Loop through the user_data array
  for (JsonVariant userData : userDataArray) 
  {
    Print the user_data object as a string
    serializeJson(userData, userDataString);
  }
  result = "[" + userDataString + "]";
  If _debug is true do the following 
  if (_debug == true)
  {
    Display the user data that was extracted
    Serial.println("Function \"extractObject\":");
    Serial.println();
    Serial.println(result);
    Serial.println("................................................................................................." );
  }
  return result;
}

//////////////new functions start


String parseServerResponse(String _serverResponse)
{
  1) extract orderInfo from server response
  2) create orderstatusJson based on orderInfo
  3) use this information to populate orderStatus
  4) if orderinfo Status is "created", change orderStatus status to "added" 

  Serial.println("     function: parseServerResponse");
  String _output;
  int _length = _serverResponse.length();
  DynamicJsonDocument doc(5024 + _length); //json for server response
  deserializeJson(doc, _serverResponse);
  JsonObject obj;
  JsonArray arr;
  String arrString = "";
  obj = doc["result"].as<JsonObject>();
  obj = doc.as<JsonObject>();
  arr = doc["result"].as<JsonArray>(); //get orderInfoArray
  int arrSize = arr.size();
  JsonObject orderEntry;
  String orderEntryStatusString = "";
  String orderEntryIdString = "";

  this part is creating orderStatusJson
  orderStatusJson = "";
  DynamicJsonDocument docOrder(5024);
  deserializeJson(docOrder, orderStatusJson); //json for orderStatus
  JsonArray nestedOrder = docOrder.createNestedArray("Orders");
  JsonObject arrayOrder = nestedOrder.createNestedObject();//creates a blank nest?
  
  for(int i = 0; i < arrSize; i++)  //For every entry in orderstatus response
  {
    orderEntry = arr[i].as<JsonObject>();
    serializeJson(orderEntry["orderStatus"], orderEntryStatusString);
    Serial.print("orderEntryString ");
    Serial.println(i);
    Serial.println(orderEntryStatusString);

    nestedOrder[i]["OrderID"] = orderEntry["idTransaction"];
    nestedOrder[i]["orderStatus"] = orderEntry["orderStatus"];
    nestedOrder[i]["linkToken"] = orderEntry["linkToken"];
    nestedOrder[i]["email"] = orderEntry["email"];
    nestedOrder[i]["name"] = orderEntry["name"];
    
    JsonArray arrAllocation = orderEntry["traysAllocated"];
    String allocationBuffer = "";
    int allocationSize = arrAllocation.size();
    serializeJson(arrAllocation, allocationBuffer);

    int allocationSize = orderEntry["trayAllocation"].size();
    Serial.println("allocationSize");
    Serial.println(allocationSize);
    for(int k = 0; k < allocationSize; k++)
    {
     nestedOrder[i]["trayAllocation"][k] = orderEntry["trayAllocation"][k];
     nestedOrder[i]["traysAllocated"][k] = arrAllocation[k];
    }

    if(orderEntryStatusString.equals("\"created\""))
    {
      Serial.println("found created");
      nestedOrder[i]["orderStatus"] = "added";
      serializeJson(orderEntry["idTransaction"], orderEntryIdString);
      Serial.println(orderEntryIdString);
    }
    orderEntryStatusString = "";
  }
  serializeJson(arr, arrString);
  Serial.println("obj");
  Serial.println(obj);
  Serial.println("arr");
  Serial.println(arrString);

  serializeJson(docOrder, orderStatusJson );

  serializeJsonPretty(doc, _output);
  return _output; // may be uneccessary

}

void setOrderStatus(String _id, String _status)
{
  ISSUE id not being found
  Serial.println("***********************************function: setOrderStatus");
  String _output;
  String _orderIDString = "";
  int _length = orderStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, orderStatusJson);
  JsonArray nestedOrder = docOrder.createNestedArray("Orders");
  JsonArray nestedArray = doc.
  int orderStatusSize = doc["Orders"].size();
  Serial.println("orderStatusSize");
  Serial.println(orderStatusSize);

  _id = "\"" + _id + "\"";
  Serial.println("_id");
  Serial.println(_id);
  
  for(int i = 0 ; i < orderStatusSize ; i++)
  {
    serializeJson(doc["Orders"][i]["OrderID"],_orderIDString);
    if(_orderIDString.equals("\"" + _id + "\""))
    
    Serial.println("_orderIDString");
    Serial.println(_orderIDString);
    Serial.println("_id");
    Serial.println(_id);

    if(_orderIDString.equals(_id))
    if(_orderIDString.equals(_id))
    {
      Serial.println("id found");
      Serial.print("in position: ");
      Serial.println(i);
      Serial.print("_status: ");
      Serial.println(_status);

      doc["Orders"][i]["orderStatus"] = _status;
      break;
    }
    if( i == orderStatusSize - 1)
    {
      Serial.println("id NOT found");
    }
    _orderIDString = "";
  }
  orderStatusJson = "";
  serializeJson(doc, orderStatusJson);

}

String trayStatusInit(int _totalTrays,bool _debug = false)
{
  String _output = "";
  DynamicJsonDocument docTray(5024);
  deserializeJson(docTray, trayStatusJson);
  JsonArray nestedOrder = docTray.createNestedArray("trays");
  JsonObject arrayOrder = nestedOrder.createNestedObject();
  for(int i = 0; i < _totalTrays ; i++) // +1 because 
  {
    nestedOrder[i]["Addr"] = String(i+1); //because tray count starts at 1
    nestedOrder[i]["status"] = "available";
    nestedOrder[i]["trayInstruction"] = "standby";
    nestedOrder[i]["orderID"] = "0";

  }
  serializeJson(docTray, _output);
  if(_debug)
  {
    Serial.println("function: trayStatusInit");
    serializeJson(docTray,Serial);
  }
  return _output;
}

void setTrayStatusAndInstruction(String _addr, String _status, String _trayInstruction)
{
  Serial.println("setTrayStatusAndInstruction");
  String _output;
  String _addrString = "";
  int _length = trayStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, trayStatusJson);
  JsonArray docVal = doc["trays"];
  int trayStatusSize = docVal.size();
  Serial.println("trayStatusJson before for");
  Serial.println(trayStatusJson);
  Serial.println("trayStatusSize before for");
  Serial.println(trayStatusSize);

  for(int i = 0 ; i < trayStatusSize ; i++)
  {
    serializeJson(doc["trays"][i]["Addr"],_addrString);
    Serial.println("_addrString");
    Serial.println(_addrString);
    if(_addrString.equals("\"" + _addr + "\""))
    {
      Serial.println("addr found");
      Serial.print("in position: ");
      Serial.println(i);

      doc["trays"][i]["status"] = _status;
      doc["trays"][i]["trayInstruction"] = _trayInstruction;
      docVal[i]["status"] = _status;
      docVal[i]["trayInstruction"] = _trayInstruction;
      break;
    }
    if( i == (trayStatusSize - 1) )
    {
      Serial.println("addr NOT found");
    }
    Serial.println("i");
    Serial.println(i);
    _addrString = ""; //reset buffer
  }
  trayStatusJson = "";
  serializeJson(doc, trayStatusJson);
}


void setTraysForOrderToStandby(String _id)
{
  set all of the instructions for the trays for a given order that is being picked up to remove
  Serial.println("setTraysForOrderToRemove");
  String _output;
  String _statusString = "";
  String _orderIDString = "";
  int _length = trayStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, trayStatusJson);
  JsonArray docVal = doc["trays"];
  int trayStatusSize = docVal.size();

  Serial.println("trayStatusJson before for");
  Serial.println(trayStatusJson);
  Serial.println("trayStatusSize before for");
  Serial.println(trayStatusSize);

  for(int i = 0 ; i < trayStatusSize ; i++)
  {
    serializeJson(doc["trays"][i]["orderID"],_orderIDString);
    Serial.println("_addrString");
    Serial.println(_addrString);
    Serial.println("tray for that order found");
    if(_orderIDString.equals(_id))//equals("\"" + _id +"\""))
    {
      _id.replace("\"","");
      Serial.println("tray for that order found");
      Serial.print("in position: ");
      Serial.println(i);
      _output = String(i);

      doc["trays"][i]["status"] = _status;
      doc["trays"][i]["trayInstruction"] = _trayInstruction;
      docVal[i]["status"] = "available";
      docVal[i]["trayInstruction"] = "standby"; //Maybe move some of these outside?
      _id.replace("\"","");
      docVal[i]["orderID"] = "0";

      String _orderIDBuffer = "";
      serializeJson(doc["trays"][i]["status"], _orderIDBuffer);
      setOrderStatus(_id, "removing");
      addTrayAllocation(_id, String(i+1)); //because trays start at 1
    }
    if( i == (trayStatusSize - 1) )
    {
      Serial.println("no tay for this order  found");
    }
    Serial.println("i");
    Serial.println(i);
    _statusString = ""; //reset buffer
  }
  
  trayStatusJson = "";
  serializeJson(doc, trayStatusJson);
  Serial.println("trayStatusJson after settraysforordertoremove");
  Serial.println(trayStatusJson);
  delay(10000);

  return _output;
}

void updateAllTraysForOrder( String _id)
{
  Serial.println("function: updateAllTraysForOrder");
  String _output;
  int _length = orderStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, orderStatusJson);
  int orderStatusSize = doc["Orders"].size();
  Serial.println("orderStatusSize");
  Serial.println(orderStatusSize);
  _id = "\"" + _id + "\"";
  for(int i = 0 ; i < orderStatusSize ; i++)
  {
    String _orderIDString = "";
    serializeJson(doc["Orders"][i]["OrderID"],_orderIDString);
    _orderIDString = _orderIDString.substring(1, _orderIDString.length() - 1);
    Serial.println("_id");
    Serial.println(_id);
    Serial.println("_orderIDString");
    Serial.println(_orderIDString);
    if(_orderIDString.equals(_id))
    {
      Serial.println("id found");
      Serial.print("in position: ");
      Serial.println(i);
      String _arrString = "";
      Extract each value in "traysAllocated"
      JsonArray traysAllocated = doc["Orders"][i]["traysAllocated"].as<JsonArray>();
      String _orderStatus = doc["Orders"][i]["orderStatus"].as<String>();
      for (JsonVariant tray : traysAllocated) 
      {
        String _slaveAddr = tray.as<String>();
        Serial.println("_slaveAddr");
        Serial.println(_slaveAddr);
        if(_orderStatus.equals("staged"))
        {
          setTrayStatusAndInstruction(_slaveAddr, "occupied", "keep");
        }
        else if(_orderStatus.equals("removing"))
        {
          setTrayStatusAndInstruction(_slaveAddr, "occupied", "remove");
        }
        else if(_orderStatus.equals("completed"))
        {
          setTrayStatusAndInstruction(_slaveAddr, "available", "standby");
        }
      }
     /* serializeJson(doc["Orders"][i]["traysAllocated"],_arrString);
      _arrString = _arrString.substring(1,4);
      Serial.println("_arrString");
      Serial.println(_arrString);
*/
      TO DO:
      get each entry for arrString, 
      use in a for loop to 
      setTrayStatusAndInstruction(_slaveAddr, "occupied", "keep");
      one by one

    } 
  }
  orderStatusJson = "";
  serializeJson(doc, orderStatusJson);
}

void parseSlaveResponse(String _slaveResponse, String _slaveAddr)
{
  code for master parsing slave response
  Serial.println("******************************************function: parseSlaveResponse");
  String _output;
  String _currentInstruction = "";
  String _previousInstruction = "";
  String _orderIDString = "";
  String _instructionBuffer = "";
  String _addrBuffer = "";
  String _slaveAddrBuffer = _slaveAddr;
  String _orderIDBuffer = "";
  String _stateBuffer = "";
  String _eventsBuffer = "";
  String _buttonBuffer = "";
  String _errorsBuffer = "";
  int _length = _slaveResponse.length();
  DynamicJsonDocument sResp(5024 + _length);
  deserializeJson(sResp, _slaveResponse);
  serializeJson(sResp["Addr"], _addrBuffer);
  serializeJson(sResp["slaveAddr"], _addrBuffer);
  serializeJson(sResp["orderID"], _orderIDBuffer);
  serializeJson(sResp["state"], _stateBuffer);
  serializeJson(sResp["button"], _buttonBuffer);
  serializeJson(sResp["events"], _eventsBuffer);
  serializeJson(sResp["errors"], _errorsBuffer);
  Serial.println("***************** _stateBuffer");
  Serial.println( _stateBuffer);
  Serial.println("***************** _buttonBuffer");
  Serial.println( _buttonBuffer);
  Serial.println("_orderIDBuffer");
  Serial.println(_orderIDBuffer);
  Serial.println("Button Pressed");
  if(_stateBuffer.equals("\"place\""))
  {
    if(_buttonBuffer.equals("\"pressed\""))
    {
      Serial.println("***************** statebufferequalsplace");
      Serial.println("_orderIDBuffer");
      _orderIDBuffer.replace("\"","");
      Serial.println(_orderIDBuffer);
      setOrderStatus(_orderIDBuffer, "staged");//or staged
      setTrayStatusAndInstruction(_slaveAddr, "occupied", "keep");
    }
  }
  updateAllTraysForOrder(_orderIDBuffer);
  if(_eventsBuffer.equals("itemDetected"))
  {
    do nothing
  }
  if(_eventsBuffer.equals("\"itemRemoved\""))
  {
    do nothing yet
    if(_stateBuffer.equals("\"remove\""))
    {
      Serial.println("******************************************************************** ORDER COMPLETED************");
      delay(3000);
      setTraysForOrderToStandby(_orderIDBuffer);
      _orderIDBuffer.replace("\"","");
      Serial.println("_orderIDBuffer");
      Serial.println(_orderIDBuffer);
      setOrderStatus(_orderIDBuffer,"completed");
      completedCheck = true;
      Serial.println();
      Serial.println("******************************************************************** ORDER COMPLETED************");
      Serial.println();
      Serial.println("orderStatusJson");
      Serial.println(orderStatusJson);
      Serial.println("trayStatusJson");
      Serial.println(trayStatusJson);
      delay(10000);
    }
  }
  if(_errorsBuffer)
  {
    do nothing
  }
}


void addTrayAllocation(String _id, String _trayID)
{
  search orderstatus json for orderID enterred and add the tray enterred to that order
  ISSUE id not being found
  Serial.println("function: addTrayAllocation");
  String _output;
  String _orderIDString = "";
  int _length = orderStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, orderStatusJson);
  JsonArray nestedOrder = docOrder.createNestedArray("Orders");
  JsonArray nestedArray = doc.
  int orderStatusSize = doc["Orders"].size();
  Serial.println("orderStatusSize");
  Serial.println(orderStatusSize);

  _id = "\"" + _id + "\"";
  for(int i = 0 ; i < orderStatusSize ; i++)
  {
    serializeJson(doc["Orders"][i]["OrderID"],_orderIDString);
    if(_orderIDString.equals(_id))
    {
      Serial.println("id found");
      Serial.print("in position: ");
      Serial.println(i);

      doc["Orders"][i]["orderStatus"] = _status;
      String _arrString = "";
      serializeJson(doc["Orders"][i]["traysAllocated"],_arrString);
      _arrString = _arrString.substring(1,4);
      Serial.println("_trayID");
      Serial.println(_trayID);
      Serial.println("_arrString");
      Serial.println(_arrString);

      if(_arrString.equals("\"0\"")) //if unallocated
      {
        Serial.println("no trays allocated yet - allocate now");
        doc["Orders"][i]["traysAllocated"][0] = _trayID;
      }
      else
      {
        if(_arrString.indexOf(_trayID) < 0)
        {
          Serial.println("trays alloxated not but this one - allocate now");
          doc["Orders"][i]["traysAllocated"].add(_trayID);
        }
        else
        {
          Serial.println("this entry already allocated - do nothing");
        }
      }
      break;
    }
    if( i == orderStatusSize - 1)
    {
      Serial.println("id NOT found");
    }
    _orderIDString = "";
  }
  orderStatusJson = "";
  serializeJson(doc, orderStatusJson);
}

String findFirstAvailable(String _id)
{
  Serial.println("                   Function: findFirstAvailable");
  String _output;
  String _statusString = "";
  int _length = trayStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, trayStatusJson);
  JsonArray docVal = doc["trays"];
  int trayStatusSize = docVal.size();
  Serial.println("trayStatusJson before for");
  Serial.println(trayStatusJson);
  Serial.println("trayStatusSize before for");
  Serial.println(trayStatusSize);

  for(int i = 0 ; i < trayStatusSize ; i++)
  {
    serializeJson(doc["trays"][i]["status"],_statusString);
    Serial.println("_addrString");
    Serial.println(_addrString);
    if(!_id.equals("null"))
    {
      if(_statusString.equals("\"available\""))
      {
        Serial.println("available tray found");
        Serial.print("in position: ");
        Serial.println(i);
        _output = String(i);

        doc["trays"][i]["status"] = _status;
        doc["trays"][i]["trayInstruction"] = _trayInstruction;

        commented out to test tray allocation bug
        docVal[i]["status"] = "allocated";
        docVal[i]["trayInstruction"] = "place"; //Maybe move some of these outside?
        docVal[i]["orderID"] = _id;
        
        Alternatively
      
        doc["trays"][i]["status"] = "allocated";
        doc["trays"][i]["trayInstruction"] = "place"; //Maybe move some of these outside?
        doc["trays"][i]["orderID"] = _id;
        

        String _orderIDBuffer = "";
        serializeJson(doc["trays"][i]["status"], _orderIDBuffer);
        Serial.println("##########################################################_id");
        Serial.println(_id);
        Serial.println("##########################################################_orderIDBuffer");
        Serial.println(_orderIDBuffer);
        setOrderStatus(_id, "placing");
        addTrayAllocation(_id, String(i+1));//because trays start at 1
        trayStatusJson = "";
        serializeJson(doc, trayStatusJson);
        Serial.println("trayStatusJson after findfirstavailable:");
        Serial.println(trayStatusJson);
        break;
      }
      if( i == (trayStatusSize - 1) )
      {
        Serial.println("no available tray found");
      }
      Serial.println("i");
      Serial.println(i);
      _statusString = ""; //reset buffer
    //}
    trayStatusJson = "";
    serializeJson(doc, trayStatusJson);
    Serial.println("trayStatusJson after findfirstavailable:");
    Serial.println(trayStatusJson);
  }
  return _output;
}

String searchForOrderID(String _id)
{
  searchhes to orderStatus json to see if order ID exists and returns
  said orderID if it does, otherwise it returns null
  Serial.println("function: searchForOrderID");
  String _output;
  String _orderIDString = "";
  int _length = orderStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, orderStatusJson);
  JsonArray nestedOrder = docOrder.createNestedArray("Orders");
  JsonArray nestedArray = doc.
  int orderStatusSize = doc["Orders"].size();
  Serial.println("orderStatusSize");
  Serial.println(orderStatusSize);
  String _idOutput = _id;
  _id = "\"" + _id + "\"";
  for(int i = 0 ; i < orderStatusSize ; i++)
  {
    serializeJson(doc["Orders"][i]["OrderID"],_orderIDString);
    if(_orderIDString.equals("\"" + _id + "\""))
    
    Serial.println("_orderIDString");
    Serial.println(_orderIDString);
    Serial.println("_id");
    Serial.println(_id);

    if(_orderIDString.equals(_id))
    if(_orderIDString.equals(_id))
    {
      Serial.println("id found");
      Serial.print("in position: ");
      Serial.println(i);
      _output = _idOutput;

      doc["Orders"][i]["orderStatus"] = "placing";
      break;
    }
    if( i == orderStatusSize - 1)
    {
      Serial.println("id NOT found");
      _output = "null";
    }
    _orderIDString = "";
  }
  orderStatusJson = "";
  serializeJson(doc, orderStatusJson);

  return _output;

}

String searchForLinkToken(String _token)
{
  searchhes to orderStatus json to see if order ID exists and returns
  said orderID if it does, otherwise it returns null
  Serial.println("8888888888888888888888888888888888888888888888  function: searchForLinkToken");
  String _output;
  String _linkTokenString = "";
  String _orderIDString = "";
  int _length = orderStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, orderStatusJson);
  JsonArray nestedOrder = docOrder.createNestedArray("Orders");
  JsonArray nestedArray = doc.
  int orderStatusSize = doc["Orders"].size();
  Serial.println("orderStatusSize");
  Serial.println(orderStatusSize);

  _token = "\"" + _token + "\"";
  for(int i = 0 ; i < orderStatusSize ; i++)
  {
    _orderIDString = "";
    _linkTokenString = "";
    serializeJson(doc["Orders"][i]["linkToken"], _linkTokenString);
    serializeJson(doc["Orders"][i]["OrderID"], _orderIDString);
    Serial.println("_linkTokenString");
  Serial.println(_linkTokenString);
  Serial.println("_token");
  Serial.println(_token);
    if(_orderIDString.equals("\"" + _id + "\""))
    
    Serial.println("_orderIDString");
    Serial.println(_orderIDString);
    Serial.println("_id");
    Serial.println(_id);

    if(_orderIDString.equals(_id))
    if(_linkTokenString.equals(_token))
    {
      Serial.println("token found");
      Serial.print("in position: ");
      Serial.println(i);
      _output = _orderIDString;

      doc["Orders"][i]["orderStatus"] = "removing";
      break;
    }
    if( i == orderStatusSize - 1)
    {
      Serial.println("id NOT found");
      _output = "null";
    }
    _orderIDString = "";
  }
  orderStatusJson = "";
  serializeJson(doc, orderStatusJson);
  _output.replace("\"","");
  return _output;
}

void setTraysForOrderToRemove(String _id)
{
  set all of the instructions for the trays for a given order that is being picked up to remove
  Serial.println("setTraysForOrderToRemove");
  String _output;
  String _statusString = "";
  String _orderIDString = "";
  int _length = trayStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, trayStatusJson);
  JsonArray docVal = doc["trays"];
  int trayStatusSize = docVal.size();

  Serial.println("trayStatusJson before for");
  Serial.println(trayStatusJson);
  Serial.println("trayStatusSize before for");
  Serial.println(trayStatusSize);

  for(int i = 0 ; i < trayStatusSize ; i++)
  {
    serializeJson(doc["trays"][i]["orderID"],_orderIDString);
    Serial.println("_addrString");
    Serial.println(_addrString);
    Serial.println("tray for that order found");
    if(_orderIDString.equals(_id))//equals("\"" + _id +"\""))
    {
      _id.replace("\"","");
      Serial.println("tray for that order found");
      Serial.print("in position: ");
      Serial.println(i);
      _output = String(i);

      doc["trays"][i]["status"] = _status;
      doc["trays"][i]["trayInstruction"] = _trayInstruction;
      docVal[i]["status"] = "allocated";
      docVal[i]["trayInstruction"] = "remove"; //Maybe move some of these outside?
      _id.replace("\"","");
      docVal[i]["orderID"] = _id;

      String _orderIDBuffer = "";
      serializeJson(doc["trays"][i]["status"], _orderIDBuffer);
      setOrderStatus(_id, "removing");
      addTrayAllocation(_id, String(i+1)); //because trays start at 1
    }
    if( i == (trayStatusSize - 1) )
    {
      Serial.println("no tay for this order  found");
    }
    Serial.println("i");
    Serial.println(i);
    _statusString = ""; //reset buffer
  }
  
  trayStatusJson = "";
  serializeJson(doc, trayStatusJson);
  Serial.println("trayStatusJson after settraysforordertoremove");
  Serial.println(trayStatusJson);
  delay(10000);

  return _output;
}

String getOrderIDforTrayID(String _trayAddr)
{
  search trays for thus tray id and return orderID associated
  Serial.println("setTrayStatusAndInstruction");
  String _output;
  String _addrString = "";
  String _orderIDBuffer = "";
  int _length = trayStatusJson.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, trayStatusJson);
  JsonArray docVal = doc["trays"];
  int trayStatusSize = docVal.size();
  Serial.println("trayStatusJson before for");
  Serial.println(trayStatusJson);
  Serial.println("trayStatusSize before for");
  Serial.println(trayStatusSize);

  for(int i = 0 ; i < trayStatusSize ; i++)
  {
    serializeJson(doc["trays"][i]["Addr"],_addrString);
    Serial.println("_addrString");
    Serial.println(_addrString);
    if(_addrString.equals("\"" + _trayAddr + "\""))
    {
      Serial.println("addr found");
      Serial.print("in position: ");
      Serial.println(i);

      doc["trays"][i]["status"] = _status;
      doc["trays"][i]["trayInstruction"] = _trayInstruction;
      serializeJson(docVal[i]["orderID"], _orderIDBuffer);
      docVal[i]["status"] = _status;
      docVal[i]["trayInstruction"] = _trayInstruction;
      break;
    }
    if( i == (trayStatusSize - 1) )
    {
      Serial.println("addr NOT found");
    }
    Serial.println("i");
    Serial.println(i);
    _addrString = ""; //reset buffer
  }
  _orderIDBuffer.replace("\"","");
  _output = _orderIDBuffer;
  return _output;
}

void updateSlaveResponse(String _instruction, String _state, String _events, String _buttons, String _errors)
{
  for slave to update slaveresponse
  will change statuses based on conditions
  ping will be:
  /*
  "addr": "0",
  "orderID": "3",
  "trayInstruction": "place",
  */
  
  take instruction, put it in status
  take order Id, put it in order ID
  set Addr to 0
  set state based on externals
  set events based on externals
  buttons too
  
  _instruction.replace("\"","");
  String _output;
  String _currentInstruction = "";
  String _previousInstruction = "";
  String _orderIDString = "";
  String _instructionBuffer = "";
  String _addrBuffer = "1";
  String _slaveAddrBuffer = _slaveAddr;
  String _orderIDBuffer = currentOrderID;//getOrderIDforTrayID(thisTraysAddress);
  String _stateBuffer = _instruction;
  String _eventsBuffer = _events;
  String _buttonBuffer = _buttons;
  String _errorsBuffer = _errors;
  int _length = slaveResponse.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, slaveResponse);
  serializeJson(doc["Addr"]= _addrBuffer);
  //serializeJson(doc["slaveAddr"], _addrBuffer);
  serializeJson(doc["orderID"] = _orderIDBuffer);
  serializeJson(doc["state"]= _stateBuffer);
  serializeJson(doc["button"]= _buttonBuffer);
  serializeJson(doc["events"]= _eventsBuffer);
  serializeJson(doc["errors"]= _errorsBuffer);
   doc["Addr"]= _addrBuffer;
  serializeJson(doc["slaveAddr"], _addrBuffer);
 doc["orderID"] = _orderIDBuffer;
 doc["state"]= _stateBuffer;
 doc["button"]= _buttonBuffer;
 doc["events"]= _eventsBuffer;
 doc["errors"]= _errorsBuffer;

  doc["state"] = _instruction;

  slaveResponse = "";
  serializeJson(doc, slaveResponse);

}

String createMasterPing(String _addr)
{
  creates the ping that the master sends to a tray at address _addr
  based on contents of trayStatusJson

  Serial.println("createMasterPing");
  String _output;
  deserialize trayStatusJson in docStatus(Array in docVal)
  String _addrString = "";
  int _length = trayStatusJson.length();
  DynamicJsonDocument docStatus(5024 + _length);
  deserializeJson(docStatus, trayStatusJson);
  JsonArray docVal = docStatus["trays"];
  int trayStatusSize = docVal.size();

  create Json for Master ping in docPing
  String _masterPing = "";
  DynamicJsonDocument docPing(5024);
  deserializeJson(docPing, _masterPing);

  for(int i = 0 ; i < trayStatusSize ; i++)
  {
    serializeJson(docStatus["trays"][i]["Addr"],_addrString);
    Serial.println("_addrString");
    Serial.println(_addrString);
    if(_addrString.equals("\"" + _addr + "\""))
    {
      Serial.println("addr found");
      Serial.print("in position: ");
      Serial.println(i);

      docPing["Addr"] = docStatus["trays"][i]["Addr"];
      docPing["orderID"] = docStatus["trays"][i]["orderID"];
      docPing["trayInstruction"] = docStatus["trays"][i]["trayInstruction"];
      
      break;
    }
    if( i == (trayStatusSize - 1) )
    {
      Serial.println("addr NOT found");
    }
    _addrString = ""; //reset buffer
  }
  serializeJson(docPing, _masterPing);
  return _masterPing;
  

}

String parseMasterPing(String _masterPing, bool _debug = false)
{
  parses master ping to see if master is talking to this tray
  responds with instruction if it is
  Serial.println("function: parseMasterPing");
  String _output;
  String _orderIDString = "";
  String _instructionBuffer = "";
  String _addrBuffer = "";
  int _length = _masterPing.length();
  DynamicJsonDocument doc(5024 + _length);
  deserializeJson(doc, _masterPing);
  serializeJson(doc["Addr"], _addrBuffer);
  if(_debug)
  {
    Serial.println("_addrBuffer");
    Serial.println(_addrBuffer);
    Serial.println("thisTraysAddress");
    Serial.println(thisTraysAddress);
  }
  if(_addrBuffer.equals("\"" + thisTraysAddress + "\""))
  {
    if(_debug)
    {
      Serial.println("master is talking to me");
    }
    serializeJson(doc["trayInstruction"], _instructionBuffer);
    currentOrderID = doc["orderID"].as<String>();
    _output = _instructionBuffer;
  }
  else
  { 
    if(_debug)
    {
      Serial.println("master is not talking to me ):");
    }
    _output = "null";
  }
  return _output;
}
//////////////new functions end
//new slave 
void ledOFF(uint32_t _ledColor)
{
   if (ledCheck == 1)
   {
      Adafruit_NeoPixel strip1(60, ledA, NEO_GRB + NEO_KHZ800);
      Adafruit_NeoPixel strip2(60, ledB, NEO_GRB + NEO_KHZ800);
      Adafruit_NeoPixel strip3(60, ledC, NEO_GRB + NEO_KHZ800);
      Adafruit_NeoPixel strip4(60, ledD, NEO_GRB + NEO_KHZ800);
      strip1.begin();
      strip2.begin();  
      strip3.begin();  
      strip4.begin();            
      strip1.show(); 
      strip2.show();
      strip3.show();  
      strip4.show();
      for (int i=8; i<17; i++)     
      {
        strip1.setBrightness(0);
        strip1.setPixelColor(i, _ledColor);   
        strip1.show(); 
      }
      for (int i=8; i<17; i++)     
      {
        strip2.setBrightness(0);
        strip2.setPixelColor(i, _ledColor);   
        strip2.show(); 
      }
      for (int i=8; i<17; i++)     
      {
        strip3.setBrightness(0);
        strip3.setPixelColor(i, _ledColor);   
        strip3.show(); 
      }
      for (int i=8; i<17; i++)     
        {
          strip4.setBrightness(0);
          strip4.setPixelColor(i, _ledColor);   
          strip4.show(); 
        }
    }
     ledCheck = 0; 
}
/*/
void blinkLed(uint32_t _ledColor,int _delay)
{
  if (ledCheck == 0)
  {
    Adafruit_NeoPixel strip1(60, ledA, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel strip2(60, ledB, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel strip3(60, ledC, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel strip4(60, ledD, NEO_GRB + NEO_KHZ800);
    strip1.begin();
    strip2.begin();  
    strip3.begin();  
    strip4.begin();            
    strip1.show(); 
    strip2.show();
    strip3.show();  
    strip4.show();
    for (int i=8; i<17; i++)     
    {
      strip1.setBrightness(trayBrightness);
      strip1.setPixelColor(i, _ledColor);   
      strip1.show(); 
    }
    for (int i=8; i<17; i++)     
    {
      strip2.setBrightness(trayBrightness);
      strip2.setPixelColor(i, _ledColor);   
      strip2.show(); 
    }
    for (int i=8; i<17; i++)     
    {
      strip3.setBrightness(trayBrightness);
      strip3.setPixelColor(i, _ledColor);   
      strip3.show(); 
    }
    for (int i=8; i<17; i++)     
    {
      strip4.setBrightness(trayBrightness);
      strip4.setPixelColor(i, _ledColor);   
      strip4.show(); 
    }
  }
  ledCheck = 1;
  ledOFF(white);
  delay(_delay);
  Adafruit_NeoPixel strip1(60, ledA, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip2(60, ledB, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip3(60, ledC, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip4(60, ledD, NEO_GRB + NEO_KHZ800);
  strip1.begin();
  strip2.begin();  
  strip3.begin();  
  strip4.begin();            
  strip1.show(); 
  strip2.show();
  strip3.show();  
  strip4.show();
  for (int i=8; i<17; i++)     
  {
    strip1.setBrightness(trayBrightness);
    strip1.setPixelColor(i, _ledColor);   
    strip1.show(); 
  }
  for (int i=8; i<17; i++)     
  {
    strip2.setBrightness(trayBrightness);
    strip2.setPixelColor(i, _ledColor);   
    strip2.show(); 
  }
  for (int i=8; i<17; i++)     
  {
    strip3.setBrightness(trayBrightness);
    strip3.setPixelColor(i, _ledColor);   
    strip3.show(); 
  }
  for (int i=8; i<17; i++)     
  {
    strip4.setBrightness(trayBrightness);
    strip4.setPixelColor(i, _ledColor);   
    strip4.show(); 
  }
  ledCheck = 1;
}  
*/
void blinkLed(uint32_t _ledColor,int _delay)
{
  if (ledCheck == 0)
  {
    Adafruit_NeoPixel strip1(60, ledA, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel strip2(60, ledB, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel strip3(60, ledC, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel strip4(60, ledD, NEO_GRB + NEO_KHZ800);
    strip1.begin();
    strip2.begin();  
    strip3.begin();  
    strip4.begin();            
    strip1.show(); 
    strip2.show();
    strip3.show();  
    strip4.show();
    for (int i=8; i<17; i++)     
    {
      strip1.setBrightness(20);
      strip1.setPixelColor(i, _ledColor);   
      strip1.show(); 
    }
    for (int i=8; i<17; i++)     
    {
      strip2.setBrightness(trayBrightness);
      strip2.setPixelColor(i, _ledColor);   
      strip2.show(); 
    }
    for (int i=8; i<17; i++)     
    {
      strip3.setBrightness(trayBrightness);
      strip3.setPixelColor(i, _ledColor);   
      strip3.show(); 
    }
    for (int i=8; i<17; i++)     
    {
      strip4.setBrightness(20);
      strip4.setPixelColor(i, _ledColor);   
      strip4.show(); 
    }
  }
  ledCheck = 1;
  ledOFF(white);
  delay(_delay);
  Adafruit_NeoPixel strip1(60, ledA, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip2(60, ledB, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip3(60, ledC, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip4(60, ledD, NEO_GRB + NEO_KHZ800);
  strip1.begin();
  strip2.begin();  
  strip3.begin();  
  strip4.begin();            
  strip1.show(); 
  strip2.show();
  strip3.show();  
  strip4.show();
  for (int i=8; i<17; i++)     
  {
    strip1.setBrightness(20);
    strip1.setPixelColor(i, _ledColor);   
    strip1.show(); 
  }
  for (int i=8; i<17; i++)     
  {
    strip2.setBrightness(trayBrightness);
    strip2.setPixelColor(i, _ledColor);   
    strip2.show(); 
  }
  for (int i=8; i<17; i++)     
  {
    strip3.setBrightness(trayBrightness);
    strip3.setPixelColor(i, _ledColor);   
    strip3.show(); 
  }
  for (int i=8; i<17; i++)     
  {
    strip4.setBrightness(20);
    strip4.setPixelColor(i, _ledColor);   
    strip4.show(); 
  }
  ledCheck = 1;
} 

void defBlinkLED(uint32_t _ledColor,int _delay,int _blinkNum)
{
  for (int i=0; i < _blinkNum; i++)
  {
    blinkLed(_ledColor,100);
    delay(_delay);
  }
  ledOFF(_ledColor);
}
Define the interrupt service routine (ISR)
void IRAM_ATTR function_ISR() 
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= 100) 
  { // debounce time: 50ms
    ets_printf("#####################################################################ISR triggered\n");
    buttonState = "pressed";
    lastMillis = currentMillis;
  }
}
bool readSensor(int _threshold)
{
  bool result = false;
  if (( analogRead(sensA) < _threshold )|| (analogRead(sensB) < _threshold ) || (analogRead(sensC) < _threshold ) || (analogRead(sensD) < _threshold ))
  if (( analogRead(sensA) < _threshold ) || (analogRead(sensD) < _threshold ))
  {
    Serial.print ("Sensor triggered   ");
    result = true;
  }
  else
  {
    Serial.print("Sensor not triggered   ");
  }
    Serial.print("Threshold: ");
    Serial.print(_threshold);
    
    Serial.print(", Sensor A: ");
    Serial.print(analogRead(sensA));

    Serial.print(", Sensor B: ");
    Serial.print(analogRead(sensB));

    Serial.print(", Sensor C: ");
    Serial.print(analogRead(sensC));

    Serial.print(", Sensor D: ");
    Serial.println(analogRead(sensD));
    return result;
}
void connectToWiFi(String _ssid,String _password,long _wifiTimeOut,bool _debug = false) 
{
  Set WiFi mode to both Station and access point
  WiFi.mode(WIFI_AP_STA);
  If _debug is true display the following
  if(_debug)
  {
    Serial.println("Function \"connectToWiFi\":");
    Serial.println();
    Serial.print("WiFi Connecting to \"");
    Serial.print(_ssid);
    Serial.print("\" with password \"");
    Serial.print(_password);
    Serial.println("\"");
    Serial.println("Connecting");
  }
  Try connecting to WiFi
  WiFi.begin(_ssid.c_str(), _password.c_str());
  long _startTime = millis();
  Repeat if WiFi status is connecting
  while (millis() < _startTime + _wifiTimeOut )
  {
    If _debug is true display the following
    if(_debug)
    {
      Serial.print('.');
      delay(200);
    }
    if(WiFi.status() == WL_CONNECTED) 
    {
      break;
    }
  }
  If _debug is true display the following
  if(_debug)
  {
    if(WiFi.status() == WL_CONNECTED) 
    {
      Serial.println("\nWiFi Connection: Successful");
      Serial.println("................................................................................................." );
    }
    else
    {
      Serial.println("\nWiFi Connection: Failed");
      Serial.println("................................................................................................." );
    }
  }
}
void callServer()
{
  Serial.println();
  Serial.println("***********calling server!!!!!********");
  Serial.println();
  If orderStatusJson has change do the following
  if (!prevOrderStatusJson.equals(orderStatusJson)) 
  {
    If orderStatusJson is not empty do the following 
    if(orderStatusJson.length() > 16)
    {
      Save orderStatusJson to "mobiArrayData" in EEPROM
      writeEEPROM("mobiArrayData",orderStatusJson,true);
    }
    Update the previous order status to the current order status 
    prevOrderStatusJson = orderStatusJson;
  }
  If wifi is not connected do the following
  if(WiFi.status() != WL_CONNECTED)
  {
    Try connecting to wifi
    connectToWiFi(ssid,password,wifiTimeOut,true);
    Retrieve "mobiArrayData" from EEPROM and store in mobiData
    serverResponse = readEEPROM("mobiArrayData",true);
  }
  Else if wifi is already connected do the following
  else
  {
    Update orderStatusJson using the online server
    serverResponse = updatePayload(orderStatusJson,orderJsonUrl,true);
    If GET request failed do the following 
    if(serverResponse.length() < 16)
    {
      Retrieve "mobiArrayData" from EEPROM and store in serverResponse
      serverResponse = readEEPROM("mobiArrayData",true);
    }
  }
  Parse serverResponse
  String buffer = parseServerResponse(serverResponse);
  Serial.println("orderStatusJson");
  Serial.println(orderStatusJson);
  Serial.println("serverResponse callServer");
  Serial.println(serverResponse);
  Serial.println("buffer");
  Serial.println(buffer);
}

void callServer3()
{
  Serial.println();
  Serial.println("***********calling server!!!!!********");
  Serial.println();
  If wifi is not connected do the following
  if(WiFi.status() != WL_CONNECTED)
  {
    Try connecting to wifi
    initWiFi(ssid,password,true);
    Retrieve "mobiArrayData" from EEPROM and store in mobiData
    mobiData = readEEPROM("mobiArrayData",true);
    mobiData = readEEPROM("mobiArrayData",true);
    mobiData = readEEPROM("orderInfo",true);
  }
  else if wifi is already connected
  else
  {
    if (orderStatusJson != prevOrderStatusJson) 
    {
      Call the sendPayload() function only if the order status has changed
      sendPayload(orderStatusJson, orderJsonUrl);
      Update the previous order status to the current order status 
      prevOrderStatusJson = orderStatusJson;
    }
    client.publish("Customer/Data/orderJson",orderStatusJson.c_str());
    Serial.println("client.publish orderStatusJson");
    Serial.println(orderStatusJson);
    Call server to get list of orders from mobi point of sale machine
    HTTPClient http;
    Your IP address with path or Domain name with URL path 
    http.begin(orderJsonUrl);
    Set the request headers (optional)
    http.addHeader("Content-Type", "application/json");
    Send the POST request with the payload
    int httpResponseCode = http.POST(orderStatusJson);
    Check for a successful response
    if (httpResponseCode == HTTP_CODE_OK) 
    {
      If _debug is true display the following
      if(1)
      {
        Serial.println("Payload sent successfully:");
        Serial.println(orderStatusJson);
      }
      Send the GET request for the payload
      orderStatusJson = http.getString(); 
      If _debug is true display the following
      if(1)
      {
        if (orderStatusJson.length() > 0) 
        {
          Serial.println("Payload received successfully:");
          Serial.println(orderStatusJson);
          Serial.println("................................................................................................." );
        }
      }
    } 
    else 
    {
      Serial.print("Error sending payload: ");
      Serial.println(httpResponseCode);
    }
    mobiData = orderStatusJson;
    http.end();
    Extract list of orders and from mobiData
    serverResponse = mobiData;
    serverResponse = extractNestedArray(mobiPOSdata,true);
    Save mobiData to "mobiArrayData" in EEPROM
    writeEEPROM("orderInfo",orderInfo,true);
    String buffer = parseServerResponse(serverResponse);
    Serial.println("orderStatusJson");
    Serial.println(orderStatusJson);
    Serial.println("serverResponse callServer");
    Serial.println(serverResponse);
    Serial.println("buffer");
    Serial.println(buffer);
  }
}

/*
void compareOrders()
{
    orderInfoSize = countJsonObjects(orderInfo,true);
    orderStatusSize = countJsonObjects(orderStatus,true);
    int processedLogSize = countJsonObjects(processedTransactionLog,true);
  compare all orderInfo objects to see if they exist on orderStatus array
  on tray. if not, create new entry(ies) in orderStatus array
  in OrderStatus based on this
  
  for(int i = 0; i < orderInfoSize; i++)
  {
    for(int k = 0; k < orderStatusSize; k++ )
    {
      if(orderStatus[k] == orderInfo[i])
      {
        Serial.println("order found" + i.toString())
        break; //break out of this and start looking for next order
      }
    }
  }
}
*/

void checkScans()
{
  evaluate response from scanner
  TO DO: put code for this
  unsigned long _startTime = millis();
  String _buttonsBuffer = "";
  Serial.println();
  Serial.println("---------------------------starting checkScans!!!--------------------------");
  Serial.println();
  if(scannedData.length() > 0)
  {i = 1;
    scanCheck = true;
    if(scannedData[0] == 'T')
    {
      scannedData.replace("T","");
      int temp = scannedData.toInt();
      scannedData = String(temp);
      Serial.println("scannedData");
      Serial.println(scannedData);
      String orderIDTemp = searchForOrderID(scannedData);

      findFirstAvailable(orderIDTemp);
      Serial.println("orderIDTemp");
      Serial.println(orderIDTemp);
      Serial.println("trayStatusJson");
      Serial.println(trayStatusJson);
      Serial.println("orderStatusJson");
      Serial.println(orderStatusJson);
      scannedData = "";
    }
    else
    {
      scannedData.replace("\r","");
      scannedData.replace("\n","");
      Serial.println("trayStatusJson before");
      Serial.println(trayStatusJson);
      Serial.println("orderStatusJson before");
      Serial.println(orderStatusJson);
      Serial.println("**********************************************************************scannedData");
      Serial.println(scannedData);
      delay(10000);
      String orderIDTemp = searchForLinkToken(scannedData);
      Serial.println("orderIDTemp ");
      Serial.println(orderIDTemp);
    

      setTraysForOrderToRemove(orderIDTemp);
      
      Serial.println("trayStatusJson after");
      Serial.println(trayStatusJson);
      Serial.println("orderStatusJson after");
      Serial.println(orderStatusJson);
      scannedData = "";
    }
    unsigned long _endTime = millis() - _startTime;
    Serial.println(_endTime);
   while(1);
  }
  else
  {
    Serial.println("no scan found");
  }
}
int checkForConfigurationMode(String _rMsg, int _config, bool _debug = false)
{
  If data recieved is a configuration command do the following
  if (_rMsg.equals("M?") || _rMsg.equals("?"))
  {
    Set configuration Mode status to "on"
    _config = 1;
  }
  if(_debug)
  {
    Serial.println("checkForConfigurationMode");
    if(_config == 1)
    {
      Serial.println("Configuration Mode");
    }
    else if(_config == 2)
    {
      Serial.println("Seting up Configuration Mode");
    }
  }
  return _config;
}
void masterResponseToPing()
{
  generate message for the master
  String masterPingBuffer = createMasterPing(String(i));
  parse message and get instructions for master
  String instructionBuffer = parseMasterPing(masterPingBuffer);
  String currentInstruction = "";
  Serial.println("masterPingBuffer");
  Serial.println(masterPingBuffer);
  Serial1.print(masterPingBuffer);
  responding to "masterping part
  tray leds and 
  update tray response part
  String addrBuffer = "";
  String orderBuffer = "";
  String stateBuffer = instructionBuffer;
  String eventsBuffer = "";
  String errorBuffer = "";
  currentInstruction = instructionBuffer;//TrayStatus instruction
  Serial.println("instructionBuffer");
  Serial.println(instructionBuffer);
  Serial.println("currentInstruction");
  Serial.println(currentInstruction);
  Serial.println("previousInstruction");
  Serial.println(previousInstruction);

  ///////////////////Respond to harddware changes ( button + sensors)

  if(buttonState.equals("pressed"))
  {
    buttonsBuffer = "pressed";
    buttonState = "idle";
    setTrayStatusAndInstruction(String(i), String _status, String _trayInstruction);
    update the Tray status button press for 1(master) to pressed)
  }
  if(readSensor(IRthresh)) //ojects are seen
  {
    update the Tray status events to itemDetected
    eventsBuffer = "itemDetected";
    serialPrintS("itemDetected", eventsBuffer, true);
  }
  else if(!readSensor(IRthresh))
  {
    update the Tray status events to itemRemoved
    eventsBuffer = "itemRemoved";
    serialPrintS("itemRemoved", eventsBuffer, true);
    if(previousInstruction.equals("\"keep\""))
    {
      errorBuffer = "wrongOrderRemoved";
      Serial.println("!!!!!!!!!!!!!!!!!!!!! wrong Order Removed !!!!!!!!!!!!!!!!!!!");
      update trayResponse error to wrongOrderRemoved
      Blink Red and beep LEDs
      Eventutually: update OrderStatusJson error to wrongOrderRemoved
    }
  }

  ///////////////////Respond to harddware changes ( button + sensors) END

  ///////////////////Respond to Instruction in ping 

  if(currentInstruction.equals("\"place\""))
  {
    LED snake pattern
    Serial.println("************in equals place");
    Serial.println(currentInstruction);
    
    blinkLed(white,100);
    if(scanCheck)
    {
      unsigned long _endTime = millis() - startTimeM;
      Serial.println(_endTime);
      while(1);
    }
  }
  if(previousInstruction.equals("\"place\"") && currentInstruction.equals("\"keep\"")/* previous instruction was place && current is keep*/)
  {
    LED Blink twice and turn off
    defBlinkLED(white,100,2);
    ledOFF(0);
  }
  if(currentInstruction.equals("\"keep\"")/* previous instruction was place && current is keep*/)
  {
    LED Blink twice and turn off
    ledOFF(0);
    Turn off LED master pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
  }
  if( currentInstruction.equals("\"remove\"")/*tray instruction is remove*/ )
  {
    Continuous blinking
    buttonsBuffer = "idle";
    blinkLed(green,100);
  }
  if(currentInstruction.equals("\"standby\"")/*current tray instruction is standby*/)
  {
  set LED pattern to identify tray as master
    Turn off LED master pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
    Turn on master LED pattern 
    ledControl(ledA, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledB, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledC, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledD, 16, 3, 5, white, ON, b, "",trayBrightness);
  }
    previousInstruction = currentInstruction;

    ///////////////////Respond to Instruction in ping END 

    Serial.println("slaveResponse before update");
    Serial.println(slaveResponse);
    updateSlaveResponse(instructionBuffer,stateBuffer,eventsBuffer,buttonsBuffer,errorBuffer);
    Serial.println("slaveResponse after update");
    Serial.println(slaveResponse);
    parseSlaveResponse(slaveResponse, String(i));
}
void pingTrays()
{
  unsigned long _startTime = 0;
  if(scanCheck)
  {
    _startTime = millis();
  }
  String buttonsBuffer = "";
  Serial.println();
  Serial.println("---------------------------starting PingTrays!!!--------------------------");
  Serial.println();
  int totalTrays = 3;//= getPathSize(trayLog,true);
  for(int i = 1; i <= totalTrays + 1; i++)
  {
    
    if(buttonState.equals("pressed"))
    {
      digitalWrite(eLed, LOW);
      delay(50);
      digitalWrite(eLed, HIGH);
      defBlinkLED(white,50,2);
      ledOFF(0);
      Serial.println("*********************************************buttonPressed!");
      setOrderStatus(getOrderIDforTrayID(String(i)), "placing");
      addTrayAllocation(getOrderIDforTrayID(String(i)), String(i+1));
    }
    if(buttonState.equals("pressed"))
    {
      buttonsBuffer = "pressed";
      buttonState = "idle";
      setTrayStatusAndInstruction(String(i), String _status, String _trayInstruction);
    update the Tray status button press for 1(master) to pressed)
    }
    Serial.println("Pinging addr " + String(i));
    if(i == 1) //master
    {
      masterResponseToPing();
    }
    else if (i > totalTrays)
    {
      Serial1.print("M?");
      Serial.println("M?");
      recieve data from trays
      trayResponse = readSerial1(200,true);
    }
    else
    {
      createMasterPing(String(i));
      send the trayStatus JSON for this tray
      receive response for that tray
      update responses array
      String masterPingBuffer;
      masterPingBuffer = createMasterPing(String(i));
      Serial.println("masterPingBuffer");
      Serial.println(masterPingBuffer);
      Serial.println("currentInstruction");
      Serial.println(currentInstruction);
      Serial.println("previousInstruction");
      Serial.println(previousInstruction);
      Serial1.print(masterPingBuffer);
      slaveResponse = readSerial1(200,true);
      Check if trayResponse is for configuration mode
      config = checkForConfigurationMode(slaveResponse,config,true);
      Serial.println("slaveResponse after update");
      Serial.println(slaveResponse);
      parseSlaveResponse(slaveResponse, String(i));
    }
  }
}
void allocateTrays()
{

}

void respondToInstructions(String _mInstruction)
{
  String buttonsBuffer = "";
  String addrBuffer = "";
  String orderBuffer = "";
  String stateBuffer = _mInstruction;
  String eventsBuffer = "";
  String errorBuffer = "";
  String currentInstruction = _mInstruction;//TrayStatus instruction
  Serial.println("_mInstruction");
  Serial.println(_mInstruction);
  Serial.println("currentInstruction");
  Serial.println(currentInstruction);
  Serial.println("previousInstruction");
  Serial.println(previousInstruction);
  if(buttonState.equals("pressed"))
  {
    digitalWrite(eLed, LOW);
    delay(50);
    digitalWrite(eLed, HIGH);
    defBlinkLED(white,50,2);
    ledOFF(0);
    Serial.println("*********************************************buttonPressed!");
    setOrderStatus(getOrderIDforTrayID(String(i)), "placing");
    addTrayAllocation(getOrderIDforTrayID(String(i)), String(i+1));
  }
  if(buttonState.equals("pressed"))
  {
    buttonsBuffer = "pressed";
    buttonState = "idle";
  }
  if(readSensor(IRthresh)) //ojects are seen
  {
    update the Tray status events to itemDetected
    eventsBuffer = "itemDetected";
    serialPrintS("itemDetected", eventsBuffer, true);
  }
  else if(!readSensor(IRthresh))
  {
    update the Tray status events to itemRemoved
    eventsBuffer = "itemRemoved";
    serialPrintS("itemRemoved", eventsBuffer, true);
    if(previousInstruction.equals("\"keep\""))
    {
      errorBuffer = "wrongOrderRemoved";
      update trayResponse error to wrongOrderRemoved
      Blink Red and beep LEDs
      Eventutually: update OrderStatusJson error to wrongOrderRemoved
    }
  }
  if(currentInstruction.equals("\"place\""))
  {
    LED snake pattern
    Serial.println("************in equals place");
    Serial.println(currentInstruction);
    blinkLed(white,100);
  }
  if(previousInstruction.equals("\"place\"") && currentInstruction.equals("\"keep\"")/* previous instruction was place && current is keep*/)
  {
    LED Blink twice and turn off
    defBlinkLED(white,100,4);
    ledOFF(0);
  }
  if( currentInstruction.equals("\"remove\"")/*tray instruction is remove*/ )
  {
    Continuous blinking
    buttonsBuffer = "idle";
    blinkLed(green,100);
  }
  if(currentInstruction.equals("\"standby\"")/*current tray instruction is standby*/)
  {
  set LED pattern to identify tray as master
    Turn off LED master pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
    Turn on master LED pattern 
    ledControl(ledA, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledB, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledC, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledD, 16, 3, 5, white, ON, b, "",trayBrightness);
  }
  previousInstruction = currentInstruction;
  Serial.println("slaveResponse before update");
  Serial.println(slaveResponse);
  updateSlaveResponse(_mInstruction,stateBuffer,eventsBuffer,buttonsBuffer,errorBuffer);
  Serial.println("slaveResponse after update");
  Serial.println(slaveResponse);
}
String addPathJSON(String _input, String _path, bool _pretty, bool _debug)
{
  String _output; // Creates a String called "output" to store serialize Json
  int _length = _input.length();
  DynamicJsonDocument componentBuffer(2048 + _length); // Creates a Dynamic Json Document to store deserialize Json called "componentBuffer"
  deserializeJson(componentBuffer, _input);            // Deserialize "input" into "componentBuffer"
  int componentIndex = 0;
  Serial.print("componentBuffer.size(): ");
  Serial.println(componentBuffer.size());
  Serial.print("length: ");
  Serial.println(_length);
  if(componentBuffer.size() == 0)//array has no or one entry
  {
    if(_length == 0 )//array is empty
    {
      componentIndex = componentBuffer.size();
    }
    else//array has one entry
    {
      Populate entry one
      componentIndex = componentBuffer.size() + 1;
    }
  }
  else //array has more than one entry
  {
    componentIndex = componentBuffer.size();
  }
  Serial.print("componentIndex: ");
  Serial.println(componentIndex);
  componentIndex = componentBuffer.size();        // Calculate the lenght of the Dynamic Json Document so it can append it by indexing the pointer
  componentBuffer[componentIndex]["Slave"] = _path;
  if (_pretty == true)
  {
    serializeJsonPretty(componentBuffer, _output);
  }
  else
  {
    serializeJson(componentBuffer, _output);
  }
  if (_debug == true)
  {
    Serial.print("Input: ");
    Serial.println(_input);
    Serial.print("Output: ");
    Serial.println(_output); // Serialize the Json document and save in "output"
  }
  return _output; // Return output
}
int configurationMode(String _rMsg, String trayType, int _config, bool _debug = false)
{
  if(trayMode == master)
  {
    if (x==0)
    {
      Serial.println("------------------------------------------Master Config Mode------------------------------------------");
      x=1;
    }
    if (!_rMsg.equals(""))
    {
      if (_config == 1)
      {
        feedBack = 1;
        S_address = (getPathSize(trayLog,true)) + 1;
        Serial.print("New Tray Address: ");
        Serial.println(S_address);
        Serial1.print(S_address);
        Serial.print("Sent: ");
        Serial.println(S_address);
        Serial.println("................................................................................................." );
        _config = 2;
      }
      if(_rMsg.equals("done"))
      {
        feedBack = 2;
        _config = 0;
        writeEEPROM("slaveAddressLog",trayLog, true);
      }
      else if(_rMsg.equals(String(S_address)))
      {
        trayLog = addPathJSON(trayLog,S_address,true,false);
        Serial1.print("done");
        Serial.println("Sent: ");
        Serial.println("done");
        Serial.println("................................................................................................." );
      }
    }
    if (feedBack == 1)
    {
      ledControl(ledA, 16, 3, 5, white, ON, b, "fade",255);
      ledControl(ledB, 16, 3, 5, white, ON, b, "fade",255);
      ledControl(ledC, 16, 3, 5, white, ON, b, "fade",255);
      ledControl(ledD, 16, 3, 5, white, ON, b, "fade",255);
      delay(10);
    }
    else if (feedBack == 2)
    {
      ledControl(ledA, 16, 3, 5, white, ON, b, "blink", 255);
      ledControl(ledB, 16, 3, 5, white, ON, b, "blink", 255);
      ledControl(ledC, 16, 3, 5, white, ON, b, "blink", 255);
      ledControl(ledD, 16, 3, 5, white, ON, b, "blink", 255);
    }
  }
  else if (trayMode == slave)
  { 
    if (y==0)
    {
      Serial.println("------------------------------------------Slave Config Mode------------------------------------------");
      y=1;
    }
    
    if (!_rMsg.equals(""))
    { 
      if (_rMsg.equals("M?"))
      {
        Serial1.print("?");
        Serial.print("Sent: ");
        Serial.println("?");
        Serial.println("................................................................................................." );
      }
      else if ((!_rMsg.equals("done")) && (_rMsg.length() < 2)) 
      {
        ledControl(ledA, 16, 3, 5, white, ON, b, "fade",255);
        ledControl(ledC, 16, 3, 5, white, ON, b, "fade",255);
        Serial1.print(_rMsg);
        Serial.println ("Sent back: " + _rMsg); 
        Serial.println("................................................................................................." ); 
        slaveAddress = _rMsg;
      }
      else if (_rMsg.equals("done"))
      {
        ledControl(ledA, 16, 3, 5, white, ON, b, "blink", 255);
        ledControl(ledC, 16, 3, 5, white, ON, b, "blink", 255);
        Serial.print("Saving my address as: ");
        Serial.println(slaveAddress);
        writeEEPROM("slaveAddress",slaveAddress, true);
        Serial1.print("done");
        Serial.println ("Sent back: done"); 
        Serial.println("................................................................................................." );
        _config = 0;
      }
    }
  }
  return _config;
}
void normalMode(String _rMsg, String trayType, bool _debug = false)
{
  if (trayMode == master)
  {
    *
    pingCheck = 0;
    Serial.println("trayStatusJson");
    Serial.println(trayStatusJson);
    Serial.println("orderStatusJson");
    Serial.println(orderStatusJson);

    loop point 1 start - checkScans interupt 1
    Serial.println("***********************************************************************loop point 1 start - checkScans interupt 1"); 
    unsigned long  point1Time = millis();
    if(scannedData.length() > 0)
    {
      Serial.println("********checkScans interupt 1");
      checkScans();
      Serial.println();
      Serial.println("*******************************************pingtrays 1");
      Serial.println();
      pingTrays();
      pingCheck = 1;
    }
    loop point 1 end 
  

    loop point 2 start - calling server
    Serial.println("*****************************************************************************loop point 2 start - calling server"); 
    unsigned long  point2Time = millis();
    callServer(); //call server to get list of orders
    loop point 2 end

    loop point 3 start - checkscans interrupt 2
    Serial.println("**********************************************************************************loop point 3 start - checkscans interrupt 2");  
    unsigned long  point3Time = millis();
    if(scannedData.length() > 0)
    {
      Serial.println("********checkScans interupt 2");
      checkScans();
      pingTrays();
    }
    loop point 3 end

    loop point 4 start - pingcheck - if trays have not been pinged et
    Serial.println("************************************************************************************loop point 4 start - pingcheck - if trays have not been pinged et");  
    unsigned long point4Time = millis();
    if(pingCheck == 0)
    {
      Serial.println();
      Serial.println("*******************************************pingtrays 1");
      Serial.println();
      pingTrays();
    }
    loop point 5 start - completed check
    Serial.println("*************************************************************************************loop point 5 start - completed check");  
    unsigned long point5Time = millis();
    if(completedCheck) // if an order has been set to complete within this loop
    {
      ping trays before calling the server
      to prevent clearing order before trays get updated
      pingTrays();
    }
    loop point 5 end
    Serial.println("********************************************************************************************loop point 5 end- completed check");
    unsigned long endTime = millis();
    print durantion
    float floatBuffer = 0.0;
    Serial.print("Durantion:");
    Serial.println(endTime - point1Time);
    total time for loop
    break down percentages and actual times for each loop point
    Serial.print("loop point 1 - checkScans interupt 1: ");
    Serial.print(point2Time - point1Time);
    Serial.print(", percentage: ");
    floatBuffer = (float) (point2Time - point1Time)/(endTime - point1Time)*100;
    Serial.println(floatBuffer);

    Serial.print("loop point 2  - calling server: ");
    Serial.print(point3Time - point2Time);
    Serial.print(", percentage: ");
    floatBuffer = (float) (point3Time - point2Time)/(endTime - point1Time)*100;
    Serial.println(floatBuffer);

    Serial.print("loop point 3 - checkscans interrupt 2: ");
    Serial.print(point4Time - point3Time);
    Serial.print(", percentage: ");
    floatBuffer = (float) (point4Time - point3Time)/(endTime - point1Time)*100;
    Serial.println(floatBuffer);

    Serial.print("loop point 4 - pingcheck: ");
    Serial.print(point5Time - point4Time);
    Serial.print(", percentage: ");
    floatBuffer = (float) (point5Time - point4Time)/(endTime - point1Time)*100;
    Serial.println(floatBuffer);

    Serial.print("loop point 5 - completedCheck: ");
    Serial.print(endTime - point5Time);
    Serial.print(", percentage: ");
    floatBuffer = (float) (endTime - point5Time)/(endTime - point1Time)*100;
    Serial.println(floatBuffer);

    renoving check scans outside of scanned data checks
    Serial.println("checkScans");
    checkScans();
    Serial.println("checkScans");
    /*/
    checkScans();
    pingTrays2();
    callServer();
    */
  }
  else if (trayMode == slave)
  {
    masterPing = readSerial1(100, true);
    instructionBuffer = parseMasterPing(masterPing);
    if(!instructionBuffer.equals("null"))
    {
      Serial.println(instructionBuffer);
      respondToInstructions(instructionBuffer);
      Serial1.print(slaveResponse);
    }
    else
    {
      Serial.println("not my ping");
    }
  }
  if(!prevTrayStatusJson.equals(trayStatusJson))
  {
    writeEEPROM("trayStatusJson",trayStatusJson,true);
    prevTrayStatusJson = trayStatusJson;
  }
}
long buttHoldCheck(int cbutt /*, unsigned long time*/)
{
  long timehold;
  if (!digitalRead(cbutt))
  {
    unsigned long startTime = millis();
    while (!digitalRead(cbutt))
    {
      timehold = millis() - startTime;
    }
  }
  return timehold;
}
void checkReprogramButton(int _button)
{
  Assign pins to be input or output
  pinMode(_button, INPUT_PULLUP);
  if (digitalRead(_button) == LOW)
  {
    int trayMode;
    If short press then set to slave
    if ((buttHoldCheck(_button) < 5000))
    {
      trayMode = slave;
      Serial.println("Slave");
      writeEEPROM("slaveAddress","",false);
    }
    else if long press then set to master
    else if (buttHoldCheck(_button) >= 5000)
    {
      trayMode = master;
      writeEEPROM("slaveAddress","1",false);
      trayLog = addPathJSON(trayLog,"1",true,false);
      writeEEPROM("slaveAddressLog",trayLog,false);
    }
    Save tray mode
    writeEEPROM("trayMode",String(trayMode),false);
    Clear memory and reprogramme device
    Serial.print("Device memory has been whiped and reset to be a ");
    if (trayMode == slave)
    {
      Serial.println("Slave");
    }
    else
    {
      Serial.println("Master");
    }
    Reset ESP32
    ESP. restart();
  }
}
void getScannerData(const char* soft_ap_ssid,const char* soft_ap_password,bool _debug)  
{
  Set wifi mode to be both station and an accesspoint
  WiFi.mode(WIFI_AP_STA);
  Configuring ESP32 access point SSID and password 
  WiFi.softAP(soft_ap_ssid, soft_ap_password);   
  If _debug is true display the following
  if (_debug == true)
  {
    Serial.println("Function \"getScannerData\":");
    Serial.println();
    Printing the AP IP address
    Serial.print("Access point Created with IP Gateway "); 
    Serial.println(WiFi.softAPIP());  
    Serial.println("................................................................................................." );
  }  
  Initiate server
  server.begin();  
  Handle GET requests to the root URL 
  server.on("/scanner/data", HTTP_GET, [](AsyncWebServerRequest *request) 
  { 
    Send a response with the current value of orderJson 
    request->send(200, "text/html", "Order JSON: " + scanner); 
  }); 
  Handle POST requests to the root URL 
  server.on("/scanner/data", HTTP_POST, [](AsyncWebServerRequest *request) 
  { 
    Clear the orderJson variable
    scanner = "";  
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) 
  { startTimeM = millis();
    unsigned long _startTime = millis();
    Receive each character
    for (size_t i = 0; i < len; i++)  
    { 
      scanner += (char)data[i]; 
    } 
    if(index + len == total) 
    { 
      request->send(200); 
      If _debug is true display the following
      Serial.println("#################################################################################Function \"getScannerData\":");
      Serial.println();
      Serial.println("Received POST request with data:"); 
      Serial.println(scanner);
      if(scanner.length() > 0)
      {
        scannedData = scanner;
        Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ checkScans web server interupt 1");
        checkScans();
        Serial.println();
        Serial.println("*******************************************pingtrays 1");
        Serial.println();
        pingTrays();
        pingCheck = 1;
      }
      Serial.print("local server time = ");
      unsigned long _endTime = millis() - _startTime;
      Serial.println(_endTime);

      Serial.println("................................................................................................." );
      while(1);
      
    } 
  }); 
}
void initTrayType(int _trayType)
{
  if(_trayType == master) 
  {
    Create a local server for scanner to pass data 
    getScannerData(soft_ap_ssid,soft_ap_password,true);
    Turn off LED master pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
    Turn on master LED pattern 
    ledControl(ledA, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledB, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledC, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledD, 16, 3, 5, white, ON, b, "",trayBrightness);
    Turn on master buzzer pattern 
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    delay(200);
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    Enable communication hardware switch 
    digitalWrite(mComm,HIGH);
    Set and display the Master UART communication
    Serial1.begin(115200, SERIAL_8N1, 17, 16);
    Serial.println ("Master: TX = 16 and Rx = 17");
    Serial.println("................................................................................................." );
    thisTraysAddress = slaveAddress;
    Read Tray log 
    Read master address
    slaveAddress = readEEPROM("slaveAddress",true);
    Read Tray log 
    trayLog = readEEPROM("slaveAddressLog",true);
  }
  Else if device is slave do the following 
  else if(_trayType == slave)
  {
    Turn off slave LED pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
    Turn on slave LED pattern
    ledControl(ledA, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledB, 16, 3, 5, white, ON, b, "",0);
    ledControl(ledC, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledD, 16, 3, 5, white, ON, b, "",0);
    Turn on slave buzzer pattern 
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    Read slave address
    slaveAddress = readEEPROM("slaveAddress",false);
    Disable communication hardware switch 
    digitalWrite(mComm,LOW);
    Set slave UART communication
    Serial1.begin(115200, SERIAL_8N1, 16, 17);
    Serial.print("Slave ");
    Serial.print(slaveAddress);
    Serial.println (": Tx = 17 and Rx = 16");
  } 
}
void setup()
{
  Set serial monitor to a baud of 115200
  Serial.begin(115200);
  Read Tray log 
  trayLog = readEEPROM("slaveAddress",true);
  Display on the serial monitor that the device is currently in Setup
  Serial.println("\n------------------------------------------------Setup------------------------------------------------");
  Assign pins to be input or output
  pinMode(buzzer,OUTPUT);
  pinMode(mComm,OUTPUT);
  pinMode(eButt,INPUT_PULLUP);
  pinMode(eLed,OUTPUT);
  attachInterrupt(eButt, function_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(eButt), button_isr, FALLING); 
  Turn on external Tray led
  digitalWrite(eLed,HIGH);
  Read tray mode 
  trayMode = (readEEPROM("trayMode",false)).toInt();
  If device is master then do the following
  if(trayMode == master) 
  {
    Create a local server for scanner to pass data 
    getScannerData(soft_ap_ssid,soft_ap_password,true);
    Turn off LED master pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
    Turn on master LED pattern 
    ledControl(ledA, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledB, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledC, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledD, 16, 3, 5, white, ON, b, "",trayBrightness);
    Turn on master buzzer pattern 
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    delay(200);
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    Enable communication hardware switch 
    digitalWrite(mComm,HIGH);
    Set and display the Master UART communication
    Serial1.begin(115200, SERIAL_8N1, 17, 16);
    Serial.println ("Master: TX = 16 and Rx = 17");
    Serial.println("................................................................................................." );
    thisTraysAddress = slaveAddress;
    Read Tray log 
    Read master address
    slaveAddress = readEEPROM("slaveAddress",true);
    Read Tray log 
    trayLog = readEEPROM("slaveAddressLog",true);
    trayStatusJson = (readEEPROM("trayStatusJson",false));
    if(trayStatusJson.length() <= 0)
    {
      Serial.println("New device setup");
      totalTrays = getPathSize(trayLog,true);
      trayStatusJson = trayStatusInit(totalTrays,true);
      writeEEPROM("trayStatusJson",trayStatusJson,true);
    }
  }
  Else if device is slave do the following 
  else if(trayMode == slave)
  {
    Turn off slave LED pattern
    ledControl(ledA, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledB, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledC, 16, 0, 16, white, ON, b, "",0);
    ledControl(ledD, 16, 0, 16, white, ON, b, "",0);
    Turn on slave LED pattern
    ledControl(ledA, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledB, 16, 3, 5, white, ON, b, "",0);
    ledControl(ledC, 16, 3, 5, white, ON, b, "",trayBrightness);
    ledControl(ledD, 16, 3, 5, white, ON, b, "",0);
    Turn on slave buzzer pattern 
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    Read slave address
    slaveAddress = readEEPROM("slaveAddress",false);
    If the device has no address set the config
  
    Disable communication hardware switch 
    digitalWrite(mComm,LOW);
    Set slave UART communication
    Serial1.begin(115200, SERIAL_8N1, 16, 17);
    Serial.print("Slave ");
    if(slaveAddress.toInt() < 2)
    {
      Serial.print("?");
    }
    else
    {
      Serial.print(slaveAddress);
    }
    Serial.println (": Tx = 17 and Rx = 16");
    thisTraysAddress = slaveAddress;
  } 
  orderStatusJson = testOrderStatusJson;
  trayStatusJson = testTrayStatusJson;
  serverResponse = testServerResponse;
  Read tray mode 
  Serial.println("................................................................................................." );
  writeEEPROM("slaveAddressLog","[{\"Slave\":\"1\"},{\"Slave\":\"2\"},{\"Slave\":\"3\"},{\"Slave\":\"4\"},{\"Slave\":\"5\"},{\"Slave\":\"6\"}]",false);
  writeEEPROM("slaveAddress","3",false);
  writeEEPROM("trayMode",String(slave),false);
  trayStatusJson = "{\"trays\":[{\"Addr\":\"1\",\"status\":\"available\",\"trayInstruction\":\"standby\",\"orderID\":\"0\"},{\"Addr\":\"2\",\"status\":\"available\",\"trayInstruction\":\"standby\",\"orderID\":\"0\"},{\"Addr\":\"3\",\"status\":\"available\",\"trayInstruction\":\"standby\",\"orderID\":\"0\"},{\"Addr\":\"4\",\"status\":\"available\",\"trayInstruction\":\"standby\",\"orderID\":\"0\"},{\"Addr\":\"5\",\"status\":\"available\",\"trayInstruction\":\"standby\",\"orderID\":\"0\"},{\"Addr\":\"6\",\"status\":\"available\",\"trayInstruction\":\"standby\",\"orderID\":\"0\"}]}";
  writeEEPROM("trayStatusJson",trayStatusJson,true);
}
void loop()
{
  String trayResponse = readSerial1(100, true);
  If not configuration mode do the following 
  if(!config)
  {
    Run normal mode sequence
    normalMode(trayResponse,trayType);
  }
  Else if it is configuration mode do the following 
  else
  {
    Run configuration mode sequence
    config = configurationMode(trayResponse,trayType, config);
  }
  Check if the reprogram button was pressed and set trayMode accordingly then reboot the device
  checkReprogramButton(button);
  trayResponse = readSerial1(100, true);
}

