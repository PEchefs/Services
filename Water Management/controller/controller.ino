#include <shiftReg.h>
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

// Mux Pin assigments : Enable Pins, Slave Select Pins (commonly connected to both Multiplexers)
const unsigned short int NumOfMux = 2;
const int MuxEnablePin[NumOfMux]={6,10};
const int MuxLine[NumOfMux]={2,10};
const int MuxSlaveSelectPins[3]={9,8,7};

// Shift Register Pin assigments: SCL, SCK, SER, RCK
const int ShiftRegPinSCL = 3;
const int ShiftRegPinSCK = 4;
const int ShiftRegPinSER = 5;
const int ShiftRegPinRCK = 11;
shiftReg shiftReg1;

// Ultrasonic sensor(depth sensor) TRIG line
const int UltrasonicTRIG = A1;

const unsigned short int NumOfDepthSensors=4;
const unsigned short int NumOfFlowSensors=5;
const unsigned short int NumOfBallSensors=7;
const unsigned short int NumOfRelays=8;

enum sensors_t {ball,depth,flow,unknownS};
enum actuators_t {relay,unknownA};
#define ON 1
#define OFF 0

#define EXIT_SUCCESS 1
#define EXIT_FAILURE 0

// ERROR HANDLER CODES:
#define senseError 101
#define ActuationError 102
#define invalidCommand 103
#define invalidType 104
#define IDforTypeNotFound 105

typedef struct
{
  byte         sensorID;
  sensors_t    sensorType;
  byte         sensorValue;
  byte         muxID;
  byte         muxPosition;
}gtype_Sensor_st;

typedef struct
{
  byte actuatorID;
  actuators_t actuatorType;
  boolean actuatorValue;
  byte muxID;
  byte muxPosition;
}gtype_Actuator_st;

union
{
  struct
  {
  int receivedCommand;
  byte receivedID;
  byte receivedType;
  byte receivedValue;
  byte receivedReserved1;
  byte receivedReserved2;
  byte receivedReserved3;
  byte receivedReserved4;
  byte receivedReserved5;
  } receiveBufferStruct;
  byte receivedSerialCommand[10];
}receivedDataUnion;

//TODO: IDs for sensors and actuation devices to be defined earlier and read using a variable or #DEFINE

gtype_Sensor_st g_DepthSensor_st[NumOfDepthSensors]={{33,depth,0,1,0},{34,depth,0,1,1},{35,depth,0,1,2},{36,depth,0,1,3}};
gtype_Sensor_st g_TankBallSensor_st[NumOfBallSensors]={{71,ball,0,2,1},{72,ball,0,2,2},{73,ball,0,2,3},{74,ball,0,2,4},{75,ball,0,2,5},{76,ball,0,2,6},{77,ball,0,2,7}};
gtype_Sensor_st g_FlowSensor_st[NumOfFlowSensors]={{61,flow,0,1,4},{62,flow,0,1,5},{63,flow,0,1,6},{64,flow,0,1,7},{65,flow,0,2,0}};
gtype_Sensor_st g_SensorToRead={0,unknownS,0,0,0};

gtype_Actuator_st g_Relay_st[NumOfRelays]={{51,relay,OFF,0,0},{52,relay,OFF,0,0},{53,relay,OFF,0,0},{54,relay,OFF,0,0},{55,relay,OFF,0,0},{56,relay,OFF,0,0},{57,relay,OFF,0,0},{58,relay,OFF,0,0}};
gtype_Actuator_st g_DeviceToActuate = {0,unknownA,0,0,0};


boolean commandComplete = false; 

unsigned short int NumberofSensorReadsRequested=0;
unsigned short int NumberofDeviceActuationsRequested=0;
byte toSenseID;
byte toSenseType;
byte SensorsToRead[8][8]={};
byte toActuateID;
byte toActuateType;
byte toActuateValue;
byte DevicesToActuate[8][8]={};
int sensorIndexNow=0xFF;
int actuatorIndexNow=0xFF;

void setPinConfig()
{
  // MUX pins
  for (int i=0;i<NumOfMux;i++)
    pinMode(MuxEnablePin[i],OUTPUT);
  for (int i=0;i<3;i++)
    pinMode(MuxSlaveSelectPins[i],OUTPUT);      
  for (int i=0;i<NumOfMux;i++)
    pinMode(MuxLine[i],INPUT);  
  
  // Shift Register Pins  
  pinMode(ShiftRegPinSCL,OUTPUT);
  pinMode(ShiftRegPinSCK,OUTPUT);
  pinMode(ShiftRegPinSER,OUTPUT);
  pinMode(ShiftRegPinRCK,OUTPUT);
  
  // Ultrasonic TRIG Pin
  pinMode(UltrasonicTRIG,OUTPUT);
}

void initializeActuators()
{
  
}

void initializeSensors()
{
  
}


