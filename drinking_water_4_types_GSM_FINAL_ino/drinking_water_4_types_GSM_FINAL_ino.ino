 /*
Tank circulation in - in1[]
Tank out - in[]

General in[0],in1[0]
Type1 in[1],in1[1]
Type2 in[2],in1[2]
Type3 in[3],in1[3]

*/

#include <IRremote.h>      //include the header containing functions related to IR remote
#include <shiftReg16.h>
#include <SoftwareSerial.h>

//Table that specifies the characteristics of the pump
/*
//       floor no.: 0     1     2     3     4     5     6     7      8      9 
int table[4][10]={{100,  200,  300,  400,  500,  600,  700,  800,   900,   1000},  //10 liters
                  {200,  300,  400,  500,  600,  700,  800,  900,   1000,  1100},  //20 liters
                  {300,  400,  500,  600,  700,  800,  900,  1000,  1100,  1200},  //30 liters
                  {400,  500,  600,  700,  800,  900,  1000, 1100,  1200,  1300}}; //40 liters
*/
//       floor no.: 0     1     2     3     4     5     6     7      8      9                   
int table[4][10]={{30,   40,   45,   50,   55,   60,   65,   70,   75,   80},
                  {60,   80,   90,   100,  110,  120,  130,  140,  150,  160},    
                  {160,  190,  220,  250,  270,  300,  325,  350,  380,  400},
                  {300,  370,  430,  500,  550,  600,  650,  700,  750,  800}};                  
const int IR_recv = 10;  //IR input pin
//const int pump = 12;    //pump relay
//const int circ = 11;    //circulation valve relay
//const int outlet = 10;  //outlet valve relay
//const int in[4] = {9,8,7,6};  //relays corresponding to {general water or water0 , water1, water2, water3}
//const int in1[4] = {20,21,22,23};
const int in[4] = {0,1,2,3};  //relays corresponding to {general water or water0 , water1, water2, water3}
const int in1[4] = {4,5,6,7};
const int pump=8;
const int circ=9;
const int outlet=10;

SoftwareSerial mySerial(11, 12);

int shiftRegData=0;
int clientID=-1;
int clientIDnum[3]={0,0,0};
unsigned short index=0;
boolean clientIDledState=false;
unsigned long clientIDledTimeOut=0;
const int led_pump = 13, led_ht = 7, led_type = 8, led_quant = 9, led_err = 5, led_gsm=6, led_clientID=7;;  //different LEDs for different actions
int height, type, quantity;
int total_quantity=0;

#define DEBUG 0
#define DEBUG1 1
//#define DEBUG2 1

shiftReg16 shiftReg1;
const int ShiftRegPinSRCLR = A0;
const int ShiftRegPinSRCK = A1;
const int ShiftRegPinSER = A3;
const int ShiftRegPinRCK = A2;

const float liters_per_sec[10]={5,4.5,4,3.5,3,2.5,2,1.5,1,0.5};    //liters per second value of the pump for floors 0 to 9

boolean power=0, clientID_flag=0, ht_flag=0, type_flag=0, quant_flag=0, go=0, gsm_flag=0,time_flag=0;  
unsigned long time1, time2, time3, time4, time_diff;
IRrecv irrecv(IR_recv);   //receive IR information from the pin IR_recv

const char phone_no[]="9008178070";   //phone number to which the SMS has to be sent
//const char phone_no[]="9972811280";   //phone number to which the SMS has to be sent

decode_results results;    //decode the IR info and store it in results

void sendDataToShiftReg(int _shiftRegData)
{
  shiftReg1.writeData(_shiftRegData);
}

boolean valveControl(byte _valveIndex, boolean OnorOFF)
{
  int i=_valveIndex;
  if (OnorOFF == 1)
    shiftRegData = shiftRegData | (1UL << i);
  else
    shiftRegData = shiftRegData & ~(1UL<< i);
    
 /* if (_valveIndex<8)
  {
    bitWrite(lowByte(shiftRegData),_valveIndex,OnorOFF);
  }
  else
  {
    bitWrite(highByte(shiftRegData),_valveIndex,OnorOFF);
  }
  */
//  if(DEBUG1)
//    Serial.println("Sending Data to shiftReg16");
  sendDataToShiftReg(shiftRegData);
//  if(DEBUG1)
//    Serial.println("Sent Data to shiftReg16");
  
  return 1;
}

