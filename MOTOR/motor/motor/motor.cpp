// Motor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
//#include "ControlMotor.h"
#include <iostream>
#include<math.h>
#include "CRs232.h"

CRs232 SerialPort;
	
int motorComm(/*CRs232 SerialPort,*/int motorId, int actionType, int motorSpeed)
{
	unsigned char sendBuffer[6];                //发送缓冲：6字节大小（参照协议）
	unsigned char receiveBuffer[4];             //接收缓冲：4字节大小 (参照协议)

	sendBuffer[0] = motorId + '0';              //int转换为char 
    sendBuffer[1] = actionType + '0'; 
    sendBuffer[2] = motorSpeed / 1000 + '0';
    sendBuffer[3] = ( motorSpeed % 1000 ) / 100 + '0';
    sendBuffer[4] = ( motorSpeed % 100) / 10 + '0';
	sendBuffer[5] = ( motorSpeed % 10) + '0';

	SerialPort.WriteCommBlock( sendBuffer, 6);        ///写串口数据

	Sleep(50);    ///采集返回数据的间隔时间

	SerialPort.ReadCommBlock( receiveBuffer, 4);     	///读串口数据

	if(('1' == receiveBuffer[0]) && ('1' == receiveBuffer[1]) && ('\r' == receiveBuffer[2]) && ('\n' == receiveBuffer[3]))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}


int _tmain(int argc, _TCHAR* argv[])
{
	/*
	CRs232 SerialPort;
	//if(argc!=2)return 1;
	//else{
	//SerialPort.OpenConnection();
	int i=10;
	while(i--){
	printf("%d\n",motor(SerialPort,0,0,i*1000));
	Sleep(100);
	}
	printf("%d\n",motor(SerialPort,0,3,0));
	//SerialPort.CloseConnection();
	//}
	*/

	
	SerialPort.OpenConnection("\\\\.\\COM29",9600,8,ONESTOPBIT,NOPARITY,0,0,0);
	motorComm(0,1,1000);
	Sleep(1000);
	motorComm(0,3,0000);

	return 0;
}

