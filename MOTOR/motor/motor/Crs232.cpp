/***********************************************************
VER:Version 1.0
DATE: 2011.7.26


***********************************************************/
#include "stdafx.h"
#include <stdio.h>
#include "Crs232.h"
#include "ControlMotor.h"


CRs232::CRs232(void)
{
	 memset(  &m_osWrite,  0,  sizeof(OVERLAPPED)  )  ;
	 memset(  &m_osRead,  0,  sizeof(OVERLAPPED)  )  ;
	 m_bConnected  =  FALSE;

	 m_hThreadTerm  =  CreateEvent(  NULL,  FALSE,  FALSE,  NULL  ); 

	 m_osRead.hEvent  =  CreateEvent(  NULL,  TRUE,  FALSE,  NULL  );  
	 m_osWrite.hEvent  =  CreateEvent(  NULL,  TRUE,  FALSE,  NULL  );  
}

CRs232::~CRs232(void)
{
	 if( m_bConnected )          //    如果已打开串口
	 {
	   CloseConnection();            //  关闭、释放串口
	 }
	 CloseHandle(  m_hThreadTerm  ); 
	 CloseHandle(  m_osRead.hEvent)  ;
	 CloseHandle(  m_osWrite.hEvent)  ;
}

/*=======================================================================
Function:		打开串口设备
Prototype:		int OpenConnection(  LPCTSTR  szComPort, DWORD  dwBaudRate, BYTE  cByteSize,   
                BYTE  cStopBits,BYTE  cParity,BOOL  bDTR_DSR, BOOL  bRTS_CTS,BOOL  bXON_XOFF  )
	@param:		szComPort选择串口号，默认为
	@param:     dwBaudRate设置波特率
	@param:     cByteSize设置数据位  
	@param:		cStopBits设置停止位 
	@param:     cParity设置奇偶校验位 
	@param:		bDTR_DSR设置DSR线硬件控制
	@param:		bRTS_CTS设置CTS硬件控制
	@param:		bXON_XOFF设置软件控制
	@return:	成功返回TRUE,失败返回FALSE
Description:    默认为:COM1,9600,8,ONESTOPBIT,NOPARITY,0,0,0
=======================================================================*/

int  CRs232::OpenConnection(  LPCTSTR  szComPort, DWORD  dwBaudRate, BYTE  cByteSize,   
            BYTE  cStopBits,BYTE  cParity,BOOL  bDTR_DSR, BOOL  bRTS_CTS,BOOL  bXON_XOFF  )
{
	BOOL e_InitializeComm =TRUE;
	BOOL e_OpenComm =FALSE;
	if(  m_bConnected  )        //  已经打开 
	{ 
		printf("The Port have open!\n");
		return  0;   
	}
	SetEvent(m_osWrite.hEvent);  
    m_hComDev  =  CreateFile(  szComPort,  GENERIC_READ  |  GENERIC_WRITE,  0,  NULL,  OPEN_ALWAYS,  FILE_FLAG_OVERLAPPED,  NULL  );
	if(  m_hComDev  ==  INVALID_HANDLE_VALUE  ) 
	 {
		 
		 printf("Cannot open the port!\n");
		 printf("%d\n",GetLastError());
		 return  e_OpenComm;    //  返回不能打开串口错误代码
	 }

	DCB  dcb;
    dcb.DCBlength  =  sizeof(DCB);   
	GetCommState(m_hComDev,&dcb  );       
	dcb.BaudRate  =  dwBaudRate;  
	dcb.ByteSize  =  cByteSize; 
	dcb.Parity  =  cParity;    
	dcb.StopBits  =  ONESTOPBIT ;    

	//  硬件流控制.  bDTR_DSR、bRTS_CTS一般取默认值0，有Modem时应取1 
	dcb.fOutxDsrFlow = ( bDTR_DSR!=0 );    //   
	if(bDTR_DSR)   
	{
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE  ;    // 
	}
	else   
	{
		dcb.fDtrControl = DTR_CONTROL_ENABLE  ;
	}     
	dcb.fOutxCtsFlow  = (bRTS_CTS!=0)  ; 
        
	if  (bRTS_CTS)   
	{
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE ;
	}
	else   
	{
		dcb.fRtsControl = RTS_CONTROL_ENABLE  ;
	}

	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;

	dcb.fInX = dcb.fOutX = (bXON_XOFF!=0);    //  软件流控制 
    dcb.fBinary = TRUE;    //  
    dcb.fParity = FALSE;   
    if(  SetCommState(m_hComDev,&dcb)  ) 
    {   
		//  配置串口参数成功   
		SetCommMask(m_hComDev,EV_RXCHAR);
		//  设置接收、发送缓冲区大小   
		SetupComm(m_hComDev,4096,4096);
		COMMTIMEOUTS    CommTimeOuts  ;
		CommTimeOuts.ReadIntervalTimeout  =  MAXDWORD;       
		CommTimeOuts.ReadTotalTimeoutMultiplier  =  0;  
		CommTimeOuts.ReadTotalTimeoutConstant  =  10;       
		CommTimeOuts.WriteTotalTimeoutMultiplier  =  0;       
		CommTimeOuts.WriteTotalTimeoutConstant  =  0;      
		SetCommTimeouts(  m_hComDev,  &CommTimeOuts  );  
		m_bConnected  =  TRUE;   
		printf("Set Succed!\n");	
		return  TRUE;
	 } 

	m_bConnected  =  FALSE;
	CloseHandle( m_hComDev ); 
	return FALSE;
}

