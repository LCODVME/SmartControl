/*****************************************************************
* Copyright (C) 2019 Ziming Technology Co.,Ltd.*
******************************************************************
* ld3320.c
*
* DESCRIPTION:
*     LD3320 driver
* AUTHOR:
*     Ziming
* CREATED DATE:
*     2019/3/9
* REVISION:
*     v0.1
*
* MODIFICATION HISTORY
* --------------------
* $Log:$
*
*****************************************************************/
/*************************************************************************************************************************
 *                                                       INCLUDES                                                        *
 *************************************************************************************************************************/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "spi.h"
#include "gpio.h"
#include "ld3320.h"
/*************************************************************************************************************************
 *                                                        MACROS                                                         *
 *************************************************************************************************************************/

/*************************************************************************************************************************
 *                                                      CONSTANTS                                                        *
 *************************************************************************************************************************/
static const char antistop[9][20] = {
                                   {"xiao ming tong xue" },
                                   {"da kai deng guang" },
                                   {"guan bi deng guang" },
                                   {"guan jia" },
                                   {"kai deng" },
                                   {"guan deng" },
                                   {"da kai" },
                                   {"guan bi" },
                                   {"xiao bai" },
                                  };
static const uint8_t antistopID[9] = { WEAK_UP,OPEN_LIGHT,CLOSE_LIGHT,WEAK_UP,OPEN_LIGHT,CLOSE_LIGHT,OPEN_LIGHT,CLOSE_LIGHT,WEAK_UP };
/*************************************************************************************************************************
 *                                                       TYPEDEFS                                                        *
 *************************************************************************************************************************/
 
/*************************************************************************************************************************
 *                                                   GLOBAL VARIABLES                                                    *
 *************************************************************************************************************************/
uint8_t ldMode = LD_MODE_IDLE;
/*************************************************************************************************************************
 *                                                  EXTERNAL VARIABLES                                                   *
 *************************************************************************************************************************/
 
/*************************************************************************************************************************
 *                                                    LOCAL VARIABLES                                                    *
 *************************************************************************************************************************/
 
/*************************************************************************************************************************
 *                                                 FUNCTION DECLARATIONS                                                 *
 *************************************************************************************************************************/
static uint8_t  ldReadData( uint8_t a_addr );
static void     ldWriteData( uint8_t a_addr, uint8_t a_data );
/*************************************************************************************************************************
 *                                                   PUBLIC FUNCTIONS                                                    *
 *************************************************************************************************************************/
 
/*************************************************************************************************************************
 *                                                    LOCAL FUNCTIONS                                                    *
 *************************************************************************************************************************/

/*****************************************************************
* DESCRIPTION: ldCommandInit
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
void ldCommandInit( void )
{
    //for(uint8_t i = 0; i < 5; i++)
    ldReset();
    ldReadData(0x06);  // FIFO状态寄存器 只读
    ldWriteData(0x17,0x35); //17寄存器 0x35：
    delayMs(10);
    ldReadData(0x06);
    ldWriteData(0x89,0x03); // 模拟电路控制 0x03：初始化
    delayMs(5);
    ldWriteData(0xCF,0x43); // 内部省电模式 0x43：初始化
    delayMs(5);
    ldWriteData(0xCB,0x02);
    
    /* PLL setting */
    ldWriteData(0x11,LD_PLL_11);
    ldWriteData(0x1E,0x00);
    ldWriteData(0x19, LD_PLL_ASR_19); 
    ldWriteData(0x1B, LD_PLL_ASR_1B);		
    ldWriteData(0x1D, LD_PLL_ASR_1D);
    delayMs(5);
	
	ldWriteData(0xCD, 0x04);
	ldWriteData(0x17, 0x4c); 
	delayMs(1);
	ldWriteData(0xB9, 0x00);
	ldWriteData(0xCF, 0x4F); 
	ldWriteData(0x6F, 0xFF);
}