void func_water0(int ht, int quant)    //function for releasing general water (water0)
{
  if(DEBUG1)
    {
      //mySerial.println("Releasing water: general");
    //mySerial.print("Quantity: ");//mySerial.println(quant);
    //mySerial.print("Height: " );//mySerial.println(ht);
    }
  int i;
  if(quant==1)      //quant=1 means unspecified amount of water
  {
    time3=millis();    //get the current time in milliseconds
    valveControl(outlet,HIGH);  //open the outlet valve
    delay(200);
    valveControl(circ,LOW);    //close circulation valve
    delay(200);
    while(1)    
    {
      if(irrecv.decode(&results))    //check if any button is pressed 
      {
        irrecv.resume();      //get the latest pressed button code 
        if(results.value==0xA90)     //if power button is pressed
        {
          func_power_off();    //power off the system
          debounce();          //debounce to erase the multiple presses by mistake
          return;              //return from the function
        }
        else if(results.value==0xDD0)    //if 'default OFF' is pressed
          break;
        else 
          func_invalid_key();     //if any other invalid key
      }
      else delay(200);
    }
    
    valveControl(circ,HIGH);    //open the circulation valve
    delay(200);
    valveControl(outlet,LOW);    //close the outlet valve
    time4=millis();              //get curent time in milliseconds
    delay(200);
    time_diff=time4-time3;      //time for which the outlet valve was open
    total_quantity=total_quantity+func_time_to_quant(time_diff);    //convert time_diff to amount of water released
    debounce();    
    for(i=0;i<10;i=i+1)      //wait for 10 seconds for any button press
    {
      if(irrecv.decode(&results))    //if any button is pressed, read the value and break from the loop
      {
        irrecv.resume();
        delay(200);
        break;
      }
      else
        delay(1000);
    }
    if(i!=10)
    {
      if(results.value==0xA90)     //if power button
      {
        func_power_off();    //power off the system
        debounce();
        gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);  //send SMS about the type and quantity of water released to specific phone number
        if(gsm_flag==0)    //if SMS is not sent,
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //try again after a second
        }
        if(gsm_flag==1)   //if SMS is sent,
          func_gsm_led_blink();    //blink a LED
        
        total_quantity=0;     //clear the totla_quantity value
        return;            //return from the function
      }
      else if(results.value==0xB90)  //if 'default ON' button is pressed,
        func_water0(ht,1);            //go to func_water0() to released unspecified amount of water0
      else     //any other button pressed
      {
        func_invalid_key();    //blink the invalid key
        gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //send SMS about the type and quantity of water released to specific phone number
        if(gsm_flag==0)    //if SMS is not sent,
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //try again after a second
        }
        if(gsm_flag==1)    //if SMS is sent,
          func_gsm_led_blink();    //blink a LED
        
        debounce(); 
        total_quantity=0;
        func_power_on();        //go to idle state
      }
    }
    else //if no button is pressed,
    {
      gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //send the SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht); //if failed, try sending SMS again
        }
        if(gsm_flag==1)    //on successful SMS,
          func_gsm_led_blink();  //blink a LED
          
        total_quantity=0;
        debounce();
        func_power_on();  //go to idle state
    }
  }
  
  else
  {
    valveControl(outlet,HIGH);  //open outlet valve
    delay(200);
    valveControl(circ,LOW);    //close circulation valve
    
    for(unsigned short count=0;count<100;count++)
    delay((table[(quant/10)-1][ht])*10);    //release water0 for time specified in the table depending on the values of ht and quant
    
    valveControl(circ,HIGH);    //open circulation valve
    delay(200);
    valveControl(outlet,LOW);    //close outlet valve
    
    total_quantity=total_quantity+quant;  //add amout of water released to total_quantity
    
    debounce();
    for(i=0;i<10;i=i+1)    //wait of 10 seconds
    {
      if(irrecv.decode(&results))    //if any button is pressed
      {
        irrecv.resume();    //get the button value and
        delay(200);
        break;                 //come out od for loop   
      }
      else
        delay(1000);
    }
    if(i!=10)
    {
      if(results.value==0xA90) //if power button,
      {
        func_power_off();      //power off the system
        debounce();
        gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //send SMS
        if(gsm_flag==0)    //if failed,
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);  //send SMS agian
        }     
        if(gsm_flag==1)    //if SMS is sent
          func_gsm_led_blink();    //blink a LED
          
        total_quantity=0;
        return;
      }
      else if(results.value==0xB90)   //if 'default ON' button, 
        func_water0(ht,1);    //release unspecified amount of water0 
      else 
      {
        func_invalid_key();    //blink invalid_key LED
        gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //srnd SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //try again if SMS fails
        }
        if(gsm_flag==1)
          func_gsm_led_blink();    //blink LED on successful SMS
        
        debounce();  
        total_quantity=0;
        func_power_on();     //go to idle state
        
      }
    }
    else    //if no button is pressed,
    {
      gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);    //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,0,total_quantity,ht);
        }
        if(gsm_flag==1)
          func_gsm_led_blink();    //notify the successful SMS
          
        debounce();
        total_quantity=0;
        func_power_on();    //go to idle state
      
    }
  }
}