/*=======================================================================
Function:		从串口读数据
Prototype:		int ReadCommBlock(  unsigned char*  lpszBlock,  int  nMaxLength  )
	@param:		lpszBlock数组存储读取的数据
	@param:     nMaxLength期待读取到的数据大小字节 
	@return:	成功,返回读取的数据字节数
Description:    读取的数据的大小由nMaxLength和串口的输入缓冲区数据的大小决定
=======================================================================*/

int  CRs232::ReadCommBlock(  unsigned char*  lpszBlock,  int  nMaxLength  )
{ 
	COMSTAT  ComStat;    //  串口状态结构 
	DWORD  dwLength, dwErrorFlags; 
	ClearCommError(m_hComDev,&dwErrorFlags,&ComStat);
	//printf("%d\n",GetLastError());
	dwLength = min( (DWORD) nMaxLength, ComStat.cbInQue);  //    将要读取的数据字节数
	if( dwLength > 0 ) 
	{ 
		int temp = ReadFile(m_hComDev, lpszBlock, dwLength, &dwLength, &m_osRead);
		if(  !temp )    //  从串口读入数据     
		{
			//printf("%d\n", GetLastError() );
			if(  GetLastError()  ==  ERROR_IO_PENDING)      
			{
				if(  !GetOverlappedResult(m_hComDev,  &m_osRead,  &dwLength,  TRUE) )  //  挂起，直到操作完成       
				{	
					ClearCommError( m_hComDev,  &dwErrorFlags,  &ComStat );   
				}
			}
		}
	}
	else
	{
		printf("dwLength = 0\n");
	
	}
	return  dwLength;    //  返回已读入的数据字节数 
}

/*=======================================================================
Function:		往串口写数据
Prototype:		int WriteCommBlock(  LPCVOID  lpByte,  UINT  nBytesToWrite)
	@param:		lpByte指针指向存储写入数据的地址
	@param:     nBytesToWrite写入的数据大小 
	@return:	成功。返回1；失败，返回0
Description:   
=======================================================================*/

int CRs232::WriteCommBlock(  LPCVOID  lpByte,  UINT  nBytesToWrite)
{ 
	int e_NotConnected =0;
	int  e_TransmitError =0;
	int e_TransmitNotFinished =0;
	DWORD   dwBytesWritten,  dwErrorFlags;
	COMSTAT  ComStat;
    if(  !m_bConnected  )               //  串口没有打开     
	{
		return  e_NotConnected; 
	}
	if(WaitForSingleObject(m_osWrite.hEvent,0)  !=  WAIT_OBJECT_0)     
	{
		return  e_TransmitNotFinished;   
	}
	ResetEvent(m_osWrite.hEvent); 
	if(!WriteFile(m_hComDev,lpByte,nBytesToWrite,&dwBytesWritten,&m_osWrite))
	{
		                         //  往串口发送数据
		if(  GetLastError()  ==  ERROR_IO_PENDING  )     
		{
			//printf("%d\n",GetLastError());
			return  0;           //  正在后台发送   
		}
		ClearCommError(  m_hComDev,  &dwErrorFlags,  &ComStat  )  ;  //  出现错误，清除错误标志 
		return    e_TransmitError;                 //  发送出错 
	 } 
	return  0;             //  WriteFile返回１,发送成功
}

/*=======================================================================
Function:		关闭串口
Prototype:		bool CloseConnection( )
	@param:		无
	@return:	成功返回TRUE，否则返回FALSE
Description:	
=======================================================================*/

BOOL CRs232::CloseConnection( )
{
	if( !m_bConnected )
	{
		return FALSE; 
	}
	SetEvent(m_hThreadTerm);
	m_bConnected = FALSE; 
	SetCommMask( m_hComDev, 0); 
 
	EscapeCommFunction(m_hComDev,CLRDTR);
	if(!CloseHandle(m_hComDev)){
		printf("%d\n",GetLastError());
		return FALSE;//  关闭、释放串口
	}else{
		printf("Close succed!\n");
		return ( TRUE );
	}
}



/*=======================================================================
Function:		判断串口是否打开
Prototype:		bool IsConnected( )
	@param:		无
	@return:	成功返回TRUE，否则返回FALSE
Description:	
=======================================================================*/

BOOL CRs232::IsConnected( )
{
	if( !m_bConnected )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}