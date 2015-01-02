#include "menu.h"
#include "functions.h"
#include "U8glib.h"
#include "gLCD_u8glib.h"
#include "Keypad.h"

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

void updateState(state stateToUpdateTo)
{
  previousState=currentState;
  currentState=stateToUpdateTo;
}



void enterMenuItem()
{
   if(Menu[currentMenu].nextLevelMenuIndex[currentMenuIndex] != 255)
   {
     //next level submenu exists
     prevMenu=currentMenu;
     currentMenu=Menu[currentMenu].nextLevelMenuIndex[currentMenuIndex];
   
     prevDisplayStartIndex=displayStartIndex;
     displayStartIndex=0;

     prevMenuIndex=currentMenuIndex;
     currentMenuIndex=0;
   }
   else
   {
    //next level submenu doesn't exist. Call, function corresponding to the menu item selected
    MenuFunctionToCallIndex=Menu[currentMenu].functionPointerIndex[currentMenuIndex];
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

}

void loop()
{
  switch (currentState)
  {
    case HOMESCREEN:break;
    
    case MENUSCREEN:if(isKeyPressed())
                    {
                      switch (keypressed)
                      {
                        case SCROLLUP:  scrollup();
                                        break;
                        case SCROLLDOWN:scrolldown();
                                        break;
                        case ENTER:     enterMenuItem();
                                        break;
                        case ESC:       escMenuList();
                                        break;
                      }
                    }
                    displayMenu();
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



