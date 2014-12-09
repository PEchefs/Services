/*
Sensors to Multiplexer Pin mapping configuration:
MUX1:
0  -  UltrasonicSensor1
1  -  UltrasonicSensor2
2  -  UltrasonicSensor3
3  -  UltrasonicSensor4
4  -  FlowSensor1
5  -  FlowSensor2
6  -  FlowSensor3
7  -  FlowSensor4

MUX2:
0  -  FlowSensor5
1  -  MagneticBallSensor1
2  -  MagneticBallSensor2
3  -  MagneticBallSensor3
4  -  MagneticBallSensor4
5  -  MagneticBallSensor5
6  -  MagneticBallSensor6
7  -  MagneticBallSensor7

SlaveSelect Lines are common for both.
After setting the SS lines CBA, the Mux which is to be used for sensor reading is enabled.

Pin configurations:

Arduino - Mux1
6  -  EN (Enable)
2  -  Multiplexed line (Input for Arduino to be read)

Arduino - Mux2
10 -  EN (Enable)
A0 -  Multiplexed line (Input for Arduino to be read)

Arduino - Mux1 and Mux2 (common)
7  -  SlaveSelectPin A
8  -  SlaveSelectPin B
9  -  SlaveSelectPin C

Arduino - Shift Register
3  -  SCL
4  -  SCK
5  -  SER
11 -  RCK

Arduino - Ultrasonic
A1 -  TRIG (Trigger SIG line)  

Arduino - RPi (Raspberry Pi)
TX -  RX
RX -  TX

*/
#include <shiftReg.h>
#include <NewPing.h>

#define ON 1
#define OFF 0
#define DEBUG 0
#define DEBUG1  1

#define EXIT_SUCCESS 1
#define EXIT_FAILURE 0

// ERROR HANDLER CODES:
#define senseError 101         //0x65
#define ActuationError 102     //0x66
#define invalidCommand 103     //0x67
#define invalidType 104        //0x68
#define IDforTypeNotFound 105  //0x69

// RESPONSE CODES
#define RESPOND_SUCCESS 201
#define RESPOND_FAILURE 202

// Mux Pin assigments : Enable Pins, Slave Select Pins (commonly connected to both Multiplexers)
const unsigned short int NumOfMux = 2;
const int MuxEnablePin[NumOfMux]={6,10};
const int MuxLine[NumOfMux]={2,10};
const int MuxSlaveSelectPins[3]={7,8,9};

// Shift Register Pin assigments: SCL, SCK, SER, RCK
const int ShiftRegPinSRCLR = 3;
const int ShiftRegPinSRCK = 4;
const int ShiftRegPinSER = 5;
const int ShiftRegPinRCK = 11;

const int UltrasonicTRIGpin = A1;                           // Ultrasonic sensor(depth sensor) TRIG line
const int MaxDepth=500;                                     //Max distance input required for Ultrasonic sensor library.

shiftReg shiftReg1;                                         //Shift Register instance: Relays are triggered based in Shift Register parallel output lines
NewPing depthSensor(UltrasonicTRIGpin,MuxLine[0],MaxDepth); // One Ultrasonic sensor instance to be used for all ultrasonic sensors connected to MUX1

// Defining the number of sensors and actators of each type
const unsigned short int NumOfDepthSensors=4;
const unsigned short int NumOfFlowSensors=5;
const unsigned short int NumOfBallSensors=7;
const unsigned short int NumOfRelays=8;

volatile int flowSensorCountPulses=0;                       // FlowSensor interrupt count 
int sensorIndexNow=0xFF;
int actuatorIndexNow=0xFF;
boolean readCommand=0;                                      // Flag to indicate if received command was a READ sensor command
boolean actuateCommand=0;                                   // Flag to indicate if received command was a ACTUATE device command
boolean commandComplete = false;                            // Flag to indicate if serial data received is COMPLETE or not - '\n' is the termination character

enum sensors_t {ball=151,depth=152,flow=153,unknownS};
enum actuators_t {relay=161,unknownA};

