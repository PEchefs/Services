#include <Ext_EEPROM.h>
#include <Functions.h>
#include <gLCD_u8glib.h>
#include <Menu.h>
#include <Wire.h>
#include <U8glib.h>

extern menu_un Menu;
void setup() {
i2c_eeprom_init();
Serial.begin(9600);
Serial.println("Ready to upload Menu");

}
void string_to_array(char* array,char* str)
{
  for(int i=0;i<sizeof(str);i++)
    array[i]=str[i];
}
void loop() {
  Menu.menu_struct.numOfMenuListItems=4;
  string_to_array(Menu.menu_struct.heading,"MENU");
  string_to_array(Menu.menu_struct.List[0],"1.  Enroll");
  string_to_array(Menu.menu_struct.List[1],"2.  Delete");
  string_to_array(Menu.menu_struct.List[2],"3.  Set Time");
  string_to_array(Menu.menu_struct.List[3],"4.  Settings");
  Menu.menu_struct.nextLevelMenuIndex[0]=1;
  Menu.menu_struct.nextLevelMenuIndex[1]=255;
  Menu.menu_struct.nextLevelMenuIndex[2]=255;
  Menu.menu_struct.nextLevelMenuIndex[3]=2;
  Menu.menu_struct.functionPointerIndex[0]=255;
  Menu.menu_struct.functionPointerIndex[1]=0;
  Menu.menu_struct.functionPointerIndex[2]=1;
  Menu.menu_struct.functionPointerIndex[3]=255;                               
  setMenu(0);                  
  Menu.menu_struct.numOfMenuListItems=2;
  string_to_array(Menu.menu_struct.heading,"ENROLL");
  string_to_array(Menu.menu_struct.List[0],"1.   Enroll User");
  string_to_array(Menu.menu_struct.List[1],"2.   Enroll Admin");
  Menu.menu_struct.nextLevelMenuIndex[0]=3;
  Menu.menu_struct.nextLevelMenuIndex[1]=4;
  Menu.menu_struct.functionPointerIndex[0]=255;
  Menu.menu_struct.functionPointerIndex[1]=255;
  setMenu(1);

  Menu.menu_struct.numOfMenuListItems=4;
  string_to_array(Menu.menu_struct.heading,"SETTINGS");
  string_to_array(Menu.menu_struct.List[0],"1.  Access");
  string_to_array(Menu.menu_struct.List[1],"2.  Information");
  string_to_array(Menu.menu_struct.List[2],"3.  Self Test");
  string_to_array(Menu.menu_struct.List[3],"4.  Factory Reset");
  Menu.menu_struct.nextLevelMenuIndex[0]=5;
  Menu.menu_struct.nextLevelMenuIndex[1]=6;
  Menu.menu_struct.nextLevelMenuIndex[2]=255;
  Menu.menu_struct.nextLevelMenuIndex[3]=255;
  Menu.menu_struct.functionPointerIndex[0]=255;                               
  Menu.menu_struct.functionPointerIndex[1]=255;                               
  Menu.menu_struct.functionPointerIndex[2]=8;  
  Menu.menu_struct.functionPointerIndex[3]=9;  
  setMenu(2);

  Menu.menu_struct.numOfMenuListItems=3;
  string_to_array(Menu.menu_struct.heading,"ENROLL USER");
  string_to_array(Menu.menu_struct.List[0],"1.  Fingerprint");
  string_to_array(Menu.menu_struct.List[1],"2.  Password");
  string_to_array(Menu.menu_struct.List[2],"3.  Card");
  Menu.menu_struct.nextLevelMenuIndex[0]=255;
  Menu.menu_struct.nextLevelMenuIndex[1]=255;
  Menu.menu_struct.nextLevelMenuIndex[2]=255;
  Menu.menu_struct.functionPointerIndex[0]=2;
  Menu.menu_struct.functionPointerIndex[1]=3;
  Menu.menu_struct.functionPointerIndex[2]=4; 
  setMenu(3);

  Menu.menu_struct.numOfMenuListItems=3;
  string_to_array(Menu.menu_struct.heading,"ENROLL ADMIN");
  string_to_array(Menu.menu_struct.List[0],"1.  Fingerprint");
  string_to_array(Menu.menu_struct.List[1],"2.  Password");
  string_to_array(Menu.menu_struct.List[2],"3.  Card");
  Menu.menu_struct.nextLevelMenuIndex[0]=255;
  Menu.menu_struct.nextLevelMenuIndex[1]=255;
  Menu.menu_struct.nextLevelMenuIndex[2]=255;
  Menu.menu_struct.functionPointerIndex[0]=5;
  Menu.menu_struct.functionPointerIndex[1]=6;
  Menu.menu_struct.functionPointerIndex[2]=7; 
  setMenu(4);

  Menu.menu_struct.numOfMenuListItems=6;
  string_to_array(Menu.menu_struct.heading,"ACCESS");
  string_to_array(Menu.menu_struct.List[0],"1.  Attend. Set");
  string_to_array(Menu.menu_struct.List[1],"2.  Open Time");
  string_to_array(Menu.menu_struct.List[2],"3.  Open TimeOut");
  string_to_array(Menu.menu_struct.List[3],"4.  Door Sensor");
  string_to_array(Menu.menu_struct.List[4],"5.  Bell Time");
  string_to_array(Menu.menu_struct.List[5],"6.  Verify Mode");
//////  Menu[11].List[11]="6.  Set DuressFP";
  Menu.menu_struct.nextLevelMenuIndex[0]=255;
  Menu.menu_struct.nextLevelMenuIndex[1]=255;
  Menu.menu_struct.nextLevelMenuIndex[2]=255;
  Menu.menu_struct.nextLevelMenuIndex[3]=255;
  Menu.menu_struct.nextLevelMenuIndex[4]=255;
  Menu.menu_struct.nextLevelMenuIndex[5]=255;
  Menu.menu_struct.functionPointerIndex[0]=10;
  Menu.menu_struct.functionPointerIndex[1]=11;
  Menu.menu_struct.functionPointerIndex[2]=12;                               
  Menu.menu_struct.functionPointerIndex[3]=13;                               
  Menu.menu_struct.functionPointerIndex[4]=14;  
  Menu.menu_struct.functionPointerIndex[5]=15;  
  setMenu(5);

  Menu.menu_struct.numOfMenuListItems=7;
  string_to_array(Menu.menu_struct.heading,"INFORMATION");
  string_to_array(Menu.menu_struct.List[0],"1.  User");
  string_to_array(Menu.menu_struct.List[1],"2.  Manager");
  string_to_array(Menu.menu_struct.List[2],"3.  Finger");
  string_to_array(Menu.menu_struct.List[3],"4.  DuressFP");
  string_to_array(Menu.menu_struct.List[4],"5.  Password");
  string_to_array(Menu.menu_struct.List[5],"6.  Card");
  string_to_array(Menu.menu_struct.List[6],"7.  Log");
  Menu.menu_struct.nextLevelMenuIndex[0]=255;
  Menu.menu_struct.nextLevelMenuIndex[1]=255;
  Menu.menu_struct.nextLevelMenuIndex[2]=255;
  Menu.menu_struct.nextLevelMenuIndex[3]=255;
  Menu.menu_struct.nextLevelMenuIndex[4]=255;
  Menu.menu_struct.nextLevelMenuIndex[5]=255;
  Menu.menu_struct.nextLevelMenuIndex[6]=255;
  Menu.menu_struct.nextLevelMenuIndex[7]=255;
  Menu.menu_struct.functionPointerIndex[0]=16;
  Menu.menu_struct.functionPointerIndex[1]=17;
  Menu.menu_struct.functionPointerIndex[2]=18;
  Menu.menu_struct.functionPointerIndex[3]=19;                               
  Menu.menu_struct.functionPointerIndex[4]=20;                               
  Menu.menu_struct.functionPointerIndex[5]=21;  
  Menu.menu_struct.functionPointerIndex[6]=22; 
  setMenu(6);
}
