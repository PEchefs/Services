#include <functions.h>
#include <Ext_EEPROM.h>
#include <Menu.h>
#include <gLCD_u8glib.h>
#include <U8glib.h>
#include <Time.h>
#include <DS1307RTC.h>
//#include <TimerOne.h>
//#include <TimerThree.h>
#include <Wire.h>
#include <stdlib.h>

#define DEBUG 1
#define DEBUGWOWIRE 0

extern unsigned short currentMenu;
extern state previousState;
extern state currentState;
extern state newState;
extern menu_un Menu;
extern unsigned short MenuFunctionToCallIndex;
extern unsigned int serialInputNumber;

key keypressed=NOKEY;
boolean keyPressDetected = false;
byte x= 0;
boolean timeUpdateRequired=false;
unsigned long timeUpdateCheck1=0,timeUpdateCheck2=0;
  
tmElements_t currentTime;
boolean RTCreadError=false;

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

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void updateTime()
{
                        if (RTC.read(currentTime))
                        {
                          print2digits(currentTime.Hour);
                          Serial.write(':');
                          print2digits(currentTime.Minute);
                          Serial.write(':');
                          print2digits(currentTime.Second);
                          Serial.println("");
                        }
                        else
                        {
                         Serial.println("RTC read error");
                         RTCreadError=true;
                        }
                       // timeUpdateRequired=false;
                     
}

void setup() {
  
  Serial.begin(115200);
  pinMode(17,OUTPUT);
  digitalWrite(17,HIGH);
  if(!DEBUGWOWIRE)
  {
   i2c_eeprom_init();
  }
 // Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
 // Timer1.attachInterrupt( updateTime ); // attach the service routine here
  Serial.println("End of setup");
  //initializeMenus();
  getMenu(0);
}


void loop()
{
   

  //Serial.println("Beginning Menu assignments");
 
  //Serial.println("Ending Menu Assignments");
  switch (currentState)
  {
    case HOMESCREEN:Serial.println("Current State: HOMESCREEN");
                    //Serial.print("Time:  ");
                    displayHomeScreen(currentTime);

                     poll();
                    delay(900);
                      if(isKeyPressed())
                      {
                        updateState(MENUSCREEN);
                      } 
                    break;
    
    case MENUSCREEN:if(isKeyPressed())
                    {
                      switch (keypressed)
                      {
                        case SCROLLUP:  scrollup();
                                        break;
                        case SCROLLDOWN:scrolldown();
                                        break;
                        case ENTER:     //getMenu(currentMenu);
                                        enterMenuItem(Menu.menu_struct);
                                        break;
                        case ESC:       escMenuList();
                                        break;
                      }
                      if(DEBUG)
                        Serial.println("Calling Display Menu Function");
                    
                    
                     // displayMenu();
                    }
                     getMenu(currentMenu);
                     displayMenu(Menu.menu_struct);
                    break;

    case FPAUTH:break;
    
    case PWDAUTH:break;
    
    case RFIDAUTH:break;
    
    case MENUFUNCTION: 
                      switch(MenuFunctionToCallIndex)
                      {
                        case 0:deleteUser();
                               break;
                        case 1:setTimeFunction();
                               break;
                        case 2://Enroll User Fingerprint 
                               enroll(0,0);
                               updateState(MENUSCREEN);
                               break;
                        case 3://Enroll User Password 
                               enroll(0,1);
                               updateState(MENUSCREEN);
                               break;
                        case 4://Enroll User RFID Card 
                               enroll(0,2);
                               break;
                        case 5://Enroll Admin Fingerprint 
                               enroll(1,0);
                               break;
                        case 6://Enroll Admin Password 
                               enroll(1,1);
                               break;
                        case 7://Enroll Admin RFID Card 
                               enroll(1,2);
                               break;
                        case 8:// Self Test
                               selfTest();
                               break;
                        case 9:// Factory Reset
                               factoryReset();
                               break;
                        case 10:// Set Attendance Mode
                                setAttendance();
                                break;
                        case 11:// set Door open Time
                                setOpenTime();
                                break;
                        case 12:setOpenTimeOut();
                                break;
                        case 13:setDoorSensor();
                                break;
                        case 14:setBellTime();
                                break;
                        case 15:setVerifyMode();
                                break;
                        case 16:infoUser();
                                break;
                        case 17:infoManager();
                                break;
                        case 18:infoFinger();
                                break;
                        case 19:infoDuressFP();
                                break;
                        case 20:infoPassword();
                                break;
                        case 21:infoCard();
                                break;
                        case 22:infoLog();
                                break;
                        
                        
                        
                      }
                      
                      break;
    
    default : break;
  
  }
  

  if(timeUpdateRequired==false)
    timeUpdateCheck2=millis();
  else
    {
      timeUpdateCheck1=millis();
      timeUpdateCheck2=millis();
      timeUpdateRequired=false;
    }
    if(timeUpdateCheck2-timeUpdateCheck1>10000)
      {
        timeUpdateRequired=true;
        updateTime();
      }
      

}
void serialEvent() {
  
  //  if(currentState!=MENUFUNCTION)
  //{  
     while (Serial.available()>0) 
     {
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
     //   if(DEBUG)
     //     Serial.println("Key press serial event function exit");
     }
  //}
 /* else
  {
    unsigned short count=0;
    char inchar[10]="";
    while (Serial.available()>0) 
    {
        inchar[count]=char(Serial.read());
        count++;        
    }
    serialInputNumber=strtoul(inchar,NULL,0);
  }*/  
}