void func_other(int ht, int type, int quant)
{
  
  if(DEBUG1)
    {
      //mySerial.print("Releasing water: ");//mySerial.println(type);
    //mySerial.print("Quantity: ");//mySerial.println(quant);
    //mySerial.print("Height: " );//mySerial.println(ht);
    }
  int i;
  if(quant==1)    //unspecified amout of water
  {
    valveControl(in[type],HIGH);    //open specific water type valve
    delay(200);
    valveControl(in[0],LOW);      //close general water valve
    delay(200);
    valveControl(in1[type],HIGH);    //open specific water type valve
    delay(200);
    valveControl(in1[0],LOW);      //close general water valve    
    delay(1000);                  //higher delay to force all the water0 present in the pipe into its tank
    time3=millis();    //get the time in milliseconds
    valveControl(outlet,HIGH);    //open outlet valve
    delay(200);
    valveControl(circ,LOW);    //close circulation valve
    delay(200);
    valveControl(in1[type],LOW);    //open specific water type valve    
    delay(200);
    while(1)    
    {
      if(irrecv.decode(&results))    //check for button press
      {
        irrecv.resume();    //get the latest button value
        if(results.value==0xA90) //if power button,
        {
          func_power_off();      //power off the system
          debounce();
          return;
        }
        else if(results.value==0xDD0)    //if 'defualt OFF' button
          break;    
        else 
          func_invalid_key(); //if other buttons
      }
      else delay(200);
    }
    
    valveControl(circ,HIGH);    //open the circulation valve
    delay(200);
    valveControl(in1[type],HIGH);
    delay(200);
    valveControl(outlet,LOW);    //close the outlet valve
    time4=millis();    //get time in milliseconds
    delay(200);
    valveControl(in[0],HIGH);    //open water0 valve
    delay(200);
    valveControl(in[type],LOW);  //close specofic water type valve
    delay(200);
    valveControl(in1[0],HIGH);    //open water0 valve
    delay(200);
    valveControl(in1[type],LOW);  //close specofic water type valve
    delay(200);
    time_diff=time4-time3;    //calculate the time for which the water was released
    total_quantity=total_quantity+func_time_to_quant(time_diff);  //calculate the amount of water released
    debounce();
    for(i=0;i<10;i=i+1)    //wait of 10 secounds
    {
      if(irrecv.decode(&results))    //check if any button is pressed
      {
        irrecv.resume();  //get the button value
        delay(200);
        break;
      }
      else
        delay(1000);
    }
    if(i!=10)  //if any button id pressed,
    {
      if(results.value==0xA90) //if power button,
      {
        func_power_off();     //power off the system
        gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht); //if SMS fails, try agian
        }
        if(gsm_flag==1)
          func_gsm_led_blink();    //blink a LED on successful SMS
          
        total_quantity=0;
        debounce();
        return;
      }
      else if(results.value==0xB90)   //if 'default ON' button,
        func_other(ht,type,1);        //release the unspecified amount of water[type] 
      else 
      {
        func_invalid_key();    //if any other button, blink a LED
        gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //if SMS fails, try agian
        }     
        if(gsm_flag==1)
          func_gsm_led_blink();    //blink a LED on successful SMS
         
        debounce(); 
        total_quantity=0;
        func_power_on();    //go to idle state
        
      }
    }
    else     //if no button is pressed,
    {
      gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //if SMS fails, try agian
        }     
        if(gsm_flag==1)
          func_gsm_led_blink();    //blink a LED on successful SMS
          
        debounce();
        total_quantity=0;
        func_power_on();    //go to idle state
    }
  }
  
  else    //if specific amount of water
  {                              //open & close required valves for the release of specified type of water
    valveControl(in[type],HIGH);    
    delay(200);
    valveControl(in[0],LOW);
    delay(200);
    valveControl(in1[type],HIGH);    
    delay(200);
    valveControl(in1[0],LOW);
    delay(1000);
    valveControl(outlet,HIGH);
    delay(200);
    valveControl(circ,LOW);
    delay(200);
    valveControl(in1[type],LOW);    //open specific water type valve    
    delay(200);
    
    for(unsigned short count=0;count<100;count++)
    delay((table[(quant/10)-1][ht])*10);    //wait time depends on the values of quant and ht
                                   //bring the valves back to water0 circulation state 
    valveControl(circ,HIGH);
    delay(200);
    valveControl(in1[type],HIGH);
    delay(200);
    valveControl(outlet,LOW);
    delay(200);
    valveControl(in[0],HIGH);
    delay(200);
    valveControl(in[type],LOW);
    delay(200);
    valveControl(in1[0],HIGH);
    delay(200);
    valveControl(in1[type],LOW);
    delay(200);
    
    total_quantity=total_quantity+quant;    
    
    debounce();
    for(i=0;i<10;i=i+1)    //wait for 10 seconds
    {
      if(irrecv.decode(&results))
      {
        irrecv.resume();
        delay(200);
        break;
      }
      else
        delay(1000);
    }
    if(i!=10)    //if any button is pressed 
    {
      if(results.value==0xA90)   //if power button,
      {
        func_power_off();      //power off the system
        gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);  //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //if SMS fails, try again
        }     
        if(gsm_flag==1)
          func_gsm_led_blink();    //blink a LED on successful SMS
          
        total_quantity=0;
        debounce();
        return;
      }
      else if(results.value==0xB90)   //if 'default ON' button 
        func_other(ht,type,1);        //release unspecified amount of water[type]
      else 
      {
        func_invalid_key();    
        gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //if SMS fails, try again
        }     
        if(gsm_flag==1)
          func_gsm_led_blink();    //blink a LED on successful SMS
          
        debounce();
        total_quantity=0;
        func_power_on();    //go to idle state
        
      }
    }
    else    //if no button is pressed
    {
      
      gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //send SMS
        if(gsm_flag==0)
        {
          delay(1000);
          gsm_flag=SMS("Water released",phone_no,type,total_quantity,ht);    //if SMS fails, try again
        }    
       if(gsm_flag==1)
          func_gsm_led_blink();    //blink a LED on successful SMS
          
       debounce();  
       total_quantity=0;
       func_power_on();    //go to idle state
    }
  }
}

