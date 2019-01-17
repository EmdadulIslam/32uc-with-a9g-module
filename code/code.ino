#include<stdio.h>
#include<string.h>
//#include<SoftwareSerial.h>
#define A7board Serial1 // for leo
//SoftwareSerial A7board(2,3);// for esp
//HardwareSerial A7board(2);
#define DEBUG true
String target_phone = "+8801737860976"; // Your phone number, not number of 32U4 with A7/GSM/GPS.
long starttime, endtime ;
String gps;
void setup()
{
  Serial.begin(115200);
  A7board.begin(115200);

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8,OUTPUT);
  digitalWrite(5, HIGH); 
  digitalWrite(4, LOW); 
  digitalWrite(8, LOW); 
  Serial.println("After 2s, test begin!!");
  delay(2000);
  
 //sendData("AT+GPS=1",1000,DEBUG);  
 // sendData("AT+GPSRD=1",3000,DEBUG);
 // sendData("AT",1000,DEBUG); 
 //funtion_test();

 //testGPS();
  //Serial.println("After 2s, test begin!!");
  //delay(2000);
}

void loop()
{
  if (A7board.available()>0) {
    Serial.write(A7board.read());
  }
  if (Serial.available()>0) {
    A7board.write(Serial.read());
  }
 // sendData("AT+CCLK?",1000,DEBUG);   
  // A7board.println("AT+CCLK?");
  // delay(300);
 sendData("AT+GPS=1",1000,DEBUG);    
 sendData("AT+GPSRD=1",3000,DEBUG);
//funtion_test();
//GPS2();
TCP_MQTT();
}

void funtion_test(){
  
   Serial.println("Test begin!!");
   digitalWrite(8, HIGH); 
   delay(3000);       
   digitalWrite(8, LOW); //Power ON..
   Serial.println("A7 Power ON!");
   Serial.println("You may receive the AT   OK"); 
   sendData( "AT",1000,DEBUG);
   delay(500);
   digitalWrite(5, LOW);  //Sleep
   Serial.println("A7 go to sleep now!");
   delay(3000);
   Serial.println("test GPS function");
 //TCP_MQTT();
 testGPS();//no call, A7 sleeping   
  //GPS2();
   sendData( "AT",1000,DEBUG);
   delay(500);
   Serial.println("if you receive GPS data,the sleep test failed!");
   digitalWrite(5, HIGH);   // wake up
   delay(1000);
   Serial.println("A7 WAKE UP!"); 
   Serial.println("test whether wake up or not, if OK, A7 wake up"); 
   sendData( "AT",1000,DEBUG);
   delay(500);     
   digitalWrite(4, HIGH); // power off A6
   Serial.println("A7 power off!");
   delay(3000);
   digitalWrite(4, LOW);
   Serial.println("print AT and you not receive OK"); 
   sendData( "AT",1000,DEBUG);
   delay(500); 
   Serial.println("A7 not Respond"); 
   digitalWrite(8, HIGH);           //POWER UP
   delay(3000);       
   digitalWrite(8, LOW);
   delay(3000);
   Serial.println("A7 Power ON!"); 
   Serial.println("................................."); 
   sendData( "AT",1000,DEBUG); //
   delay(1000);
   Serial.println("The funtion is Get GPS..."); 
  // testGPS();
   Serial.println("The funtion is Dial Voice Call..."); 
   delay(1000);
  // DialVoiceCall();
   Serial.println("The funtion is Send SMS ..."); 
   delay(1000);
 //  SendTextMessage();
   Serial.println("This function is submit a HTTP request...");
   delay(1000);
//   TCP_GPRS();
 
// TCP_MQTT();
  // Serial.println("All the test of 32U4 with A7 is complete!");
}

void testGPS(void){
  
  sendData("AT+GPS=1",1000,DEBUG);    
   sendData("AT+GPSRD=1",3000,DEBUG);
  sendData("AT+GPS=0",10000,DEBUG);
 
}

void GPS2()
{   
  // sendData("AT+CREG?",5000,DEBUG); //Query network registration
  // delay(100);
  
  sendData("AT+CGATT=1",5000,DEBUG);   
  delay(200); 
  
 // sendData("AT+CGDCONT=1,\"IP\",\"CMNET\"",2000,DEBUG);//setting PDP parameter 
//   delay(100);

  sendData("AT+CGACT=1,1",10000,DEBUG); //Activate PDP, open Internet service
   delay(200); 
   
 sendData("AT+GPS=1\r\n",10000,DEBUG);
 delay(3000);
    
   sendData("AT+AGPS=1\r\n",20000,DEBUG);  
   delay(25000);
    sendData("AT+GPSRD=5",3000,DEBUG);
    delay(2000);
   
  // sendData("AT+GPSMD=1",3000,DEBUG);
 //   delay(200);
  //  sendData("AT+LOCATION=2",20000,DEBUG);
  //  delay(200);
  }