typedef struct
{
  byte         sensorID;
  sensors_t    sensorType;
  unsigned int sensorValue;
  byte         muxID;
  byte         muxPosition;
} gtype_Sensor_st;

typedef struct
{
  byte         actuatorID;
  actuators_t  actuatorType;
  boolean      actuatorValue;
  byte         muxID;
  byte         muxPosition;
} gtype_Actuator_st;

union
{
  struct
  {
    unsigned int    receivedCommand;
    byte   receivedID;
    byte   receivedType;
    byte   receivedValue;
    byte   receivedReserved1;
    byte   receivedReserved2;
    byte   receivedReserved3;
    byte   receivedReserved4;
    byte   receivedReserved5;
  } receiveBufferStruct;
  byte   receivedSerialCommand[10];
} receivedDataUnion;

union
{
  struct 
  {
    byte resultOfOperation;
    unsigned int value;
  } responseToPi_st;
  byte responseMessage[3];
} responseToPi_u;

//TODO: IDs for sensors and actuation devices to be defined earlier and read using a variable or #DEFINE

gtype_Sensor_st g_DepthSensor_st[NumOfDepthSensors]={{33,depth,0,1,0},{34,depth,0,1,1},{35,depth,0,1,2},{36,depth,0,1,3}};
gtype_Sensor_st g_TankBallSensor_st[NumOfBallSensors]={{71,ball,0,2,1},{72,ball,0,2,2},{73,ball,0,2,3},{74,ball,0,2,4},{75,ball,0,2,5},{76,ball,0,2,6},{77,ball,0,2,7}};
gtype_Sensor_st g_FlowSensor_st[NumOfFlowSensors]={{61,flow,0,1,4},{62,flow,0,1,5},{63,flow,0,1,6},{64,flow,0,1,7},{65,flow,0,2,0}};

gtype_Actuator_st g_Relay_st[NumOfRelays]={{51,relay,OFF,0,0},{52,relay,OFF,0,0},{53,relay,OFF,0,0},{54,relay,OFF,0,0},{55,relay,OFF,0,0},{56,relay,OFF,0,0},{57,relay,OFF,0,0},{58,relay,OFF,0,0}};

gtype_Sensor_st g_SensorToRead={0,unknownS,0,0,0};
gtype_Actuator_st g_DeviceToActuate = {0,unknownA,0,0,0};

void flowSensorCount()
{
 flowSensorCountPulses++; 
}

void muxEnable(byte _muxID)
{
  //TODO: Check MuxEnable should be LOW or HIGH
  digitalWrite(MuxEnablePin[_muxID],LOW);
}

void muxDisable(byte _muxID)
{
  //TODO: Check MuxEnable should be LOW or HIGH
  digitalWrite(MuxEnablePin[_muxID],HIGH);
}


void setMuxSlaveSelectLines(byte _slaveSelectID)
{
  //TODO: Check if original variable is also altered while shift operations are performed
  //TODO: Check HIGH and LOW values in Arduino.h. Is 0x01 HIGH or 0xFF?
  //TODO: Same to be checked for boolean also. Boolean used in writing to shiftregister
/*  digitalWrite(MuxSlaveSelectPins[2],(_slaveSelectID & ( 1 << 2 )) >> 2 );
  digitalWrite(MuxSlaveSelectPins[1],(_slaveSelectID & ( 1 << 1 )) >> 1 );
  digitalWrite(MuxSlaveSelectPins[0],(_slaveSelectID & ( 1 << 0 )) >> 0 );
  */
  digitalWrite(MuxSlaveSelectPins[2],bitRead(_slaveSelectID,2));
  digitalWrite(MuxSlaveSelectPins[1],bitRead(_slaveSelectID,1));
  digitalWrite(MuxSlaveSelectPins[0],bitRead(_slaveSelectID,0));
}

void setPinConfig()
{
  if(DEBUG)
    Serial.println("Set pin config");
  // MUX pins
  for (int i=0;i<NumOfMux;i++)
    pinMode(MuxEnablePin[i],OUTPUT);
  for (int i=0;i<3;i++)
    pinMode(MuxSlaveSelectPins[i],OUTPUT);      
  for (int i=0;i<NumOfMux;i++)
    pinMode(MuxLine[i],INPUT);  
  
  // Ultrasonic TRIG Pin
  pinMode(UltrasonicTRIGpin,OUTPUT);
}