void func_gsm_led_blink()    //function to blink the led_gsm on successful SMS
{
  //mySerial.println("Blink GSM LED");
  int i;
  for(i=0;i<3;i++)    //3 pulses
  {
  analogWrite(led_gsm,0);  //LED- OFF
  delay(100);
  analogWrite(led_gsm,255);  //LED- ON
  delay(100);
  }
}

int func_time_to_quant(unsigned long time_delay)    //function to convert time into quantity of water released
{
  int q;
  q=(int)(time_delay/1000)*liters_per_sec[height];  //formula. 
  return(q);    //return amout of water corresponding to time_delay
}

void func_power_off()    //function to power off the system
{
 // if(DEBUG1)
    //mySerial.println("POWERING OFF");
  int i;
  valveControl(pump,LOW);    //switch of the pump
  delay(200);
  valveControl(circ,LOW);    //close the circulation vvalve
  delay(200);
  valveControl(outlet,LOW);    //close the outlet valve
  delay(200);
  valveControl(in[0],LOW);  //close water0 valve
  delay(200);
  valveControl(in1[0],LOW);  //close water0 valve
  delay(200);
  for(i=1;i<4;i=i+1)
  {
    valveControl(in[i],LOW);    //close water1, water2 & water3 valves
    delay(200);
    valveControl(in1[i],LOW);  //close water0 valve
    delay(200);
  }
        //clear the variables
  power=0;
  clientID_flag=0;
  ht_flag=0;
  type_flag=0;
  quant_flag=0;
        //turn OFF the LEDs
  analogWrite(led_pump,0);    
  delay(100);
  analogWrite(led_ht,0);
  delay(100);
  analogWrite(led_type,0);
  delay(100);
  analogWrite(led_quant,0);
  delay(100);
  analogWrite(led_err,0);
  delay(100);  
 // if(DEBUG1)
    //mySerial.println("TURNED OFF ALL LEDs");
}

void func_invalid_key()    //function for blinking  invalid_key LED
{
 // if(DEBUG1)
    //mySerial.println("INVALID KEY");
  int i;
  for(i=0;i<5;i=i+1)    //5 pulses
  {
    analogWrite(led_err,255);    //LED- ON
    delay(100);
    analogWrite(led_err,0);      //LED- OFF
    delay(100);
  }
  debounce();
}

