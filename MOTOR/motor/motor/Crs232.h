/***********************************************************
VER:Version 1.0
DATE: 2011.7.26


***********************************************************/
#ifndef _CRS232_H_
#define _CRS232_H_

#include <windows.h>
#include <Winbase.h> 
#include <windef.h> 

#pragma once
//#define _CRS232_H_
#define RXQUEUE         4096����// ���ڽ��ջ�������С
#define TXQUEUE         1024    // ���ڷ��ͻ�������С
#define MAXBLOCK        1024
class CRs232
{
public: 
	 CRs232(); 
	 ~CRs232(); 

	BOOL m_bConnected;		//�����Ƿ��Ѿ���

    BOOL  IsConnected();   //�жϴ����Ƿ��Ѿ���

	HANDLE m_hComDev;      // �����豸���

	OVERLAPPED m_osRead, m_osWrite;

	HANDLE m_hThreadTerm;  // �¼����������ͨѶ�߳��Ƿ���ֹ��ͬ������


	int OpenConnection(LPCTSTR szComPort ="COM29", DWORD dwBaudRate =9600, BYTE cBycByteSize=8, BYTE cStopBits=ONESTOPBIT, BYTE cParity=NOPARITY, BOOL bDTR_DSR=0, BOOL bRTS_CTS=0, BOOL bXON_XOFF=0);// �򿪲����ô��� 
	
	BOOL CloseConnection();  // �رա��ͷŴ��� 
	

	int ReadCommBlock(unsigned char* lpszBlock, int nMaxLength); // �Ӵ��ڶ�����

	int WriteCommBlock( LPCVOID lpByte, UINT nBytesToWrite);     //������д���� 


};


#endif