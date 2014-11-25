
enum sensors_t {ball,depth,flow};
enum actuators_t {relay};

#define ON 1
#define OFF 0

// ERROR HANDLER CODES:
#define senseError 101
#define ActuationError 102

typedef struct
{
  byte         SensorID;
  sensors_t    SensorType;
  byte         SensorValue;
  byte         MuxID;
  byte         MuxPosition;
}gtype_Sensor_st;

typedef struct
{
  byte ActuatorID;
  actuators_t ActuatorType;
  boolean ActuatorValue;
  byte MuxID;
  byte MuxPosition;
}gtype_Actuator_st;

gtype_Sensor_st g_DepthSensor_st[6]={{33,depth,0,0,0},{34,depth,0,0,1},{35,depth,0,0,2},{36,depth,0,0,3},{37,depth,0,0,4},{38,depth,0,0,5}};
gtype_Sensor_st g_TankBallSensor_st[8]={{71,ball,0,1,0},{72,ball,0,1,1},{73,ball,0,1,2},{74,ball,0,1,3},{75,ball,0,1,4},{76,ball,0,1,5},{77,ball,0,1,6},{78,ball,0,1,7}};
gtype_Sensor_st g_FlowSensor_st[8]={{61,flow,0,2,0},{62,flow,0,2,1},{63,flow,0,2,2},{64,flow,0,2,3},{65,flow,0,2,4},{66,flow,0,2,5},{67,flow,0,2,6},{68,flow,0,2,7}};

gtype_Actuator_st g_Relay_st[8]={51,relay,OFF,0,0};

boolean commandComplete = false; 
uint8_t commandReceived[10] = {0};

boolean isCommandValid()
{
  
}

boolean actuate()
{
  
}

boolean sense(byte ID, sensors_t type)
{
  
}

void errorhandler()
{
  
}

boolean motorControl(byte motorID, boolean OnorOFF)
{
  
}

boolean isTankBallPositionClosed()
{
  
}

int RateofWaterFlow()
{
  
}

int depthofWater()
{
  
}

void clearAll()
{
 // SensorsToRead,DevicesToActuate
  for(int i=0;i<10;i++)
    commandReceived[i]=0;
  commandComplete = false;
}


void setup()
{
  Serial.begin(115000);
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
            if(!actuate(toActuateID,toActuateType))
              errorHandler(ActuationError);
          }
      }
              
  clearAll();
}

void serialEvent() {
  static int i=0;
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') {
      commandComplete = true;
      i=0;
    } 
    else
    {
      commandReceived[i]= inChar;
      i++;
    }
  }
}
