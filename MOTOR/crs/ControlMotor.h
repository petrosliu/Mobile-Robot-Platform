/***********************************************************
VER:Version 1.0
DATE: 2011.7.26


***********************************************************/
#include <stdio.h>
#include<math.h>
#include "CRs232.h"


//ȫ�ֱ���, ��������βһ��Ҫ�ǵùرմ��ںš�SerialPort.CloseConnection()
CRs232 SerialPort;


/*=======================================================================
Function:        ���Ƶ���˶�
Prototype:        int motor(int motorId, int actionType, int motorSpeed)
    @param:        motorId: 0 ������1 �ҵ��
    @param:        actionType: 0 ǰ���� 1 ���ˣ� 2 ɲ���� 3 ֹͣ
    @param:        motorSpeed: 0-100, �������ٶ�
    @return:       �ɹ�����0�����򷵻�-1
Description:   
=======================================================================*/
int motor(/*CRs232 &SerialPort,*/int motorId, int actionType, int motorSpeed)
{
	unsigned char sendBuffer[6];                //���ͻ��壺6�ֽڴ�С������Э�飩
	unsigned char receiveBuffer[4];             //���ջ��壺4�ֽڴ�С (����Э��)

	sendBuffer[0] = motorId + '0';              //intת��Ϊchar 
    sendBuffer[1] = actionType + '0'; 
    sendBuffer[2] = motorSpeed / 1000 + '0';
    sendBuffer[3] = ( motorSpeed % 1000 ) / 100 + '0';
    sendBuffer[4] = ( motorSpeed % 100) / 10 + '0';
	sendBuffer[5] = ( motorSpeed % 10) + '0';

	SerialPort.WriteCommBlock( sendBuffer, 6);        ///д��������

	Sleep(50);    ///�ɼ��������ݵļ��ʱ��

	SerialPort.ReadCommBlock( receiveBuffer, 4);     	///����������

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
Function:        ��ȡ���ת���Ƕ�
Prototype:        int getMotorAngle(int motorId)
    @param:        motorId: 0 ������1 �ҵ��
    @return:    ���һ��ת���ĽǶ�
Description:   
=======================================================================*/
double  getMotorAngle(/*CRs232 &SerialPort,*/int motorId)
{
	unsigned char sendBuffer[6] = {'0','4','#','#','#','#'};        //���ͻ��壺6�ֽڴ�С������Э�飩
	unsigned char receiveBuffer[16] = {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'};         //���ջ�����,16�ֽ�ǰ�������ֽ�'\r\n'  ���յ������ֵΪ'\r'+'\n'+2147483647+'\r'+'\n'

	sendBuffer[0] = motorId + '0';                                  //intת��Ϊchar 


	SerialPort.WriteCommBlock( sendBuffer, 6);                   //д��������

	Sleep(50);												   //�ɼ��������ݵļ��ʱ��

	SerialPort.ReadCommBlock( receiveBuffer, 16);              //����������

	double angle = 0;
	int firstIndex = 0;    //��һ����ǵ�λ��
	int secondIndex = 0;   //�ڶ�����ǵ�λ�� 

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
					goto loop;  //�ҵ�λ������ѭ��,ִ����һ�����
				}
			}
		}
	}

	loop:for(int n = firstIndex ; n <= secondIndex; n++)
	{
		angle += (receiveBuffer[firstIndex] - '0') * pow(10.0, secondIndex - firstIndex);
		firstIndex++;
	}

	return ( angle * 360 / ( 16 * 131 ) );  //�����ֲ���;16��2������������ٱ�1:131

}