void toggleClientIDled()
{
 if (clientIDledState==true)
 {
   analogWrite(led_clientID,0); 
   clientIDledState=false;
 }
 else
 {
   analogWrite(led_clientID,255); 
   clientIDledState=true;
 }
}
void func_clientID()    //function for entering the height (floor number) (0 to 9)
{
  if(millis()-clientIDledTimeOut>500)
  {
    toggleClientIDled();    //clientID notification LED- ON
    clientIDledTimeOut=millis();
  }
  delay(50);
  if (irrecv.decode(&results))    //check for button press
   {
     
     irrecv.resume();      //get the latest button value
     delay(200);
                         //buttons 0 - 9 correspond to floors 0 - 9
     if(results.value==0x910) clientIDnum[index]=0;      
     else if(results.value==0x010) clientIDnum[index]=1;
     else if(results.value==0x810) clientIDnum[index]=2;
     else if(results.value==0x410) clientIDnum[index]=3;
     else if(results.value==0xC10) clientIDnum[index]=4;
     else if(results.value==0x210) clientIDnum[index]=5;
     else if(results.value==0xA10) clientIDnum[index]=6;
     else if(results.value==0x610) clientIDnum[index]=7;
     else if(results.value==0xE10) clientIDnum[index]=8;
     else if(results.value==0x110) clientIDnum[index]=9;
     
     else if(results.value==0xA90) //if power button
     {
       func_power_off();    //power off the system
       debounce();
       return;
     }
     else func_invalid_key();    //if any other invalid key
     debounce();  
    
      index++;
   }
   
   if(index==3)
   {
     clientID=clientIDnum[0]*100+clientIDnum[1]*10+clientIDnum[2];
    // if(DEBUG1)
//       //mySerial.println("ClientID = ");//mySerial.println(clientID);   
   }
     
}


void func_height()    //function for entering the height (floor number) (0 to 9)
{
  
  analogWrite(led_ht,255);    //height notification LED- ON
  delay(50);
  if (irrecv.decode(&results))    //check for button press
   {
     irrecv.resume();      //get the latest button value
     delay(200);
                         //buttons 0 - 9 correspond to floors 0 - 9
     if(results.value==0x910) height=0;      
     else if(results.value==0x010) height=1;
     else if(results.value==0x810) height=2;
     else if(results.value==0x410) height=3;
     else if(results.value==0xC10) height=4;
     else if(results.value==0x210) height=5;
     else if(results.value==0xA10) height=6;
     else if(results.value==0x610) height=7;
     else if(results.value==0xE10) height=8;
     else if(results.value==0x110) height=9;
     
     else if(results.value==0xA90) //if power button
     {
       func_power_off();    //power off the system
       debounce();
       return;
     }
     else func_invalid_key();    //if any other invalid key
     debounce();  
    //if(DEBUG1)
     //mySerial.print("HEIGHT = ");//mySerial.println(height);   
   }
}

void func_type()    //function for entering type of water (0 to 3)
{
  analogWrite(led_type,255);    //height notification LED- OFF
  delay(100);
  analogWrite(led_ht,0);        //height notification LED- ON
  delay(100);
  
  if (irrecv.decode(&results))    //check for button press
   {
     irrecv.resume();    //get the latest value
     delay(200);
                   //buttons 0 - 3 correspond to water type 0 - 3
     if(results.value==0x910) type=0;
     else if(results.value==0x010) type=1;
     else if(results.value==0x810) type=2;
     else if(results.value==0x410) type=3;
       
     else if(results.value==0xA90)    //if power button,
     {
       func_power_off();        //power off the system
       debounce();
       return;
     }
     else func_invalid_key();    //if any other button
     debounce();
        // if(DEBUG1)
     //mySerial.print("TYPE = ");//mySerial.println(type);
   }
}

void func_quantity()    //function for entering quantity of water required (10 liters to 40 liters or unspecified amount)
{
  analogWrite(led_quant,255);    // quantity notification LED- ON
  delay(100);
  analogWrite(led_type,0);      //type notification LED- OFF
  delay(100);
  
  if(irrecv.decode(&results))    //check for button press
   {
     irrecv.resume();    //get the latest valve
     delay(200);    
                       //buttons 1 - 4 correspond to quantity: 10 liters - 40 liters
     if(results.value==0x010) quantity=10;
     else if(results.value==0x810) quantity=20;
     else if(results.value==0x410) quantity=30;
     else if(results.value==0xC10) quantity=40;
     else if(results.value==0xB90) quantity=1;    //unspecified amount of water
      
     else if(results.value==0xA90)   //if power button,
     {
       func_power_off();    //power off the system
       debounce();
       return;
     }
     else func_invalid_key();    //if any other button
     debounce();
         //     if(DEBUG1)
     //mySerial.print("QUANTITY = ");//mySerial.println(quantity);
   }
}
   
