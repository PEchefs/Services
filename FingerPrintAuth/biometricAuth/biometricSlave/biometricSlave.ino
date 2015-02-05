#include <FingerPrint.h>
#include <Wire.h>
#include "Keypad.h"
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define LOCALADDRESS 2


// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)


byte input;
const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1','2','3','E'},
  {'4','5','6','M'},
  {'7','8','9','U'},
  {'P','0','O','D'}
};
byte rowPins[rows] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[cols] = {A4, A5, A6, A7}; //connect to the column pinouts of the keypad
Keypad keypad1 = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

byte dataToSend[10]={0,0,0,0,0,0,0,0,0,0};

unsigned int g_StartByte_u16 = 0x7771; //Start Pattern
unsigned int g_EndByte_u16 = 0x7177; //End Pattern

union
{
  struct
  {
    unsigned int  header;
    unsigned int  cmd;
    byte data[12];
  }g_RecByte_split;
  byte g_RecByte_byt[16];  
}g_ReceivedByte_un;
byte response[16]={0x40,0,0};
int flag=0;
int req_flag=0;
void setup()
{
  Wire.begin(LOCALADDRESS);                // join i2c bus with address #4
  
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);   // start serial for output
  fingerprint_init();
  Serial.println(fingerprint_check(),DEC);
}

void loop()
{
  delay(1000);
  Serial.println("Waiting for Omar");  
  switch(flag)
  {
    case 1:if(fingerprint_check())
                      response[1]=0x40;
                    else
                      response[1]=0x41;  flag=0;break;
    case 2:if(getFingerprintEnroll1())
                       response[1]=0x40;
                    else
                      response[1]=0x41;  flag=0;break;
    case 3:if(getFingerprintEnroll2())
                      response[1]=0x40;
                    else
                      response[1]=0x41;  flag=0;break;
    case 4:if(storeFingerprint(g_ReceivedByte_un.g_RecByte_split.data[0]))
                      response[1]=0x40;
                    else
                      response[1]=0x43;  flag=0;break;
    case 5:if(deleteFingerprint(g_ReceivedByte_un.g_RecByte_split.data[0]))
                      response[1]=0x40;
                    else
                      response[1]=0x43;break;
    case 6:response[1]=0x44;
           response[2]=getFingerprintIDez();  flag=0;break;
    default:break;
  }
  if(req_flag==1)
  {
    Serial.println("response sent");
    Wire.write(response,16);req_flag=0;   
  }
}
void receiveEvent(int howMany)
{
  if(Wire.available()==16) // loop through all but the last
  {
    for(int i=0;i<16;i++)
      g_ReceivedByte_un.g_RecByte_byt[i]=Wire.read();
    Serial.print("header");Serial.println(g_ReceivedByte_un.g_RecByte_split.header);
    Serial.print("cmd");Serial.println(g_ReceivedByte_un.g_RecByte_split.cmd);
    switch(g_ReceivedByte_un.g_RecByte_split.cmd)
    {
      case 0x3130:  flag=1;
                      break;
      case 0x3230:  flag=2;
                     break;
      case 0x3330:  flag=3;
                    break;
      case 0x3430:  flag=4;
                    break;
      case 0x3530:  flag=5;
                    break;
      case 0x3536:  flag=6;break;
      default:break;
    }
  }  
}

void requestEvent()
{
  req_flag=1;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()

//void serialEvent()
//{
//  Serial.println("Entered I2C receive function");
//  unsigned short i=0;
//  if (Serial.available()>5) // loop through all but the last
//  {
//    char c = Serial.read(); // receive byte as a character
//    Serial.print(c);         // print the character
//    g_ReceivedByte_un.g_RecByte_byt[i]=c;
//    i++;
//  
//    Serial.println("");
//    Serial.println(g_ReceivedByte_un.header, DEC);
//    Serial.println(g_StartByte_u16,DEC);
//    Serial.println(g_ReceivedByte_un.footer, DEC);
//    Serial.println(g_EndByte_u16,DEC);
//    Serial.println(g_ReceivedByte_un.command, DEC);
//    Serial.println(g_ReceivedByte_un.RFID, DEC);
//    
//  if ((g_ReceivedByte_un.header == g_StartByte_u16)&&(g_ReceivedByte_un.footer == g_EndByte_u16))
//  {
//    if (g_ReceivedByte_un.command == 'a')
//    {
//      Serial.println("Command received : Enroll");
//      enroll(g_ReceivedByte_un.RFID);
//    }
//
//    if (g_ReceivedByte_un.command == 'd')
//    {
//      Serial.println("Command received : Delete");
//      deletetemp(g_ReceivedByte_un.RFID);
//    }
//    
//    if (g_ReceivedByte_un.command == 'f')
//    {
//      Serial.println("Command received : Download Fingerprint");
//      showtemplate(g_ReceivedByte_un.RFID);
//    }
//    
//    if (g_ReceivedByte_un.command == 'g')
//    {
//      Serial.println("Command received : Show Number of FingerPrint Id's Stored");
//      tempnumber();
//    }
//      
//    if (g_ReceivedByte_un.command == 'j')
//    {
//      Serial.println("Command received : Search Fingerprint");
//      getFingerprintID();
//    }
//    
//    if (g_ReceivedByte_un.command == 'h')
//    {
//      Serial.println("Command received : Upload Fingerprint");
//      uploadtemp(g_ReceivedByte_un.RFID);
//    }
//  }
// }
//}
//
//void sendToMaster(byte data[])
//{
//  for(unsigned short i=0;i<10;i++)
//  {
//    Wire.write(data[i]); // respond with message of 6 bytes
//  // as expected by master
//  }
//}




