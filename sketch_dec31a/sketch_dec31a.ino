#include <NMEAGPS.h>
//#include <GPSTime.h>
#include <ArduinoJson.h>
#include<string.h>
//#include<SoftwareSerial.h>
//SoftwareSerial A7board(4, 5); // for esp
#define A7board Serial1  // Best:  Serial1 on a Mega, Leo or Due
//      or
//#include <AltSoftSerial.h>
//AltSoftSerial A7board; Next best, but you must use pins 8 & 9!  This is very efficient.
//      or
//#include <NeoSWSerial.h>
//NeoSWSerial A7thinker( 10, 11 ); // 2nd best on whatever two pins you want.  Almost as efficient.

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

void setup()
{
  Serial.begin( 115200 );
  A7board.begin( 115200 );
  while (!Serial) continue;
  stateTime = millis();
time1();

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
  // Serial.print(response);
  String time_1 = response;
 // Serial.println(time_1);
  //delay(4000);
 // Serial.println("time print over");
  return time_1;
}


void loop()
{  
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
        mqttloop();
       // delay(10000);

        // GPS data stopped, now send SMS with location values ?
        //A7board.print( SMS commands? );
        //A7board.print( F("lat=") );
        //A7board.print( fix.latitude(), 6 ); // use the latitude field of the fix structure
        //  ...

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

  // mqttloop();
 
}

void mqttloop()
{
  
  a = (String)time1();
 String value2 = String(String("{'device_id':'bus_1',") + String("'datetime':") + "'" + (String)a + "'" + String("'longitude':") + "'" + String(fix.longitude(), 6) + "'," + String("'latitude':") + "'" + String(fix.latitude(), 6) + "'" + String("}"));
  Serial.println("Function calling(): mqttloop");
 
  A7board.println(  F("AT+CREG?") );
  delay(100);
  

  A7board.println(  F("AT+CGATT=1") );
 delay(100);
  

  A7board.println(  F("AT+CGDCONT=1,\"IP\",\"CMNET\"") );
  delay(100);
  

  A7board.println(  F("AT+CGACT=1,1") );
  delay(100);
   

  A7board.println(  F("AT+MQTTCONN=\"www.rumytechnologies.com\",1883,\"Bus_1\",120,0") );
  delay(2000);
   
  //String gps_link= at + serial + gps + 0,0,0 ;
  String cmd = String("AT+MQTTPUB=");

  //String value3 = "{'device_id':'bus_1','datetime':'2018-10-15','longitude':'91.82343','latitude':'22.374569'}";
  // String value2 = String(String("{'device_id':'bus_1',")+String("'datetime':")+ String("'2019/01/07',")+ String("'longitude':") +"'"+ String(fix.longitude(),6) +"',"+String("'latitude':") +"'"+ String(fix.latitude(),6)+"'"+String("}"));

  //String value2 = String(' {"device_id":"1","datetime":"1542265253","longitude":"') +  String(fix.longitude())  +  String('","latitude":"') +   String(fix.latitude()) + String('"}');
  String value1 = String(("location/1/data"));
  String data_packet = cmd + '"' + value1 + '"' + "," + '"' + value2 + '"' + "," + 0 + "," + 0 + "," + 0;
 // Serial.print(value2);
 // delay(2000);

  Serial.print(data_packet);
 // delay(5000);
  
  // String value3 = "emdad";
  // String data_packet2 = cmd + '"' + value1 + '"' + "," + '"' + value3 + '"' + "," + 0 + "," + 0 + "," + 0;
  //
  // Serial.print(data_packet2);
  // delay(5000);


  //uint32_t starttime=millis();
 
  A7board.println(data_packet );
  delay(3000);

 
  // uint32_t eltime = millis()- timespan;
  //   Serial.println(eltime);
  // String data_packet2 = cmd + '"' + value1 + '"' + "," + '"' + value3 + '"' + "," + 0 + "," + 0 + "," + 0;
  //  Serial.print("\n");
  // Serial.print(data_packet2);
  // delay(5000);
  // A7board.println(data_packet2 );
  // delay(5000);


}