void TCP_GPRS(){
   sendData("AT+CREG?",5000,DEBUG); //Query network registration
   delay(100);
   sendData("AT+CGATT=1",5000,DEBUG);
   delay(100); 
   sendData("AT+CGDCONT=1,\"IP\",\"CMNET\"",2000,DEBUG);//setting PDP parameter 
   delay(100); 
   sendData("AT+CGACT=1,1",10000,DEBUG); //Activate PDP, open Internet service
   delay(100);  
    starttime= millis();
   sendData("AT+CIPSTART=\"TCP\",\"www.google.com\",80",10000,DEBUG);
    endtime= millis()-starttime;
      Serial.println(endtime);
   delay(100);
   
   sendData("AT+CIPSEND=5,\"12345\"",2000,DEBUG); //Send string "12345" 
   delay(100); 
   sendData("AT+CIPCLOSE",2000,DEBUG);     //Close TCP
   delay(100); 
   /*
     sendData("AT+CREG?",3000,DEBUG);     
     sendData("AT+CGATT=1",1000,DEBUG);
     sendData("AT+CGDCONT=1,\"IP\",\"CMNET\"",1000,DEBUG);
     sendData("AT+CGACT=1,1",1000,DEBUG);
     sendData("AT+CIPSTART=\"TCP\",\"google.com\",80",3000,DEBUG);
     sendData("AT+CIPSEND=80",1000,DEBUG);
     sendData("GET http://www.google.com HTTP/1.0\r\n",100,DEBUG);
     */
}

void SendTextMessage()
{ 
  sendData("",2000,DEBUG);
  sendData("AT+CMGF=1",2000,DEBUG);//Because we want to send the SMS in text mode
  delay(100);
  sendData("AT+CMGS="+target_phone,2000,DEBUG);//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  sendData("GSM test message!",2000,DEBUG);//the content of the message
  delay(100);
  A7board.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
}

void DialVoiceCall()
{
   sendData("AT+SNFS=0",5000,DEBUG);
   delay(100);
   sendData("ATD"+target_phone,5000,DEBUG);// "ATD+86137xxxxxxxx"dial the number
   delay(100);
}


String sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    A7board.println(command); 
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(A7board.available())
      {       
        char c = A7board.read(); 
        response+=c;
      }  
    }    
    if(debug)
    {
      Serial.print(response);
    }    
    return response;
}

void httpRec(const int timeout){
    String response = "";    
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(A7board.available())
      {       
        char c = A7board.read(); 
        response+=c;
      }  
    }    
    Serial.print(response); 
}
void TCP_MQTT(){
   //Serial.print("AT+CREG?");
   sendData("AT+CREG?",5000,DEBUG); //Query network registration
   delay(100);
  // Serial.print("AT+CGATT=1");
  sendData("AT+CGATT=1",5000,DEBUG);   
  delay(200); 
 // Serial.print("AT+CGDCONT=1,\"IP\",\"CMNET\"");
  sendData("AT+CGDCONT=1,\"IP\",\"CMNET\"",2000,DEBUG);//setting PDP parameter 
   delay(100);
   //Serial.print("AT+CGACT=1,1");
  sendData("AT+CGACT=1,1",10000,DEBUG); //Activate PDP, open Internet service
   delay(200); 
   // Serial.print("AT+MQTTCONN=\"stellarbd.com\",1883,\"123456\",120,0");
 sendData("AT+MQTTCONN=\"rumytechnologies.com\",1883,\"123456\",120,0",10000,DEBUG);
   delay(2000);
   
 //String gps_link= at + serial + gps + 0,0,0 ;
 String cmd=String("AT+MQTTPUB=");
 String value1=String(("emdad"));
 String value2=String(("123456"));
 String data_packet=cmd+'"'+value1+'"'+","+'"'+value2+'"'+","+0+","+0+","+0;
 //Serial.println(data_packet);
// delay(2000);
 // sendData(data_packet,30000,DEBUG);
  // delay(2000);
   
   sendData("AT+MQTTPUB= \"emdad\" ,\"124563\",0,0,0",10000,DEBUG);
   delay(2000);
  //  sendData("AT+MQTTSUB=\"emdad\",1,0",10000,DEBUG);
  //  delay(10000);
  // sendData("AT+MQTTDISCONN",10000,DEBUG); //Activate PDP, open Internet service
  // delay(100);  

}