void func_power_on()    //function for powering ON the system and to go to idle state
{ 
  //if(DEBUG1)  
    //mySerial.println("POWERING ON");
  power=1;
  valveControl(outlet,LOW);    //close the outlet valve
  delay(200);
  valveControl(in[0],HIGH);    //open the water0 valve for circulation
  delay(200);
  valveControl(in1[0],HIGH);
  delay(6000);
  valveControl(circ,HIGH);    //open the circulation valve
  delay(3000);

  valveControl(pump,HIGH);    //switch ON the pump relay
  analogWrite(led_pump,255);   //led_pump- ON
                            //other notification LEDs- OFF
  analogWrite(led_ht,0);       
  analogWrite(led_type,0);
  analogWrite(led_quant,0); 
  clientID_flag=1;
}

void debounce()    //function for clearing the miss-pressed buttons
{
  int i;
  for(i=0;i<5;i=i+1)    
  {
    results.value=0;    //clear the value
    irrecv.resume();    //get the next value
    delay(100);
  }
}

int CHECK_ERROR(int no_of_bytes)    //function for checking the errot in GSM communication
{
  int i,error_flag=0;
  char buff[200]= {0};    //buffer for storing read values from GSM modem via serial port (Rx)
  for(i=0;i<no_of_bytes;i++)
    buff[i]=Serial.read();    //read specified number of bytes from the Rx
  for(i=0;i<no_of_bytes-4;i++)
  {
    if((buff[i]=='E')&&(buff[i+1]=='R')&&(buff[i+2]=='R')&&(buff[i+3]=='O')&&(buff[i+4]=='R'))    //search for the word 'ERROR' in the read data
    {
      error_flag=1;    //if word 'ERROR' is found,
      return 1;        //return 1 from the function, indicating the presence of error in GSM communication
    }
  }
  if(error_flag==0)    //if no 'ERROR' word,
    return 0;          //return 0, indicating no error
}

int gsm_ping()    //function for pinging the GSM
{
  if(DEBUG)
    return 1;
  //mySerial.write("GSM ping called!");
  //mySerial.println("");
  int i, ok_flag=0; 
  for(i=0;i<3;i++)    //ping 3 times if no answer from the GSM modem
  {
    char buff_ping[30]={0};  
    Serial.write(0x1B);  
    delay(200);
    Serial.write("AT+CREG?\r");    //write AT\r into GSM
    delay(500);
    for(i=0;i<30;i++)  
    {
      buff_ping[i]=Serial.read();    //rad 20 values from GSM
      //mySerial.print(buff_ping[i]);
    }
    //mySerial.println("");
    for(i=0;i<29;i++)
    {
  //    if(buff_ping[i]=='O'&&buff_ping[i+1]=='K')    //check for the word 'OK' in the read data
      if(buff_ping[i]=='C'&&buff_ping[i+1]=='R'&&buff_ping[i+2]=='E'&&buff_ping[i+3]=='G'&&buff_ping[i+4]==':'&&buff_ping[i+5]==' '&&buff_ping[i+6]=='0'&&buff_ping[i+7]==','&&buff_ping[i+8]=='1')    //check for the word 'OK' in the read data
      {
        //mySerial.write("GSM AT OK!");
        //mySerial.println("");
        ok_flag=1;    //if 'OK' is found,
        return 1;    //return 1, indication that the GSM is working properly
      }
    }
  }
  
  //mySerial.write("ERROR! GSM RESPONSE NO OK RECEIVED");
  //mySerial.println("");
  ////mySerial.write("Response:");
  //for(i=0;i<20;i++)  
    ////mySerial.write(buff_ping[i]);
  //mySerial.println("");
  if(ok_flag==0)  //if 'OK' is not sent back by GSM all 3 times,
  return 0;      //return 0, indicating that the GSM is not working
}

