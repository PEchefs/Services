

#define glcdData0Pin        8
#define glcdData1Pin        9
#define glcdData2Pin        10
#define glcdData3Pin        11
#define glcdData4Pin        4
#define glcdData5Pin        5
#define glcdData6Pin        6
#define glcdData7Pin        7

/* Arduino pins used for Commands
 * default assignment uses the first five analog pins
 */

#define glcdCSEL1        A0
#define glcdCSEL2        A1
#define glcdRW           16
#define glcdDI           17
#define glcdEN           12

//const char* dataPins[8]={"glcdData0Pin","glcdData1Pin","glcdData2Pin","glcdData3Pin","glcdData4Pin","glcdData5Pin","glcdData6Pin","glcdData7Pin"};
int dataPins[8]={ 8,9,10,11,4,5,6,7};
void displayOFF()
{
  
  
  digitalWrite(glcdCSEL1,HIGH);
  digitalWrite(glcdCSEL2,HIGH);
  
  digitalWrite(glcdEN,HIGH);
  digitalWrite(glcdRW,HIGH);
  delay(1);
  
  
  digitalWrite(glcdData7Pin,LOW);
  digitalWrite(glcdData6Pin,LOW);
  digitalWrite(glcdData5Pin,HIGH);
  digitalWrite(glcdData4Pin,HIGH);
  digitalWrite(glcdData3Pin,HIGH);
  digitalWrite(glcdData2Pin,HIGH);
  digitalWrite(glcdData1Pin,HIGH);
  digitalWrite(glcdData0Pin,LOW);
  digitalWrite(glcdDI,LOW);
  
  digitalWrite(glcdEN,LOW);
  delay(1);
  
  digitalWrite(glcdRW,LOW);
  delay(1);
  digitalWrite(glcdEN,HIGH);
  delay(1);
  digitalWrite(glcdEN,LOW);
  delay(1);
  digitalWrite(glcdRW,HIGH);
  delay(1);
}
 
void displayON()
{
 
  
  digitalWrite(glcdCSEL1,HIGH);
  digitalWrite(glcdCSEL2,HIGH);
  
  digitalWrite(glcdEN,HIGH);
  digitalWrite(glcdRW,HIGH);
  delay(1);
  
  
  digitalWrite(glcdData7Pin,LOW);
  digitalWrite(glcdData6Pin,LOW);
  digitalWrite(glcdData5Pin,HIGH);
  digitalWrite(glcdData4Pin,HIGH);
  digitalWrite(glcdData3Pin,HIGH);
  digitalWrite(glcdData2Pin,HIGH);
  digitalWrite(glcdData1Pin,HIGH);
  digitalWrite(glcdData0Pin,HIGH);
  digitalWrite(glcdDI,LOW);
  
  digitalWrite(glcdEN,LOW);
  delay(1);
  
  digitalWrite(glcdRW,LOW);
  delay(1);
  digitalWrite(glcdEN,HIGH);
  delay(1);
  digitalWrite(glcdEN,LOW);
  delay(1);
  digitalWrite(glcdRW,HIGH);
  delay(1);
}


void writeNow()
{
  
  digitalWrite(glcdCSEL1,HIGH);
  digitalWrite(glcdCSEL2,HIGH);
  
  digitalWrite(glcdEN,HIGH);
  digitalWrite(glcdRW,HIGH);
  delayMicroseconds(1);
  
  
  digitalWrite(glcdEN,LOW);
  delayMicroseconds(1);
  
  digitalWrite(glcdRW,LOW);
  delayMicroseconds(1);
  digitalWrite(glcdEN,HIGH);
  delayMicroseconds(1);
  digitalWrite(glcdEN,LOW);
  delayMicroseconds(1);
  digitalWrite(glcdRW,HIGH);
  delayMicroseconds(1);
  digitalWrite(glcdEN,HIGH);
  
}

void setYaddress(int Yaddr)
{
  int TmpYShift=Yaddr;
  digitalWrite(glcdData7Pin,LOW);
  digitalWrite(glcdData6Pin,HIGH);
  digitalWrite(glcdDI,LOW);
  for(int dpin=0;dpin<=5;dpin++)
  {
//    Serial.print(TmpYShift,BIN);
//    Serial.print(" ");
//    Serial.println(((TmpYShift>>dpin)&(0x01)),BIN);
    //Serial.println(dataPins[dpin],DEC);
    if(((TmpYShift>>dpin)&(0x01))==0x01)
      digitalWrite(dataPins[dpin],HIGH);
    else
       digitalWrite(dataPins[dpin],LOW);
    TmpYShift=Yaddr;
  }
  writeNow();
}

