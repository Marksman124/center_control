/**
******************************************************************************
* @file    		modbus.c
* @brief   		Modbus �ӿ�
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "modbus.h"

/* ----------------------- Defines ------------------------------------------*/
// 03
#define REG_HOLDING_START               ( 1 )
#define REG_HOLDING_NREGS               ( 0x013F )

// 04
#define REG_INPUT_START 								( 1 )
#define REG_INPUT_NREGS 								( 0x003F )

// 21
#define REG_FILE_NUMBER_MAX 								( 0x270F )
#define REG_FILE_NUMBER_STAR 								( 0 )
#define REG_FILE_NUMBER_END 								( 0xFFFF )
#define REG_FILE_LENTH_MAX 									( 0x77 )

#define MODBUS_RESTART_TIMEOUT 							( 10 )					// 100MS * N

/* Private variables ---------------------------------------------------------*/
ULONG OTA_Pack_Len=0;					// �ܳ���

uint32_t ModbusTimerCnt=0;



/* ----------------------- Static variables ---------------------------------*/
//USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS+1];

//USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS+1];

USHORT		MB_Data_Addr_Need_CallOut[] = {MB_DATA_ADDR_SLAVE_ADDRESS,MB_DATA_ADDR_BAUD_RATE,MB_DATA_ADDR_GEAR_PUMP_1,
MB_DATA_ADDR_GEAR_PUMP_2,MB_DATA_ADDR_CURRENT_PUMP_1,MB_DATA_ADDR_CURRENT_PUMP_2,MB_DATA_ADDR_VOLTAGE_PUMP_1,MB_DATA_ADDR_VOLTAGE_PUMP_2,
MB_DATA_ADDR_CONNECTION_MODE,MB_DATA_ADDR_METERING_MODULE_CLEAN,MB_DATA_ADDR_METERING_MODULE_ADDR, MB_DATA_ADDR_METERING_MODULE_BAUD,
MB_DATA_ADDR_METERING_MODULE_FORMAT,REG_DATA_ADDR_DMX512_LENTH,REG_DATA_ADDR_DMX512_END,
};

BOOL Dmx512_Data_Change_Sign =0;
/* ----------------------- Start implementation -----------------------------*/
extern TIM_HandleTypeDef htim1;

void Clean_ModbusTimerCnt()
{
	ModbusTimerCnt = 0;
}

BOOL 
Check_Need_CallOut(USHORT addr)
{
	USHORT sum,i;
	
	sum = sizeof(MB_Data_Addr_Need_CallOut)/sizeof(MB_Data_Addr_Need_CallOut[0]);
	
	if( (addr >= REG_DATA_ADDR_DMX512_START)&& (addr <= REG_DATA_ADDR_DMX512_END) )
	//if( addr == REG_DATA_ADDR_DMX512_END )
		Set_Dmx512_Data_Change(1);
	
	for(i=0; i<sum; i++)
	{
		if(addr == MB_Data_Addr_Need_CallOut[i])
			return TRUE;
	}
	
	return FALSE;
}

extern UART_HandleTypeDef* p_huart_mb;		 //UART���

