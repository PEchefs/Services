#include "menu.h"
#include "functions.h"
#include "gLCD_u8glib.h"

void serialEvent() {

/*  char  inchar[2];
  while (Serial.available()) {
    if(j==0)
      {
      
      inchar[0]=char(Serial.read());
      inchar[1]=0;
        displayStartIndex=atoi(inchar);
        Serial.print("j=");Serial.println(j);
        j=1;
       Serial.print("displayStartIndex = ");Serial.println(displayStartIndex);
       Serial.print("j=");Serial.println(j);
      }
    else if (j==1)
      {
        inchar[0]=char(Serial.read());
        inchar[1]=0;
        menu_current=atoi(inchar);
        Serial.print("j=");Serial.println(j);
        j=0;
        Serial.print("menu_current = ");Serial.println(menu_current);
        Serial.print("j=");Serial.println(j);
      }
    
  }
  */
  while (Serial.available()) {
    char inchar=char(Serial.read());
    if (inchar == 'd')
      scrolldown();
    else if(inchar=='u')
      scrollup();
  }
}

void scrolldown()
{
 menu_current++; 
}

void scrollup()
{
 menu_current--; 
}

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

}

void loop()
{
displayLCD();  
}