void setXaddress(int Xaddr)
{
  int TmpXShift=Xaddr;
  digitalWrite(glcdDI,LOW);
  digitalWrite(glcdData7Pin,HIGH);
  digitalWrite(glcdData6Pin,LOW);
  digitalWrite(glcdData5Pin,HIGH);
  digitalWrite(glcdData4Pin,HIGH);
  digitalWrite(glcdData3Pin,HIGH);
  for(int dpin=0;dpin<=2;dpin++)
  {
    if(((TmpXShift>>dpin)&(0x01))==0x01)
      digitalWrite(dataPins[dpin],HIGH);
    else
       digitalWrite(dataPins[dpin],LOW);
//    digitalWrite(dataPins[dpin],(((TmpXShift>>dpin)&(0x01))==1)?HIGH:LOW);
    TmpXShift=Xaddr;
  }
  writeNow();
}

void writeData(int data, int Xaddr, int Yaddr)
{
  int TmpDataShift=data;
  setYaddress(Yaddr);
  delay(1);
  setXaddress(Xaddr);
  delay(1);
  digitalWrite(glcdDI,HIGH);
  for(int dpin=0;dpin<=7;dpin++)
  {
    digitalWrite(dataPins[dpin],(((TmpDataShift>>dpin)&(0x01))==1)?HIGH:LOW);
    TmpDataShift=data;
  }
  writeNow();  
}

void clearall()
{
  for(int x=0;x<8;x++)
    for(int y=0;y<64;y++)
      writeData(0x00,x,y);
}

void setup()
{
  ///Serial.begin(9600);
  pinMode(glcdData0Pin,OUTPUT);
  pinMode(glcdData1Pin,OUTPUT);
  pinMode(glcdData2Pin,OUTPUT);
  pinMode(glcdData3Pin,OUTPUT);
  pinMode(glcdData4Pin,OUTPUT);
  pinMode(glcdData5Pin,OUTPUT);
  pinMode(glcdData6Pin,OUTPUT);
  pinMode(glcdData7Pin,OUTPUT);
  pinMode(glcdCSEL1,OUTPUT);
  pinMode(glcdCSEL2,OUTPUT);
  pinMode(glcdRW,OUTPUT);
  pinMode(glcdDI,OUTPUT);
  pinMode(glcdEN,OUTPUT);
  displayON();
  delay(3000);
displayOFF();
delay(3000);
displayON();
delay(3000);
displayOFF();
delay(3000);
displayON();
/*
  //Y address
  digitalWrite(glcdData7Pin,LOW);
  digitalWrite(glcdData6Pin,HIGH);
  digitalWrite(glcdData5Pin,LOW);
  digitalWrite(glcdData4Pin,LOW);
  digitalWrite(glcdData3Pin,LOW);
  digitalWrite(glcdData2Pin,LOW);
  digitalWrite(glcdData1Pin,LOW);
  digitalWrite(glcdData0Pin,LOW);
  digitalWrite(glcdDI,LOW);
  writeNow();
  delay(10);
  //X address
  digitalWrite(glcdData7Pin,HIGH);
  digitalWrite(glcdData6Pin,LOW);
  digitalWrite(glcdData5Pin,HIGH);
  digitalWrite(glcdData4Pin,HIGH);
  digitalWrite(glcdData3Pin,HIGH);
  digitalWrite(glcdData2Pin,LOW);
  digitalWrite(glcdData1Pin,LOW);
  digitalWrite(glcdData0Pin,LOW);
  digitalWrite(glcdDI,LOW);
  writeNow();
  delay(10);
 //Write Data 10101010
  digitalWrite(glcdData7Pin,LOW);
  digitalWrite(glcdData6Pin,LOW);
  digitalWrite(glcdData5Pin,LOW);
  digitalWrite(glcdData4Pin,LOW);
  digitalWrite(glcdData3Pin,LOW);
  digitalWrite(glcdData2Pin,LOW);
  digitalWrite(glcdData1Pin,LOW);
  digitalWrite(glcdData0Pin,LOW);
  digitalWrite(glcdDI,HIGH); 
  */
 
  
 for(int i=0;i<64;i++)
 {
 //  writeNow();
  writeData(0xFF,4,i);
  delay(10);
 }

clearall();  

}

  
 void loop()
  {
  
  }
  