void
HoldingCallOut( USHORT usAddress )
{
	uint16_t iRegIndex;
	uint32_t message_info=0;
	
	iRegIndex = ( usAddress );
	
	if( ( usAddress >= MB_DATA_ADDR_CONNECTION_MODE ) && ( usAddress <= MB_DATA_ADDR_METERING_MODULE_FORMAT ) )  // 20~24
	{
		// �·� ��������
		message_info = usAddress<<16 | usRegHoldingBuf[iRegIndex];
		osMessagePut(MeteringQueueHandle,message_info,1);
		if( usAddress == MB_DATA_ADDR_METERING_MODULE_CLEAN )
			usRegHoldingBuf[iRegIndex] = 0; // д��1�����õ�ַ�Զ�����Ϊ 0 
	}
	else if(usAddress == MB_DATA_ADDR_BAUD_RATE) // ���ò�����
	{
		__HAL_UART_DISABLE(p_huart_mb);
#if MODBUS_USART == 2
		MX_USART2_UART_Init();
#elif MODBUS_USART == 3
		MX_USART3_UART_Init();
#elif MODBUS_USART == 5
		MX_UART5_Init();
#endif
		__HAL_UART_ENABLE(p_huart_mb);
	}
	
	//������2048�� ���� usRegHoldingBuf һ��д
	STMFLASH_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

	Clean_ModbusTimerCnt();
	
    if( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS )
    {
        //iRegIndex = ( int )( usAddress - usRegInputStart );
				iRegIndex = ( int )( usAddress );
        while( usNRegs > 0 )
        {
					if((iRegIndex >= REG_DATA_ADDR_DMX512_START) && (iRegIndex <= REG_DATA_ADDR_DMX512_END))//DMX С��
					{
						*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
						*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
					}
					else
					{
						*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
						*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
					}
					iRegIndex++;
					usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
	
		Clean_ModbusTimerCnt();
	
    if( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) 
    {
				//iRegIndex = ( int )( usAddress - usRegHoldingStart );
        iRegIndex = ( int )( usAddress );
        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
							if((iRegIndex >= REG_DATA_ADDR_DMX512_START) && (iRegIndex <= REG_DATA_ADDR_DMX512_END))//DMX С��
							{
								*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
								*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
							}
							else
							{
								*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
								*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
							}
              iRegIndex++;
              usNRegs--;
            }
            break;

        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
							
							if((iRegIndex >= REG_DATA_ADDR_DMX512_START) && (iRegIndex <= REG_DATA_ADDR_DMX512_END))//DMX С��
							{
								usRegHoldingBuf[iRegIndex] = *pucRegBuffer++;
								usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++ << 8;
							}
							else
							{
								usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
								usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
								
								if(iRegIndex == MB_DATA_ADDR_METERING_MODULE_ADDR)
								{
									if(*pucRegBuffer > METERING_ADDR_MAX)
									{
										Set_DataAddr_Value( MB_FUNC_READ_HOLDING_REGISTER,  MB_DATA_ADDR_METERING_MODULE_ADDR,  MB_METERING_ADDR_DEFAULT);
									}
								}
							}
							iRegIndex++;
              usNRegs--;
            }
						
						if(Check_Need_CallOut(usAddress))
						{
							HoldingCallOut(usAddress);
						}
						break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegFileCB( UCHAR * pucRegBuffer, USHORT fileNumber, USHORT fileLength,
                 eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
	ULONG write_addr=0;						// flash д���ַ
	ULONG sign=0;
	
	Clean_ModbusTimerCnt();
	
	if( (( fileNumber <= REG_FILE_NUMBER_MAX ) || ( fileNumber == REG_FILE_NUMBER_END )) && ( fileLength <= REG_FILE_LENTH_MAX ) )
	{
		switch ( eMode )
		{
		case MB_REG_READ:
			break;
		case MB_REG_WRITE:
			if(fileNumber == REG_FILE_NUMBER_STAR) // ��ʼ��
			{OTA_Pack_Len = 0;}
						
			write_addr = (FLASH_APP_PATCH_ADDR + OTA_Pack_Len);
			iap_write_appbin(write_addr,pucRegBuffer,fileLength);
			OTA_Pack_Len += (fileLength*2);
			
			if(fileNumber == REG_FILE_NUMBER_END)//���һ֡
			{
				//STMFLASH_Write(USER_FLASH_ADDR_SOFTWARE_VERSION,&fileNumber,1); // д�汾��Ϣ 
				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PACK_LEN,(uint16_t*)&OTA_Pack_Len,2); // д������ (��crc)
				sign = PRODUCT_BOOT_PASSWORD;
				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&sign,2); // ����OTA 

				SysSoftReset();// �����λ
			}
			break;
		}
	}
	else
	{
			eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
	Clean_ModbusTimerCnt();
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
//	//�Ĵ�������
//	int16_t iNCoils = ( int16_t )usNCoils;
//	//�Ĵ���ƫ����
//	int16_t usBitOffset;

//	//���Ĵ����Ƿ���ָ����Χ��
//	if( ( (int16_t)usAddress >= REG_COILS_START ) &&
//	( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
//	{
//	//����Ĵ���ƫ����
//	usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
//	switch ( eMode )
//	{
//	//������
//	case MB_REG_READ:
//	while( iNCoils > 0 )
//	{
//	*pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
//	( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
//	iNCoils -= 8;
//	usBitOffset += 8;
//	}
//	break;

//	//д����
//	case MB_REG_WRITE:
//	while( iNCoils > 0 )
//	{
//	xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
//	( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
//	*pucRegBuffer++ );
//	iNCoils -= 8;
//	}
//	break;
//	}

//	}
//	else
//	{
//	eStatus = MB_ENOREG;
//	}
	return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	Clean_ModbusTimerCnt();
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
//	//�����Ĵ�������
//	int16_t iNDiscrete = ( int16_t )usNDiscrete;
//	//ƫ����
//	uint16_t usBitOffset;

//	//�жϼĴ���ʱ�����ƶ���Χ��
//	if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
//	( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
//	{
//	//���ƫ����
//	usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );

//	while( iNDiscrete > 0 )
//	{
//	*pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
//	( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
//	iNDiscrete -= 8;
//	usBitOffset += 8;
//	}

//	}
//	else
//	{
//	eStatus = MB_ENOREG;
//	}
	return eStatus;
}




void Modbus_Init(void)
{
	
	eMBInit( MB_RTU, 0xAA, 0, 115200, MB_PAR_ODD);//��ʼ��modbus����modbusRTU����վ��ַΪ0xAA������Ϊ2��
	eMBEnable(  );//ʹ��modbus
	
}

void Modbus_Handle_Task(void)
{
	( void )eMBPoll(  );//����modbus����
	
	if(ModbusTimerCnt++ > MODBUS_RESTART_TIMEOUT)
	{
		ModbusTimerCnt = 0;
		vMBPortSerialEnable( TRUE, FALSE );
//		memset(UART_BUF,0,sizeof(UART_BUF));
//		HAL_FLASH_Unlock();						//����
//		STMFLASH_Read(FLASH_APP_PROGRAM_ADDR+(Number*238),(uint16_t*)&UART_BUF[10],0x77);
//		HAL_FLASH_Lock();							//����
//		memcpy(UART_BUF,temp,10);
//		UART_BUF[7] = Number;
//		Number++;
//		HAL_UART_Transmit(&huart5,UART_BUF,250,0xffff);
//		HAL_Delay(50);
	}
}



// *********  Buf ������ݽӿ�  *************************************

void Modbus_Buffer_Init(void)
{
	STMFLASH_Read(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
	
	// Ĭ�ϲ�����  ������
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_BAUD_RATE,3);
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_METERING_MODULE_BAUD,3);
	
}

