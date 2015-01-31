#include <Wire.h>
#include "Keypad.h"

#define LOCALADDRESS 2

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

byte dataToSend[10]={0,0,0,0,0,0,0,0,0,0};

void setup()
{
  Wire.begin(LOCALADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveFromMaster); // register event
  Serial.begin(9600);           // start serial for output
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()

void receiveFromMaster(int howMany)
{
  Serial.println("Entered I2C receive function");
  byte   receivedCommand[10];
  unsigned short i=0;
  while (Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    receivedCommand[i]=c;
    i++;
  }
  if (receivedCommand[0]==0x50)
  {
    Serial.println("Command received : poll");
    dataToSend[0]=0x30;
    sendToMaster(dataToSend);
    for(unsigned short i=0;i<10;i++)
  {
    dataToSend[i]=0;
  }
 // int x = Wire.read();    // receive byte as an integer
 // Serial.println(x);         // print the integer
 
  }
}

void sendToMaster(byte data[])
{
  for(unsigned short i=0;i<10;i++)
  {
    Wire.write(data[i]); // respond with message of 6 bytes
  // as expected by master
  }
}
