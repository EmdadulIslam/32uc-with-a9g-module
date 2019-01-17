#include <NMEAGPS.h>
//#include <GPSTime.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <ArduinoJson.h>
#include<string.h>

//SoftwareSerial A7board(16, 17); // for esp
//#define A7board Serial1  // Best:  Serial1 on a Mega, Leo or Due
/*************************************************************************************************************************/
int scanTime = 5; //In seconds
String result = "";
uint64_t chipid;
String unit_id;
byte counts = 0;
void addresult(String ss);
void BLE_SCAN() ;
/*************************************************************************************************************************/

HardwareSerial A7board(2);

static NMEAGPS gps;
static gps_fix fix;
String a = "";

//  Finite-State Machine declarations
enum state_t
{
  WAITING,           // until time to check again
  GPS_ON_WAIT,       // after AT+GPS=1
  GPS_READING,       // after AT+GPSRD=1
  GPS_READING_WAIT,  // after location received, AT+GPSRD=0
  GPS_OFF_WAIT,      // after AT+GPS=0
  SENDING_SMS
  // other states?
};
state_t  state = WAITING; // start here
uint32_t stateTime      ; // used for timeouts, instead of delay

const uint32_t CHECK_LOCATION_TIME = 5000; // ms, how often to check


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      addresult(advertisedDevice.toString().c_str());
    }
};

void setup()
{
  Serial.begin( 115200 );
  A7board.begin( 115200 );
  // while (!Serial) continue;
  stateTime = millis();
  chipid = ESP.getEfuseMac();
  Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); //print High 2 bytes
  Serial.printf("%08X\n", (uint32_t)chipid); //print Low 4bytes.
  if (String(((uint32_t)ESP.getEfuseMac()), HEX).length() == 7)
    unit_id = "BT" + String(((uint16_t)(ESP.getEfuseMac() >> 32)), HEX) + '0' + String(((uint32_t)ESP.getEfuseMac()), HEX);
  else
    unit_id = "BT" + String(((uint16_t)(ESP.getEfuseMac() >> 32)), HEX) + String(((uint32_t)ESP.getEfuseMac()), HEX);

  Serial.println(unit_id);
 
}

void loop()
{
  /*
  switch (state) {

    case WAITING:
      echoA7chars();

      if (millis() - stateTime >= CHECK_LOCATION_TIME) {

        // Turn GPS data on
        A7board.println( F("AT+GPS=1") );

        stateTime = millis();
        state     = GPS_ON_WAIT;
      }
      break;

    case GPS_ON_WAIT:
      echoA7chars();

      // Wait for the GPS to turn on and acquire a fix
      if (millis() - stateTime >= 5000) { // 5 seconds

        //  Request GPS data
        A7board.println( F("AT+GPSRD=1") );

        Serial.print( F("Waiting for GPS fix...") );

        stateTime = millis();
        state     = GPS_READING;
      }
      break;

    case GPS_READING:
      while (gps.available( A7board )) { // parse the NMEA data
        fix = gps.read(); // this structure now contains all the GPS fields

        if (fix.valid.location) {
          Serial.println();

          // Now that we have a fix, turn GPS data off
          A7board.println(  F("AT+GPSRD=0") );

          stateTime = millis();
          state     = GPS_READING_WAIT;
        }
      }

      if (millis() - stateTime > 1000) {
        Serial.print( '.' ); // still waiting for fix, print a dot.
        stateTime = millis();
      }
      break;

    case GPS_READING_WAIT:
      ignoreA7chars();

      // Wait for the GPS data to stop
      if (millis() - stateTime >= 1000) {

        // Turn GPS power off
        A7board.println(  F("AT+GPS=0") );

        stateTime = millis();
        state     = GPS_OFF_WAIT;
      }
      break;
    case GPS_OFF_WAIT:
      ignoreA7chars();

      // Wait for the GPS data to stop
      if (millis() - stateTime >= 1000) {

        // Show the location we will send via SMS
        Serial.print( F("location:\n ") );
        Serial.print( F("latitude :"));
        Serial.print( fix.latitude(), 6 ); // use the latitude field of the fix structure
        Serial.print( F("\n longitude :  ") );
        Serial.println( fix.longitude(), 6 ); // use the longitude field of the fix structure
        Serial.print( F("\n Time and date: ") );
        Serial.println(time1());
        // Serial.println("time1()");
      //  mqttloop();
        // delay(10000);
        /*
 time1();
  BLE_SCAN();
  create_ble_json();
  mqttloop();
  
 // 
  counts = 0;
  result = "";

        stateTime = millis();
        state     = SENDING_SMS;
      }
      break;
      
          case SENDING_SMS:
            echoA7chars();

            if (millis() - stateTime >= 2000) {
              stateTime = millis();
              state     = WAITING; // start over, or other states...
            }
            break;
      
      //  ... other states ...

  }
*/
  
 BLE_SCAN();
  create_ble_json();
  mqttloop();
  
 // 
  counts = 0;
  result = "";
 
  
  
}

