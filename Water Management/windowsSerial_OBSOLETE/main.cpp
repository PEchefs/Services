#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include<string.h>
#include "rs232.h"

#define MAX_NUMBER_OF_SENSORS 16
#define MAX_NUMBER_OF_RELAYS 8

#define senseError 101         //0x65
#define ActuationError 102     //0x66
#define invalidCommand 103     //0x67
#define invalidType 104        //0x68
#define IDforTypeNotFound 105  //0x69

unsigned short NUMBER_OF_SENSORS=0;
unsigned short NUMBER_OF_RELAYS=0;
char sensorInfoFilePath[]="sensorsInfo2.txt";
char sensorDataFilePath[]="sensorsData.txt";
char relayDataFilePath[]="relaysData.txt";
char relayInfoFilePath[]="relaysInfo.txt";
char errorLogFilePath[]="errorLog.txt";
unsigned short COMport=0;

struct {
    char sensorID[3];
    char sensorType[3];
} sensorReadString;


struct {
    char relayID[3];
    char relayType[3];
} relayReadString;

struct {
    unsigned char sensorID;
    unsigned char sensorType;
    int sensorValue=-1;
} sensorTable[MAX_NUMBER_OF_SENSORS];

struct {
    unsigned char relayID;
    unsigned char relayType;
    int relayValue=-1;
} relayTable[MAX_NUMBER_OF_RELAYS];


/*
union{
        struct {
            unsigned short val;
            unsigned char responseCode;
            }data;
        unsigned char responseString[3];
} response;
*/
unsigned short responseVal;
unsigned char responseCode;
unsigned char responseString[3];

void clearResponse()
{
    unsigned short int i=0;
    for (i=0;i<3;i++)
        responseString[i]=0;
    unsigned short int readSensorFlag=0;
    unsigned short int relayOperateFlag=0;
}

int readSensorsInfo(char* sensorInfoFilePath)
{
    FILE *sensorInfo;
    unsigned short int i=0;
    sensorInfo = fopen(sensorInfoFilePath,"r");
    if(sensorInfo == NULL)
        return -1;
    while (fscanf(sensorInfo,"%s",sensorReadString.sensorID)!=EOF)
    {
        if(sensorReadString.sensorID[0] == '#')
        { char null[255];
            fgets(null, 255, sensorInfo);
            continue;
        }
        //printf("%d: sensorID: %s\n",i,sensorReadString.sensorID);
        fscanf(sensorInfo,"%s",sensorReadString.sensorType);
        //printf("%d: sensorType: %s\n",i,sensorReadString.sensorType);
        sensorTable[i].sensorID=atoi(sensorReadString.sensorID);
        sensorTable[i].sensorType=atoi(sensorReadString.sensorType);
        printf("%d: sensorID: %d\n",i,sensorTable[i].sensorID);
        printf("%d: sensorType: %d\n",i,sensorTable[i].sensorType);
        i++;
    }
	NUMBER_OF_SENSORS=i;
    fclose(sensorInfo);
    return 1;
}

int readRelayInfo(char* relayInfoFilePath)
{
    FILE *relayInfo;
    unsigned short int i=0;
    relayInfo = fopen(relayInfoFilePath,"r");
    if(relayInfo == NULL)
        return -1;
    while (fscanf(relayInfo,"%s",relayReadString.relayID)!=EOF)
    {
        if(relayReadString.relayID[0] == '#')
        { char null[255];
            fgets(null, 255, relayInfo);
            continue;
        }

        fscanf(relayInfo,"%s",relayReadString.relayType);

        relayTable[i].relayID=atoi(relayReadString.relayID);
        relayTable[i].relayType=atoi(relayReadString.relayType);
        printf("%d: relayID: %d\n",i,relayTable[i].relayID);
        printf("%d: relayType: %d\n",i,relayTable[i].relayType);
        i++;
    }
	NUMBER_OF_RELAYS=i;
    fclose(relayInfo);
    return 1;
}

int writeSensorsInfo(char* sensorDataFilePath)
{
    FILE *sensorInfo;
    unsigned short int i=0;
    sensorInfo = fopen(sensorDataFilePath,"a");
    time_t mytime;
    mytime = time(NULL);
    fprintf(sensorInfo,"%ld",mytime);
    for( int index=0;index<NUMBER_OF_SENSORS;index++)
    {
        fprintf(sensorInfo,",%d,%d,%d",sensorTable[index].sensorID,sensorTable[index].sensorType,sensorTable[index].sensorValue);
    }
    fprintf(sensorInfo,";\n");
    fclose(sensorInfo);
    return 1;
}

