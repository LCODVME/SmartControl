/*****************************************************************
* Copyright (C) 2019 Ziming Technology Co.,Ltd.*
******************************************************************
* ld3320.h
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
#ifndef LD3320_H
#define LD3320_H
#ifdef __cplusplus
extern "C"
{
#endif
//#include "stdint.h"
/*************************************************************************************************************************
 *                                                        MACROS                                                         *
 *************************************************************************************************************************/
/* Command */
#define READ_COMMAND            0x05
#define WRITE_COMMAND           0x04
/* Register */
#define FIFO_STATUS             0x06

    
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR_RUN		0x08
#define LD_MODE_MP3		 	0x40


//	以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE				0x00	//	表示没有在作ASR识别
#define LD_ASR_RUNING			0x01	//	表示LD3320正在作ASR识别中
#define LD_ASR_FOUNDOK			0x10	//	表示一次识别流程结束后，有一个识别结果
#define LD_ASR_FOUNDZERO 		0x11	//	表示一次识别流程结束后，没有识别结果
#define LD_ASR_ERROR	 		0x31	//	表示一次识别流程中LD3320芯片内部出现不正确的状态


#define CLK_IN   22	/* user need modify this value according to clock in */
#define LD_PLL_11			(uint8_t)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8_t)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8_t)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

// LD chip fixed values.
#define RESUM_OF_MUSIC               0x01
#define CAUSE_MP3_SONG_END           0x20
        
#define MASK_INT_SYNC				0x10
#define MASK_INT_FIFO				0x04
#define MASK_AFIFO_INT				0x01
#define MASK_FIFO_STATUS_AFULL		0x08


#define MIC_VOL                     0x43
/*************************************************************************************************************************
 *                                                      CONSTANTS                                                        *
 *************************************************************************************************************************/
 
/*************************************************************************************************************************
 *                                                       TYPEDEFS                                                        *
 *************************************************************************************************************************/
typedef enum
{
    NO_RESULT = 0U,
    WEAK_UP = 1U,   
    OPEN_LIGHT,
    CLOSE_LIGHT,  
}E_command;
/*************************************************************************************************************************
 *                                                  EXTERNAL VARIABLES                                                   *
 *************************************************************************************************************************/
extern uint8_t ldMode;
/*************************************************************************************************************************
 *                                                   PUBLIC FUNCTIONS                                                    *
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
void ldCommandInit( void );
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
void ldAsrInit( void );
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
void ldAddAntistop( void );
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
uint8_t ldAsrStart( void );
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
uint8_t getAsrResult( void );
void ldReset( void );
void delayMs( uint32_t a_time );

#ifdef __cplusplus
}
#endif
#endif /* ld3320.h */
