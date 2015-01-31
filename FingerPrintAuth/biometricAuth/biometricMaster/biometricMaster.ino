
#include "menu.h"
#include "functions.h"
#include "U8glib.h"
#include "gLCD_u8glib.h"
#include "Keypad.h"
#include <Wire.h>

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

byte x = 0;

void writeToSalve()
{
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
}

void readFromSlave()
{
  Wire.requestFrom(2, 10);    // request 6 bytes from slave device #2

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
 
Serial.begin(115200);
Wire.begin(); // join i2c bus (address optional for master)

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
