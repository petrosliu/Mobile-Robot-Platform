/***********************************************************
VER:Version 1.0
DATE: 2011.7.26


***********************************************************/
#include <stdio.h>
#include<math.h>
#include "CRs232.h"


//全局变量, 主函数结尾一定要记得关闭串口号。SerialPort.CloseConnection()
CRs232 SerialPort;


/*=======================================================================
Function:        控制电机运动
Prototype:        int motor(int motorId, int actionType, int motorSpeed)
    @param:        motorId: 0 左电机；1 右电机
    @param:        actionType: 0 前进； 1 后退； 2 刹车； 3 停止
    @param:        motorSpeed: 0-100, 代表电机速度
    @return:       成功返回0，否则返回-1
Description:   
=======================================================================*/
int motor(/*CRs232 &SerialPort,*/int motorId, int actionType, int motorSpeed)
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
 

/*=======================================================================
Function:        获取电机转动角度
Prototype:        int getMotorAngle(int motorId)
    @param:        motorId: 0 左电机；1 右电机
    @return:    最近一次转动的角度
Description:   
=======================================================================*/
double  getMotorAngle(/*CRs232 &SerialPort,*/int motorId)
{
	unsigned char sendBuffer[6] = {'0','4','#','#','#','#'};        //发送缓冲：6字节大小（参照协议）
	unsigned char receiveBuffer[16] = {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'};         //接收缓冲区,16字节前后两个字节'\r\n'  接收到的最大值为'\r'+'\n'+2147483647+'\r'+'\n'

	sendBuffer[0] = motorId + '0';                                  //int转换为char 


	SerialPort.WriteCommBlock( sendBuffer, 6);                   //写串口数据

	Sleep(50);												   //采集返回数据的间隔时间

	SerialPort.ReadCommBlock( receiveBuffer, 16);              //读串口数据

	double angle = 0;
	int firstIndex = 0;    //第一个标记的位置
	int secondIndex = 0;   //第二个标记的位置 

	for(int j = 0; j < 16; j++)
	{
		if(('$' == receiveBuffer[j]) && ('\r' == receiveBuffer[j + 1]) && ('\n' == receiveBuffer[j + 2]))
		{
			for (int k = j ; k< 16; k++)
			{
				if(('\r' == receiveBuffer[k]) && ('\n' == receiveBuffer[k + 1]) && ('$' == receiveBuffer[k + 2]))
				{
					firstIndex = j + 3;
					secondIndex = k -1;
					goto loop;  //找到位置跳出循环,执行下一条语句
				}
			}
		}
	}

	loop:for(int n = firstIndex ; n <= secondIndex; n++)
	{
		angle += (receiveBuffer[firstIndex] - '0') * pow(10.0, secondIndex - firstIndex);
		firstIndex++;
	}

	return ( angle * 360 / ( 16 * 131 ) );  //编码轮参数;16线2相编码器，减速比1:131

}