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
#define RXQUEUE         4096　　// 串口接收缓冲区大小
#define TXQUEUE         1024    // 串口发送缓冲区大小
#define MAXBLOCK        1024
class CRs232
{
public: 
	 CRs232(); 
	 ~CRs232(); 

	BOOL m_bConnected;		//串口是否已经打开

    BOOL  IsConnected();   //判断串口是否已经打开

	HANDLE m_hComDev;      // 串口设备句柄

	OVERLAPPED m_osRead, m_osWrite;

	HANDLE m_hThreadTerm;  // 事件句柄：控制通讯线程是否终止的同步对象


	int OpenConnection(LPCTSTR szComPort ="COM29", DWORD dwBaudRate =9600, BYTE cBycByteSize=8, BYTE cStopBits=ONESTOPBIT, BYTE cParity=NOPARITY, BOOL bDTR_DSR=0, BOOL bRTS_CTS=0, BOOL bXON_XOFF=0);// 打开并配置串口 
	
	BOOL CloseConnection();  // 关闭、释放串口 
	

	int ReadCommBlock(unsigned char* lpszBlock, int nMaxLength); // 从串口读数据

	int WriteCommBlock( LPCVOID lpByte, UINT nBytesToWrite);     //往串口写数据 


};


#endif