/*****************************************************************
* DESCRIPTION: ldAsrInit
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
void ldAsrInit( void )
{
    if(ldMode != LD_MODE_IDLE) return;
	ldCommandInit();

	ldWriteData(0xBD, 0x00); // 启动ASR
	ldWriteData(0x17, 0x48); //激活DSP
	delayMs( 5 );
	ldWriteData(0x3C, 0x80);  //FIFO_EXT 下限低8位
	ldWriteData(0x3E, 0x07); //下限高8位
	ldWriteData(0x38, 0xff);  //上限低8位
	ldWriteData(0x3A, 0x07); //上限高8位
	ldWriteData(0x40, 0);    //MCU 水线低8位
	ldWriteData(0x42, 8);   //水线高8位
	ldWriteData(0x44, 0);   //DSP水线低8位
	ldWriteData(0x46, 8);   //水线高8位
	delayMs( 5 );
}
/*****************************************************************
* DESCRIPTION: ldAddAntistop
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
void ldAddAntistop( void )
{
    uint32_t duration = 0;
    for( uint8_t count = 0; count < 9; count++ )
    {
        duration = HAL_GetTick();
        while( ldReadData(0xb2) != 0x21 )
        {
            if( ( HAL_GetTick() - duration ) >= 5000 )
            {
                NVIC_SystemReset();
                while(1);
            }
        }
        ldWriteData(0xc1, antistopID[count] );
		ldWriteData(0xc3, 0 );
		ldWriteData(0x08, 0x04);
		delayMs(1);
		ldWriteData(0x08, 0x00);
		delayMs(1);
        for( uint8_t cnt = 0; cnt < 25; cnt++ )
        {
            if( antistop[count][cnt] == '\0' )
            {
                ldWriteData(0xb9, cnt);
                break;
            }
            ldWriteData(0x05, antistop[count][cnt]);
        }
		ldWriteData(0xb2, 0xff);
		ldWriteData(0x37, 0x04);
        delayMs(5);
    }
    
    return;
}

/*****************************************************************
* DESCRIPTION: ldAsrStart
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
uint8_t asdfg = 0;
uint8_t ldAsrStart( void )
{
    if(ldMode == LD_MODE_ASR_RUN)
    {
        return 0;
    }
    //ldWriteData(0xB3, 0x15);
    ldWriteData(0x35, MIC_VOL);
	ldWriteData(0x1C, 0x09);
	ldWriteData(0xBD, 0x20);
	ldWriteData(0x08, 0x01);
	delayMs( 50 );
	ldWriteData(0x08, 0x00);
	delayMs( 50 );

	if(ldReadData(0xb2) != 0x21 )
	{
		return 0;
	}
    ldMode = LD_MODE_ASR_RUN;
	ldWriteData(0xB2, 0xff);	
	ldWriteData(0x37, 0x06);
	ldWriteData(0x37, 0x06);
	delayMs( 50 );
	ldWriteData(0x1C, 0x0b);
	ldWriteData(0x29, 0x10);
	ldWriteData(0xBD, 0x00);
    return 1;
}

/*****************************************************************
* DESCRIPTION: getAsrResult
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
uint8_t getAsrResult( void )
{
    uint8_t result;
    ldMode=LD_MODE_IDLE;
    
    result = ldReadData(0xc5);
    
    ldWriteData(0x29,0) ;
    ldWriteData(0x02,0) ;
    ldWriteData(0x2b, 0);
    ldWriteData(0x1C,0);/*写0:ADC不可用*/
    
    ldWriteData(0x29,0) ;
    ldWriteData(0x02,0) ;
    ldWriteData(0x2B,  0);
    ldWriteData(0xBA, 0);	
    ldWriteData(0xBC,0);	
    ldWriteData(0x08,1);	 /*清除FIFO_DATA*/
    ldWriteData(0x08,0);	/*清除FIFO_DATA后 再次写0*/
    return result;
}

/*****************************************************************
* DESCRIPTION: delayMs
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
void delayMs( uint32_t a_time )
{
    uint32_t count  = HAL_GetTick();
    while( (HAL_GetTick() - count) < a_time );
}
/*****************************************************************
* DESCRIPTION: ldRead
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
static uint8_t ldReadData( uint8_t a_addr )
{
    uint8_t readData;
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD_CS_GPIO_Port, LD_CS_Pin, GPIO_PIN_RESET);
    /* Send read command first */
    spiTransmitData( READ_COMMAND );
    spiTransmitData( a_addr );
    readData =spiTransmitData( 0x00 );
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD_CS_GPIO_Port, LD_CS_Pin, GPIO_PIN_SET);
    return readData;
}

/*****************************************************************
* DESCRIPTION: ldWrite
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/
static void ldWriteData( uint8_t a_addr, uint8_t a_data )
{
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD_CS_GPIO_Port, LD_CS_Pin, GPIO_PIN_RESET);
    /* Send write command first */
    spiTransmitData( WRITE_COMMAND );
    /* Write address */
    spiTransmitData( a_addr );
    /* Write data */
    spiTransmitData( a_data );
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD_CS_GPIO_Port, LD_CS_Pin, GPIO_PIN_SET);
}
/*****************************************************************
* DESCRIPTION: ldReset
*     
* INPUTS:
*     
* OUTPUTS:
*     
* NOTE:
*     null
*****************************************************************/

void ldReset( void )
{
    uint8_t chec1,chec2,chec3,chec4; 
    do
    {
        /* LD_Reset high */
        HAL_GPIO_WritePin(LD_Reset_GPIO_Port, LD_Reset_Pin, GPIO_PIN_SET);
        delayMs(5);
        /* LD_Reset low */
        HAL_GPIO_WritePin(LD_Reset_GPIO_Port, LD_Reset_Pin, GPIO_PIN_RESET);
        delayMs(5);
        /* LD_Reset high */
        HAL_GPIO_WritePin(LD_Reset_GPIO_Port, LD_Reset_Pin, GPIO_PIN_SET);
        delayMs(5);
        /* LD_CS Low */
        HAL_GPIO_WritePin(LD_CS_GPIO_Port, LD_CS_Pin, GPIO_PIN_RESET);
        delayMs(5);
        /* LD_CS High */
        HAL_GPIO_WritePin(LD_CS_GPIO_Port, LD_CS_Pin, GPIO_PIN_SET);
        delayMs(100);
        chec1 = ldReadData(0x06);
        chec2 = ldReadData(0x06);
        chec3 = ldReadData(0x35);
        chec4 = ldReadData(0xb3);
    }while( chec1 != 0x87 || chec2 != 0x87 || chec3 != 0x80 || chec4 != 0xFF );
}


/****************************************************** END OF FILE ******************************************************/