void MB_Flash_Buffer_Write(void)
{
//������2048�� ���� usRegHoldingBuf һ��д
	STMFLASH_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
}



uint16_t* Get_DataAddr_Pointer(UCHAR ucFunctionCode, USHORT addr)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			return &usRegHoldingBuf[addr];
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			return &usRegInputBuf[addr];
		}
	}
	
	return NULL;
}


uint16_t Get_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			return usRegHoldingBuf[addr];
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			return usRegInputBuf[addr];
		}
	}
	
	return NULL;
}

void Set_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr, uint16_t value)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			usRegHoldingBuf[addr] = value;
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			usRegInputBuf[addr] = value;
		}
	}
}


void Set_Dmx512_Data(uint8_t* p_buff, uint16_t len)
{
	uint16_t i,j,sum;
	sum = len/2;
	if(len%2)
		sum++;

	for(i=0,j=0; i<sum; i++,j+=2)
	{
		if( (REG_DATA_ADDR_DMX512_START+i) > REG_DATA_ADDR_DMX512_END )
			break;
		usRegHoldingBuf[REG_DATA_ADDR_DMX512_START+i] = *(p_buff+j) << 8;
		usRegHoldingBuf[REG_DATA_ADDR_DMX512_START+i] |= *(p_buff+j+1);
	}
}


uint8_t Get_Dmx512_Data_Change(void)
{
	return Dmx512_Data_Change_Sign;
}

void Set_Dmx512_Data_Change(uint8_t type)
{
	Dmx512_Data_Change_Sign = type;
}




