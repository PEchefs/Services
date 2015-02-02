#include "menu.h"
#include "functions.h"
#include "U8glib.h"
#include "gLCD_u8glib.h"

#include "Wire.h"

#define SLAVEADDRESS 2
#define DEBUG 1
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
 
Serial.begin(115200);
pinMode(17,OUTPUT);
  digitalWrite(17,HIGH);
Wire.begin();
Serial.println("End of setup");
// assign default color value
 /* if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  */
}

void assignString(String stringToAssign, menu_st * MenuToAssign, unsigned short index, unsigned short mode)
{
  if(mode == 0)
  {
    stringToAssign.toCharArray(MenuToAssign->heading,stringToAssign.length());
  }
  else
  {
    stringToAssign.toCharArray(MenuToAssign->List[index],stringToAssign.length());
  }
}

void loop()
{
  menu_st Menu[14];
  byte poll[10]={0,0,0,0,0,0,0,0,0,0};
  Serial.println("Beginning Menu assignments");
// picture loop
/*  u8g.firstPage();  
  do {
// graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 22, "Hello World!");
  } while( u8g.nextPage() );
  */
  Menu[0].numOfMenuListItems=4;
  Menu[0].heading="MENU";
  Menu[0].List[0]="Enroll";
  Menu[0].List[1]="Delete";
  Menu[0].List[2]="Set Time";
  Menu[0].List[3]="Settings";
  Menu[0].nextLevelMenuIndex[0]=1;
  Menu[0].nextLevelMenuIndex[1]=255;
  Menu[0].nextLevelMenuIndex[2]=255;
  Menu[0].nextLevelMenuIndex[3]=4;
  Menu[0].functionPointerIndex[0]=255;
  Menu[0].functionPointerIndex[1]=0;
  Menu[0].functionPointerIndex[2]=1;
  Menu[0].functionPointerIndex[3]=255;                               

                            
  Menu[1].numOfMenuListItems=2;
  Menu[1].heading="ENROLL";
  Menu[1].List[0]="Enroll User";
  Menu[1].List[1]="Enroll Admin";
  Menu[1].nextLevelMenuIndex[0]=5;
  Menu[1].nextLevelMenuIndex[1]=6;

  Menu[1].functionPointerIndex[0]=255;
  Menu[1].functionPointerIndex[1]=255;

  Menu[4].numOfMenuListItems=4;
  Menu[4].heading="SETTINGS";
//  Menu[4].List[0]="1.  General";
 // Menu[4].List[1]="2.  Log";
 //Menu[4].List[2]="3.  Communication";
  Menu[4].List[0]="4.  Access";
  Menu[4].List[1]="5.  Information";
  Menu[4].List[2]="6.  Self Test";
  Menu[4].List[3]="6.  Factory Reset";
 // Menu[4].nextLevelMenuIndex[0]=8;
 // Menu[4].nextLevelMenuIndex[1]=9;
 // Menu[4].nextLevelMenuIndex[2]=10;
  Menu[4].nextLevelMenuIndex[0]=11;
  Menu[4].nextLevelMenuIndex[1]=12;
//  Menu[4].nextLevelMenuIndex[3]=13;
  Menu[4].nextLevelMenuIndex[2]=255;
  Menu[4].nextLevelMenuIndex[3]=255;
 // Menu[4].functionPointerIndex[0]=255;
 // Menu[4].functionPointerIndex[1]=255;
 // Menu[4].functionPointerIndex[2]=255;
  Menu[4].functionPointerIndex[0]=255;                               
  Menu[4].functionPointerIndex[1]=255;                               
  Menu[4].functionPointerIndex[2]=255;  
  Menu[4].functionPointerIndex[3]=73;  
////////////////////////////////////////////////////////////////
/*
  Menu[5].numOfMenuListItems=3;
  Menu[5].heading="ENROLL USER";
  Menu[5].List[0]="1.  Fingerprint";
  Menu[5].List[1]="2.  Password";
  Menu[5].List[2]="3.  Card";
  Menu[5].nextLevelMenuIndex[0]=255;
  Menu[5].nextLevelMenuIndex[1]=255;
  Menu[5].nextLevelMenuIndex[2]=255;
  Menu[5].functionPointerIndex[0]=2;
  Menu[5].functionPointerIndex[1]=3;
  Menu[5].functionPointerIndex[2]=4; 
  
  Menu[6].numOfMenuListItems=3;
  Menu[6].heading="ENROLL ADMIN";
  Menu[6].List[0]="1.  Fingerprint";
  Menu[6].List[1]="2.  Password";
  Menu[6].List[2]="3.  Card";
  Menu[6].nextLevelMenuIndex[0]=255;
  Menu[6].nextLevelMenuIndex[1]=255;
  Menu[6].nextLevelMenuIndex[2]=255;
  Menu[6].functionPointerIndex[0]=5;
  Menu[6].functionPointerIndex[1]=6;
  Menu[6].functionPointerIndex[2]=7; 
  
 /* Menu[7].numOfMenuListItems=3;
  Menu[7].heading="ENROLL MANAGER";
  Menu[7].List[0]="1.  Fingerprint";
  Menu[7].List[1]="2.  Password";
  Menu[7].List[2]="3.  Card";
  Menu[7].nextLevelMenuIndex[0]=255;
  Menu[7].nextLevelMenuIndex[1]=255;
  Menu[7].nextLevelMenuIndex[2]=255;
  Menu[7].functionPointerIndex[0]=8;
  Menu[7].functionPointerIndex[1]=9;
  Menu[7].functionPointerIndex[2]=10; 
*/
/*  Menu[8].numOfMenuListItems=11;
  Menu[8].heading="GENERAL";
  Menu[8].List[0]="1.  Language";
  Menu[8].List[1]="2.  Managers";
  Menu[8].List[2]="3.  Verify Mode";
  Menu[8].List[3]="4.  LCD Contrast";
  Menu[8].List[4]="5.  Volume";
  Menu[8].List[5]="6.  Voice";
  Menu[8].List[6]="7.  EventOut";
  Menu[8].List[7]="8.  LockPowKey";
  Menu[8].List[8]="9.  Sleep";
  Menu[8].List[9]="10.  AutoOff";
  Menu[8].List[10]="11.  Adjust Sensor";
  Menu[8].nextLevelMenuIndex[0]=255;
  Menu[8].nextLevelMenuIndex[1]=255;
  Menu[8].nextLevelMenuIndex[2]=255;
  Menu[8].nextLevelMenuIndex[3]=255;
  Menu[8].nextLevelMenuIndex[4]=255;
  Menu[8].nextLevelMenuIndex[5]=255;
  Menu[8].nextLevelMenuIndex[6]=255;
  Menu[8].nextLevelMenuIndex[7]=255;
  Menu[8].nextLevelMenuIndex[8]=255;
  Menu[8].nextLevelMenuIndex[9]=255;
  Menu[8].nextLevelMenuIndex[10]=255;
  Menu[8].functionPointerIndex[0]=18;
  Menu[8].functionPointerIndex[1]=19;
  Menu[8].functionPointerIndex[2]=20;
  Menu[8].functionPointerIndex[3]=21;                               
  Menu[8].functionPointerIndex[4]=22;                               
  Menu[8].functionPointerIndex[5]=23;  
  Menu[8].functionPointerIndex[6]=24;  
  Menu[8].functionPointerIndex[7]=25;  
  Menu[8].functionPointerIndex[8]=26;  
  Menu[8].functionPointerIndex[9]=27;
  Menu[8].functionPointerIndex[10]=28;  
 */ 
 /* Menu[9].numOfMenuListItems=3;
  Menu[9].heading="LOG";
  Menu[9].List[0]="1.  Log Alarm";
  Menu[9].List[1]="2.  SLog Alarm";
  Menu[9].List[2]="3.  Re-verify";
  Menu[9].nextLevelMenuIndex[0]=255;
  Menu[9].nextLevelMenuIndex[1]=255;
  Menu[9].nextLevelMenuIndex[2]=255;
  Menu[9].functionPointerIndex[0]=29;
  Menu[9].functionPointerIndex[1]=30;
  Menu[9].functionPointerIndex[2]=31; 
*/
/*  Menu[10].numOfMenuListItems=9;
  Menu[10].heading="COMMUNICATION";
  Menu[10].List[0]="1.  Machine ID";
  Menu[10].List[1]="2.  Password";
  Menu[10].List[2]="3.  Use DHCP";
  Menu[10].List[3]="4.  IP Address";
  Menu[10].List[4]="5.  Subnet Mask";
  Menu[10].List[5]="6.  Gateway";
  Menu[10].List[6]="7.  TCP Port";
  Menu[10].List[7]="8.  PC IP Addr";
  Menu[10].List[8]="9.  PC Port";
  Menu[10].nextLevelMenuIndex[0]=255;
  Menu[10].nextLevelMenuIndex[1]=255;
  Menu[10].nextLevelMenuIndex[2]=255;
  Menu[10].nextLevelMenuIndex[3]=255;
  Menu[10].nextLevelMenuIndex[4]=255;
  Menu[10].nextLevelMenuIndex[5]=255;
  Menu[10].nextLevelMenuIndex[6]=255;
  Menu[10].nextLevelMenuIndex[7]=255;
  Menu[10].nextLevelMenuIndex[8]=255;
  Menu[10].functionPointerIndex[0]=32;
  Menu[10].functionPointerIndex[1]=33;
  Menu[10].functionPointerIndex[2]=34;
  Menu[10].functionPointerIndex[3]=35;                               
  Menu[10].functionPointerIndex[4]=36;                               
  Menu[10].functionPointerIndex[5]=37;  
  Menu[10].functionPointerIndex[6]=38;  
  Menu[10].functionPointerIndex[7]=39;  
  Menu[10].functionPointerIndex[8]=40;  
*/

////////////////////////////////////////////////////////////////
/*
  Menu[11].numOfMenuListItems=6;
  Menu[11].heading="ACCESS";
  Menu[11].List[0]="1.  Attend. Set";
 // Menu[11].List[1]="2.  Attendance TZ";
  Menu[11].List[1]="3.  Open Time";
  Menu[11].List[2]="4.  Open TimeOut";
  Menu[11].List[3]="5.  Door Sensor";
  Menu[11].List[4]="6.  Bell Time";
 // Menu[11].List[6]="6.  TimeZone";
 // Menu[11].List[7]="6.  User Timezone";
 // Menu[11].List[8]="6.  Holiday";
  Menu[11].List[5]="6.  Verify Mode";
//  Menu[11].List[10]="6.  Anti-Pass";
//////  Menu[11].List[11]="6.  Set DuressFP";
  Menu[11].nextLevelMenuIndex[0]=255;
  Menu[11].nextLevelMenuIndex[1]=255;
  Menu[11].nextLevelMenuIndex[2]=255;
  Menu[11].nextLevelMenuIndex[3]=255;
  Menu[11].nextLevelMenuIndex[4]=255;
  Menu[11].nextLevelMenuIndex[5]=255;
//  Menu[11].nextLevelMenuIndex[6]=255;
//  Menu[11].nextLevelMenuIndex[7]=255;
//  Menu[11].nextLevelMenuIndex[8]=255;
//  Menu[11].nextLevelMenuIndex[9]=255;
//  Menu[11].nextLevelMenuIndex[10]=255;
//  Menu[11].nextLevelMenuIndex[11]=255;
  Menu[11].functionPointerIndex[0]=41;
//  Menu[11].functionPointerIndex[1]=42;
  Menu[11].functionPointerIndex[1]=43;
  Menu[11].functionPointerIndex[2]=44;                               
  Menu[11].functionPointerIndex[3]=45;                               
  Menu[11].functionPointerIndex[4]=46;  
//  Menu[11].functionPointerIndex[6]=47;  
//  Menu[11].functionPointerIndex[7]=48;  
//  Menu[11].functionPointerIndex[8]=49;  
  Menu[11].functionPointerIndex[5]=50;  
//  Menu[11].functionPointerIndex[10]=51;  
//  Menu[11].functionPointerIndex[11]=52;  

  Menu[12].numOfMenuListItems=7;
  Menu[12].heading="INFORMATION";
  Menu[12].List[0]="1.  User";
  Menu[12].List[1]="2.  Manager";
  Menu[12].List[2]="3.  Finger";
  Menu[12].List[3]="4.  DuressFP";
  Menu[12].List[4]="5.  Password";
  Menu[12].List[5]="6.  Card";
  Menu[12].List[6]="7.  Log";
 // Menu[12].List[7]="8.  SLog";
 // Menu[12].List[8]="9.  Communication";
 // Menu[12].List[9]="10.  Manufacturer";
 // Menu[12].List[10]="11.  F/W Version";
  Menu[12].nextLevelMenuIndex[0]=255;
  Menu[12].nextLevelMenuIndex[1]=255;
  Menu[12].nextLevelMenuIndex[2]=255;
  Menu[12].nextLevelMenuIndex[3]=255;
  Menu[12].nextLevelMenuIndex[4]=255;
  Menu[12].nextLevelMenuIndex[5]=255;
  Menu[12].nextLevelMenuIndex[6]=255;
  Menu[12].nextLevelMenuIndex[7]=255;
//  Menu[12].nextLevelMenuIndex[8]=255;
//  Menu[12].nextLevelMenuIndex[9]=255;
//  Menu[12].nextLevelMenuIndex[10]=255;
//  Menu[12].nextLevelMenuIndex[11]=255;
  Menu[12].functionPointerIndex[0]=53;
  Menu[12].functionPointerIndex[1]=54;
  Menu[12].functionPointerIndex[2]=55;
  Menu[12].functionPointerIndex[3]=56;                               
  Menu[12].functionPointerIndex[4]=57;                               
  Menu[12].functionPointerIndex[5]=58;  
  Menu[12].functionPointerIndex[6]=59;  
//  Menu[12].functionPointerIndex[7]=60;  
//  Menu[12].functionPointerIndex[8]=61;  
//  Menu[12].functionPointerIndex[9]=62;  
//  Menu[12].functionPointerIndex[10]=63;  
////////////////////////////////////////////////////////////
*/

 /* Menu[13].numOfMenuListItems=9;
  Menu[13].heading="SELF-TEST";
  Menu[13].List[0]="1.  Keyboard";
  Menu[13].List[1]="2.  Sound";
  Menu[13].List[2]="3.  Sensor";
  Menu[13].List[3]="4.  Flash";
  Menu[13].List[4]="5.  LCD";
  Menu[13].List[5]="6.  Lock";
  Menu[13].List[6]="7.  Bell";
  Menu[13].List[7]="8.  WiegandOut";
  Menu[13].List[8]="9.  WiegandIn";
  Menu[13].nextLevelMenuIndex[0]=255;
  Menu[13].nextLevelMenuIndex[1]=255;
  Menu[13].nextLevelMenuIndex[2]=255;
  Menu[13].nextLevelMenuIndex[3]=255;
  Menu[13].nextLevelMenuIndex[4]=255;
  Menu[13].nextLevelMenuIndex[5]=255;
  Menu[13].nextLevelMenuIndex[6]=255;
  Menu[13].nextLevelMenuIndex[7]=255;
  Menu[13].nextLevelMenuIndex[8]=255;
  Menu[13].functionPointerIndex[0]=64;
  Menu[13].functionPointerIndex[1]=65;
  Menu[13].functionPointerIndex[2]=66;
  Menu[13].functionPointerIndex[3]=67;                               
  Menu[13].functionPointerIndex[4]=68;                               
  Menu[13].functionPointerIndex[5]=69;  
  Menu[13].functionPointerIndex[6]=70;  
  Menu[13].functionPointerIndex[7]=71;  
  Menu[13].functionPointerIndex[8]=72;  
*/
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
                        case ENTER:   //  enterMenuItem(Menu[currentMenu]);
                                        break;
                        case ESC:       escMenuList();
                                        break;
                      }
                      if(DEBUG)
                        Serial.println("Calling Display Menu Function");
                     displayMenu(Menu[currentMenu]);
                     // displayMenu();
                    }
                 //   displayMenu(Menu[currentMenu]);
                   // displayMenu();
                    break;

    case FPAUTH:break;
    
    case PWDAUTH:break;
    
    case RFIDAUTH:break;
    
    case MENUFUNCTION:break;
  
  }
  


}








void serialEvent() {
 

   while (Serial.available()) {
      if(DEBUG)
         Serial.println("Key pressed!");
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
//void displayMenu()
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