int SMS(char message[30], const char phone_number_local[],int type_local, int quantity_local, int height_local)    //function for sending SMS 
{
  //mySerial.println("SMS function called");
  if(DEBUG)
    return 1;
  int i,gsm_status=0,err=0;
  gsm_status=gsm_ping();    //ping GSM for 'OK'
  if(gsm_status==0)        //if GSM is not working,
    return 0;              //return 0, indicating the SMS failure
  else                   //if GSM is working
  {
    Serial.write("AT\r");   //write 'AT\r'
    delay(200);
    err=CHECK_ERROR(15);    //check for error
    if(err==1)     
      return 0;    //return 0 in case of error
    Serial.write("AT+CMGF=1\r");  //SMS mode
    delay(200);
    err=CHECK_ERROR(20);
    if(err==1) 
      return 0;
    Serial.write("AT+CMGS=\"");    //SMS content
    Serial.write(phone_number_local);    //phone number to which the SMS has to be sent
    Serial.write("\"\r");    
    delay(200);
    err=CHECK_ERROR(35);
    if(err==1) 
      return 0;
    Serial.write(message);    //write the text
    Serial.write("\nClient ID: ");
    Serial.print(clientID);  //write the floor to which water released 
    Serial.write("\nType: ");  
    Serial.print(type_local);   //write the type of water
    Serial.write("\nQuantity: ");
    Serial.print(quantity_local);  //write the quantity of water released 
    Serial.write("\nFloor: ");
    Serial.print(height_local);  //write the floor to which water released 
    delay(200);
    err=CHECK_ERROR(64);
    if(err==1) 
      return 0;
    Serial.write(0x1A);  
    err=CHECK_ERROR(20);
    if(err==1) 
      return 0;
    char buff[64]={0};
    delay(4500);
    for(i=0;i<64;i++)
      buff[i]=Serial.read();    //read 64 bytes from the GSM
   
    for(i=0;i<63;i++)
    {
      if(buff[i]=='O' && buff[i+1]=='K')    //check for 'OK'
        return 1;    //successful SMS 
    } 
    if(i==64)        
      return 0;    //SMS failure
  }
}

void setup()    //this function is called only once during the arduino startup for initial setup
{              //declare required digital pins as OUTPUT
  pinMode(pump,OUTPUT);
  pinMode(circ,OUTPUT);
  pinMode(outlet,OUTPUT);
  pinMode(in[0],OUTPUT);
  pinMode(in[1],OUTPUT);
  pinMode(in[2],OUTPUT);
  pinMode(in[3],OUTPUT);
  pinMode(in1[0],OUTPUT);
  pinMode(in1[1],OUTPUT);
  pinMode(in1[2],OUTPUT);
  pinMode(in1[3],OUTPUT);
  
 shiftReg1.srinit(ShiftRegPinSER,ShiftRegPinRCK,ShiftRegPinSRCK,ShiftRegPinSRCLR);
    
  irrecv.enableIRIn();     //enable the IR receiver
  Serial.begin(9600);      //9600 baude rate for serial communication with GSM modem 
  delay(100);
  //mySerial.begin(9600);
//  Serial.println("Serial begun");
}

