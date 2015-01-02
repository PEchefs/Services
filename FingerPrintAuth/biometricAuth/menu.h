/*
Menu/Sun Menu List Index:

0  -  Main Menu
1  -  Main -> Enroll Menu
2  -  Main -> View Log Menu
3  -  Main -> USB Disk Menu
4  -  Main -> Settings Menu
5  -  Main -> Enroll -> Enroll User Menu
6  -  Main -> Enroll -> Enroll Admin Menu
7  -  Main -> Enroll -> Enroll Manager Menu
8  -  Main -> Settings -> General
9  -  Main -> Settings -> Log
10 -  Main -> Settings -> Communication
11 -  Main -> Settings -> Access 
12 -  Main -> Settings -> Information
13 -  Main -> Settings -> Self-Test


*/

# define MAX_NUM_OF_MENU_LIST_ITEMS 15



typedef struct  {    
                   unsigned short numOfMenuListItems;
                   char *heading;
                   char *List[MAX_NUM_OF_MENU_LIST_ITEMS];        
                   unsigned short nextLevelMenuIndex[MAX_NUM_OF_MENU_LIST_ITEMS];
                   unsigned short functionPointerIndex[MAX_NUM_OF_MENU_LIST_ITEMS];
                   // void (*functionPointer[MAX_NUM_OF_MENU_LIST_ITEMS])(void);
                   
                  // struct * nextLevelMenu[MAX_NUM_OF_MENU_LIST_ITEMS];
                //  menu_st *ptr[MAX_NUM_OF_MENU_LIST_ITEMS]=
                  //struct p *ptr;
} menu_st;

menu_st Menu[14];

void initializeMenus()
{


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
}
/*
typedef struct {
  char *elementName[10];
  void (*functionPointer)(void);
} treeElement;

treeElement setTime_menu = {
  {"Set Time"}, setTimeFunction};
*/

unsigned short currentMenu=0;
unsigned short prevMenu=0;
unsigned short displayStartIndex=0;
unsigned short prevDisplayStartIndex=0;
int currentMenuIndex=0;
unsigned short prevMenuIndex=0;
unsigned short MenuFunctionToCallIndex=0;

enum state {HOMESCREEN,MENUSCREEN,FPAUTH,PWDAUTH,RFIDAUTH,MENUFUNCTION,NONE};
enum key {POWER,SCROLLDOWN,SCROLLUP,ENTER,ESC,MENU,ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,NOKEY};
state previousState=NONE;
state currentState=MENUSCREEN;
state newState=NONE;

