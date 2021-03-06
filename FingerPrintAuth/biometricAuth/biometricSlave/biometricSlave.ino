#include <FingerPrint.h>
#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

#define LOCALADDRESS 2


// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)


byte input;

unsigned short relays[4]={A0,A1,A2,A3};
unsigned short buzzer=2; //Dummy pin number TODO: Check actual value
const byte rows = 1; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'Q','U','D','E'}
};  // Q - QUIT/ESC, U - SCROLL UP, D - SCROLL DOWN, E - ENTER
byte rowPins[rows] = {6}; //connect to the row pinouts of the keypad
byte colPins[cols] = {7, 8, 9, 10}; //connect to the column pinouts of the keypad
Keypad keypad1 = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

byte dataToSend[10]={0,0,0,0,0,0,0,0,0,0};
unsigned long pollTimeCheck=0;
byte RFID[12];
boolean RFIDdetected=false;

char keypressed='F';
  
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
byte response[16]={0,0,0x40,0x46,0};
int id=0;
int flag=0;
int req_flag=0;
void setup()
{
  Wire.begin(LOCALADDRESS);                // join i2c bus with address #4
  
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);   // start serial for output
  fingerprint_init();
  Serial.println(fingerprint_check(),DEC);
  for (unsigned short i=0;i<4;i++)
    pinMode(relays[i],OUTPUT);
}

void poll()
{
  id=-1;
  id=getFingerprintIDez();

}
    

void loop()
{
  if(millis()-pollTimeCheck>100)
  {  
    poll();
    pollTimeCheck=millis();
   // Serial.println("I'm back");
  }
  //Check for keypress
  if(keypressed=='F')    //Checking for keypress before previous keypressed is sent to Master will result in loss of the previous keypressed.
  {
    keypressed=keypress();
  }
  //Serial.println("Waiting for Omar");  
  switch(flag)
  {
    case 1:if(fingerprint_check())
                      response[3]=0x40;
                    else
                      response[3]=0x41;  flag=0;break;
    case 2:if(getFingerprintEnroll1()==1)
                       response[3]=0x40;
                    else
                      response[3]=0x41;  flag=0;break;
    case 3:if(getFingerprintEnroll2()==FINGERPRINT_OK)
                      response[3]=0x40;
                    else
                      response[3]=0x41;  flag=0;break;
    case 4://Serial.println(g_ReceivedByte_un.g_RecByte_split.data[i]);
           if(storeFingerprint(g_ReceivedByte_un.g_RecByte_split.data[0])==FINGERPRINT_OK)
                      response[3]=0x40;
                    else
                      response[3]=0x43;  flag=0;break;
    case 5:if(deleteFingerprint(g_ReceivedByte_un.g_RecByte_split.data[0])==FINGERPRINT_OK)
                      response[3]=0x40;
                    else
                      response[3]=0x43;  flag=0;break;
    case 6:
         ///  id=0;
           //response[4]=getFingerprintIDez();  flag=0;break;
         ///  id=getFingerprintIDez();
           //Serial.print("ID: ");Serial.println(id);
            //Serial.println("Fingerprint match found.");
                //Fingerprint Match found
            if(RFIDdetected==true)
            {
              Serial.println("RFID detected");
              response[3]=0x47;
              for(int i=4;i<=15;i++)
                response[i]=RFID[i-4];
            }
            
           else if(id==-1)
              {
                //No Finger Detected
                Serial.println("No Finger Detected");
                
                response[4]=0;
                response[5]=0;
                response[3]=0x48;
                
              }    
           else if(id==2)
              {
                Serial.println("Finger Detected, No Match Found");

                response[4]=0;
                response[5]=0;
                response[3]=0x45;                
                //Fingerprint Detected, No match found
              }
           else    
            {
              response[4]=lowByte(id);
              response[5]=highByte(id);
              response[3]=0x44;                
            }
             flag=0;
              break; 
           //flag=0;
          
           //break;
    case 7://Check Key press
           //keypressed=keypress();
            if(keypressed!='F')
            {
//             0x40 0x49 - Key pressed              
               response[3]=0x49;
               response[4]=keypressed;
               Serial.print("Key pressed:");
               Serial.println(keypressed);
               keypressed='F';
            }
            else
            {
//             0x40 0x4A - No key pressed
               response[3]=0x4A;
            }
            flag=0;
            break;
     case 8:// Open Door
            openDoor(g_ReceivedByte_un.g_RecByte_split.data[0]);
            break;
     case 9:// Open Door
            closeDoor(g_ReceivedByte_un.g_RecByte_split.data[0]);
            break;
     case 10://Sound Buzzer
             soundBuzzer(200); //For 200 ms     
                        
    default:break;
  }
//  flag=0;
}
void receiveEvent(int howMany)
{
  if(Wire.available()==16) // loop through all but the last
  {
    for(int i=0;i<16;i++)
      g_ReceivedByte_un.g_RecByte_byt[i]=Wire.read();
//    Serial.print("header");Serial.println(g_ReceivedByte_un.g_RecByte_split.header);
//    Serial.print("cmd");Serial.println(g_ReceivedByte_un.g_RecByte_split.cmd);
    response[2]=0x40;
    response[3]=0x46;
    for(int i=4;i<16;i++)
      response[i]=0x00;
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
      case 0x3830:  flag=6;
                    break;
      case 0x3932:  flag=7;
                    break;
      case 0x3532:  flag=8;
                    break;
      case 0x3632:  flag=9;
                    break;
      case 0x3732:  flag=10;
                    break;                    
                                        
      default:break;
    }
      req_flag=1;
  }  
}

void requestEvent()
{
  //Serial.println("Received Request");
 // Serial.println("Response sent");
  //Serial.write(response,16);
  if(RFIDdetected)
  {
    int i=0;
    for(i=4;i<16;i++)
      if(response[i]==0)
        break;
    if(i==16)
    {
      RFIDdetected=false;
      Serial.println("RFID Response sent");
    }
  }
  Wire.write(response,16);
  req_flag=1;
}

void serialEvent()
{
  if(Serial.available()>11)
  {
    for(int i=0;i<12;i++)
      {
         RFID[i]=Serial.read();
         Serial.print(RFID[i]);
      }
      
    RFIDdetected=true;
  }    
}

char keypress()
{
  char key = keypad1.getKey();

  if (key != NO_KEY)
      return(key);    
  else
    return('F'); //Failure
   
}


void openDoor(unsigned short doorIndex)
{
  digitalWrite(relays[doorIndex],HIGH);
}

void closeDoor(unsigned short doorIndex)
{
  digitalWrite(relays[doorIndex],LOW);
}

void soundBuzzer(unsigned int timeDelay)
{
 digitalWrite(buzzer,HIGH);
 delay(timeDelay);
 digitalWrite(buzzer,LOW);
}
