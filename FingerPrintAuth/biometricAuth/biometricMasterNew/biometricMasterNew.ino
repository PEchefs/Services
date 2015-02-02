
#include <functions.h>
#include <Menu.h>
#include <U8glib.h>
#include "Wire.h"
#define DEBUG 1
#define SLAVEADDRESS 2

extern unsigned short currentMenu;
extern state previousState;
extern state currentState;
extern state newState;
extern menu_st Menu[7];

key keypressed=NOKEY;
boolean keyPressDetected = false;
byte x= 0;



unsigned short isKeyPressed()
{
//  Non zero - true
  if(keyPressDetected == true)
  {
    keyPressDetected=false;
    return 1;
  }
  else
    return 0;
}
void writeToSlave(byte sendCommand[] )
{
  //Serial.println("Sending command to Slave:");
  
  Wire.beginTransmission(SLAVEADDRESS); // transmit to device #4
  unsigned short i=0;
  while(sendCommand[i]!=NULL)
  {
    Serial.print(sendCommand[i]);
    Wire.write(sendCommand[i]);        // sends five bytes
    i++;
  }
  
  delay(500);
}

void readFromSlave()
{
  Wire.requestFrom(SLAVEADDRESS, 10);    // request 6 bytes from slave device #2
  //Serial.println("Received Data:");
  while (Wire.available())   // slave may send less than requested
  {
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
  
  delay(500);
}
void setup() {
  Serial.begin(115200);
  pinMode(17,OUTPUT);
  digitalWrite(17,HIGH);
  Wire.begin();
  Serial.println("End of setup");
  initializeMenus();
}


void loop()
{
  byte poll[10]={0,0,0,0,0,0,0,0,0,0};
  //Serial.println("Beginning Menu assignments");
 
  //Serial.println("Ending Menu Assignments");
  switch (currentState)
  {
    case HOMESCREEN:Serial.println("Current State: HOMESCREEN");
                    poll[0]=0x50;
                    writeToSlave(poll);
                    delay(500);
                    readFromSlave();
                    break;
    
    case MENUSCREEN:if(isKeyPressed())
                    {
                      switch (keypressed)
                      {
                        case SCROLLUP:  scrollup();
                                        break;
                        case SCROLLDOWN:scrolldown();
                                        break;
                        case ENTER:     enterMenuItem(Menu[currentMenu]);
                                        break;
                        case ESC:       escMenuList();
                                        break;
                      }
                      if(DEBUG)
                        Serial.println("Calling Display Menu Function");
                     displayMenu(Menu[currentMenu]);
                     // displayMenu();
                    }
                    break;

    case FPAUTH:break;
    
    case PWDAUTH:break;
    
    case RFIDAUTH:break;
    
    case MENUFUNCTION: break;
    
    default : break;
  
  }
  


}
void serialEvent() {
   while (Serial.available()>0) {
      
    char inchar=char(Serial.read());
    
    keyPressDetected=true;
    if (inchar == 'd')
      //scrolldown();
      keypressed=SCROLLDOWN;
    else if(inchar=='u')
//    scrollup();
      keypressed=SCROLLUP;
    else if(inchar=='e')
      keypressed=ENTER;
    else if (inchar=='q')
      keypressed=ESC;
    else if(inchar=='0')
      keypressed=ZERO;
    else if (inchar=='1')
      keypressed=ONE;
    else if(inchar=='2')
      keypressed=TWO;
    else if (inchar=='3')
      keypressed=THREE;
    else if(inchar=='4')
      keypressed=FOUR;
    else if (inchar=='5')
      keypressed=FIVE;
    else if(inchar=='6')
      keypressed=SIX;
    else if (inchar=='7')
      keypressed=SEVEN;
    else if(inchar=='8')
      keypressed=EIGHT;
    else if (inchar=='9')
      keypressed=NINE;
    else if(inchar=='m')
      keypressed=MENU;
    else if (inchar=='p')
      keypressed=POWER;
    if(DEBUG)
      Serial.println("Key press serial event function exit");
  }
}






