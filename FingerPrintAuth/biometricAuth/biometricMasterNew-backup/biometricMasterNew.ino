#include "menu.h"
#include "functions.h"
#include "U8glib.h"
#include "gLCD_u8glib.h"

#include "Wire.h"

#define SLAVEADDRESS 2


/*
enum state {HOMESCREEN,MENUSCREEN,FPAUTH,PWDAUTH,RFIDAUTH,MENUFUNCTION,NONE};
enum key {POWER,SCROLLDOWN,SCROLLUP,ENTER,ESC,MENU,ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,NOKEY};
state previousState=NONE;
state currentState=MENUSCREEN;
state newState=NONE;
*/

/* temporary*/
key keypressed=NOKEY;
boolean keyPressDetected = false;
byte x= 0;
void writeToSlave(byte sendCommand[] )
{
  Serial.println("Sending command to Slave:");
  
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
  Serial.println("Received Data:");
  while (Wire.available())   // slave may send less than requested
  {
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
  
  delay(500);
}

void updateState(state stateToUpdateTo)
{
  previousState=currentState;
  currentState=stateToUpdateTo;
}



void enterMenuItem(menu_st currentMenuList)
{
   if(currentMenuList.nextLevelMenuIndex[currentMenuIndex] != 255)
   {
     //next level submenu exists
     prevMenu=currentMenu;
     currentMenu=currentMenuList.nextLevelMenuIndex[currentMenuIndex];
   
     prevDisplayStartIndex=displayStartIndex;
     displayStartIndex=0;

     prevMenuIndex=currentMenuIndex;
     currentMenuIndex=0;
   }
   else
   {
    //next level submenu doesn't exist. Call, function corresponding to the menu item selected
    MenuFunctionToCallIndex=currentMenuList.functionPointerIndex[currentMenuIndex];
    newState=MENUFUNCTION;
    updateState(newState);
   }
}

void escMenuList()
{
   currentMenu=prevMenu;
   prevMenu=0;
   displayStartIndex=prevDisplayStartIndex;
   prevDisplayStartIndex=0;
   currentMenuIndex=prevMenuIndex;
   prevMenuIndex=0;
}

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

void setup() {
 
Serial.begin(9600);
Wire.begin();
Serial.println("End of setup");
}

void loop()
{
  byte poll[10]={0,0,0,0,0,0,0,0,0,0};
  Serial.println("Beginning Menu assignments");
menu_st Menu[14];
Menu[0]={6,"MENU",{ 
                                  "1.  Enroll",
                                  "2.  Delete", 
                                  "3.  Set Time", 
                                  "4.  View Log", 
                                  "5.  USB Disk", 
                                  "6.  Settings"
                               },
                             {1,255,255,2,3,4},
                             {255,0,1,255,255,255}};
                               

//menu_st *ptr;

//1  -  Main -> Enroll Menu


Menu[1]={4,"ENROLL",{
                                  "1.  Enroll User",
                                  "2.  Enroll Admin",
                                  "3.  Enroll Manager",
                                  "4.  Time Period"
                                 },
                                 {5,6,7,255},
                                 {255,255,255,11}};

//2  -  Main -> View Log Menu

                                 
Menu[2] = {3,"LOG",{
                                  "1.  View GLog",
                                  "2.  View SLog",
                                  "3.  Delete All Logs"
                                  },
                                  {255,255,255},
                                  {74,75,76}
                                };
                                  
//3  -  Main -> USB Disk Menu

Menu[3] = {5,"USB DISK",{
                                  "1.  Download GLog",
                                  "2.  Download SLog",
                                  "3.  Download DB",
                                  "4.  Upload DB",
                                  "5.  Upgrade F/W"
                                 },
                                 {255,255,255,255,255},
                                 {12,13,14,15,16}
                               };
                                 
//4  -  Main -> Settings Menu
                                 
Menu[4] = {7,"SETTINGS",{
                                  "1.  General",
                                  "2.  Log",
                                  "3.  Communication",
                                  "4.  Access",
                                  "5.  Information",
                                  "6.  Self Test",
                                  "7.  Factory Reset"
                                 },
                                 {8,9,10,11,12,13,255},
                                 {255,255,255,255,255,73}
                               };
                                 
//5  -  Main -> Enroll -> Enroll User Menu
                                 
                                 
Menu[5] = {3,"ENROLL USER",{
                                     "1.  Fingerprint",
                                     "2.  Password",
                                     "3.  Card"
                                   },
                                   {255,255,255},
                                   {2,3,4}
                                 };  

//6  -  Main -> Enroll -> Enroll Admin Menu

                                   
Menu[6] = {3,"ENROLL ADMIN",{
                                     "1.  Fingerprint",
                                     "2.  Password",
                                     "3.  Card"
                                   },
                                   {255,255,255},
                                   {5,6,7}
                                 };
                                   
//7  -  Main -> Enroll -> Enroll Manager Menu
                                   
Menu[7] = {3,"ENROLL MANAGER",{
                                     "1.  Fingerprint",
                                     "2.  Password",
                                     "3.  Card"
                                   },
                                   {255,255,255},
                                   {8,9,10}
                                 };                                   


//8  -  Main -> Settings -> General

Menu[8] = {11,"GENERAL",{
                                   "1.  Language",
                                   "2.  Managers",
                                   "3.  Verify Mode",
                                   "4.  LCD Contrast",
                                   "5.  Volume",
                                   "6.  Voice",
                                   "7.  EventOut",
                                   "8.  LockPowKey",
                                   "9.  Sleep",
                                   "10. AutoOFF",
                                   "11. Adjust Sensor"
                                  },
                                  {255,255,255,255,255,255,255,255,255,255,255},
                                  {18,19,20,21,22,23,24,25,26,27,28}
                                };

// 9  -  Main -> Settings -> Log

                                  
Menu[9] = {3,"LOG",{
                                   "1.  GLog Alarm",
                                   "2.  SLog Alarm",
                                   "3.  Re-verify"
                              },
                              {255,255,255},
                              {29,30,31}};

//10 -  Main -> Settings -> Communication
                              
Menu[10] = {9,"COMMUNICATION", {
                                   "1.  Machine ID",
                                   "2.  Password",
                                   "3.  Use DHCP",
                                   "4.  IP Address",
                                   "5.  Subnet Mask",
                                   "6.  Gateway",
                                   "7.  TCP Port",
                                   "8.  PC IP Addr",
                                   "9.  PC Port"
                                  },
                                  {255,255,255,255,255,255,255,255,255},
                                  {32,33,34,35,36,37,38,39,40}
                                };

//11 -  Main -> Settings -> Access 
                                   
Menu[11] = {12,"ACCESS",{
                                   "1.  Attend. Set",
                                   "2.  Attendance TZ",
                                   "3.  Open Time",
                                   "4.  Open TimeOut",
                                   "5.  Door Sensor",
                                   "6.  Bell Time",
                                   "7.  Timezone",
                                   "8.  User Timezone",
                                   "9.  Holiday",
                                   "10. VerifyMode TZ",
                                   "11. Anti-Pass",
                                   "12. Set DuressFP"
                                  },
                                  {255,255,255,255,255,255,255,255,255,255,255,255},
                                  {41,42,43,44,45,46,47,48,49,50,51,52}
                                };

//12 -  Main -> Settings -> Information
                                  
Menu[12] = {11,"INFORMATION",{
                                   "1.  User",
                                   "2.  Manager",
                                   "3.  Finger",
                                   "4.  DuressFP",
                                   "5.  Password",
                                   "6.  Card",
                                   "7.  GLog",
                                   "8.  SLog",
                                   "9.  Communication",
                                   "10. Manufacturer",
                                   "11. F/W Version"
                                  },
                                  {255,255,255,255,255,255,255,255,255,255,255},
                                  {53,54,55,56,57,58,59,60,61,62,63}
                                };          
                                  
//13 -  Main -> Settings -> Self-Test
                                  
                                  
Menu[13] = {9,"SELF-TEST",{
                                   "1.  Keyboard",
                                   "2.  Sound",
                                   "3.  Sensor",
                                   "4.  Flash",
                                   "5.  LCD",
                                   "6.  Lock",
                                   "7.  Bell",
                                   "8.  WiegandOut",
                                   "9.  WiegandIn"
                                  },
                                  {255,255,255,255,255,255,255,255,255},
                                  {64,65,66,67,68,69,70,71,72}
                                }; 
  Serial.println("Ending Menu Assignments");
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
                    }
                    displayMenu(Menu[currentMenu]);
                    break;

    case FPAUTH:break;
    
    case PWDAUTH:break;
    
    case RFIDAUTH:break;
    
    case MENUFUNCTION:break;
  
  }


}