void initializeActuators()
{
  if(DEBUG)
    Serial.println("Initialize Actuators");
}

void initializeSensors()
{
  //initialize ultrasonic sensors
  
}


boolean isCommandValid()
{
    if(DEBUG)
      {
        Serial.print("RecevivedDataUnion.ReceivedSerialCommand = ");
        for(unsigned short int i=0;i<10;i++)
          Serial.println(receivedDataUnion.receivedSerialCommand[i]);
        Serial.print("Received command = ");
        Serial.println(receivedDataUnion.receiveBufferStruct.receivedCommand);
      }
  switch(receivedDataUnion.receiveBufferStruct.receivedCommand)
  {

    case 0xADEA:// Command to READ sensor data: EAAD but unsigned int stores lower byte in higher index and higher byte in lower index and hence reversed
                switch(receivedDataUnion.receiveBufferStruct.receivedType)
                  {
                    case depth:// Identify depth sensor structure that contains ID received
                                if(DEBUG)
                                  Serial.println("Type: Depth");
                                for (int i=0;i<NumOfDepthSensors;i++)
                                {
                                  if(DEBUG)
                                    { Serial.print("Searching sensor index = ");Serial.println(i);Serial.print("SensorID = ");Serial.println(g_DepthSensor_st[i].sensorID);}
                                  if (g_DepthSensor_st[i].sensorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                  {
                                    sensorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                    readCommand=1;  
                                    return(EXIT_SUCCESS);
                                  }
                                }
                                errorHandler(IDforTypeNotFound);
                                return(EXIT_FAILURE);
                    case flow:// Identify flow sensor structure that contains ID received
                                if(DEBUG)
                                  Serial.println("Type: Flow");
                                for (int i=0;i<NumOfFlowSensors;i++)
                                {
                                  if(DEBUG)
                                    { Serial.print("Searching sensor index = ");Serial.println(i);Serial.print("SensorID = ");Serial.println(g_FlowSensor_st[i].sensorID);}
                                  if (g_FlowSensor_st[i].sensorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                  {
                                    sensorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                    readCommand=1;                                      
                                    return(EXIT_SUCCESS);
                                  }
                                }
                                errorHandler(IDforTypeNotFound);
                                return(EXIT_FAILURE);                               

                    case ball:// Identify ball sensor structure that contains ID received
                                if(DEBUG)
                                  Serial.println("Type: Ball");
                                for (int i=0;i<NumOfBallSensors;i++)
                                {
                                  if(DEBUG)
                                    { Serial.print("Searching sensor index = ");Serial.println(i);Serial.print("SensorID = ");Serial.println(g_TankBallSensor_st[i].sensorID);}
                                  if (g_TankBallSensor_st[i].sensorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                  {
                                    sensorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                    readCommand=1;                                      
                                    return(EXIT_SUCCESS);
                                  }
                                }
                                errorHandler(IDforTypeNotFound);
                                return(EXIT_FAILURE);
                    default: errorHandler(invalidType);
                             return(EXIT_FAILURE);
                  }
    case 0xAEAC:// Command to ACTUATE a device: ACAE but unsigned int stores lower byte in higher index and higher byte in lower index and hence reversed
                switch(receivedDataUnion.receiveBufferStruct.receivedType)
                  {
                    case relay:// Identify relay structure that contains ID received
                                 for (int i=0;i<NumOfRelays;i++)
                                  {
                                    if (g_Relay_st[NumOfRelays].actuatorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                    {
                                      actuatorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                      actuateCommand=1;                                      
                                      return(EXIT_SUCCESS);
                                    }
                                  }
                                  errorHandler(IDforTypeNotFound);
                                  return(EXIT_FAILURE);                                                    
                    default: errorHandler(invalidType);
                             return(EXIT_FAILURE);
                  }
    default: errorHandler(invalidCommand);
             return(EXIT_FAILURE); 
    //TODO: Check the flow of control after errorHandler is called. Should a flag be checked and exit with 'false' as return code immediately?
  }
  
}

boolean actuate(byte _actuatorIndex, unsigned short int _actuatorType,byte _valueToSet)
{
   switch(_actuatorType)
  {
    //TODO : check failures in sensor read operations
    case relay: motorControl(_actuatorIndex,_valueToSet);
    default:errorHandler(invalidType);
            return(EXIT_FAILURE);
    break;
  }
}

boolean sense(byte _sensorIndex, int _sensorType)
{
  if(DEBUG)
    Serial.println("Function call to sense sensor value");
   switch(_sensorType)
  {
    //TODO : check failures in sensor read operations
    case depth:
               if(DEBUG)
                 Serial.println("Sense function: case type: depth");
               g_DepthSensor_st[_sensorIndex].sensorValue=depthofWater(g_DepthSensor_st[_sensorIndex].muxID-1,g_DepthSensor_st[_sensorIndex].muxPosition);
               responseToPi_u.responseToPi_st.resultOfOperation=RESPOND_SUCCESS;
               responseToPi_u.responseToPi_st.value=g_DepthSensor_st[_sensorIndex].sensorValue;
               return(EXIT_SUCCESS);
    case flow:if(DEBUG)
                 Serial.println("Sense function: case type: flow");
              
              g_FlowSensor_st[_sensorIndex].sensorValue=RateofWaterFlow(g_FlowSensor_st[_sensorIndex].muxID-1,g_FlowSensor_st[_sensorIndex].muxPosition);
              
              responseToPi_u.responseToPi_st.resultOfOperation=RESPOND_SUCCESS;
              responseToPi_u.responseToPi_st.value=g_FlowSensor_st[_sensorIndex].sensorValue;
              if(DEBUG)
                Serial.println("Sense complete!");
              return(EXIT_SUCCESS);
    case ball:if(DEBUG)
                 Serial.println("Sense function: case type: ball");
              g_TankBallSensor_st[_sensorIndex].sensorValue=isTankBallPositionClosed(g_TankBallSensor_st[_sensorIndex].muxID-1,g_TankBallSensor_st[_sensorIndex].muxPosition);
              responseToPi_u.responseToPi_st.resultOfOperation=RESPOND_SUCCESS;
              responseToPi_u.responseToPi_st.value=g_TankBallSensor_st[_sensorIndex].sensorValue;
              return(EXIT_SUCCESS);
    default:errorHandler(invalidType);
            return(EXIT_FAILURE);
  }
}

void errorHandler(unsigned short int errorCode)
{
  if(DEBUG)
    {Serial.print("Error handler called with errorCode: ");Serial.println(errorCode);}
  responseToPi_u.responseToPi_st.resultOfOperation=RESPOND_FAILURE;
  responseToPi_u.responseToPi_st.value=errorCode;
}

boolean motorControl(byte _relayIndex, boolean OnorOFF)
{
  byte shiftRegData=0;
  g_Relay_st[_relayIndex].actuatorValue=OnorOFF;
  // Form Shift register data
  for (unsigned short int i=0;i<NumOfRelays;i++)
  {
 /*   if(g_Relay_st[i].actuatorValue == ON)
      shiftRegData = shiftRegData | i;
    else
      shiftRegData = shiftRegData & ~i;
  */
  bitWrite(shiftRegData,i,g_Relay_st[i].actuatorValue);
  }
  sendDataToShiftReg(shiftRegData);
  return EXIT_SUCCESS;
}

void sendDataToShiftReg(byte _shiftRegData)
{
  shiftReg1.writeData(_shiftRegData);
}

boolean isTankBallPositionClosed(byte _muxID, byte _muxPosition)
{
  boolean retVal;
  muxEnable(_muxID);
  setMuxSlaveSelectLines(_muxPosition);
  retVal=digitalRead(MuxLine[_muxID]);
  muxDisable(_muxID);
  return retVal;
}

int RateofWaterFlow(byte _muxID, byte _muxPosition)
{
 if(DEBUG)
   Serial.println("Function call: Rate of Water flow");
 int litresPerHour=0; 
 muxEnable(_muxID);
 setMuxSlaveSelectLines(_muxPosition);
 attachInterrupt(0, flowSensorCount, FALLING);
 delay(1000);
 detachInterrupt(0);
// TODO: Calibration required
 litresPerHour=flowSensorCountPulses*60/7.5;
 muxDisable(_muxID);
 return litresPerHour;
}

unsigned int depthofWater(byte _muxID, byte _muxPosition)
{
 unsigned int roundTripEchoTimeInUs=0,depthInCM=0;
 muxEnable(_muxID);
 setMuxSlaveSelectLines(_muxPosition);
 if(DEBUG)
   Serial.println("Function : depthofWater - before ping");
 roundTripEchoTimeInUs=depthSensor.ping();
 if(DEBUG)
   Serial.println("Function : depthofWater - ping complete");
 depthInCM=depthSensor.convert_cm(roundTripEchoTimeInUs);
 muxDisable(_muxID);
 return depthInCM;
}

void sendDataToPi(byte _responseToPi[])
{
 if(DEBUG)
  Serial.println("Sending Result to Server"); 
 for(unsigned short i=0;i<3;i++)
   Serial.write(_responseToPi[i]);
   
}

void clearAll()
{
  //TODO: Check if all necessary parameters are cleared
 // SensorsToRead,DevicesToActuate,receivedDataUnion,g_SensorToRead,actuatorIndexNow,sensorIndexNow
  for(int i=0;i<10;i++)
    receivedDataUnion.receivedSerialCommand[i]=0;
  commandComplete = false;
  sensorIndexNow=0xFF;
  actuatorIndexNow=0xFF;
  readCommand=0;
  actuateCommand=0;
  responseToPi_u.responseToPi_st.resultOfOperation=0;
  responseToPi_u.responseToPi_st.value=0;
  flowSensorCountPulses=0;
}


void setup()
{
  Serial.begin(115200);
  delay(200);
  if(DEBUG)
    {
      Serial.println("ON");
      Serial.println("Water Sensor and Control Unit");
      Serial.println("Line 2 : Water SCU");
      Serial.println("Line 3 : Water SCU");
      Serial.println("Line 4 : Water SCU");
      Serial.println("Line 5 : Water SCU");
    }
  setPinConfig();
  initializeActuators();
  initializeSensors();
  // Initialize shift register - sets pin configurations/mapping and clear shiftregister
  shiftReg1.srinit(ShiftRegPinSER,ShiftRegPinRCK,ShiftRegPinSRCK,ShiftRegPinSRCLR);
}

void loop()
{
  if(commandComplete == true)
    {
      if(DEBUG)
        Serial.println("Command complete=true");
      if (isCommandValid())
      {
        if(DEBUG)
          Serial.println("Command Valid!");
        if(readCommand)
          {
            if(DEBUG)
              Serial.println("Read Command construct");
            if(!sense(sensorIndexNow,receivedDataUnion.receiveBufferStruct.receivedType))
              errorHandler(senseError);
            else
              if(DEBUG)
                Serial.println("Read sensor value complete!");  
          }
        if(actuateCommand)
          {
            if(DEBUG)
              Serial.println("Read Command construct"); 
            if(!actuate(actuatorIndexNow,receivedDataUnion.receiveBufferStruct.receivedType,receivedDataUnion.receiveBufferStruct.receivedValue))
              errorHandler(ActuationError);
            else
              if(DEBUG)
                Serial.println("Actuate device complete!");  
          }
      }
     if(DEBUG)
       Serial.println("Next Step: Communicate result to server");
     sendDataToPi(responseToPi_u.responseMessage);
    }        
  clearAll();
}

void serialEvent() {
  static int i=0;
  int _unionindex=0;
  while (Serial.available()) {
    byte inChar = Serial.read(); 
    if (inChar == '\n') {
      commandComplete = true;
      i=0;
    } 
    else
    {
      delay(10);
      if(DEBUG)
      {
        Serial.print("inChar = ");
        Serial.println(inChar);
      }      
      receivedDataUnion.receivedSerialCommand[i]= inChar;
      i++;
    }
  }
}

