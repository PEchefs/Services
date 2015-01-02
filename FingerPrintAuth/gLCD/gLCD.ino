#include <openGLCD.h>

//unsigned char Menu[]
String Main_MenuList[] = { 
                                  "1.  Enroll",
                                  "2.  Delete", 
                                  "3.  Set Time", 
                                  "4.  View Log", 
                                  "5.  USB Disk", 
                                  "6.  Settings"
                               };
  
String Enroll_MenuList[] = {
                                  "1.  Enroll User",
                                  "2.  Enroll Admin",
                                  "3.  Enroll Manager",
                                  "4.  Time Period"
                                 };
                                 
String ViewLog_MenuList[] = {
                                  "1.  View GLog",
                                  "2.  View SLog",
                                  "3.  Delete All Logs"
                                  };
String USBDisk_MenuList[] = {
                                  "1.  Download GLog",
                                  "2.  Download SLog",
                                  "3.  Download DB",
                                  "4.  Upload DB",
                                  "5.  Upgrade F/W"
                                 };
String Settings_MenuList[] = {
                                  "1.  General",
                                  "2.  Log",
                                  "3.  Communication",
                                  "4.  Access",
                                  "5.  Information",
                                  "6.  Self Test",
                                  "7.  Factory Reset"
                                 };
                                 
String Enroll_EnrollUser_MenuList[]= {
                                     "1.  Fingerprint",
                                     "2.  Password",
                                     "3.  Card"
                                   };  
String Enroll_EnrollAdmin_MenuList[]= {
                                     "1.  Fingerprint",
                                     "2.  Password",
                                     "3.  Card"
                                   };
String Enroll_EnrollManager_MenuList[]= {
                                     "1.  Fingerprint",
                                     "2.  Password",
                                     "3.  Card"
                                   };                                   

String Settings_General_Menulist[]= {
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
                                  };
                                  
String Settings_Log_Menulist[]= {
                                   "1.  GLog Alarm",
                                   "2.  SLog Alarm",
                                   "3.  Re-verify"
                              };
                              
String Settings_Communication_Menulist[]= {
                                   "1.  Machine ID",
                                   "2.  Password",
                                   "3.  Use DHCP",
                                   "4.  IP Address",
                                   "5.  Subnet Mask",
                                   "6.  Gateway",
                                   "7.  TCP Port",
                                   "8.  PC IP Addr",
                                   "9.  PC Port"
                                  };
                                   
String Settings_Access_Menulist[]= {
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
                                  };
                                  
String Settings_Information_Menulist[]= {
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
                                  };          
                                  
String Settings_SelfTest_Menulist[]= {
                                   "1.  Keyboard",
                                   "2.  Sound",
                                   "3.  Sensor",
                                   "4.  Flash",
                                   "5.  LCD",
                                   "6.  Lock",
                                   "7.  Bell",
                                   "8.  WiegandOut",
                                   "9.  WiegandIn"
                                  };  
          

                        
                                  
                                                                                                                           
 int flag=0;
 
void setup() {
  GLCD.Init();
  GLCD.SelectFont(System5x7);
  Serial.begin(9600);
 
}
void displayLogo()
{
GLCD.DrawBitmap(pec1, 5, 0);  
delay(3000);
}

void displayMenu(String toDisplay[] )
{
  GLCD.println("        MENU");
  GLCD.println();
      for(unsigned short i=0;toDisplay[i]!='\0';i++)
    {
      GLCD.println(toDisplay[i]);
//     Serial.println(menu[i][i][i]);
    }
}

void loop() {  
  
  if(flag==0)
  {
    displayLogo();
    displayMenu(Main_MenuList);
  }
  flag=1;
}