void loop()     //this function is called indefinitely
{
//  Serial.println("In loop");
  int i;
  if(time_flag==0)    
  {
    time1=millis();    //get the time in millisecond
    time_flag=1;
  }
  time2=millis();      //get the time in millisecond
  if((time2-time1)>10000)  //if the time delay is greater than 10 seconds, ping the GSM modem
  {
    //if(DEBUG1)
      //mySerial.println("Checking GSM");
    time_flag=0;
    if(!DEBUG)
    {
      for(i=0;i<2;i++)  //ping GSM modem 2 times
    {
      gsm_flag=gsm_ping();    //call the function for pinging the GSM modem
      
      if(DEBUG)
      {
        gsm_flag=1;
        //mySerial.println("DEBUG IS ON");
      }
      if(gsm_flag==1)        //if ping is successful,
        break;               //come out of for loop
      delay(500);            //else ttry again
    }
    }
    else
    {
      gsm_flag=1;
    }
  }
  
  if(gsm_flag==1)        //if the GSM modem is working, enter the main function that controls the relays to release the water
  {
    analogWrite(led_gsm,0);     //if GSM is working, switch OFF the GSM notification LED
    if ((irrecv.decode(&results)) && (power==0))    //check for remote button press when the system is powered OFF
    {
      //if(DEBUG1)
        //mySerial.println("Checking for button press");
      irrecv.resume();    //get the latedt button value
      delay(200);
      if(results.value==0xA90)    //if power button is pressed
      {
        func_power_on();    //power ON the system
        debounce();
      }
    }
    
    if((power==1) && (clientID_flag==1))    //if flag for entering the height is ON & the system is ON,
    {
      //if(DEBUG1)
     //   //mySerial.write("ENTER CLIENT ID");
      
        index=0;
        clientID=-1;
      while(clientID==-1)    //check if the clientID is updated with the valid value
      {
        
        if(power==0)    //if power is OFF,
          return;       //return from the function
       func_clientID();  //else go to function that takes clientID from the user
      }
      debounce();
      clientID_flag=0;
      ht_flag=1;
    }
    if((power==1) && (ht_flag==1))    //if flag for entering the height is ON & the system is ON,
    {
      //if(DEBUG1)
        //mySerial.println("ENTER HEIGHT");
      height=-1;          //initialize the height variable to -1
      while(height==-1)    //check if the height variable is updated with the valid value
      {
        if(power==0)    //if power is OFF,
          return;       //return from the function
        func_height();  //else go to function that takes height from the user
      }
      debounce();
      ht_flag=0;    //clear the flag for entering the height upon successful update of height flag
      type_flag=1;   //set the flag for entering the type of water
    }
    
    if((power==1) && (type_flag==1))    //if flag for entering the type is ON & the system is ON,
    {
     // if(DEBUG1)
        //mySerial.println("ENTER TYPE");
      type=-1;          //initialize the type variable to -1
      while(type==-1)  //check if the type variable is updated with the valid value
      {
        if(power==0)    //if power is OFF,
          return;        //return form the function
        func_type();    //else go to function that takes type from the user
      }
      debounce();
      type_flag=0;    //clear the flag for entering the type upon successful update of type flag
      quant_flag=1;    //set the flag for entering the quantity of water
    }
    
    if((power==1) && (quant_flag==1))      //if flag for entering the quantity is ON & the system is ON,
    {
      //if(DEBUG1)
        //mySerial.println("ENTER QUANTITY");
      quantity=-1;    //initialize the quantity variable to -1
      while(quantity==-1)    //check if the quantity variable is updated with the valid value
      {
        if(power==0)      //i power is OFF,
          return;        //return from the function
        func_quantity();  //else go to function that takes quantity from the user
      }
      debounce();
      quant_flag=0;    //clear the flag for entering the quantity on successful update of quantity variable
      go=1;            //set the flag for controling the relays on successful update of height, type and quantity flags
      analogWrite(led_quant,0);    //switch OFF the quantity notification LED  
    }
    if((power==1) && (go==1))    //check if flag for controling the relays is set and the power is ON
    {
    for(i=0;i<2;i++)  //ping GSM modem 2 times
    {
      //mySerial.println("Pinging GSM");
      gsm_flag=gsm_ping();    //call the function for pinging the GSM modem
      
      if(DEBUG)
      {
        gsm_flag=1;
        //mySerial.println("DEBUG IS ON");
      }
      if(gsm_flag==1)        //if ping is successful,
        break;               //come out of for loop
      delay(500);            //else ttry again
    }
    //gsm_flag=gsm_ping();      //ping the GSM to check if it is working
      if(gsm_flag==1)          //if GSM is working,
      {
        switch(type)          //call the relevant functions corresponding to type 
        {
        case 0: func_water0(height, quantity);    //function that releases water0 (general water) depending on the values of height and quantity    
                  break;
        case 1:           
        case 2:            
        case 3: func_other(height,type,quantity);    //function that releases other water (water1, water2 & water3) depending on the values of height and quantity 
                  break;      
        }  
      }
      else       //if GSM is not working, 
      {
        //mySerial.println("GSM Not Working! Aborting and Powering OFF");
        func_power_off();    //power OFF the system 
        //mySerial.println("Turning ON GSM Error LED");
        analogWrite(led_gsm,255);    //notify the user about the GSM error by switching ON a LED
        return;    
      }
      
    }
    if((power==1) && (clientID_flag==0) && (ht_flag==0) && (type_flag==0) && (quant_flag==0))    //if all the flags are cleared and power is ON,
    {
      if(irrecv.decode(&results))          //check for button press
      {
        irrecv.resume();      //get the latest button value
        delay(200);      
        if(results.value==0xA90)     //if power button,
        {
          //mySerial.println("POWER OFF BUTTON PRESSED! POWERING OFF!");
          func_power_off();          //power OFF the system
          debounce();
          return;
        }
      }
      else      //is no button is pressed,
      {
        //mySerial.println("Going To Idle State");
        func_power_on();    //go to idle state
        debounce();
      } 
    }
  }
 else      //if GSM if not working,
  {
    //mySerial.println("GSM NOT WORKING! POWERING OFF!");
    func_power_off();    //power OFF the system
    //mySerial.println("Turning ON GSM Error LED");
    analogWrite(led_gsm,255);    //notify the user about the GSM error by switching ON a LED
    delay(200);
  }
}