void mqttloop()
{

  a = (String)time1();
  String value2 = String(String("{'device_id':'bus_1',") + String("'datetime':") + "'" + (String)a + "'" + String("'longitude':") + "'" + String(fix.longitude(), 6) + "'," + String("'latitude':") + "'" + String(fix.latitude(), 6) + "'" + String("}"));
String value4 = String(result);

  String value3 = String(String("rams/")+String(unit_id)+"/"+String("bluetooth"));
  Serial.println("Function calling(): mqttloop");
   A7board.println(  F("AT") );
  delay(1000);

  A7board.println(  F("AT+CREG?") );
  delay(100);


  A7board.println(  F("AT+CGATT=1") );
  delay(100);


  A7board.println(  F("AT+CGDCONT=1,\"IP\",\"CMNET\"") );
  delay(100);


  A7board.println(  F("AT+CGACT=1,1") );
  delay(100);


  A7board.println(  F("AT+MQTTCONN=\"www.rumytechnologies.com\",1883,\"Bus_1\",120,0") );
  delay(3000);


  String cmd = String("AT+MQTTPUB=");
  /*
  String value1 = String(("location/1/data"));
  String data_packet = cmd + '"' + value1 + '"' + "," + '"' + value2 + '"' + "," + 0 + "," + 0 + "," + 0;
   Serial.print(value2);
   delay(2000);

  Serial.print(data_packet);
  A7board.println(data_packet );
  delay(3000);
*/  


   String data_packet2 = cmd + '"' + value3 + '"' + "," + '"' + value4 + '"' + "," + 0 + "," + 0 + "," + 0;
    Serial.println(data_packet2);
  A7board.println(data_packet2 );
  delay(5000);
 
   String data_packet3 = cmd + '"' + value3 + '"' + "," + '"' + "emdad" + '"' + "," + 0 + "," + 0 + "," + 0;
    Serial.println(data_packet3);
  A7board.println(data_packet3 );
  delay(5000);
  
}

void echoA7chars()
{
  if (A7board.available())
    Serial.write( A7board.read() ); // just echo what we are receiving
}

void ignoreA7chars()
{
  if (A7board.available())
    A7board.read(); // ignore what we are receiving
}
String time1()
{
  String response = "";

  A7board.println("AT+CCLK?");
  long int time = millis();
  while ( (time + 2000) > millis())
  {
    while (Serial1.available())
    {
      char a = Serial1.read();

      response += a;
    }
  }
  // Serial.print(response);
  response.remove(40, 42);
  response.remove(0, 20);
  // Serial.println(response);
  String time_1 = response;
 // Serial.println(time_1);
 // Serial.println("time print over");
  return time_1;
}
void BLE_SCAN() {

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
}
void addresult(String ss) {

  if (ss != "")
  {
    result += ss;
   // result += ',';
    counts++;
    Serial.println(result);
  }
  else
    Serial.println(ss);
}
void create_ble_json() {
    DynamicJsonBuffer JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();
    JSONencoder["UNIT_ID"] =  unit_id;
    JsonArray& values = JSONencoder.createNestedArray("MAC_ID");
    for (int i = 0, j = 0; i < counts; i++)
    {
      values.add(result.substring(j, j + 12));
      j += 12;
    }
    String data1, data2;
  
    JSONencoder.printTo(data1);
    Serial.println(data1);
}