int writeRelayInfo(char* relayDataFilePath, unsigned short index)
{
    printf("Called Relay Info write routine!");
    FILE *relayInfo;
    //unsigned short int i=0;
    relayInfo = fopen(relayDataFilePath,"a");
    if(relayInfo==NULL)
    {
        printf("Relay Info file pointer null");
    }
    time_t mytime;
    mytime = time(NULL);
    printf("Relay info: ");
    printf("%ld,%d,%d",mytime,relayTable[index].relayID,relayTable[index].relayValue);
    int ret=fprintf(relayInfo,"%ld",mytime);
    if(ret<0)
        printf("Write Failed!\n");
    else
        printf("Bytes written:%d\n",ret);
    ret=fprintf(relayInfo,",%d,%d",relayTable[index].relayID,relayTable[index].relayValue);
    if(ret<0)
        printf("Write Failed!\n");
    else
        printf("Bytes written:%d\n",ret);
    ret=fprintf(relayInfo,";\n");
    if(ret<0)
        printf("Write Failed!\n");
    else
        printf("Bytes written:%d\n",ret);
    fclose(relayInfo);
    return 1;
}

int writeErrorLog(char* sensorDataFilePath,unsigned char ID,unsigned char Type,char* errorText)
{
    FILE *errorLog;
    unsigned short int i=0;
    errorLog = fopen(sensorDataFilePath,"a");
    time_t mytime;
    mytime = time(NULL);
    fprintf(errorLog,"%ld:\t ID: %d Type: %d Error: %s\n",mytime,ID,Type,errorText);
    fclose(errorLog);
    return 1;
}