void serialEvent() {
   while (Serial.available()) {
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
      
  }
}

void drawMenu(menu_st menuToDisplay) {
	uint8_t i,count, h;
	u8g_uint_t w, d;

	//  u8g.setFont(u8g_font_7x13B);
	u8g.setFont(u8g_font_7x14);
	u8g.setFontRefHeightText();
	u8g.setFontPosTop();
	h = u8g.getFontAscent()-u8g.getFontDescent();

	w = u8g.getWidth();
        
        // Heading for Menu
        unsigned short mid = (w-u8g.getStrWidth(menuToDisplay.heading))/2;
//        u8g.drawStr(mid, 0*h+1, menuToDisplay.heading);
  /*      if (currentMenuIndex == 2)
          return; */
	if (( menuToDisplay.numOfMenuListItems - displayStartIndex ) <= MAX_NUM_DISPLAY_LINES )
	{
		if(currentMenuIndex>=menuToDisplay.numOfMenuListItems)
		{
			currentMenuIndex=0;
			displayStartIndex=0;
		}
		else if(currentMenuIndex<0)
		{
			currentMenuIndex=menuToDisplay.numOfMenuListItems-1;
			displayStartIndex=0;
		}
		if(currentMenuIndex < displayStartIndex)
		{
			displayStartIndex--;
		}


        
		//loop(from displayStartIndex to numOfMenuListItems)                
		for( i = displayStartIndex,count=1; i < menuToDisplay.numOfMenuListItems; i++,count++ ) 
                {
			d=0;
			u8g.setDefaultForegroundColor();
                        // Display Heading
                        u8g.drawStr(mid, 0*h+1, menuToDisplay.heading);
			if ( i == currentMenuIndex ) 
                        {
				u8g.drawBox(0, count*h+1, w, h);
				//    u8g.drawBox(0, i*h+1, w, h+2);
				u8g.setDefaultBackgroundColor();
			}
			//   u8g.setFontPosBaseline();
			u8g.drawStr(d, count*h+1, menuToDisplay.List[i]);
		}
	}
	else
	{
		if(DEBUG)
		{ 
			Serial.print("currentMenuIndex=");Serial.println(currentMenuIndex);
			Serial.print("displayStartIndex=");Serial.println(displayStartIndex);
		}
		if(currentMenuIndex>=displayStartIndex+MAX_NUM_DISPLAY_LINES)
		{
			displayStartIndex++;
		}
		else if(currentMenuIndex<0)
		{
			currentMenuIndex=menuToDisplay.numOfMenuListItems-1;
			displayStartIndex=menuToDisplay.numOfMenuListItems-MAX_NUM_DISPLAY_LINES;
		}
		if(currentMenuIndex < displayStartIndex)
		{
			displayStartIndex--;
		}
		//loop(from displayStartIndex to displayStartIndex+maxNumOfDisplayLines-1)

		for( i = displayStartIndex,count=1; i < displayStartIndex+MAX_NUM_DISPLAY_LINES; i++,count++ ) 
                {
			d=0;
			u8g.setDefaultForegroundColor();
                        //Display Heading
                        u8g.drawStr(mid, 0*h+1, menuToDisplay.heading);
			if ( i == currentMenuIndex ) {
				u8g.drawBox(0, count*h+1, w, h);
				//    u8g.drawBox(0, i*h+1, w, h+2);
				u8g.setDefaultBackgroundColor();
			}
			//   u8g.setFontPosBaseline();
			u8g.drawStr(d, count*h+1, menuToDisplay.List[i]);
		}
	}




	if(DEBUG)
	{ 
		Serial.print("currentMenuIndex=");Serial.println(currentMenuIndex);
		Serial.print("displayStartIndex=");Serial.println(displayStartIndex);
	}
}





void displayMenu(menu_st MenuToDisplay)
{
  
	// put your main code here, to run repeatedly:
	//initializeMenus();
	u8g.firstPage();  
	do {

		drawMenu(MenuToDisplay);
	} while( u8g.nextPage() );

}

void scrolldown()
{
	currentMenuIndex++; 
}

void scrollup()
{
	currentMenuIndex--; 
}