boolean isCommandValid()
{
  switch(receivedDataUnion.receiveBufferStruct.receivedCommand)
  {
    case 0xEAAD:// Command to READ sensor data
                switch(receivedDataUnion.receiveBufferStruct.receivedType)
                  {
                    case depth:// Identify depth sensor structure that contains ID received
                                for (int i=0;i<NumOfDepthSensors;i++)
                                {
                                  if (g_DepthSensor_st[NumOfDepthSensors].sensorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                  {
                                    sensorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                    return(EXIT_SUCCESS);
                                  }
                                }
                                errorHandler(IDforTypeNotFound);
                                return(EXIT_FAILURE);
                    case flow:// Identify flow sensor structure that contains ID received
                                for (int i=0;i<NumOfFlowSensors;i++)
                                {
                                  if (g_FlowSensor_st[NumOfFlowSensors].sensorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                  {
                                    sensorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                    return(EXIT_SUCCESS);
                                  }
                                }
                                errorHandler(IDforTypeNotFound);
                                return(EXIT_FAILURE);                               

                    case ball:// Identify ball sensor structure that contains ID received
                                for (int i=0;i<NumOfBallSensors;i++)
                                {
                                  if (g_TankBallSensor_st[NumOfBallSensors].sensorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                  {
                                    sensorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
                                    return(EXIT_SUCCESS);
                                  }
                                }
                                errorHandler(IDforTypeNotFound);
                                return(EXIT_FAILURE);
                    default: errorHandler(invalidType);
                             return(EXIT_FAILURE);
                  }
    case 0xACAE:// Command to ACTUATE a device
                switch(receivedDataUnion.receiveBufferStruct.receivedType)
                  {
                    case relay:// Identify relay structure that contains ID received
                                 for (int i=0;i<NumOfRelays;i++)
                                  {
                                    if (g_Relay_st[NumOfRelays].actuatorID == receivedDataUnion.receiveBufferStruct.receivedID)
                                    {
                                      actuatorIndexNow=i;
                                      //TODO: Check the above statement's validity/working. Or else simply save index and use type again in sense or actuate function 
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
   switch(_sensorType)
  {
    //TODO : check failures in sensor read operations
    case depth:g_DepthSensor_st[_sensorIndex].sensorValue=depthofWater(g_DepthSensor_st[_sensorIndex].muxID,g_DepthSensor_st[_sensorIndex].muxPosition);break;
    case flow:g_FlowSensor_st[_sensorIndex].sensorValue=RateofWaterFlow(g_FlowSensor_st[_sensorIndex].muxID,g_FlowSensor_st[_sensorIndex].muxPosition);break;
    case ball:g_TankBallSensor_st[_sensorIndex].sensorValue=isTankBallPositionClosed(g_TankBallSensor_st[_sensorIndex].muxID,g_TankBallSensor_st[_sensorIndex].muxPosition);break;
    default:errorHandler(invalidType);
            return(EXIT_FAILURE);
    break;
  }
}

void errorHandler(unsigned short int errorCode)
{
  
}

boolean motorControl(byte _relayIndex, boolean OnorOFF)
{
  byte shiftRegData=0;
  g_Relay_st[_relayIndex].actuatorValue=OnorOFF;
  // Form Shift register data
  for (unsigned short int i=0;i<NumOfRelays;i++)
  {
    if(g_Relay_st[i].actuatorValue == ON)
      shiftRegData = shiftRegData | i;
    else
      shiftRegData = shiftRegData & ~i;
  }
  sendDataToShiftReg(shiftRegData);
  return EXIT_SUCCESS;
}

boolean sendDataToShiftReg(byte _shiftRegData)
{
  
}

boolean isTankBallPositionClosed(byte _muxID, byte _muxPosition)
{
  
}

int RateofWaterFlow(byte _muxID, byte _muxPosition)
{
  
}

int depthofWater(byte _muxID, byte _muxPosition)
{
  
}

void clearAll()
{
 // SensorsToRead,DevicesToActuate,receivedDataUnion,g_SensorToRead,actuatorIndexNow,sensorIndexNow
  for(int i=0;i<10;i++)
    receivedDataUnion.receivedSerialCommand[i]=0;
  commandComplete = false;
  sensorIndexNow=0xFF;
  actuatorIndexNow=0xFF;
}


void setup()
{
  Serial.begin(115000);
  setPinConfig();
  initializeActuators();
  initializeSensors();
}

void loop()
{
  if(commandComplete == true)
    if (isCommandValid())
      {
        for (unsigned short int readIndex=0;readIndex < NumberofSensorReadsRequested;readIndex++)
          {
            toSenseID = SensorsToRead[readIndex][0];
            toSenseType = SensorsToRead[readIndex][1];
            if(!sense(toSenseID,toSenseType))
              errorHandler(senseError);
          }
          
        for (unsigned short int actuateIndex=0;actuateIndex < NumberofDeviceActuationsRequested;actuateIndex++)
          {
            toActuateID = DevicesToActuate[actuateIndex][0];
            toActuateType = DevicesToActuate[actuateIndex][1];
            if(!actuate(toActuateID,toActuateType,toActuateValue))
              errorHandler(ActuationError);
          }
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
      receivedDataUnion.receivedSerialCommand[i]= inChar;
      i++;
    }
  }
}