int main(int argc, char *argv[])
{
    char mode[]={'8','N','1',0};
    int bdrate=9600,retCode=0;
    unsigned short int index=0;
    unsigned short int readSensorFlag=0,relayOperateFlag=0;
    unsigned char relayIndex=100;
    unsigned char relayState=0;
	COMport=atoi(argv[1])-1;

    if (argc == 2)
    {
        readSensorFlag=1;
        relayOperateFlag=0;

    }
    else if (argc == 4)
    {
        relayOperateFlag=1;
        readSensorFlag=0;
		relayIndex=atoi(argv[2]);
		relayState=atoi(argv[3]);
    }
    else
    {
        printf("COM port not specified!!!\n");
        printf("Usage:\n");
        printf("readSensors_win.c <COM port>\n");
        EXIT_FAILURE;
    }
   // strcat(COMport,argv[1]);


    if (readSensorFlag == 1)
    {



	// Read SenorsInfo file to obtain information regarding SensorIDs and SensorType to query for data
		if(!readSensorsInfo(sensorInfoFilePath))
			{
				printf("ERROR!!! Opening Sensor Info file failed!\n");
				writeErrorLog(errorLogFilePath,-1,-1,"ERROR!!! Opening Sensor Info file failed!\n");
				EXIT_FAILURE;
			}

		if(RS232_OpenComport(COMport, bdrate, mode))
		{
			printf("ERROR!!! Could not open COM port! %d\n",COMport+1);
			writeErrorLog(errorLogFilePath,-1,-1,"ERROR!!! Could not open COM port!");
			EXIT_FAILURE;
		}
 /*
		responseString[0]=0xC9;
		responseString[1]=1;
		responseString[2]=33;


		//responseCode=1;
		//responseVal=22;
		printf("respose.data.responseCode=%d\t",responseCode);
		printf("respose.data.val=%d\t",responseVal);
*/
		for(index=0;index<NUMBER_OF_SENSORS;index++)
		{
			unsigned char commandToSend[5]={};
			commandToSend[0]=0xEA;
			commandToSend[1]=0xAD;
			commandToSend[2]=sensorTable[index].sensorID;
			commandToSend[3]=sensorTable[index].sensorType;
			commandToSend[4]=0x0A;
			if(RS232_SendBuf(COMport,commandToSend,5) == -1)
			{
				printf("Sending Serial Data failed!");
				writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"Sending Serial Data failed!");
				EXIT_FAILURE;
			}
			Sleep(2000);
			int n = RS232_PollComport(COMport, responseString, 3);
			if(n == 0)
			{
				printf("Warning! No data received for ID:%d Type:%d\n",sensorTable[index].sensorID,sensorTable[index].sensorType);
				writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"Warning!!! No data received!");
				continue;
			}
			if(n == 3)
		    {
				responseCode=responseString[0];
				responseVal=responseString[1]+(responseString[2]*256);
			/*
				printf("responseString[0]=%X\n",responseString[0]);
				printf("responseString[1]=%X\n",responseString[1]);
				printf("responseString[2]=%X\n",responseString[2]);
				*/
				printf ("%d \t",index);
				printf("%X \t",responseCode);
				printf("%d \n",responseVal);
				if(responseCode==0xC9)
					sensorTable[index].sensorValue=responseVal;
				else if (responseCode==0xCA)
				{
					switch(responseVal)
					{
						case senseError:        writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"Sense Error!");
												break;
						case ActuationError:    writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"Actuation Error!");
												break;
						case invalidCommand:    writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"Invalid Command Error!");
												break;
						case invalidType:       writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"Invalid Type Error!");
												break;
						case IDforTypeNotFound: writeErrorLog(errorLogFilePath,sensorTable[index].sensorID,sensorTable[index].sensorType,"ID for Type Not Found Error!");
												break;
						default:break;
					}
				}
			}
		}

		writeSensorsInfo(sensorDataFilePath);
		EXIT_SUCCESS;
		}
		else if (relayOperateFlag == 1)
		{

		// Read SenorsInfo file to obtain information regarding SensorIDs and SensorType to query for data
		if(!readRelayInfo(relayInfoFilePath))
			{
				printf("ERROR!!! Opening Relay Info file failed!\n");
				writeErrorLog(errorLogFilePath,-1,-1,"ERROR!!! Opening Relay Info file failed!\n");
				EXIT_FAILURE;
			}

		if(RS232_OpenComport(COMport, bdrate, mode))
		{
			printf("ERROR!!! Could not open COM port! %d\n",COMport+1);
			writeErrorLog(errorLogFilePath,-1,-1,"ERROR!!! Could not open COM port!");
			EXIT_FAILURE;
		}

			unsigned char commandToSend[6]={};
			commandToSend[0]=0xAC;
			commandToSend[1]=0xAE;
			commandToSend[2]=relayTable[relayIndex].relayID;
			commandToSend[3]=relayTable[relayIndex].relayType;
			commandToSend[4]=relayState;
			commandToSend[5]=0x0A;

			if(RS232_SendBuf(COMport,commandToSend,6) == -1)
			{
				printf("Sending Serial Data failed!");
				writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"Sending Serial Data failed!");
				EXIT_FAILURE;
			}
			Sleep(2000);
			int n = RS232_PollComport(COMport, responseString, 3);
			if(n == 0)
			{
				printf("Warning! No data received for ID:%d Type:%d\n",relayTable[relayIndex].relayID,relayTable[relayIndex].relayType);
				writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"Warning!!! No data received!");
				EXIT_FAILURE;
			}
			if(n == 3)
			{
				responseCode=responseString[0];
				responseVal=responseString[1]+(responseString[2]*256);
			/*
				printf("responseString[0]=%X\n",responseString[0]);
				printf("responseString[1]=%X\n",responseString[1]);
				printf("responseString[2]=%X\n",responseString[2]);
				*/
				printf ("%d \t",relayIndex);
				printf("%X \t",responseCode);
				printf("%d \n",responseVal);
				if(responseCode==0xC9)
					relayTable[relayIndex].relayValue=responseVal;
				else if (responseCode==0xCA)
				{
					switch(responseVal)
					{
						case senseError:        writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"Sense Error!");
												break;
						case ActuationError:    writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"Actuation Error!");
												break;
						case invalidCommand:    writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"Invalid Command Error!");
												break;
						case invalidType:       writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"Invalid Type Error!");
												break;
						case IDforTypeNotFound: writeErrorLog(errorLogFilePath,relayTable[relayIndex].relayID,relayTable[relayIndex].relayType,"ID for Type Not Found Error!");
												break;
						default:break;
					}
				}
			}


		writeRelayInfo(relayDataFilePath,relayIndex);

		EXIT_SUCCESS;
		}
}
