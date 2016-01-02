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
	 if( m_bConnected )          //    ����Ѵ򿪴���
	 {
	   CloseConnection();            //  �رա��ͷŴ���
	 }
	 CloseHandle(  m_hThreadTerm  ); 
	 CloseHandle(  m_osRead.hEvent)  ;
	 CloseHandle(  m_osWrite.hEvent)  ;
}

/*=======================================================================
Function:		�򿪴����豸
Prototype:		int OpenConnection(  LPCTSTR  szComPort, DWORD  dwBaudRate, BYTE  cByteSize,   
                BYTE  cStopBits,BYTE  cParity,BOOL  bDTR_DSR, BOOL  bRTS_CTS,BOOL  bXON_XOFF  )
	@param:		szComPortѡ�񴮿ںţ�Ĭ��Ϊ
	@param:     dwBaudRate���ò�����
	@param:     cByteSize��������λ  
	@param:		cStopBits����ֹͣλ 
	@param:     cParity������żУ��λ 
	@param:		bDTR_DSR����DSR��Ӳ������
	@param:		bRTS_CTS����CTSӲ������
	@param:		bXON_XOFF�����������
	@return:	�ɹ�����TRUE,ʧ�ܷ���FALSE
Description:    Ĭ��Ϊ:COM1,9600,8,ONESTOPBIT,NOPARITY,0,0,0
=======================================================================*/

int  CRs232::OpenConnection(  LPCTSTR  szComPort, DWORD  dwBaudRate, BYTE  cByteSize,   
            BYTE  cStopBits,BYTE  cParity,BOOL  bDTR_DSR, BOOL  bRTS_CTS,BOOL  bXON_XOFF  )
{
	BOOL e_InitializeComm =TRUE;
	BOOL e_OpenComm =FALSE;
	if(  m_bConnected  )        //  �Ѿ��� 
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
		 return  e_OpenComm;    //  ���ز��ܴ򿪴��ڴ������
	 }

	DCB  dcb;
    dcb.DCBlength  =  sizeof(DCB);   
	GetCommState(m_hComDev,&dcb  );       
	dcb.BaudRate  =  dwBaudRate;  
	dcb.ByteSize  =  cByteSize; 
	dcb.Parity  =  cParity;    
	dcb.StopBits  =  ONESTOPBIT ;    

	//  Ӳ��������.  bDTR_DSR��bRTS_CTSһ��ȡĬ��ֵ0����ModemʱӦȡ1 
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

	dcb.fInX = dcb.fOutX = (bXON_XOFF!=0);    //  ��������� 
    dcb.fBinary = TRUE;    //  
    dcb.fParity = FALSE;   
    if(  SetCommState(m_hComDev,&dcb)  ) 
    {   
		//  ���ô��ڲ����ɹ�   
		SetCommMask(m_hComDev,EV_RXCHAR);
		//  ���ý��ա����ͻ�������С   
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
Function:		�Ӵ��ڶ�����
Prototype:		int ReadCommBlock(  unsigned char*  lpszBlock,  int  nMaxLength  )
	@param:		lpszBlock����洢��ȡ������
	@param:     nMaxLength�ڴ���ȡ�������ݴ�С�ֽ� 
	@return:	�ɹ�,���ض�ȡ�������ֽ���
Description:    ��ȡ�����ݵĴ�С��nMaxLength�ʹ��ڵ����뻺�������ݵĴ�С����
=======================================================================*/

int  CRs232::ReadCommBlock(  unsigned char*  lpszBlock,  int  nMaxLength  )
{ 
	COMSTAT  ComStat;    //  ����״̬�ṹ 
	DWORD  dwLength, dwErrorFlags; 
	ClearCommError(m_hComDev,&dwErrorFlags,&ComStat);
	//printf("%d\n",GetLastError());
	dwLength = min( (DWORD) nMaxLength, ComStat.cbInQue);  //    ��Ҫ��ȡ�������ֽ���
	if( dwLength > 0 ) 
	{ 
		int temp = ReadFile(m_hComDev, lpszBlock, dwLength, &dwLength, &m_osRead);
		if(  !temp )    //  �Ӵ��ڶ�������     
		{
			//printf("%d\n", GetLastError() );
			if(  GetLastError()  ==  ERROR_IO_PENDING)      
			{
				if(  !GetOverlappedResult(m_hComDev,  &m_osRead,  &dwLength,  TRUE) )  //  ����ֱ���������       
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
	return  dwLength;    //  �����Ѷ���������ֽ��� 
}

/*=======================================================================
Function:		������д����
Prototype:		int WriteCommBlock(  LPCVOID  lpByte,  UINT  nBytesToWrite)
	@param:		lpByteָ��ָ��洢д�����ݵĵ�ַ
	@param:     nBytesToWriteд������ݴ�С 
	@return:	�ɹ�������1��ʧ�ܣ�����0
Description:   
=======================================================================*/

int CRs232::WriteCommBlock(  LPCVOID  lpByte,  UINT  nBytesToWrite)
{ 
	int e_NotConnected =0;
	int  e_TransmitError =0;
	int e_TransmitNotFinished =0;
	DWORD   dwBytesWritten,  dwErrorFlags;
	COMSTAT  ComStat;
    if(  !m_bConnected  )               //  ����û�д�     
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
		                         //  �����ڷ�������
		if(  GetLastError()  ==  ERROR_IO_PENDING  )     
		{
			//printf("%d\n",GetLastError());
			return  0;           //  ���ں�̨����   
		}
		ClearCommError(  m_hComDev,  &dwErrorFlags,  &ComStat  )  ;  //  ���ִ�����������־ 
		return    e_TransmitError;                 //  ���ͳ��� 
	 } 
	return  0;             //  WriteFile���أ�,���ͳɹ�
}

/*=======================================================================
Function:		�رմ���
Prototype:		bool CloseConnection( )
	@param:		��
	@return:	�ɹ�����TRUE�����򷵻�FALSE
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
		return FALSE;//  �رա��ͷŴ���
	}else{
		printf("Close succed!\n");
		return ( TRUE );
	}
}



/*=======================================================================
Function:		�жϴ����Ƿ��
Prototype:		bool IsConnected( )
	@param:		��
	@return:	�ɹ�����TRUE�����򷵻�FALSE
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