/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include <stdio.h>
//#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "main_user.h"
#include "gconfig.h"
#include "stm32h745i_discovery.h"
#include "SerialAPI.h"
#include "zpal_init.h"
#include "ZW_basis_api.h"
#include "ZW_classcmd.h"
#include "cmds_management.h"
#include "NodeMask.h"
#include "ZW_controller_api.h"
#include "zpal_radio.h"

//#ifndef WOLFSSL_USER_SETTINGS
//    #include <wolfssl/options.h>
//#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
//#include <wolfssl/wolfcrypt/error-crypt.h>
//#include <wolfcrypt/test/test.h>
//#include <wolfcrypt/benchmark/benchmark.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/curve25519.h>
#include <wolfssl/wolfcrypt/cmac.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

// Enable host control of Z-Wave controller
// 0 = disable host control of Z-Wave controller (let PC Controller take control)
// 1 = enable  host control of Z-Wave controller
#define ENABLE_ZWAVE_CONTROLLER_HOST 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c4;

JPEG_HandleTypeDef hjpeg;
MDMA_HandleTypeDef hmdma_jpeg_infifo_th;
MDMA_HandleTypeDef hmdma_jpeg_outfifo_th;

LTDC_HandleTypeDef hltdc;

QSPI_HandleTypeDef hqspi;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

MMC_HandleTypeDef hmmc1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

SDRAM_HandleTypeDef hsdram2;

/* Definitions for Main */
osThreadId_t MainHandle;
uint32_t MainBuffer[ 1024 ];
osStaticThreadDef_t MainControlBlock;
const osThreadAttr_t Main_attributes = {
  .name = "Main",
  .cb_mem = &MainControlBlock,
  .cb_size = sizeof(MainControlBlock),
  .stack_mem = &MainBuffer[0],
  .stack_size = sizeof(MainBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes = {
  .name = "TouchGFXTask",
  .stack_size = 3048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Input */
osThreadId_t InputHandle;
uint32_t InputBuffer[ 1024 ];
osStaticThreadDef_t InputControlBlock;
const osThreadAttr_t Input_attributes = {
  .name = "Input",
  .cb_mem = &InputControlBlock,
  .cb_size = sizeof(InputControlBlock),
  .stack_mem = &InputBuffer[0],
  .stack_size = sizeof(InputBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Output */
osThreadId_t OutputHandle;
uint32_t OutputBuffer[ 1024 ];
osStaticThreadDef_t OutputControlBlock;
const osThreadAttr_t Output_attributes = {
  .name = "Output",
  .cb_mem = &OutputControlBlock,
  .cb_size = sizeof(OutputControlBlock),
  .stack_mem = &OutputBuffer[0],
  .stack_size = sizeof(OutputBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Datalog */
osThreadId_t DatalogHandle;
uint32_t DatalogBuffer[ 1024 ];
osStaticThreadDef_t DatalogControlBlock;
const osThreadAttr_t Datalog_attributes = {
  .name = "Datalog",
  .cb_mem = &DatalogControlBlock,
  .cb_size = sizeof(DatalogControlBlock),
  .stack_mem = &DatalogBuffer[0],
  .stack_size = sizeof(DatalogBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Network */
osThreadId_t NetworkHandle;
uint32_t NetworkBuffer[ 1024 ];
osStaticThreadDef_t NetworkControlBlock;
const osThreadAttr_t Network_attributes = {
  .name = "Network",
  .cb_mem = &NetworkControlBlock,
  .cb_size = sizeof(NetworkControlBlock),
  .stack_mem = &NetworkBuffer[0],
  .stack_size = sizeof(NetworkBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ZWave */
osThreadId_t ZWaveHandle;
uint32_t ZWaveBuffer[ 1024 ];
osStaticThreadDef_t ZWaveControlBlock;
const osThreadAttr_t ZWave_attributes = {
  .name = "ZWave",
  .cb_mem = &ZWaveControlBlock,
  .cb_size = sizeof(ZWaveControlBlock),
  .stack_mem = &ZWaveBuffer[0],
  .stack_size = sizeof(ZWaveBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MainQueue */
osMessageQueueId_t MainQueueHandle;
uint8_t MainQueueBuffer[ 64 * sizeof( uint16_t ) ];
osStaticMessageQDef_t MainQueueControlBlock;
const osMessageQueueAttr_t MainQueue_attributes = {
  .name = "MainQueue",
  .cb_mem = &MainQueueControlBlock,
  .cb_size = sizeof(MainQueueControlBlock),
  .mq_mem = &MainQueueBuffer,
  .mq_size = sizeof(MainQueueBuffer)
};
/* Definitions for TouchGFXQueue */
osMessageQueueId_t TouchGFXQueueHandle;
uint8_t TouchGFXQueueBuffer[ 64 * sizeof( uint16_t ) ];
osStaticMessageQDef_t TouchGFXQueueControlBlock;
const osMessageQueueAttr_t TouchGFXQueue_attributes = {
  .name = "TouchGFXQueue",
  .cb_mem = &TouchGFXQueueControlBlock,
  .cb_size = sizeof(TouchGFXQueueControlBlock),
  .mq_mem = &TouchGFXQueueBuffer,
  .mq_size = sizeof(TouchGFXQueueBuffer)
};
/* Definitions for ZWaveRxQueue */
osMessageQueueId_t ZWaveRxQueueHandle;
uint8_t ZWaveRxQueueBuffer[ 2048 * sizeof( uint8_t ) ];
osStaticMessageQDef_t ZWaveRxQueueControlBlock;
const osMessageQueueAttr_t ZWaveRxQueue_attributes = {
  .name = "ZWaveRxQueue",
  .cb_mem = &ZWaveRxQueueControlBlock,
  .cb_size = sizeof(ZWaveRxQueueControlBlock),
  .mq_mem = &ZWaveRxQueueBuffer,
  .mq_size = sizeof(ZWaveRxQueueBuffer)
};
/* Definitions for DiagnosticRxQueue */
osMessageQueueId_t DiagnosticRxQueueHandle;
uint8_t DiagnosticRxQueueBuffer[ 256 * sizeof( uint8_t ) ];
osStaticMessageQDef_t DiagnosticRxQueueControlBlock;
const osMessageQueueAttr_t DiagnosticRxQueue_attributes = {
  .name = "DiagnosticRxQueue",
  .cb_mem = &DiagnosticRxQueueControlBlock,
  .cb_size = sizeof(DiagnosticRxQueueControlBlock),
  .mq_mem = &DiagnosticRxQueueBuffer,
  .mq_size = sizeof(DiagnosticRxQueueBuffer)
};
/* Definitions for DiagnosticMutex */
osMutexId_t DiagnosticMutexHandle;
osStaticMutexDef_t DiagnosticMutexControlBlock;
const osMutexAttr_t DiagnosticMutex_attributes = {
  .name = "DiagnosticMutex",
  .cb_mem = &DiagnosticMutexControlBlock,
  .cb_size = sizeof(DiagnosticMutexControlBlock),
};
/* USER CODE BEGIN PV */

// Output buffer for diagnostic output
char gucDiagnosticOutput[2048];
uint8_t gucDiagnosticRxBuffer[SERIAL_BUFFER_SIZE];

// ZWave TX/RX buffers
//uint8_t gucZWaveTxBuffer[SERIAL_BUFFER_SIZE];
uint8_t gucZWaveRxBuffer[SERIAL_BUFFER_SIZE];

// Real-time clock variables
RTC_TimeTypeDef sMainRTCTime;
RTC_DateTypeDef sMainRTCDate;
uint_32          sUNIXTime;
uint_32          sOldUNIXTime = 0;

// Elapsed runtime
uint32_t gulElapsedTime_Runtime_sec = 0;

// Inclusion completed or failed
uint8_t gucIsInclusionJoiningNodeFinished = FALSE;
uint8_t gucIsInclusionIncludingNodeFinished = FALSE;
uint8_t gucIsInclusionFailed   = FALSE;

// Exclusion completed
uint8_t gucIsExclusionFinished = FALSE;
uint8_t gucIsExclusionFailed   = FALSE;

// S2 bootstrap completed or failed
uint8_t gucIsBootstrapFinished = FALSE;
uint8_t gucIsBootstrapFailed   = FALSE;

/////////////////////////////
//// TEST MAB 2025.10.03
//// Temporary "saved" IP address bytes
uint8_t gucAddress1Saved = 10;
uint8_t gucAddress2Saved = 1;
uint8_t gucAddress3Saved = 2;
uint8_t gucAddress4Saved = 33;
uint8_t gucDNS1Saved = 10;
uint8_t gucDNS2Saved = 1;
uint8_t gucDNS3Saved = 2;
uint8_t gucDNS4Saved = 81;
uint8_t gucGateway1Saved = 10;
uint8_t gucGateway2Saved = 1;
uint8_t gucGateway3Saved = 2;
uint8_t gucGateway4Saved = 1;
uint8_t gucNetmask1Saved = 255;
uint8_t gucNetmask2Saved = 255;
uint8_t gucNetmask3Saved = 255;
uint8_t gucNetmask4Saved = 0;

//// Temporarily saved DHCP
uint8_t gucIsDHCPEnabled = true;

//// Temporary Standby mode and countdown
// JJR-STANDBY
uint8_t gucStandbyMode=eSTANDBY_OFF;
//  Standby Mode
uint32_t gulStandbyCounter=0;  /* In Standby mode when When <> 0 */
/////////////////////////////

// Received bytes from Z-Wave controller and Diagnostic ports
uint8_t gucReceivedByteFromZWave;
uint8_t gucReceivedByteFromDiagnostic;

/* Queue for frames transmitted to Z-Wave controller (EFR32ZG23) */
#define MAX_CALLBACK_QUEUE  32
#define MAX_UNSOLICITED_QUEUE 8

typedef struct _callback_element_{
  uint8_t wCmd;
  uint8_t wLen;
  uint8_t wBuf[BUF_SIZE_TX];
} CALLBACK_ELEMENT;

typedef struct _request_queue_{
  uint8_t requestOut;
  uint8_t requestIn;
  uint8_t requestCnt;
  CALLBACK_ELEMENT requestQueue[MAX_CALLBACK_QUEUE];
} REQUEST_QUEUE;

REQUEST_QUEUE gstructCallbackQueue = { 0 };

typedef struct _request_unsolicited_queue_{
  uint8_t requestOut;
  uint8_t requestIn;
  uint8_t requestCnt;
  CALLBACK_ELEMENT requestQueue[MAX_UNSOLICITED_QUEUE];
} REQUEST_UNSOLICITED_QUEUE;

REQUEST_UNSOLICITED_QUEUE gstructCommandQueue = { 0 };

static ZWaveRxInterface_t gtZWaveRxInterface = {
  .state = ZWAVE_RX_SOF,
  .buffer_len = 0,
};

ZWaveInterfaceFrame_ptr const ZWaveSerialFrame = (ZWaveInterfaceFrame_ptr)gtZWaveRxInterface.buffer;

//
// Dispatch tables for Z-Wave command handlers
//
typedef void (*zwave_cmd_handler_t)(void);
zwave_cmd_handler_t gtZWave_CMD_Handler[256];
zwave_cmd_handler_t gtZWave_CC_Handler[256];

// State variables for ZWave state machine
uint8_t gucRetryCount = 0;

// Payload buffer for transmitted ZWave frames
// (comparable to compl_workbuf[] in ZWave_NCP_SerialAPI_Controller_Solution
uint8_t gucZWaveWorkbuf[BUF_SIZE_TX];

// Z-Wave controller Home and Node IDs
uint32_t gulZWaveHomeID = 0;
uint16_t guiZWaveNodeID = 0;

// Z-Wave received command class buffer
uint8_t* pgucCCBuffer;
uint8_t  gucCCBufferLength;

// Node Provisioning list (i.e. DSK and state variables for end nodes)
pl_entry_t gtNodeProvisioningList[NODE_PROVISIONING_LIST_COUNT];

//
// Index of DSK currently being processed (may be NO DSK being processed)
uint8_t gucProcessingDSK = DSK_UNAVAILABLE;

// Session ID
uint8_t gucSessionID;

//
// Copy of LR nodes [0x0100, 0x04FF]
//
uint8_t gucLRNodes[MAX_LR_NODEMASK_LENGTH];

// general useage NodeID
uint16_t guiNodeID;

// Bootstrap state machine state
BootstrapState geBootstrapState;

// Flags for Bootstrap
uint8_t gucIsKEXReportReceived;
uint8_t gucIsS2Supported;
uint8_t gucIsPublicKeyReportReceived;
uint8_t gucIsIncludingNode;
uint8_t gucIsNonceGetReceived;

// NVR values
uint8_t gucNVROffset = NVR_UNUSED_OFFSET;
uint8_t gucControllerPublicKey[32];
uint8_t gucControllerPrivateKey[32];

// wolfSSL stuff
WC_RNG gtWolfSSLRng;

// For Temporary Symmetric key
static const uint8_t CKDF_TEMP_EXTRACT_KEY_C[16] = {
    /* 16 bytes of 0x33 per CKDF-TempExtract */
    /* a.k.a. ConstantPRK */
    0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
    0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33
};
static const uint8_t CKDF_TEMP_EXPAND_C[15] = {
    /* 15 bytes of 0x88 per CKDF-TempExpand */
    /* a.k.a. ConstantTE */
    /* ALSO a.k.a. ConstEntropyInput of CKDF-MEI-Expand */
    0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
    0x88,0x88,0x88,0x88,0x88,0x88,0x88
};
uint8_t gucTemporarySymmetricKey[16];
uint8_t gucTemporaryREI[16];
uint8_t gucTemporarySEI[16];

// For Nonce generation
static const uint8_t CKDF_MEI_EXTRACT_C[15] = {
    /* 16 bytes of 0x26 per CKDF-MEI-Extract */
    /* a.k.a. ConstNonce */
    0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26,
    0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26
};



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_MDMA_Init(void);
static void MX_FMC_Init(void);
static void MX_LTDC_Init(void);
static void MX_CRC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_JPEG_Init(void);
static void MX_QUADSPI_Init(void);
static void MX_I2C4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_SDMMC1_MMC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RNG_Init(void);
void MainTask(void *argument);
void TouchGFX_Task(void *argument);
void InputTask(void *argument);
void OutputTask(void *argument);
void DatalogTask(void *argument);
void NetworkTask(void *argument);
void ZWaveTask(void *argument);

/* USER CODE BEGIN PFP */

BootstrapState ZWave_Bootstrap_StateMachine(BootstrapStateMachineCommand stateMachineCommand);
uint8_t ZWave_DSK_Extract_NWIAuthHomeID(uint8_t aucDSKIndex, uint8_t* paucNWIAuthHomeIDBuffer);
uint8_t ZWave_DSK_Find_Zeroized(void);
uint8_t ZWave_DSK_IsProcessing(void);
uint8_t ZWave_DSK_IsZeroized(uint8_t aucDSKIndex);
uint8_t ZWave_DSK_Validate_String(uint8_t* paucDSKString);
void ZWave_DSK_Write(uint8_t aucDSKIndex, uint8_t* paucDSKBuffer);
uint8_t ZWave_DSK_Write_From_String(uint8_t aucDSKIndex, uint8_t* paucDSKString);
void ZWave_DSK_Write_To_String(uint8_t aucDSKIndex, uint8_t* paucDSKBuffer);
void ZWave_DSK_Zeroize(uint8_t aucDSKIndex);
void ZWave_REQ_CMD_0A_Serial_API_Started(void);
void ZWave_RES_CMD_02_Get_Init_Data(void);
void ZWave_RES_CMD_05_ZW_Get_Controller_Capabilities(void);
void ZWave_RES_CMD_07_Serial_API_Get_Capabilities(void);
void ZWave_RES_CMD_0B_Serial_API_Setup(void);
void ZWave_RSQ_CMD_12_ZW_Send_Node_Information(void);
void ZWave_RSQ_CMD_13_ZW_Send_Data(void);
void ZWave_RES_CMD_15_ZW_Get_Version(void);
void ZWave_RES_CMD_20_Memory_Get_ID(void);
void ZWave_RES_CMD_28_NVR_Get_Value(void);
void ZWave_REQ_CMD_3F_ZW_Remove_Node_ID_From_Network(void);
void ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info(void);
void ZWave_REQ_CMD_42_ZW_Set_Default(void);
void ZWave_RSQ_CMD_46_ZW_Assign_Return_Route(void);
void ZWave_RSQ_CMD_47_ZW_Delete_Return_Route(void);
void ZWave_REQ_CMD_49_ZW_Application_Update(void);
void ZWave_REQ_CMD_4A_ZW_Add_Node_To_Network(void);
void ZWave_REQ_CMD_4B_ZW_Remove_Node_From_Network(void);
void ZWave_RES_CMD_50_ZW_Set_Learn_Mode(void);
void ZWave_RSQ_CMD_51_ZW_Assign_SUC_Return_Route(void);
void ZWave_RSQ_CMD_54_ZW_Set_SUC_NodeID(void);
void ZWave_RSQ_CMD_55_ZW_Delete_SUC_Return_Route(void);
void ZWave_RES_CMD_56_ZW_Get_SUC_Node_ID(void);
void ZWave_RES_CMD_60_ZW_Request_Node_Info(void);
void ZWave_RSQ_CMD_61_ZW_Remove_Failed_Node(void);
void ZWave_RES_CMD_62_ZW_Is_Failed_Node(void);
void ZWave_RES_CMD_A6_ZW_Is_Virtual_Node(void);
void ZWave_RES_CMD_A8_Application_Command_Handler_Bridge(void);
void ZWave_RES_CMD_A9_ZW_Send_Data_Bridge(void);
void ZWave_RES_CMD_DA_Serial_API_Get_LR_Nodes(void);
void ZWave_RES_CMD_DE_Get_DCDC_Config(void);
void ZWave_RES_CMD_DF_Set_DCDC_Config(void);
void ZWave_RES_CMD_E8_Get_Radio_PTI(void);
void ZWave_RES_CMD_XX_Unsupported(void);
void ZWave_Rx_CC_26_Switch_Multilevel_V4(void);
void ZWave_Rx_CC_9F_Security_2_V2(void);
void ZWave_Rx_CC_XX_Unsupported(void);
uint8_t ZWave_Scan_ProvisioningList_For_DSK(uint8_t* paucDSKBuffer, uint8_t aucNeedExactMatch);
uint8_t ZWave_Scan_ProvisioningList_For_NodeID(uint16_t auiNodeID);
uint8_t ZWave_Scan_ProvisioningList_For_NWIHomeID(uint32_t aulNWIHomeID);
#if ENABLE_ZWAVE_CONTROLLER_HOST
void ZWave_Send_REQ_CMD_02_Serial_API_Get_Init_Data(void);
void ZWave_Send_REQ_CMD_03_Serial_API_Application_Node_Information(void);
void ZWave_Send_REQ_CMD_05_Get_Controller_Capabilities(void);
void ZWave_Send_REQ_CMD_07_Serial_API_Get_Capabilities(void);
void ZWave_Send_REQ_CMD_08_Serial_API_Soft_Reset(void);
void ZWave_Send_REQ_CMD_0B_Serial_API_Setup(eSerialAPISetupCmd aeSetupCommand, int16_t aiParameter1, int16_t aiParameter2);
void ZWave_Send_REQ_CMD_12_Send_Node_Information(uint16_t auiNodeID, uint8_t aucTxOption, uint8_t aucSessionID);
void ZWave_Send_REQ_CMD_13_Send_Data(uint16_t auiNodeID, uint8_t aucLength, uint8_t* paucData, uint8_t aucTxOption, uint8_t aucSessionID);
void ZWave_Send_REQ_CMD_20_Memory_Get_ID(void);
void ZWave_Send_REQ_CMD_28_NVR_Get_Value(uint8_t aucOffset, uint8_t aucLength);
void ZWave_Send_REQ_CMD_3F_Remove_Specific_Node_from_Network(uint8_t aucOptions, uint8_t aucSessionID, uint16_t auiNodeID);
void ZWave_Send_REQ_CMD_41_Get_Node_Protocol_Info(uint16_t auiNodeID);
void ZWave_Send_REQ_CMD_42_Set_Default(void);
void ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(uint8_t aucOptions, uint8_t aucSessionID, uint8_t* paucNWIAuthID);
void ZWave_Send_REQ_CMD_56_Get_SUC_Node_ID(void);
void ZWave_Send_REQ_CMD_61_Remove_Failed_Node(uint8_t aucSessionID, uint16_t auiNodeID);
void ZWave_Send_REQ_CMD_62_Is_Node_Failed(uint16_t auiNodeID);
void ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes(void);
void ZWave_Send_REQ_CMD_DE_Get_DCDC_Config(void);
void ZWave_Send_REQ_CMD_DF_Set_DCDC_Config(sl_dcdc_config_t atDCDCMode);
void ZWave_Send_REQ_CMD_E8_Get_Radio_PTI(void);
#endif // ENABLE_ZWAVE_CONTROLLER_HOST
uint8_t ZWave_SessionID_Randomize(void);
uint8_t ZWave_SessionID_Update(uint8_t aucSessionID);
int ZWave_Temporary_Key_Generate(void);
void ZWave_Transmit_Frame(uint8_t aucCMD, uint8_t aucType, const uint8_t* paucPayload, uint8_t aucLength);
uint8_t ZWave_XOR_Checksum(uint8_t aucInitialValue, const uint8_t *paucDataBuffer, uint8_t aucLength);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  //int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
//  /* Wait until CPU2 boots and enters in stop mode or timeout*/
//  timeout = 0xFFFF;
//  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
//  if ( timeout < 0 )
//  {
//  Error_Handler();
//  }
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
///* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
//HSEM notification */
///*HW semaphore Clock enable*/
//__HAL_RCC_HSEM_CLK_ENABLE();
///*Take HSEM */
//HAL_HSEM_FastTake(HSEM_ID_0);
///*Release HSEM in order to notify the CPU2(CM4)*/
//HAL_HSEM_Release(HSEM_ID_0,0);
///* wait until CPU2 wakes up from stop mode */
//timeout = 0xFFFF;
//while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
//if ( timeout < 0 )
//{
//Error_Handler();
//}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_MDMA_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_JPEG_Init();
  MX_QUADSPI_Init();
  MX_I2C4_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_FATFS_Init();
  MX_SDMMC1_MMC_Init();
  MX_USART2_UART_Init();
  MX_RNG_Init();
  MX_TouchGFX_Init();
  /* Call PreOsInit function */
  MX_TouchGFX_PreOSInit();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of DiagnosticMutex */
  DiagnosticMutexHandle = osMutexNew(&DiagnosticMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of MainQueue */
  MainQueueHandle = osMessageQueueNew (64, sizeof(uint16_t), &MainQueue_attributes);

  /* creation of TouchGFXQueue */
  TouchGFXQueueHandle = osMessageQueueNew (64, sizeof(uint16_t), &TouchGFXQueue_attributes);

  /* creation of ZWaveRxQueue */
  ZWaveRxQueueHandle = osMessageQueueNew (2048, sizeof(uint8_t), &ZWaveRxQueue_attributes);

  /* creation of DiagnosticRxQueue */
  DiagnosticRxQueueHandle = osMessageQueueNew (256, sizeof(uint8_t), &DiagnosticRxQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Main */
  MainHandle = osThreadNew(MainTask, NULL, &Main_attributes);

  /* creation of TouchGFXTask */
  TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL, &TouchGFXTask_attributes);

  /* creation of Input */
  InputHandle = osThreadNew(InputTask, NULL, &Input_attributes);

  /* creation of Output */
  OutputHandle = osThreadNew(OutputTask, NULL, &Output_attributes);

  /* creation of Datalog */
  DatalogHandle = osThreadNew(DatalogTask, NULL, &Datalog_attributes);

  /* creation of Network */
  NetworkHandle = osThreadNew(NetworkTask, NULL, &Network_attributes);

  /* creation of ZWave */
  ZWaveHandle = osThreadNew(ZWaveTask, NULL, &ZWave_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_R2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C4_Init(void)
{

  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x009034B6;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */

}

/**
  * @brief JPEG Initialization Function
  * @param None
  * @retval None
  */
static void MX_JPEG_Init(void)
{

  /* USER CODE BEGIN JPEG_Init 0 */

  /* USER CODE END JPEG_Init 0 */

  /* USER CODE BEGIN JPEG_Init 1 */

  /* USER CODE END JPEG_Init 1 */
  hjpeg.Instance = JPEG;
  if (HAL_JPEG_Init(&hjpeg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN JPEG_Init 2 */

  /* USER CODE END JPEG_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */
  initLtdcClocks();
  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 40;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 53;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 533;
  hltdc.Init.AccumulatedActiveH = 283;
  hltdc.Init.TotalWidth = 565;
  hltdc.Init.TotalHeigh = 285;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 272;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0xD0000000;
  pLayerCfg.ImageWidth = 480;
  pLayerCfg.ImageHeight = 272;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = 26;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_ENABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */
  initBspQuadSpi(&Error_Handler);
  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_MMC_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hmmc1.Instance = SDMMC1;
  hmmc1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hmmc1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hmmc1.Init.BusWide = SDMMC_BUS_WIDE_8B;
  hmmc1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hmmc1.Init.ClockDiv = 0;
  if (HAL_MMC_Init(&hmmc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable MDMA controller clock
  */
static void MX_MDMA_Init(void)
{

  /* MDMA controller clock enable */
  __HAL_RCC_MDMA_CLK_ENABLE();
  /* Local variables */

  /* MDMA interrupt initialization */
  /* MDMA_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MDMA_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(MDMA_IRQn);

}

/* FMC initialization function */
void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM2 memory initialization sequence
  */
  hsdram2.Instance = FMC_SDRAM_DEVICE;
  /* hsdram2.Init */
  hsdram2.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram2.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram2.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram2.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram2.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram2.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram2.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram2.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram2.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram2.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 6;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 6;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram2, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
  initBspSdRam(&Error_Handler);
  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN1_GPIO_Port, LED_GREEN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FRAME_RATE_GPIO_Port, FRAME_RATE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCU_ACTIVE_GPIO_Port, MCU_ACTIVE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN2_GPIO_Port, LED_GREEN2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED_GREEN1_Pin */
  GPIO_InitStruct.Pin = LED_GREEN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FRAME_RATE_Pin */
  GPIO_InitStruct.Pin = FRAME_RATE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(FRAME_RATE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_IRQ_Pin */
  GPIO_InitStruct.Pin = TP_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TP_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MCU_ACTIVE_Pin */
  GPIO_InitStruct.Pin = MCU_ACTIVE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(MCU_ACTIVE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_GREEN2_Pin */
  GPIO_InitStruct.Pin = LED_GREEN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(TP_IRQ_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TP_IRQ_EXTI_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */

/** *****************************************************************************************************************************
  * @brief  Convert date and time values to UNIX timestamp (epoch 1970-01-01 00:00:00)
  * @param  acYear   [00,99]
  * @param  acMonth  [01,12]
  * @param  acDate   [01,31]
  * @param  acHour   [00,23]
  * @param  acMinute [00,59]
  * @param  acSecond [00,59]
  * @retval unsigned 32-bit UNIX timestamp
  */
uint_32 ConvertTimeToUNIX(uint_8 acYear, uint_8 acMonth, uint_8 acDate, uint_8 acHour, uint_8 acMinute, uint_8 acSecond)
{
  // NOTE: since it is expected to use RTC values, there is no range checking
  static struct tm tim = {0};
  static time_t lUNIXTime;

  tim.tm_year = acYear + 2000 - 1900;
  tim.tm_mon  = acMonth - 1;
  tim.tm_mday = acDate;
  tim.tm_hour = acHour;
  tim.tm_min  = acMinute;
  tim.tm_sec  = acSecond;
  lUNIXTime = mktime(&tim);
  return (uint_32)lUNIXTime;
}
// end ConvertTimeToUNIX

/** *****************************************************************************************************************************
  * @brief  Read received FIFO bytes from Diagnostic port
  * @param  *aucResponseBuffer - pointer to buffer into which received bytes will be copied
  * @retval Count of bytes received (possibly 0)
  */
uint16_t Diagnostic_Receive_Response(uint8_t* aucReceiveBuffer)
{
  static uint16_t luiDiagnosticRxCount;
  static uint16_t i;
  static uint8_t lucReceivedChar;
  static osStatus_t ltDiagnosticRxQueueStatus;
  //LOG("%s: START\r\n", __FUNCTION__);

  // Copy bytes (if any) into RX buffer
  luiDiagnosticRxCount = osMessageQueueGetCount(DiagnosticRxQueueHandle);
  if (luiDiagnosticRxCount > 0)
  {
    LOG("%s: Initial luiDiagnosticRxCount = %d\r\n", __FUNCTION__, luiDiagnosticRxCount);
    // Read bytes from Diagnostic RX queue into RX buffer
    for (i = 0; osMessageQueueGetCount(DiagnosticRxQueueHandle) > 0 && i < SERIAL_BUFFER_SIZE-2; ++i)
    {
      ltDiagnosticRxQueueStatus = osMessageQueueGet(DiagnosticRxQueueHandle, &lucReceivedChar, NULL, 0);
      if (osOK != ltDiagnosticRxQueueStatus)
      {
        LOG("%s: *** WARNING *** ltDiagnosticRxQueueStatus = %d\r\n", __FUNCTION__, ltDiagnosticRxQueueStatus);
      }
      aucReceiveBuffer[i] = lucReceivedChar;
    }
    luiDiagnosticRxCount = i;
    //aucReceiveBuffer[i+1] = 0;
    LOG("%s:   FINAL luiDiagnosticRxCount = %d\r\n", __FUNCTION__, luiDiagnosticRxCount);
  }
//  else
//  {
//    LOG("%s: Nothing received from Diagnostic\r\n", __FUNCTION__);
//  }

  //LOG("%s: END\r\n", __FUNCTION__);
  return luiDiagnosticRxCount;
}
// end Diagnostic_Receive_Response

/** *****************************************************************************************************************************
  * @brief  Callback routine when UART/USART Rx ISR completes
  * @param  *huart - pointer to receiving UART
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint8_t lucReceivedByteFromDiagnostic;
  static uint8_t lucReceivedByteFromZWave;
  static osStatus_t ltStatus;
  static HAL_StatusTypeDef ltHALStatus;

  //////////////////////////////////////////////////////////////////////////////////////////
  //// MAB 2025.10.16
  //// Treat this callback as if it's still within the interrupt service routine,
  //// so minimal processing without delays; don't do debug prints unless
  //// absolutely necessary for debugging. Leave CPU-hogging routines
  //// and delays to task-level code.
  //////////////////////////////////////////////////////////////////////////////////////////

  //LOG("%s: huart->Instance = 0x%08X \r\n", __FUNCTION__, huart->Instance);

  //
  // Check if received a byte from the Diagnostic port
  //
  if (huart->Instance == USART1)
  {
    // Save the received byte locally
    lucReceivedByteFromDiagnostic = gucReceivedByteFromDiagnostic;

    // Stuff the received byte into the RX queue
    ltStatus = osMessageQueuePut(DiagnosticRxQueueHandle, &lucReceivedByteFromDiagnostic, 0, 0);
    if (osOK != ltStatus)
    {
      //LOG("%s: *** WARNING *** ltStatus for Diagnostic = %d\r\n", __FUNCTION__, ltStatus);
    }

    // Re-arm the UART RX interrupt for the next byte
    ltHALStatus = HAL_UART_Receive_IT(&huart1, &gucReceivedByteFromDiagnostic, 1);
    if (HAL_OK != ltHALStatus)
    {
      //LOG("%s: *** WARNING *** HAL_UART_Receive_IT(&huart1) returned 0x%02X\r\n", __FUNCTION__, ltHALStatus);
    }
    //__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  }

  //
  // Check if received a byte from the Z-Wave controller
  //
  if (huart->Instance == USART2)
  {
    // Save the received byte locally
    lucReceivedByteFromZWave = gucReceivedByteFromZWave;

    // Stuff the received byte into the RX queue
    ltStatus = osMessageQueuePut(ZWaveRxQueueHandle, &lucReceivedByteFromZWave, 0, 0);
    if (osOK != ltStatus)
    {
      LOG("%s: *** WARNING *** ltStatus for Z-Wave receive = %d\r\n", __FUNCTION__, ltStatus);
    }

    // Re-arm the UART RX interrupt for the next byte
    ltHALStatus = HAL_UART_Receive_IT(&huart2, &gucReceivedByteFromZWave, 1);
    if (HAL_OK != ltHALStatus)
    {
      //LOG("%s: *** WARNING *** HAL_UART_Receive_IT(&huart2) returned 0x%02X\r\n", __FUNCTION__, ltHALStatus);
    }
    //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  }
}
// end HAL_UART_RxCpltCallback

/** *****************************************************************************************************************************
  * @brief  Print buffer contents in hex and ASCII
  * @param  *buffer - pointer to buffer of received bytes
  * @param  len - count of received bytes
  * @retval None
  */
void PrintBytes( uint8_t* buffer, uint16_t len, sboolean printOffset, uint32_t offset)
{
  static uint16_t i;
  static uint16_t j;

  for ( i = 0; i < len; ++i)
  {
    if (printOffset && i%16 == 0)
    {
      LOG("Offset 0x%08X:   ",(unsigned int)(offset+i));
    }
    LOG("%02X",buffer[i]);
    LOG(" ");

    if ( i % 16 == 7)
    {
      LOG("      ");
    }
    if ( i % 16 == 15)
    {
      ///////////////////////////////////////////
      // Print ASCII equivalents (if printable)
      LOG("      ");
      for (j = i-15; j<i+1; ++j)
      {
        if (isprint(buffer[j]))
        {
          LOG("%c", buffer[j]);
        }
        else
        {
          LOG(".");
        }
      }
      ///////////////////////////////////////////
      LOG("\r\n");
    }
  }

  ///////////////////////////////////////////
  // Print "leftover" ASCII if any
  if (len % 16 != 0)
  {
    for (j = (len%16); j<16; ++j)
    {
      LOG("   ");
    }
    if (len%16 < 8) LOG("      ");

    LOG("      ");
    for (j = len-(len%16); j<len; ++j)
    {
      if (isprint(buffer[j]))
      {
        LOG("%c", buffer[j]);
      }
      else
      {
        LOG(".");
      }
    }
  }
  ///////////////////////////////////////////

  LOG("\r\n");

}
// end PrintBytes

/** *****************************************************************************************************************************
  * @brief  Print startup banner
  * @param  None
  * @retval None
  */
void PrintStartupBanner(void)
{
  osMutexWait(DiagnosticMutexHandle, 1000);
  LOG_NOW("\r\nSensaphone Z-Wave Sentinel starting...\r\n");
  LOG_NOW("\r\nSensaphone Z-Wave Sentinel starting...\r\n");
  LOG_NOW("\r\nSensaphone Z-Wave Sentinel starting...\r\n");
  LOG_NOW("\r\n\r\n\r\n");
  LOG_NOW("=================================<=>=================================\r\n");
  LOG_NOW("                     Sensaphone Z-Wave Sentinel                      \r\n");
  LOG_NOW("                              v%s.%s.%s.%c \r\n", VERSION_A,VERSION_B,VERSION_C,BOARD_REVISION);
  LOG_NOW("                             %s   \r\n", VERSION_DATE);
  LOG_NOW("                   Running on STM32H745I Discovery                   \r\n");
  LOG_NOW("=================================<=>=================================\r\n");
  osMutexRelease(DiagnosticMutexHandle);
}
// end PrintStartupBanner

/** *****************************************************************************************************************************
  * @brief  Return 32-bit very pseudorandom number
  * @param  None
  * @retval 32-bit unsigned value
  */
uint32_t RandomValue(void)
{
  static uint32_t lulReturnValue;

  // Using HAL library and STM32H7xx random number generator
  HAL_RNG_GenerateRandomNumber(&hrng, &lulReturnValue);

  return lulReturnValue;
}
// end RandomValue

/** *****************************************************************************************************************************
  * @brief  Check and count number of bit that is set in a nodemask
  * @param paucMask Pointer to nodemask that should be counted
  * @param aucLength Length of nodemask to count
  * @return Number of bits set in nodemask [0, 255]
  */
uint8_t ZW_NodeMaskBitsIn(const uint8_t* paucMask, uint8_t aucLength)
{
  uint16_t luiBitsSetcount = 0;
  uint8_t lucReturnValue;

  // Count each set bit in the node mask
  // NOTE: aucLength could be up to 255, so if all bits set, count is 0x800 (2-byte value)
  for (uint8_t lucByteIndex = 0; lucByteIndex < aucLength; ++lucByteIndex)
  {
    for (uint8_t lucBitIndex = 0; lucBitIndex < 8; ++lucBitIndex)
    {
      if ( paucMask[lucByteIndex] & (1<<lucBitIndex) )
      {
        ++luiBitsSetcount;
      }
    }
  }

  // Since the return value is only 8 bits long, limit return value to [0, 255]
  lucReturnValue = (luiBitsSetcount > 255) ? 255 : luiBitsSetcount;
  LOG("%s: reporting %d active nodes \r\n", __FUNCTION__, lucReturnValue);
  if (luiBitsSetcount > 255)
  {
    LOG("%s: - *** WARNING *** actually detected %d active nodes \r\n", __FUNCTION__, luiBitsSetcount);
  }

  return lucReturnValue;
}
// end ZW_NodeMaskBitsIn

/** *****************************************************************************************************************************
  * @brief  Clear the node bit in a node bitmask
  * @param  paucMask Pointer to Nodemask list that should be set
  * @param  auiNodeID Node ID that should be cleared in the mask
  * @retval None
  */
void ZW_NodeMaskClearBit(uint8_t* paucMask, node_id_t auiNodeID)
{
  // from NodeMask.h and from Z-Wave Host API specification
  // NodeID = 256 + (8 * node_list_byte_index) + (node_list_byte_bit) + (128 * 8 * node_list_start_offset)
  // Since node_list_start_offset = 0, node_list_byte_index in [0, 127] and node_list_byte_bit in [0, 7]
  //  (NodeID - 256) / 8 = node_list_byte_index
  //  (NodeID %   8)     = node_list_byte_bit

  uint8_t lucByteIndex = (auiNodeID-256) / 8;
  uint8_t lucBitIndex  = auiNodeID % 8;
  LOG("%s: NodeID = 0x%04X \t byte index = 0x%02X \t bit index = %d \t bit mask = 0x%02X \r\n", __FUNCTION__, auiNodeID, lucByteIndex, lucBitIndex, (uint8_t)~(1<<lucBitIndex) );

  // Now that we've got a byte index and bit index, clear the specific bit in the specified byte
  paucMask[lucByteIndex] &= (uint8_t)~(1 << lucBitIndex);
}
// end ZW_NodeMaskClearBit

/** *****************************************************************************************************************************
  * @brief  Find the next NodeIndex that is set in a nodemask
  * @param auiCurrentNodeID Last NodeId found (0 for first call)
  * @param paucMask Pointer to Nodemask list that should be searched
  * @return Next NodeId from the nodemask if found, or 0 if not found.
  */
node_id_t ZW_NodeMaskGetNextNodeIndex(node_id_t auiCurrentNodeID, uint8_t* paucMask)
{
  // from NodeMask.h and from Z-Wave Host API specification
  // NodeID = 256 + (8 * node_list_byte_index) + (node_list_byte_bit) + (128 * 8 * node_list_start_offset)
  // Since node_list_start_offset = 0, node_list_byte_index in [0, 127] and node_list_byte_bit in [0, 7]
  //  (NodeID - 256) / 8 = node_list_byte_index
  //  (NodeID %   8)     = node_list_byte_bit

  node_id_t luiCurrentNodeID;
  uint8_t lucCurrentByteIndex;
  uint8_t lucCurrentBitIndex;
  node_id_t ltNextNodeID = 0;
  uint8_t lucNextByteIndex;
  uint8_t lucNextBitIndex;

  // First, get the byte and bit indexes of the current NodeID
  if (auiCurrentNodeID)
  {
    // Calculate byte and bit indexes of current NodeID
    lucCurrentByteIndex = (auiCurrentNodeID-256) / 8;
    lucCurrentBitIndex  = auiCurrentNodeID % 8;
    //LOG("%s: current NodeID = 0x%04X \t byte index = 0x%02X \t bit index = %d \t bit mask = 0x%02X \r\n", __FUNCTION__, auiCurrentNodeID, lucCurrentByteIndex, lucCurrentBitIndex, (1<<lucCurrentBitIndex) );

    // Point to the next candidate NodeID and get byte and bit indexes
    luiCurrentNodeID = auiCurrentNodeID + 1;
    lucCurrentByteIndex = (luiCurrentNodeID-256) / 8;
    lucCurrentBitIndex  = (luiCurrentNodeID-256) % 8;
    //LOG("%s: start   NodeID = 0x%04X \t byte index = 0x%02X \t bit index = %d \t bit mask = 0x%02X \r\n", __FUNCTION__, luiCurrentNodeID, lucCurrentByteIndex, lucCurrentBitIndex, (1<<lucCurrentBitIndex) );
  }
  else
  {
    // NodeID is 0, so start searching at the beginning of the node list
    lucCurrentByteIndex = 0;
    lucCurrentBitIndex  = 0;
    //LOG("%s: current NodeID = 0x%04X \t byte index = 0x%02X \t bit index = %d \t bit mask = 0x%02X \r\n", __FUNCTION__, auiCurrentNodeID, lucCurrentByteIndex, lucCurrentBitIndex, (1<<lucCurrentBitIndex) );
  }

  // Next, scan the rest of the node list for the next set bit (if any)
  for (uint8_t lucByteIndex = lucCurrentByteIndex; lucByteIndex < MAX_LR_NODEMASK_LENGTH; ++lucByteIndex)
  {
    for (uint8_t lucBitIndex = lucCurrentBitIndex; lucBitIndex < 8; ++lucBitIndex)
    {
      if ( paucMask[lucByteIndex] & (1<<lucBitIndex) )
      {
        // An active NodeID has been found
        ltNextNodeID = 256 + (8*lucByteIndex) + lucBitIndex;
        lucNextByteIndex = lucByteIndex;
        lucNextBitIndex  = lucBitIndex;
        LOG("%s: next    NodeID = 0x%04X \t byte index = 0x%02X \t bit index = %d \t bit mask = 0x%02X \r\n", __FUNCTION__, ltNextNodeID, lucNextByteIndex, lucNextBitIndex, (1<<lucNextBitIndex) );

        // Break out of the for loops, we've found the next NodeID
        lucBitIndex  = 8;
        lucByteIndex = MAX_LR_NODEMASK_LENGTH;
      }
    }

    // After the first byte's remaining bits have been checked, next and subsequent bytes must be checked for all 8 bits
    lucCurrentBitIndex = 0;
  }

  if (0==ltNextNodeID)
  {
    LOG("%s: No more active nodes detected \r\n", __FUNCTION__ );
  }

  return ltNextNodeID;
}
// end ZW_NodeMaskGetNextNodeIndex

/** *****************************************************************************************************************************
  * @brief  Set the node bit in a node bitmask
  * @param  paucMask Pointer to Nodemask list that should be set
  * @param  auiNodeID Node ID that should be set in the mask
  * @retval None
  */
void ZW_NodeMaskSetBit(uint8_t* paucMask, node_id_t auiNodeID)
{
  // from NodeMask.h and from Z-Wave Host API specification
  // NodeID = 256 + (8 * node_list_byte_index) + (node_list_byte_bit) + (128 * 8 * node_list_start_offset)
  // Since node_list_start_offset = 0, node_list_byte_index in [0, 127] and node_list_byte_bit in [0, 7]
  //  (NodeID - 256) / 8 = node_list_byte_index
  //  (NodeID %   8)     = node_list_byte_bit

  uint8_t lucByteIndex = (auiNodeID-256) / 8;
  uint8_t lucBitIndex  = auiNodeID % 8;
  LOG("%s: NodeID = 0x%04X \t byte index = 0x%02X \t bit index = %d \t bit mask = 0x%02X \r\n", __FUNCTION__, auiNodeID, lucByteIndex, lucBitIndex, (1<<lucBitIndex) );

  // Now that we've got a byte index and bit index, set the specific bit in the specified byte
  paucMask[lucByteIndex] |= (1 << lucBitIndex);
}
// end ZW_NodeMaskSetBit

/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */

/** *****************************************************************************************************************************
  * @brief  Bootstrap state machine
  * @param  stateMachineCommand - INITIALIZE, RUN or STATE
  * @retval Present state
  */
/***********************************************
ZWave_Bootstrap_StateMachine

  IF command is INITIALIZE
    Clear elapsed time
    Initialize subordinate state machines
    Set state to IDLE

  ELSE IF command is RUN
    Update elapsed time
    IF timeout occurs
      Set state to ERROR
    ENDIF

    IF state is IDLE
      IF node inclusion has completed
        Reset elapsed time for next state
        Send S2 command KEX Get
        Set state to KEX
      ENDIF
    ELSE IF state is KEX
      IF KEX Report is received
        IF no S2 security levels are supported
          Set state to ERROR
        ELSE
          Reset elapsed time for next state
          Send S2 command KEX Set
          Set state to PUBLIC_KEY
        ENDIF
      ENDIF
    ELSE IF state is PUBLIC_KEY
      IF Public Key Report is received
        IF Including Node bit was set
          Set state to ERROR
        ELSE
          Reset elapsed time for next state
          Send S2 command Public Key Report (for controller)
          Set state to TEMP_NONCE_GET
        ENDIF
      ENDIF
    ELSE IF state is TEMP_NONCE_GET
      IF Nonce Get is received
        Reset elapsed time for next state
        Send Nonce Report
        Set state to TEMP_NONCE_SET
      ENDIF
    ELSE IF state is TEMP_NONCE_SET
    ELSE IF state is NETWORK_KEY_GET
    ELSE IF state is NETWORK_NONCE_GET
    ELSE IF state is NETWORK_VERIFY
    ELSE IF state is NETWORK_VERIFY_SPAN
    ELSE IF state is NETWORK_KEY_DONE
    ELSE IF state is COMPLETE
      Do nothing
    ELSE IF state is ERROR
      Do nothing
    ENDIF

  ELSE IF command is STATE
    Do nothing (present state will be returned)

  ELSE
    Flag faulty state machine call
  ENDIF (command)

  Return present state

END ZWave_Bootstrap_StateMachine
************************************************/
BootstrapState ZWave_Bootstrap_StateMachine(BootstrapStateMachineCommand stateMachineCommand)
{
  static BootstrapState leBootstrapState = BOOTSTRAP_IDLE;
  static uint32_t lulElapsedTime_sec = 0;
  static uint8_t lucOldSecond = 100; // Nonsense initial value guarantees update when RTC first read
  static uint8_t lucSendDataBuffer[80];

  //////////////////////////////////////////////////////////////////////////
  // IF command is INITIALIZE
  if (BOOTSTRAP_SM_CMD_INITIALIZE == stateMachineCommand)
  {
    LOG("%s: initializing\r\n", __FUNCTION__);
    // Clear elapsed time
    lulElapsedTime_sec = 0;

    // Initialize subordinate state machines

    // Set state to IDLE
    LOG("%s: Transitioning from initialization to IDLE\r\n", __FUNCTION__);
    leBootstrapState = BOOTSTRAP_IDLE;
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE IF command is RUN
  else if (BOOTSTRAP_SM_CMD_RUN == stateMachineCommand)
  {
    // Update elapsed time
    // (update only when the RTC seconds update, i.e. update once per second)
    if (lucOldSecond != sMainRTCTime.Seconds)
    {
      lucOldSecond = sMainRTCTime.Seconds;

      ++lulElapsedTime_sec;
    }

    // (Generic Bootstrap timeout; individual state timeouts might also occur)
    // IF timeout occurs
    if (lulElapsedTime_sec > 60 )
    {
      lulElapsedTime_sec = 0;
      // Set state to ERROR
      LOG("%s: *** WARNING *** Timeout: transitioning DSK %d Bootstrap state to ERROR\r\n", __FUNCTION__, gucProcessingDSK);
      leBootstrapState = BOOTSTRAP_ERROR;
    }
    // ENDIF


    //-------------------------------------------------------
    // IF state is IDLE
    if (BOOTSTRAP_IDLE == leBootstrapState)
    {
      // IF node inclusion has completed
      if (SMARTSTART_BOOTSTRAP == gtNodeProvisioningList[gucProcessingDSK].status)
      {
        // Send S2 command KEX Get
        gucSessionID = ZWave_SessionID_Randomize();
        memset(lucSendDataBuffer, 0x00, sizeof(lucSendDataBuffer));
        lucSendDataBuffer[0] = COMMAND_CLASS_SECURITY_2_V2;
        lucSendDataBuffer[1] = KEX_GET_V2;
        #if ENABLE_ZWAVE_CONTROLLER_HOST
        ZWave_Send_REQ_CMD_13_Send_Data(gtNodeProvisioningList[gucProcessingDSK].NodeID, 2, lucSendDataBuffer, TRANSMIT_OPTION_ACK, gucSessionID);
        #endif

        // Reset elapsed time for next state
        lulElapsedTime_sec = 0;

        // Set state to KEX
        gucIsKEXReportReceived = FALSE;
        LOG("%s: Transitioning DSK %d Bootstrap state from IDLE to KEX\r\n", __FUNCTION__, gucProcessingDSK);
        leBootstrapState = BOOTSTRAP_KEX;
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is KEX
    else if (BOOTSTRAP_KEX == leBootstrapState)
    {
      // IF KEX Report is received
      if (gucIsKEXReportReceived)
      {
        // IF no S2 security levels are supported
        if (!gucIsS2Supported)
        {
          // Set state to ERROR
          LOG("%s: Transitioning DSK %d Bootstrap state from KEX to ERROR\r\n", __FUNCTION__, gucProcessingDSK);
          leBootstrapState = BOOTSTRAP_ERROR;
        }
        // ELSE
        else
        {
          // Reset elapsed time for next state
          lulElapsedTime_sec = 0;

          // Send S2 command KEX Set
          gucSessionID = ZWave_SessionID_Randomize();
          memset(lucSendDataBuffer, 0x00, sizeof(lucSendDataBuffer));
          lucSendDataBuffer[0] = COMMAND_CLASS_SECURITY_2_V2;
          lucSendDataBuffer[1] = KEX_SET_V2;
          lucSendDataBuffer[2] = 0x00; // no client-side authentication; ECHO bit clear
          lucSendDataBuffer[3] = 0x02; // KEX Scheme 1 supported
          lucSendDataBuffer[4] = 0x01; // ECDH Profile: Curve25519 supported
          // Grant highest requested S2 level
          if (gtNodeProvisioningList[gucProcessingDSK].requested_keys & SECURITY_KEY_S2_ACCESS_BIT)
          {
            LOG("%s: S2 Access granted \r\n", __FUNCTION__);
            lucSendDataBuffer[5] = SECURITY_KEY_S2_ACCESS_BIT;
          }
          else if (gtNodeProvisioningList[gucProcessingDSK].requested_keys & SECURITY_KEY_S2_AUTHENTICATED_BIT)
          {
            LOG("%s: S2 Authenticated granted \r\n", __FUNCTION__);
           lucSendDataBuffer[5] = SECURITY_KEY_S2_AUTHENTICATED_BIT;
          }
          else if (gtNodeProvisioningList[gucProcessingDSK].requested_keys & SECURITY_KEY_S2_UNAUTHENTICATED_BIT)
          {
            LOG("%s: S2 Unauthenticated granted \r\n", __FUNCTION__);
            lucSendDataBuffer[5] = SECURITY_KEY_S2_UNAUTHENTICATED_BIT;
          }
          else
          {
            LOG("%s: *** WARNING *** NO S2 security level granted; S2 Bootstrap should fail \r\n", __FUNCTION__);
            lucSendDataBuffer[5] = 0; // no S2 security
          }
          LOG("%s: Saving DSK %d granted key \r\n", __FUNCTION__, gucProcessingDSK);
          gtNodeProvisioningList[gucProcessingDSK].granted_keys = lucSendDataBuffer[5];
          #if ENABLE_ZWAVE_CONTROLLER_HOST
          ZWave_Send_REQ_CMD_13_Send_Data(gtNodeProvisioningList[gucProcessingDSK].NodeID, 6, lucSendDataBuffer, TRANSMIT_OPTION_ACK, gucSessionID);
          #endif

          // Set state to PUBLIC_KEY
          gucIsPublicKeyReportReceived = FALSE;
          gucIsIncludingNode = FALSE;
          LOG("%s: Transitioning DSK %d Bootstrap state from KEX to PUBLIC_KEY\r\n", __FUNCTION__, gucProcessingDSK);
          leBootstrapState = BOOTSTRAP_PUBLIC_KEY;
        }
       // ENDIF
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is PUBLIC_KEY
    else if (BOOTSTRAP_PUBLIC_KEY == leBootstrapState)
    {
      // IF Public Key Report is received
      if (gucIsPublicKeyReportReceived)
      {
        // IF Including Node bit was set
        if (gucIsIncludingNode)
        {
          // Set state to ERROR
          LOG("%s: Transitioning DSK %d Bootstrap state from PUBLIC_KEY to ERROR\r\n", __FUNCTION__, gucProcessingDSK);
          leBootstrapState = BOOTSTRAP_ERROR;
        }
        // ELSE
        else
        {
          // Reset elapsed time for next state
          lulElapsedTime_sec = 0;

          // Send S2 command Public Key Report (for controller)
          gucSessionID = ZWave_SessionID_Randomize();
          memset(lucSendDataBuffer, 0x00, sizeof(lucSendDataBuffer));
          lucSendDataBuffer[0] = COMMAND_CLASS_SECURITY_2_V2;
          lucSendDataBuffer[1] = PUBLIC_KEY_REPORT_V2;
          lucSendDataBuffer[2] = PUBLIC_KEY_REPORT_PROPERTIES1_INCLUDING_NODE_BIT_MASK_V2; // Set Including Node bit
          // (Copy controller's public key into lucSendDataBuffer[3])
          memcpy(&lucSendDataBuffer[3], gucControllerPublicKey, 32);
          #if ENABLE_ZWAVE_CONTROLLER_HOST
          ZWave_Send_REQ_CMD_13_Send_Data(gtNodeProvisioningList[gucProcessingDSK].NodeID, 3+32, lucSendDataBuffer, TRANSMIT_OPTION_ACK, gucSessionID);
          #endif

          // Set state to TEMP_NONCE_GET
          gucIsNonceGetReceived = FALSE;
          LOG("%s: Transitioning DSK %d Bootstrap state from PUBLIC_KEY to TEMP_NONCE_GET\r\n", __FUNCTION__, gucProcessingDSK);
          leBootstrapState = BOOTSTRAP_TEMP_NONCE_GET;

          ///////////////////////////////////////////////////////////////
          //// MAB 2026.02.02
          //// At this point, with both the controller's and end node's
          //// ECDH public keys exchanged, both sides generate a
          //// Temporary Symmetric Key from the ECDH Shared Secret
          //// based on CKDF-TempExpand
          //// - End node's ECDH public key in the provisioning list
          ////   for the selected DSK:
          ////   gtNodeProvisioningList[gucProcessingDSK].ECDHPublicKey
          //// - Controller's ECDH public key:
          ////   gucControllerPublicKey
          //// - Controller's ECDH private key:
          ////   gucControllerPrivateKey
          //// If successful, Temporary Symmetric Key will be saved in
          //// gucTemporarySymmetricKey[]
          int liTemporaryKeyResult = ZWave_Temporary_Key_Generate();
          if (0 != liTemporaryKeyResult)
          {
            LOG("%s: *** WARNING *** failed to generate Temporary Symmetric Key \r\n", __FUNCTION__);
            LOG("%s: Transitioning DSK %d Bootstrap state from PUBLIC_KEY to ERROR\r\n", __FUNCTION__, gucProcessingDSK);
            leBootstrapState = BOOTSTRAP_ERROR;
          }
          ///////////////////////////////////////////////////////////////
        }
        // ENDIF
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is TEMP_NONCE_GET
    else if (BOOTSTRAP_TEMP_NONCE_GET == leBootstrapState)
    {
      // IF Nonce Get is received
      if (gucIsNonceGetReceived)
      {
        // Reset elapsed time for next state
        lulElapsedTime_sec = 0;

        // Send Nonce Report
        gucSessionID = ZWave_SessionID_Randomize();
        memset(lucSendDataBuffer, 0x00, sizeof(lucSendDataBuffer));
        lucSendDataBuffer[0] = COMMAND_CLASS_SECURITY_2_V2;
        lucSendDataBuffer[1] = SECURITY_2_NONCE_REPORT_V2;
        lucSendDataBuffer[2] = gucSessionID;
        lucSendDataBuffer[3] = SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK_V2; // set SOS bit, so must include REI bytes

        // (Generate 16 bytes of random data as Receiver's Entropy Input (REI); save it)
        //   wc_InitRng() is a blocking operation, so use it sparingly
        //   see https://www.wolfssl.com/documentation/manuals/wolfssl/group__Random.html#function-wc_initrng
        int liWolfSSLRngReturn = wc_InitRng(&gtWolfSSLRng);
        if (liWolfSSLRngReturn) LOG("%s: *** WARNING *** wc_InitRng() return value was %d \r\n", __FUNCTION__, liWolfSSLRngReturn);
        //liWolfSSLRngReturn = wc_RNG_GenerateBlock(&gtWolfSSLRng, gtNodeProvisioningList[gucProcessingDSK].REI, 16);
        liWolfSSLRngReturn = wc_RNG_GenerateBlock(&gtWolfSSLRng, gucTemporaryREI, 16);
        if (liWolfSSLRngReturn) LOG("%s: *** WARNING *** wc_RNG_GenerateBlock() return value was %d \r\n", __FUNCTION__, liWolfSSLRngReturn);
        //LOG("%s: - saving Receiver's Entropy Input (REI) for DSK %d \r\n", __FUNCTION__, gucProcessingDSK);
        //memcpy(&lucSendDataBuffer[4], gtNodeProvisioningList[gucProcessingDSK].REI, 16);
        LOG("%s: - saving Receiver's Entropy Input (REI) for TEMPORARY KEY \r\n", __FUNCTION__);
        memcpy(&lucSendDataBuffer[4], gucTemporaryREI, 16);
        liWolfSSLRngReturn = wc_FreeRng(&gtWolfSSLRng);
        if (liWolfSSLRngReturn) LOG("%s: *** WARNING *** wc_FreeRng() return value was %d \r\n", __FUNCTION__, liWolfSSLRngReturn);
        LOG("%s: TEMP_NONCE_GET randomized Receiver's Entropy Input (REI) \r\n", __FUNCTION__);
        PrintBytes(gucTemporaryREI, 16, FALSE, 0);

        #if ENABLE_ZWAVE_CONTROLLER_HOST
        ZWave_Send_REQ_CMD_13_Send_Data(gtNodeProvisioningList[gucProcessingDSK].NodeID, 4+16, lucSendDataBuffer, TRANSMIT_OPTION_ACK, gucSessionID);
        #endif

        // Set state to TEMP_NONCE_SET
        LOG("%s: Transitioning DSK %d Bootstrap state from TEMP_NONCE_GET to TEMP_NONCE_SET\r\n", __FUNCTION__, gucProcessingDSK);
        leBootstrapState = BOOTSTRAP_TEMP_NONCE_SET;
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is TEMP_NONCE_SET
    else if (BOOTSTRAP_TEMP_NONCE_SET == leBootstrapState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is NETWORK_KEY_GET
    else if (BOOTSTRAP_NETWORK_KEY_GET == leBootstrapState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is NETWORK_NONCE_GET
    else if (BOOTSTRAP_NETWORK_NONCE_GET == leBootstrapState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is NETWORK_VERIFY
    else if (BOOTSTRAP_NETWORK_VERIFY == leBootstrapState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is NETWORK_VERIFY_SPAN
    else if (BOOTSTRAP_NETWORK_VERIFY_SPAN == leBootstrapState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is NETWORK_KEY_DONE
    else if (BOOTSTRAP_XNETWORK_KEY_DONE == leBootstrapState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is COMPLETE
    else if (BOOTSTRAP_COMPLETE == leBootstrapState)
    {
      gucIsBootstrapFinished = TRUE;
    }

    //-------------------------------------------------------
    // ELSE IF state is ERROR
    else if (BOOTSTRAP_ERROR == leBootstrapState)
    {
      gucIsBootstrapFailed = TRUE;
    }


  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE IF command is STATE
  else if (BOOTSTRAP_SM_CMD_STATE == stateMachineCommand)
  {
    // Do nothing (present state will be returned)
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE
  else
  {
    // Flag faulty state machine call
    LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    LOG("\r\n%s: Invalid state machine command: stateMachineCommand = %d\r\n", __FUNCTION__, stateMachineCommand);
    LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
  }
  // ENDIF (command)
  //////////////////////////////////////////////////////////////////////////

  // Return present state
  geBootstrapState = leBootstrapState;
  return leBootstrapState;
}
// end ZWave_Bootstrap_StateMachine

/** *****************************************************************************************************************************
  * @brief  Display received frame data (to debug port)
  * @param  None
  * @retval None
  */
void ZWave_Display_Received_Frame_Data(void)
{
  // Display received frame data
  LOG("Received frame:\r\n");
  LOG("------------------------------------------------------------------------------\r\n");
  PrintBytes(ZWaveSerialFrame->payload, ZWaveSerialFrame->len + 1, false, 0); // Length doesn't include SOF so need to increment length
  LOG("------------------------------------------------------------------------------\r\n");
}
// end ZWave_Display_Received_Frame_Data

/** *****************************************************************************************************************************
  * @brief  Display Z-Wave Tx report
  * @param  None (uses ZWaveSerialFrame)
  * @retval None
  */
void ZWave_Display_Tx_Report(void)
{
  /* ZW->HOST: funcID | txStatus | wTransmitTicksMSB | wTransmitTicksLSB | bRepeaters | rssi_values.incoming[0] |
   *           rssi_values.incoming[1] | rssi_values.incoming[2] | rssi_values.incoming[3] | rssi_values.incoming[4] |
   *           bRouteSchemeState | repeater0 | repeater1 | repeater2 | repeater3 | routespeed |
   *           bRouteTries | bLastFailedLink.from | bLastFailedLink.to |
   *           bUsedTxpower | bMeasuredNoiseFloor | bAckDestinationUsedTxPower | bDestinationAckMeasuredRSSI |
   *           bDestinationckMeasuredNoiseFloor */
  LOG("----------------------- Tx report START -----------------------\r\n");
  PrintBytes(ZWaveSerialFrame->payload, ZWaveSerialFrame->len - 3, false, 0);
  LOG("%s: Session ID                                    = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: TX status                                     = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  switch (ZWaveSerialFrame->payload[1])
  {
  case TRANSMIT_COMPLETE_OK:
    LOG("%s: - transmit OK \r\n", __FUNCTION__);
    break;
  case TRANSMIT_COMPLETE_NO_ACK:
    LOG("%s: - transmit ERROR (no ACK received) \r\n", __FUNCTION__);
    break;
  case TRANSMIT_COMPLETE_FAIL:
    LOG("%s: - transmit ERROR (FAIL; network busy or jammed) \r\n", __FUNCTION__);
    break;
  case TRANSMIT_ROUTING_NOT_IDLE:
    LOG("%s: - transmit ERROR (routing not idle) \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** txStatus value UNKNOWN \r\n", __FUNCTION__);
    break;
  }

  LOG("%s: Transmit ticks                                = 0x%04X \r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[2]) + ZWaveSerialFrame->payload[3]);
  LOG("%s: Repeater count                                = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  LOG("%s: ACK RSSI                                      = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
  LOG("%s: Repeater 0 RSSI                               = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);
  LOG("%s: Repeater 1 RSSI                               = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[7]);
  LOG("%s: Repeater 2 RSSI                               = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[8]);
  LOG("%s: Repeater 3 RSSI                               = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[9]);
  LOG("%s: ACK channel num                               = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[10]);
  LOG("%s: Tx  channel num                               = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[11]);
  LOG("%s: Route scheme state                            = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[12]);
  LOG("%s: Repeater 0 last route                         = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[13]);
  LOG("%s: Repeater 1 last route                         = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[14]);
  LOG("%s: Repeater 2 last route                         = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[15]);
  LOG("%s: Repeater 3 last route                         = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[16]);
  LOG("%s: Beam bits/last route speed                    = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[17]);
  if (ZWaveSerialFrame->payload[17] & 0x40)
  {
    LOG("%s: - destination requires 1000 msec beam to be reached \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[17] & 0x20)
  {
    LOG("%s: - destination requires  250 msec beam to be reached \r\n", __FUNCTION__);
  }
  switch (ZWaveSerialFrame->payload[17] & 0x07)
  {
  case 0x01:
    LOG("%s: - Z-Wave 9.6 kbits/sec \r\n", __FUNCTION__);
    break;
  case 0x02:
    LOG("%s: - Z-Wave 40 kbits/sec \r\n", __FUNCTION__);
    break;
  case 0x03:
    LOG("%s: - Z-Wave 100 kbits/sec \r\n", __FUNCTION__);
    break;
  case 0x04:
    LOG("%s: - Z-Wave LR 100 kbits/sec \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** reserved/undefined value for Last Route Speed \r\n", __FUNCTION__);
    break;
  }
  LOG("%s: Routing attempts                              = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[18]);
  LOG("%s: Last route failed link     functional NodeID  = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[19]);
  LOG("%s: Last route failed link non-functional NodeID  = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[20]);
  LOG("%s: Tx power                                      = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[21]);
  LOG("%s: Measured noise floor                          = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[22]);
  LOG("%s: Destination ACK MPDU Tx power                 = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[23]);
  LOG("%s: Destination ACK MPDU measured RSSI            = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[24]);
  LOG("%s: Destination ACK MPDU measured noise floor     = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[25]);
  LOG("----------------------- Tx report  END  -----------------------\r\n");
}
// end ZWave_Display_Tx_Report

/** *****************************************************************************************************************************
  * @brief  Extract the NWI and Auth HomeID from the specified DSK in the node provisioning list
  * @param  uint8_t   aucDSKIndex   - index into node provisioning list [0, NODE_PROVISIONING_LIST_COUNT-1]
  * @param  uint8_t*  paucNWIAuthHomeIDBuffer - pointer to a byte buffer to which the NWI and Auth HomeIDs will be written
  * @retval TRUE if NWI and Auth HomeIDs extracted successfully; FALSE otherwise
  */
uint8_t ZWave_DSK_Extract_NWIAuthHomeID(uint8_t aucDSKIndex, uint8_t* paucNWIAuthHomeIDBuffer)
{
  uint8_t lucReturnValue = TRUE;

  if (0 <= aucDSKIndex && aucDSKIndex < NODE_PROVISIONING_LIST_COUNT)
  {
    // NWI HomeID derived from DSK bytes 8-11; MSB|0xC0; LSB&0xFE
    paucNWIAuthHomeIDBuffer[0] = gtNodeProvisioningList[aucDSKIndex].dsk[8] | 0xC0;
    //// TEST MAB 2026.01.22 paucNWIAuthHomeIDBuffer[0] = gtNodeProvisioningList[aucDSKIndex].dsk[8];
    paucNWIAuthHomeIDBuffer[1] = gtNodeProvisioningList[aucDSKIndex].dsk[9];
    paucNWIAuthHomeIDBuffer[2] = gtNodeProvisioningList[aucDSKIndex].dsk[10];
    paucNWIAuthHomeIDBuffer[3] = gtNodeProvisioningList[aucDSKIndex].dsk[11] & 0xFE;
    //// TEST MAB 2026.01.22 paucNWIAuthHomeIDBuffer[3] = gtNodeProvisioningList[aucDSKIndex].dsk[11];

    // Auth HomeID derived from DSK bytes 12-15; MSB|0xC0; LSB&0xFE
    paucNWIAuthHomeIDBuffer[4] = gtNodeProvisioningList[aucDSKIndex].dsk[12] | 0xC0;
    //// TEST MAB 2026.01.22 paucNWIAuthHomeIDBuffer[4] = gtNodeProvisioningList[aucDSKIndex].dsk[12];
    paucNWIAuthHomeIDBuffer[5] = gtNodeProvisioningList[aucDSKIndex].dsk[13];
    paucNWIAuthHomeIDBuffer[6] = gtNodeProvisioningList[aucDSKIndex].dsk[14];
    paucNWIAuthHomeIDBuffer[7] = gtNodeProvisioningList[aucDSKIndex].dsk[15] & 0xFE;
    //// TEST MAB 2026.01.22 paucNWIAuthHomeIDBuffer[7] = gtNodeProvisioningList[aucDSKIndex].dsk[15];
  }
  else
  {
    LOG("%s: *** WARNING *** invalid DSK index %d, NWI and Auth HomeID extraction aborted \r\n", __FUNCTION__, aucDSKIndex);
    lucReturnValue = FALSE;
  }

  return lucReturnValue;
}
// end ZWave_DSK_Extract_NWIAuthHomeID

/** *****************************************************************************************************************************
  * @brief  Find the first zeroized DSK in the node provisioning list
  * @param  None
  * @retval Index into the node provisioning list of the first zeroized DSK [0, NODE_PROVISIONING_LIST_COUNT-1]; 0xFF if none available
  */
uint8_t ZWave_DSK_Find_Zeroized(void)
{
  uint8_t lucReturnValue = DSK_UNAVAILABLE; // Assume no zeroized DSKs are present until proven otherwise
  uint8_t lucIsDSKZeroized;

  for (uint8_t lucDSKIndex = 0; lucDSKIndex < NODE_PROVISIONING_LIST_COUNT; ++lucDSKIndex)
  {
    // Assume this given DSK is zeroized until proven otherwise
    lucIsDSKZeroized = TRUE;

    for (uint8_t lucDSKByte = 0; lucDSKByte < DSK_LENGTH_BYTES; ++lucDSKByte)
    {
      if (gtNodeProvisioningList[lucDSKIndex].dsk[lucDSKByte])
      {
        lucIsDSKZeroized = FALSE;
      }
    }

    // After checking each byte of this DSK, if it's still flagged as zeroized, we've found one!
    if (lucIsDSKZeroized)
    {
      LOG("%s: DSK %d is available\r\n", __FUNCTION__, lucDSKIndex);
      // Return the DSK index of the zeroized DSK
      lucReturnValue = lucDSKIndex;

      // We can quit searching
      break;
    }
  }

  return lucReturnValue;
}
// end ZWave_DSK_Find_Zeroized

/** *****************************************************************************************************************************
  * @brief  Check if any DSK is currently being processed
  * @param  None
  * @retval TRUE if any DSK is currently being processed; FALSE otherwise
  */
uint8_t ZWave_DSK_IsProcessing(void)
{
  uint8_t lucReturnValue = FALSE;

  // Check each DSK in the provisioning list
  for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
  {
    if ( gtNodeProvisioningList[i].status != SMARTSTART_EMPTY  &&
         gtNodeProvisioningList[i].status != SMARTSTART_READY  &&
         gtNodeProvisioningList[i].status != SMARTSTART_ACTIVE    )
    {
      // A DSK has been found with is neither EMPTY, READY nor ACTIVE
      // Therefore this DSK is currently being processed
      lucReturnValue = TRUE;
      //LOG("%s: *** WARNING *** DSK %d is currently being processed\r\n", __FUNCTION__, i);

      // Might as well quit checking
      break;
    }
  }

  return lucReturnValue;
}
// end ZWave_DSK_IsProcessing

/** *****************************************************************************************************************************
  * @brief  Test if a specified DSK in the node provisioning list is zeroized
  * @param  uint8_t  aucDSKIndex - index into node provisioning list  [0, NODE_PROVISIONING_LIST_COUNT-1]
  * @retval TRUE if specified DSK is zeroized; FALSE otherwise
  */
uint8_t ZWave_DSK_IsZeroized(uint8_t aucDSKIndex)
{
  uint8_t lucIsDSKZeroized;

  // NOTE: The test for zeroization is strictly a DSK value of all zeros.
  //       Other state variables are not checked; use the ZWave_DSK_Zeroize()
  //       function to reset a DSK completely.

  if (0 <= aucDSKIndex && aucDSKIndex < NODE_PROVISIONING_LIST_COUNT)
  {
    // Assume this given DSK is zeroized until proven otherwise
    lucIsDSKZeroized = TRUE;

    for (uint8_t lucDSKByte = 0; lucDSKByte < DSK_LENGTH_BYTES; ++lucDSKByte)
    {
      if (gtNodeProvisioningList[aucDSKIndex].dsk[lucDSKByte])
      {
        lucIsDSKZeroized = FALSE;
      }
    }
  }
  else
  {
    LOG("%s: *** WARNING *** invalid DSK index %d, assume DSK is NOT zeroized \r\n", __FUNCTION__, aucDSKIndex);
    lucIsDSKZeroized = FALSE;
  }

  return lucIsDSKZeroized;
}
// end ZWave_DSK_IsZeroized

/** *****************************************************************************************************************************
  * @brief  Validate a string of decimal digits including delimiters is a valid DSK
  * @param  uint8_t*  paucDSKString - pointer to a string containing the 40-digit DSK (47 chars including delimiters)
  * @retval TRUE if DSK string is valid; FALSE otherwise
  */
uint8_t ZWave_DSK_Validate_String(uint8_t* paucDSKString)
{
  uint8_t lucIsDSKOK = TRUE;
  uint8_t* plucIntegerStart;
  uint8_t lucDSKInteger[6];
  uint32_t lulTempDSKInt;
  uint8_t lucDSKByteIndex;

  // The DSK string should be in the format NNNNN-NNNNN-NNNNN-NNNNN-NNNNN-NNNNN-NNNNN-NNNNN
  // where N is a decimal digit, and a non-digit delimiter separates each 5-digit number.
  // Since each 5-digit number is represented by 2 bytes, each 5-digit number must
  // be in the range [0, 65535].

  LOG("%s: Validating DSK %s\r\n", __FUNCTION__, paucDSKString);
  if (strlen(paucDSKString) != 47)
  {
    LOG("%s: *** WARNING *** DSK string is incorrect length of %d bytes\r\n", __FUNCTION__, strlen(paucDSKString));
    lucIsDSKOK = FALSE;
  }
  for (int i = 0; i < 47; ++i)
  {
    if ( i==5 || i==11 || i==17 || i==23 || i==29 || i==35 || i==41)
    {
      // Check if delimiters are non-digits
      if (isdigit(paucDSKString[i]))
      {
        LOG("%s: *** WARNING *** DSK string has an invalid delimiter at byte %d\r\n", __FUNCTION__, i);
        lucIsDSKOK = FALSE;
      }
    }
    else
    {
      // Check if digits are indeed digits
      if (!isdigit(paucDSKString[i]))
      {
        LOG("%s: *** WARNING *** DSK string has an invalid digit at byte %d\r\n", __FUNCTION__, i);
        lucIsDSKOK = FALSE;
      }
    }
  }

  // Parse the numerics
  plucIntegerStart = paucDSKString;
  lucDSKByteIndex = 0;
  while (lucDSKByteIndex < DSK_LENGTH_BYTES)
  {
    memset(lucDSKInteger, 0x00, sizeof(lucDSKInteger));
    memcpy(lucDSKInteger, plucIntegerStart, 5);
    lulTempDSKInt = atoi(lucDSKInteger);
    if (lulTempDSKInt > 0xFFFF)
    {
      // DSK has an invalid 5-digit number: exceeds 2 bytes
      LOG("%s: *** WARNING *** DSK number %d is too large \r\n", __FUNCTION__, lulTempDSKInt);
      lucIsDSKOK = FALSE;
    }
    //LOG("%s: lulTempDSKInt = %05d \r\n", __FUNCTION__, lulTempDSKInt);

    // Point to next 5-digit integer
    plucIntegerStart += 6;
    lucDSKByteIndex += 2;
  }

  if (lucIsDSKOK)
  {
    LOG("%s: DSK %s looks OK \r\n", __FUNCTION__, paucDSKString);
  }
  else
  {
    LOG("%s: *** WARNING *** DSK string %s FAILED validation \r\n", __FUNCTION__, paucDSKString);
  }




  return lucIsDSKOK;
}
// end ZWave_DSK_Validate_String

/** *****************************************************************************************************************************
  * @brief  Write a specified DSK to the node provisioning list
  * @param  uint8_t   aucDSKIndex   - index into node provisioning list to write [0, NODE_PROVISIONING_LIST_COUNT-1]
  * @param  uint8_t*  paucDSKBuffer - pointer to a byte buffer containing the 16-byte DSK
  * @retval None
  */
void ZWave_DSK_Write(uint8_t aucDSKIndex, uint8_t* paucDSKBuffer)
{
  // NOTE: This routine can overwrite an existing DSK
  //       Use ZWave_DSK_IsZeroized() to check if a DSK is zeroized before writing to it

  if (0 <= aucDSKIndex && aucDSKIndex < NODE_PROVISIONING_LIST_COUNT)
  {
    gtNodeProvisioningList[aucDSKIndex].lr_capable = ZWAVE_NODE_PROVISIONING_LIST_LR_CAPABLE;
    for (int j = 0; j < DSK_LENGTH_BYTES; ++j)
    {
      gtNodeProvisioningList[aucDSKIndex].dsk[j] = paucDSKBuffer[j];
    }
    gtNodeProvisioningList[aucDSKIndex].boot_mode = ZWAVE_NODE_PROVISIONING_LIST_SMARTSTART;
    gtNodeProvisioningList[aucDSKIndex].status = SMARTSTART_READY;
  }
  else
  {
    LOG("%s: *** WARNING *** invalid DSK index %d, DSK write aborted \r\n", __FUNCTION__, aucDSKIndex);
  }
}
// end ZWave_DSK_Write

/** *****************************************************************************************************************************
  * @brief  Write a specified DSK from a string of decimal digits including delimiters to the node provisioning list
  * @param  uint8_t   aucDSKIndex   - index into node provisioning list to write [0, NODE_PROVISIONING_LIST_COUNT-1]
  * @param  uint8_t*  paucDSKString - pointer to a string containing the 40-digit DSK (47 chars including delimiters)
  * @retval TRUE if DSK string successfully written to node provisioning list; FALSE otherwise
  */
uint8_t ZWave_DSK_Write_From_String(uint8_t aucDSKIndex, uint8_t* paucDSKString)
{
  uint8_t lucReturnValue = TRUE;
  uint8_t lucIsDSKOK = TRUE;
  uint8_t lucDSKBuffer[DSK_LENGTH_BYTES];
  uint8_t* plucIntegerStart;
  uint8_t lucDSKInteger[6];
  uint32_t lulTempDSKInt;
  uint8_t lucDSKByteIndex;

  // NOTE: This routine can overwrite an existing DSK
  //       Use ZWave_DSK_IsZeroized() to check if a DSK is zeroized before writing to it

  // Validate the DSK index
  if (aucDSKIndex >= NODE_PROVISIONING_LIST_COUNT)
  {
    LOG("%s: *** WARNING *** invalid DSK index %d \r\n", __FUNCTION__, aucDSKIndex);
    lucReturnValue = FALSE;
 }

  // The DSK string should be in the format NNNNN-NNNNN-NNNNN-NNNNN-NNNNN-NNNNN-NNNNN-NNNNN
  // where N is a decimal digit, and a non-digit delimiter separates each 5-digit number.
  // Since each 5-digit number is represented by 2 bytes, each 5-digit number must
  // be in the range [0, 65535].

  // Validate the string
  lucIsDSKOK = ZWave_DSK_Validate_String(paucDSKString);

  // Both the DSK index and the DSK string must validate OK to continue
  if (lucReturnValue)
  {
    lucReturnValue = lucIsDSKOK;
  }

  // Now assuming the DSK passed validation, parse the numerics into a 16-byte temporary buffer
  plucIntegerStart = paucDSKString;
  lucDSKByteIndex = 0;
  if (lucIsDSKOK)
  {
    while (lucDSKByteIndex < DSK_LENGTH_BYTES)
    {
      memset(lucDSKInteger, 0x00, sizeof(lucDSKInteger));
      memcpy(lucDSKInteger, plucIntegerStart, 5);
      lulTempDSKInt = atoi(lucDSKInteger);
      if (lulTempDSKInt > 0xFFFF)
      {
        // DSK has an invalid 5-digit number: exceeds 2 bytes
        LOG("%s: *** WARNING *** DSK number %d is too large \r\n", __FUNCTION__, lulTempDSKInt);
        lucIsDSKOK = FALSE;
      }
      //LOG("%s: lulTempDSKInt = %05d \r\n", __FUNCTION__, lulTempDSKInt);
      lucDSKBuffer[lucDSKByteIndex] = (uint8_t)(lulTempDSKInt / 0x100);
      lucDSKBuffer[lucDSKByteIndex+1] = (uint8_t)(lulTempDSKInt & 0xFF);

      // Point to next 5-digit integer
      plucIntegerStart += 6;
      lucDSKByteIndex += 2;
    }
    //PrintBytes(lucDSKBuffer, DSK_LENGTH_BYTES, false, 0);
  }

  // Is the DSK still OK? If so then write it to the node provisioning list
  if (lucReturnValue && lucIsDSKOK)
  {
    // DSK is in a buffer, so now write it to the node provisioning list
    ZWave_DSK_Write(aucDSKIndex, lucDSKBuffer);
  }
  else
  {
    LOG("%s: *** WARNING *** DSK write aborted \r\n", __FUNCTION__);
  }

  return lucReturnValue;
}
// end ZWave_DSK_Write_From_String

/** *****************************************************************************************************************************
  * @brief  Write a specified DSK from the node provisioning list to a string buffer
  * @param  uint8_t   aucDSKIndex   - index into node provisioning list to read [0, NODE_PROVISIONING_LIST_COUNT-1]
  * @param  uint8_t*  paucDSKString - pointer to a string to which the 40-digit DSK (47 chars including delimiters) will be written
  * @retval None
  */
void ZWave_DSK_Write_To_String(uint8_t aucDSKIndex, uint8_t* paucDSKBuffer)
{
  static char lucIntegerString[10];
  static char lucDSKString[60];
  static uint16_t luiTempDSKInt;

  memset(lucDSKString, 0x00, sizeof(lucDSKString));
  for (int j = 0; j < DSK_LENGTH_BYTES; j += 2)
  {
    luiTempDSKInt = 0x100*gtNodeProvisioningList[aucDSKIndex].dsk[j] + gtNodeProvisioningList[aucDSKIndex].dsk[j+1];
    memset(lucIntegerString, 0x00, sizeof(lucIntegerString));
    sprintf(lucIntegerString, "%05d", luiTempDSKInt);
    strcat(lucDSKString, lucIntegerString);
    if (j < DSK_LENGTH_BYTES-2) strcat(lucDSKString, "-");
  }
  memcpy(paucDSKBuffer, lucDSKString, strlen(lucDSKString));
}
// end ZWave_DSK_Write_To_String

/** *****************************************************************************************************************************
  * @brief  Zeroize a specified DSK in the node provisioning list
  * @param  uint8_t  aucDSKIndex - index into node provisioning list to zeroize [0, NODE_PROVISIONING_LIST_COUNT-1]
  * @retval None
  */
void ZWave_DSK_Zeroize(uint8_t aucDSKIndex)
{
  if (0 <= aucDSKIndex && aucDSKIndex < NODE_PROVISIONING_LIST_COUNT)
  {
    LOG("%s: Zeroizing DSK %d\r\n", __FUNCTION__, aucDSKIndex);
    gtNodeProvisioningList[aucDSKIndex].lr_capable = ZWAVE_NODE_PROVISIONING_LIST_MESH_ONLY;
    memset(gtNodeProvisioningList[aucDSKIndex].dsk, 0x00, DSK_LENGTH_BYTES); // zeroize DSK bytes
    gtNodeProvisioningList[aucDSKIndex].requested_keys = 0; // initialize requested keys: NO security levels supported
    gtNodeProvisioningList[aucDSKIndex].granted_keys = 0;   // initialize granted   keys: NO security levels supported
    gtNodeProvisioningList[aucDSKIndex].boot_mode = ZWAVE_NODE_PROVISIONING_LIST_S2_MANUAL; // initialize with zeroized DSK, so assume no SmartStart
    gtNodeProvisioningList[aucDSKIndex].status = SMARTSTART_EMPTY; // initialize with zeroized DSK, so status is EMPTY
    gtNodeProvisioningList[aucDSKIndex].NodeID = NODE_ID_UNAVAILABLE; // zeroize NodeID
    memset(gtNodeProvisioningList[aucDSKIndex].ECDHPublicKey, 0x00, 32); // zeroize ECDH public key
  }
  else
  {
    LOG("%s: *** WARNING *** invalid DSK index %d, DSK zeroize aborted \r\n", __FUNCTION__, aucDSKIndex);
  }
}
// end ZWave_DSK_Zeroize

/** *****************************************************************************************************************************
  * @brief  Add callback request to transmit callback queue
  * @param  aucCMD    - Command byte
  * @param  paucData  - pointer to data buffer
  * @param  aucLength - length of data buffer, in bytes
  * @retval TRUE if slot in transmit queue is available; FALSE if no slot available
  */
uint8_t ZWave_Enqueue_Request(uint8_t aucCMD, uint8_t* paucData, uint8_t aucLength)
{
  uint8_t lucReturnValue;

  // IF slot available in transmit callback queue
  if (gstructCallbackQueue.requestCnt < MAX_CALLBACK_QUEUE)
  {
    if (gstructCallbackQueue.requestCnt > (MAX_CALLBACK_QUEUE-3))
    {
      LOG("%s: *** WARNING *** gstructCallbackQueue.requestCnt=%d, MAX_CALLBACK_QUEUE=%d \r\n",
          __FUNCTION__, gstructCallbackQueue.requestCnt, MAX_CALLBACK_QUEUE);
    }

    LOG("%s: Adding CMD 0x%02X to callback queue\r\n", __FUNCTION__, aucCMD);
    // Add to transmit callback queue
    gstructCallbackQueue.requestCnt++;
    gstructCallbackQueue.requestQueue[gstructCallbackQueue.requestIn].wCmd = aucCMD;
    if (aucLength > (uint8_t)BUF_SIZE_TX)
    {
      aucLength = (uint8_t)BUF_SIZE_TX;
    }
    gstructCallbackQueue.requestQueue[gstructCallbackQueue.requestIn].wLen = aucLength;
    memcpy(&gstructCallbackQueue.requestQueue[gstructCallbackQueue.requestIn].wBuf[0], paucData, aucLength);

    // Move queue input pointer to next slot
    if (++gstructCallbackQueue.requestIn >= MAX_CALLBACK_QUEUE)
    {
      gstructCallbackQueue.requestIn = 0;
    }

    // Return TRUE
    lucReturnValue = TRUE;
  }
  // ELSE
  else
  {
    // Return FALSE
    LOG("%s: *** WARNING *** unable to enqueue CMD 0x%02X to callback queue\r\n", __FUNCTION__, aucCMD);
    lucReturnValue = FALSE;
  }
  // ENDIF

  return lucReturnValue;
}
// end ZWave_Enqueue_Request

/** *****************************************************************************************************************************
  * @brief  Add command request to transmit command queue
  * @param  aucCMD    - Command byte
  * @param  paucData  - pointer to data buffer
  * @param  aucLength - length of data buffer, in bytes
  * @retval TRUE if slot in transmit queue is available; FALSE if no slot available
  */
uint8_t ZWave_Enqueue_Request_Unsolicited(uint8_t aucCMD, uint8_t* paucData, uint8_t aucLength)
{
  uint8_t lucReturnValue;

  // IF slot available in transmit command queue
  if (gstructCommandQueue.requestCnt < MAX_UNSOLICITED_QUEUE)
  {
    if (gstructCommandQueue.requestCnt > (MAX_UNSOLICITED_QUEUE-3))
    {
      LOG("%s: *** WARNING *** gstructCommandQueue.requestCnt=%d, MAX_UNSOLICITED_QUEUE=%d \r\n",
          __FUNCTION__, gstructCommandQueue.requestCnt, MAX_UNSOLICITED_QUEUE);
    }

   LOG("%s: Adding CMD 0x%02X to command queue\r\n", __FUNCTION__, aucCMD);
   // Add to transmit command queue
    gstructCommandQueue.requestCnt++;
    gstructCommandQueue.requestQueue[gstructCommandQueue.requestIn].wCmd = aucCMD;
    if (aucLength > (uint8_t)BUF_SIZE_TX)
    {
      aucLength = (uint8_t)BUF_SIZE_TX;
    }
    gstructCommandQueue.requestQueue[gstructCommandQueue.requestIn].wLen = aucLength;
    memcpy(&gstructCommandQueue.requestQueue[gstructCommandQueue.requestIn].wBuf[0], paucData, aucLength);

    // Move queue input pointer to next slot
    if (++gstructCommandQueue.requestIn >= MAX_UNSOLICITED_QUEUE)
    {
      gstructCommandQueue.requestIn = 0;
    }

    // Return TRUE
    lucReturnValue = TRUE;
  }
  // ELSE
  else
  {
    // Return FALSE
    LOG("%s: *** WARNING *** unable to enqueue CMD 0x%02X to command queue\r\n", __FUNCTION__, aucCMD);
    lucReturnValue = FALSE;
  }
  // ENDIF

  return lucReturnValue;
}
// end ZWave_Enqueue_Request_Unsolicited

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is CHECKSUM
  * @param  aucRxByte - received byte from ZWave controller
  * @param  aucIsACKRequired - TRUE if ACK/NAK should be sent based on checksum; FALSE otherwise
  * @retval Result of validating checksum
  */
ZWaveRxParseResult_t ZWave_Handle_CHECKSUM(uint8_t aucRxByte, uint8_t aucIsACKRequired)
{
  static ZWaveRxParseResult_t ltResult;
  #if ENABLE_ZWAVE_CONTROLLER_HOST
  static uint8_t lucResponse;
  #endif

  LOG("%s: CHECKSUM byte is 0x%02X\r\n", __FUNCTION__, aucRxByte);

  // Reset byte timeout
  gtZWaveRxInterface.byte_timeout = false;

  // Stop byte timer
  gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer

  // Disable Rx active
  gtZWaveRxInterface.rx_active = false;  // Not really active now...

  /* Default values for ack == false */
  /* It means we are in the process of looking for an acknowledge to a callback request */
  // MAB 2025.10.22 - Another way to think of it:
  // comm_interface.c, SerialAPIStateHandler() state is stateTxSerial, stateCallbackTxSerial or stateCommandTxSerial
  /* Drop the new frame we received - we don't have time to handle it. */
  ltResult = ZWAVE_RX_PARSE_IDLE;
  #if ENABLE_ZWAVE_CONTROLLER_HOST
  lucResponse = CAN;
  #endif

  /* Do we send ACK/NAK according to checksum... */
  /* if not then the received frame is dropped! */
  if (aucIsACKRequired)
  {
    uint8_t checksum = ZWave_XOR_Checksum(0xFF, &(ZWaveSerialFrame->len), ZWaveSerialFrame->len);
    ltResult = (aucRxByte == checksum) ? ZWAVE_RX_PARSE_FRAME_RECEIVED : ZWAVE_RX_PARSE_FRAME_ERROR;
    #if ENABLE_ZWAVE_CONTROLLER_HOST
    lucResponse = (aucRxByte == checksum) ? ACK : NAK;
    #endif
  }

  // At this point the received frame (minus the checksum) has been saved to the Rx buffer
  // Display the received frame
  //LOG("%s: gtZWaveRxInterface.buffer_len=%d \t ZWaveSerialFrame->len=%d\r\n", __FUNCTION__, gtZWaveRxInterface.buffer_len, ZWaveSerialFrame->len);
  LOG("-----------------------  Z-Wave received frame (minus checksum) START -----------------------\r\n");
  PrintBytes(gtZWaveRxInterface.buffer, gtZWaveRxInterface.buffer_len, false, 0);
  LOG("-----------------------  Z-Wave received frame (minus checksum)  END  -----------------------\r\n");

  // Set ZWave Rx state to SOF
  //LOG("%s: Transitioning from CHECKSUM to SOF\r\n", __FUNCTION__);
  gtZWaveRxInterface.state = ZWAVE_RX_SOF;

  #if ENABLE_ZWAVE_CONTROLLER_HOST
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Transmit ACK (checksum OK), NAK (checksum error) or CAN (unable to process received frame: received frame dropped)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  HAL_UART_Transmit(&huart2, (uint8_t *)&lucResponse, 1, 1);
  switch (lucResponse)
  {
  case ACK:
    LOG("%s: Transmitting ACK (checksum OK) to Z-Wave controller\r\n", __FUNCTION__);
    break;
  case NAK:
    LOG("%s: *** WARNING *** Transmitting NAK (checksum error) to Z-Wave controller\r\n", __FUNCTION__);
    break;
  case CAN:
    LOG("%s: *** WARNING *** Transmitting CAN (unable to process received frame: received frame dropped) to Z-Wave controller\r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: *** WARNING *** Transmitting 0x%02X UNKNOWN to Z-Wave controller\r\n", __FUNCTION__, lucResponse);
    break;
  }
  #endif

  // Return result
  return ltResult;
}
// end ZWave_Handle_CHECKSUM

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is CMD
  * @param  aucRxByte - received byte from ZWave controller
  * @retval None
  */
void ZWave_Handle_CMD(uint8_t aucRxByte)
{
  //LOG("%s: CMD byte is 0x%02X\r\n", __FUNCTION__, aucRxByte);

  // Save received byte to ZWave Rx buffer
  gtZWaveRxInterface.buffer[gtZWaveRxInterface.buffer_len] = aucRxByte;
  gtZWaveRxInterface.buffer_len++;

  // IF a data payload is expected
  if (ZWaveSerialFrame->len > 3)
  {
    // Set Rx wait count to the size of the data payload
    gtZWaveRxInterface.rx_wait_count = ZWaveSerialFrame->len - 3;

    // Set ZWave Rx state to DATA
    //LOG("%s: Transitioning from CMD to DATA\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_DATA;
  }
  // ELSE (no data payload)
  else
  {
    // Set Rx wait count to 1 (the checksum byte)
    gtZWaveRxInterface.rx_wait_count = 1;

    // Set ZWave Rx state to CHECKSUM
    //LOG("%s: Transitioning from CMD to CHECKSUM\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_CHECKSUM;
  }
  // ENDIF
}
// end ZWave_Handle_CMD

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is DATA
  * @param  aucRxByte - received byte from ZWave controller
  * @retval None
  */
void ZWave_Handle_DATA(uint8_t aucRxByte)
{
  //LOG("%s: DATA byte is 0x%02X\r\n", __FUNCTION__, aucRxByte);

  // Decrement Rx wait count
  gtZWaveRxInterface.rx_wait_count--;

  // Save received byte to ZWave Rx buffer
  gtZWaveRxInterface.buffer[gtZWaveRxInterface.buffer_len] = aucRxByte;
  gtZWaveRxInterface.buffer_len++;

  // IF last data payload bytes has been received
  if ( (gtZWaveRxInterface.buffer_len >= RECEIVE_BUFFER_SIZE) || (gtZWaveRxInterface.buffer_len > ZWaveSerialFrame->len) )
  {
    // Set ZWave Rx state to CHECKSUM
    //LOG("%s: Transitioning from DATA to CHECKSUM\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_CHECKSUM;
  }
  // ENDIF
}
// end ZWave_Handle_DATA

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is an unexpected state
  * @param  None
  * @retval None
  */
void ZWave_Handle_Default(void)
{
  // If this routine is called, the ZWave Rx state is an unknown state
  // Reset to SOF

  // Set ZWave Rx state to SOF
  LOG("%s: Transitioning to SOF\r\n", __FUNCTION__);
  gtZWaveRxInterface.state = ZWAVE_RX_SOF;

  // Disable Rx active
  gtZWaveRxInterface.rx_active = false;  // Not really active now...

  // Reset ACK timeout
  gtZWaveRxInterface.ack_timeout = false;

  // Reset byte timeout
  gtZWaveRxInterface.byte_timeout = false;

  // Stop ACK timer
  gtZWaveRxInterface.ack_timeout_ms = 0; // stop the timer

  // Stop byte timer
  gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer

}
// end ZWave_Handle_Default

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is LEN
  * @param  aucRxByte - received byte from ZWave controller
  * @retval None
  */
void ZWave_Handle_LEN(uint8_t aucRxByte)
{
  // Validate length
  if (aucRxByte < FRAME_LENGTH_MIN || FRAME_LENGTH_MAX < aucRxByte)
  {
    // (invalid length; discard)
    LOG("%s: *** WARNING *** Invalid length byte = 0x%02X\r\n", __FUNCTION__, aucRxByte);

    // Set ZWave Rx state to SOF
    //LOG("%s: Transitioning from LEN to SOF\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_SOF;

    // Disable Rx active
    gtZWaveRxInterface.rx_active = false;  // Not really active now...

    // Reset byte timeout
    gtZWaveRxInterface.byte_timeout = false;

    // Stop byte timer
    gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer
  }
  else
  {
    // (valid length)
    //LOG("%s: Length byte = 0x%02X\r\n", __FUNCTION__, aucRxByte);

    // Set ZWave Rx state to TYPE
    //LOG("%s: Transitioning from LEN to TYPE\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_TYPE;

    // Save received byte to ZWave Rx buffer
    gtZWaveRxInterface.buffer[gtZWaveRxInterface.buffer_len] = aucRxByte;
    gtZWaveRxInterface.buffer_len++;
  }
}
// end ZWave_Handle_LEN

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is SOF
  * @param  aucRxByte - received byte from ZWave controller
  * @retval Result of parsing received byte
  */
ZWaveRxParseResult_t ZWave_Handle_SOF(uint8_t aucRxByte)
{
  static ZWaveRxParseResult_t ltResult;

  // Initialize result to IDLE
  ltResult = ZWAVE_RX_PARSE_IDLE;

  // IF received byte is SOF
  if (SOF == aucRxByte)
  {
    LOG("%s: Received a SOF \r\n", __FUNCTION__);

    // Set ZWave Rx state to LEN
    //LOG("%s: Transitioning from SOF to LEN\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_LEN;

    // Reset ZWave RX buffer length to 0
    gtZWaveRxInterface.buffer_len = 0;

    // Activate ZWave Rx; enable timeout
    gtZWaveRxInterface.rx_active = true; // now we're receiving - check for timeout

    // Save received byte to ZWave Rx buffer
    gtZWaveRxInterface.buffer[gtZWaveRxInterface.buffer_len] = aucRxByte;
    gtZWaveRxInterface.buffer_len++;

    // Reset byte timeout
    gtZWaveRxInterface.byte_timeout = false;
  }

  // ELSE IF ACK is pending
  else if (gtZWaveRxInterface.ack_needed)
  {
    // IF received ACK or NAK
    if (ACK == aucRxByte || NAK == aucRxByte)
    {
      // Reset ACK pending
      gtZWaveRxInterface.ack_needed = false;

      // Reset ACK timeout
      gtZWaveRxInterface.ack_timeout = false;

      // Reset byte timeout
      gtZWaveRxInterface.byte_timeout = false;

      // Stop ACK timer
      gtZWaveRxInterface.ack_timeout_ms = 0; // stop the timer

      // Stop byte timer
      gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer
    }
    // ENDIF
    // IF received byte is ACK
    if (ACK == aucRxByte)
    {
      LOG("%s: Received an ACK \r\n", __FUNCTION__);
      // Set result to FRAME_SENT
      ltResult = ZWAVE_RX_PARSE_FRAME_SENT;
   }
    // ELSE IF received byte is NAK
    else if (NAK == aucRxByte)
    {
      LOG("%s: Received a NAK \r\n", __FUNCTION__);
      // Set result to TX_TIMEOUT
      ltResult = ZWAVE_RX_PARSE_TX_TIMEOUT;
    }
    // ELSE
    else
    {
      // Discard received byte
      if (isprint(aucRxByte))
      {
        LOG("%s: Received byte 0x%02X (%c), discarding... \r\n", __FUNCTION__, aucRxByte, aucRxByte);
      }
      else
      {
        switch (aucRxByte)
        {
        case ACK: LOG("%s: Received ACK, discarding... \r\n", __FUNCTION__);                     break;
        case NAK: LOG("%s: Received NAK, discarding... \r\n", __FUNCTION__);                     break;
        case CAN: LOG("%s: Received CAN, discarding... \r\n", __FUNCTION__);                     break;
        default:  LOG("%s: Received byte 0x%02X, discarding... \r\n", __FUNCTION__, aucRxByte); break;
        } // end switch
      }
    }
    // ENDIF
  }

  // ELSE
  else
  {
    // Discard received byte
    if (isprint(aucRxByte))
    {
      LOG("%s: Received byte 0x%02X (%c), discarding... \r\n", __FUNCTION__, aucRxByte, aucRxByte);
    }
    else
    {
      switch (aucRxByte)
      {
      case ACK: LOG("%s: Received ACK, discarding... \r\n", __FUNCTION__);                     break;
      case NAK: LOG("%s: Received NAK, discarding... \r\n", __FUNCTION__);                     break;
      case CAN: LOG("%s: Received CAN, discarding... \r\n", __FUNCTION__);                     break;
      default:  LOG("%s: Received byte 0x%02X, discarding... \r\n", __FUNCTION__, aucRxByte); break;
      } // end switch
    }

    // Reset ACK timeout
    gtZWaveRxInterface.ack_timeout = false;

    // Stop ACK timer
    gtZWaveRxInterface.ack_timeout_ms = 0; // stop the timer
  }
  // ENDIF

  // Return result
  return ltResult;
}
// end ZWave_Handle_SOF

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is TYPE
  * @param  aucRxByte - received byte from ZWave controller
  * @retval None
  */
void ZWave_Handle_TYPE(uint8_t aucRxByte)
{
  // IF received byte is neither REQUEST nor RESPONSE
  if (aucRxByte > RESPONSE)
  {
    LOG("%s: *** WARNING *** Invalid TYPE byte = 0x%02X is neither REQUEST (0) nor RESPONSE (1)\r\n", __FUNCTION__, aucRxByte);
    // Set ZWave Rx state to SOF
    LOG("%s: Transitioning from TYPE to SOF\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_SOF;

    // Disable Rx active
    gtZWaveRxInterface.rx_active = false;  // Not really active now...

    // Reset byte timeout
    gtZWaveRxInterface.byte_timeout = false;

    // Stop byte timer
    gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer
  }
  // ELSE
  else
  {
    if (REQUEST == aucRxByte)
    {
      LOG("%s: TYPE byte is REQUEST (0)\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: TYPE byte is RESPONSE (1)\r\n", __FUNCTION__);
    }
    // Set ZWave Rx state to CMD
    //LOG("%s: Transitioning from TYPE to CMD\r\n", __FUNCTION__);
    gtZWaveRxInterface.state = ZWAVE_RX_CMD;

    // Save received byte to ZWave Rx buffer
    gtZWaveRxInterface.buffer[gtZWaveRxInterface.buffer_len] = aucRxByte;
    gtZWaveRxInterface.buffer_len++;
  }
  // ENDIF
}
// end ZWave_Handle_TYPE

/** *****************************************************************************************************************************
  * @brief  Identify Z-Wave Basic device type
  * @param  aucBasicDeviceType
  * @retval None
  */
void ZWave_Identify_Basic_Device_Type(uint8_t aucBasicDeviceType)
{
  switch (aucBasicDeviceType)
  {
  case BASIC_TYPE_CONTROLLER:
    LOG("%s: - Node is a portable controller \r\n", __FUNCTION__);
    break;
  case BASIC_TYPE_ROUTING_END_NODE:
    LOG("%s: - Node is an End Node with routing capabilities \r\n", __FUNCTION__);
    break;
  case BASIC_TYPE_END_NODE:
    LOG("%s: - Node is an End Node \r\n", __FUNCTION__);
    break;
  case BASIC_TYPE_STATIC_CONTROLLER:
    LOG("%s: - Node is a static controller \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** Basic node type 0x%02X is UNKNOWN \r\n", __FUNCTION__, aucBasicDeviceType);
    break;
  }
}
// end ZWave_Identify_Basic_Device_Type

/** *****************************************************************************************************************************
  * @brief  Identify Z-Wave Generic device type
  * @param  aucGenericDeviceType
  * @retval None
  */
void ZWave_Identify_Generic_Device_Type(uint8_t aucGenericDeviceType)
{
  switch (aucGenericDeviceType)
  {
  case GENERIC_TYPE_AV_CONTROL_POINT:
    LOG("%s: - AV Control Point \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_DISPLAY:
    LOG("%s: - Display \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_ENTRY_CONTROL:
    LOG("%s: - Entry Control \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_GENERIC_CONTROLLER:
    LOG("%s: - Remote Controller \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_METER:
    LOG("%s: - Meter \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_METER_PULSE:
    LOG("%s: - Pulse Meter \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_NON_INTEROPERABLE:
    LOG("%s: - Non interoperable \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_REPEATER_END_NODE:
    LOG("%s: - Repeater End Node \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SECURITY_PANEL:
    LOG("%s: - Security panel \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SEMI_INTEROPERABLE:
    LOG("%s: - Semi Interoperable \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SENSOR_ALARM:
    LOG("%s: - Sensor alarm \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SENSOR_BINARY:
    LOG("%s: - Binary Sensor \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SENSOR_MULTILEVEL:
    LOG("%s: - Multilevel Sensor \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_STATIC_CONTROLLER:
    LOG("%s: - Static Controller \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SWITCH_BINARY:
    LOG("%s: - Binary Switch \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SWITCH_MULTILEVEL:
    LOG("%s: - Multilevel Switch \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SWITCH_REMOTE:
    LOG("%s: - Remote Switch \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SWITCH_TOGGLE:
    LOG("%s: - Toggle Switch \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_THERMOSTAT:
    LOG("%s: - Thermostat \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_VENTILATION:
    LOG("%s: - Ventilation \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_WINDOW_COVERING:
    LOG("%s: - Window Covering \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_ZIP_NODE:
    LOG("%s: - Zip node \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_WALL_CONTROLLER:
    LOG("%s: - Wall controller \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_NETWORK_EXTENDER:
    LOG("%s: - Network Extender \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_APPLIANCE:
    LOG("%s: - Appliance \r\n", __FUNCTION__);
    break;
  case GENERIC_TYPE_SENSOR_NOTIFICATION:
    LOG("%s: - Sensor Notification \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** generic device type 0x%02X UNKNOWN \r\n", __FUNCTION__, aucGenericDeviceType);
    break;
  }
}
// end ZWave_Identify_Generic_Device_Type

/** *****************************************************************************************************************************
  * @brief  Identify Z-Wave Specific device type for a given Generic device type
  * @param  aucGenericDeviceType
  * @param  aucSpecificDeviceType
  * @retval None
  */
void ZWave_Identify_Specific_Device_Type(uint8_t aucGenericDeviceType, uint8_t aucSpecificDeviceType)
{
  // MAB 2025.11.21
  // For now, let's just identify specific types for generic types ZWave Sentinel is likely to encounter
  // Add additional cases in the future if needed

  if (GENERIC_TYPE_ENTRY_CONTROL == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_DOOR_LOCK:                        LOG("%s: - Door Lock \r\n", __FUNCTION__);                                 break;
    case SPECIFIC_TYPE_ADVANCED_DOOR_LOCK:               LOG("%s: - Advanced Door Lock \r\n", __FUNCTION__);                        break;
    case SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK:          LOG("%s: - Door Lock (keypad -lever) Device Type \r\n", __FUNCTION__);     break;
    case SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK_DEADBOLT: LOG("%s: - Door Lock (keypad - deadbolt) Device Type \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_SECURE_DOOR:                      LOG("%s: - Barrier Operator Specific Device Class \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_SECURE_GATE:                      LOG("%s: - Barrier Operator Specific Device Class \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_SECURE_BARRIER_ADDON:             LOG("%s: - Barrier Operator Specific Device Class \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_SECURE_BARRIER_OPEN_ONLY:         LOG("%s: - Barrier Operator Specific Device Class \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_SECURE_BARRIER_CLOSE_ONLY:        LOG("%s: - Barrier Operator Specific Device Class \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_SECURE_LOCKBOX:                   LOG("%s: - SPECIFIC_TYPE_SECURE_LOCKBOX \r\n", __FUNCTION__);              break;
    case SPECIFIC_TYPE_SECURE_KEYPAD:                    LOG("%s: - SPECIFIC_TYPE_SECURE_KEYPAD \r\n", __FUNCTION__);               break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
    }
  }

  if (GENERIC_TYPE_GENERIC_CONTROLLER == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER: LOG("%s: - Remote Control (Multi Purpose) \r\n", __FUNCTION__);        break;
    case SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER:  LOG("%s: - Portable Scene Controller \r\n", __FUNCTION__);             break;
    case SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL:    LOG("%s: - SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_REMOTE_CONTROL_AV:          LOG("%s: - Remote Control (AV) \r\n", __FUNCTION__);                   break;
    case SPECIFIC_TYPE_REMOTE_CONTROL_SIMPLE:      LOG("%s: - Remote Control (Simple) \r\n", __FUNCTION__);               break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_METER == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_SIMPLE_METER:            LOG("%s: - Sub Energy Meter \r\n", __FUNCTION__);                   break;
    case SPECIFIC_TYPE_ADV_ENERGY_CONTROL:      LOG("%s: - Whole Home Energy Meter (Advanced) \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_WHOLE_HOME_METER_SIMPLE: LOG("%s: - Whole Home Meter (Simple) \r\n", __FUNCTION__);          break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__);                               break;
   }
  }

  if (GENERIC_TYPE_SENSOR_ALARM == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_ADV_ZENSOR_NET_ALARM_SENSOR:   LOG("%s: - SPECIFIC_TYPE_ADV_ZENSOR_NET_ALARM_SENSOR \r\n", __FUNCTION__);   break;
    case SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR:   LOG("%s: - SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR \r\n", __FUNCTION__);   break;
    case SPECIFIC_TYPE_BASIC_ROUTING_ALARM_SENSOR:    LOG("%s: - SPECIFIC_TYPE_BASIC_ROUTING_ALARM_SENSOR \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR:    LOG("%s: - SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR \r\n", __FUNCTION__);    break;
    case SPECIFIC_TYPE_BASIC_ZENSOR_NET_ALARM_SENSOR: LOG("%s: - SPECIFIC_TYPE_BASIC_ZENSOR_NET_ALARM_SENSOR \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR: LOG("%s: - SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_ROUTING_ALARM_SENSOR:          LOG("%s: - SPECIFIC_TYPE_ROUTING_ALARM_SENSOR \r\n", __FUNCTION__);          break;
    case SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR:          LOG("%s: - SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR \r\n", __FUNCTION__);          break;
    case SPECIFIC_TYPE_ZENSOR_NET_ALARM_SENSOR:       LOG("%s: - SPECIFIC_TYPE_ZENSOR_NET_ALARM_SENSOR \r\n", __FUNCTION__);       break;
    case SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR:       LOG("%s: - SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR \r\n", __FUNCTION__);       break;
    case SPECIFIC_TYPE_ALARM_SENSOR:                  LOG("%s: - SPECIFIC_TYPE_ALARM_SENSOR \r\n", __FUNCTION__);                  break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_SENSOR_MULTILEVEL == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL: LOG("%s: - SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_CHIMNEY_FAN: LOG("%s: - SPECIFIC_TYPE_CHIMNEY_FAN \r\n", __FUNCTION__);                             break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_STATIC_CONTROLLER == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_PC_CONTROLLER: LOG("%s: - Central Controller \r\n", __FUNCTION__);                          break;
    case SPECIFIC_TYPE_SCENE_CONTROLLER: LOG("%s: - Scene Controller \r\n", __FUNCTION__);                         break;
    case SPECIFIC_TYPE_STATIC_INSTALLER_TOOL: LOG("%s: - SPECIFIC_TYPE_STATIC_INSTALLER_TOOL \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_SET_TOP_BOX: LOG("%s: - Set Top Box \r\n", __FUNCTION__);                                   break;
    case SPECIFIC_TYPE_SUB_SYSTEM_CONTROLLER: LOG("%s: - Sub System Controller \r\n", __FUNCTION__);               break;
    case SPECIFIC_TYPE_TV: LOG("%s: - TV \r\n", __FUNCTION__);                                                     break;
    case SPECIFIC_TYPE_GATEWAY: LOG("%s: - Gateway \r\n", __FUNCTION__);                                           break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_SWITCH_BINARY == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_POWER_SWITCH_BINARY: LOG("%s: - On/Off Power Switch \r\n", __FUNCTION__);                   break;
    case SPECIFIC_TYPE_SCENE_SWITCH_BINARY: LOG("%s: - Binary Scene  \r\n", __FUNCTION__);                         break;
    case SPECIFIC_TYPE_POWER_STRIP: LOG("%s: - Power Strip \r\n", __FUNCTION__);                                   break;
    case SPECIFIC_TYPE_SIREN: LOG("%s: - Siren \r\n", __FUNCTION__);                                               break;
    case SPECIFIC_TYPE_VALVE_OPEN_CLOSE: LOG("%s: - Valve (open/close)  \r\n", __FUNCTION__);                      break;
    case SPECIFIC_TYPE_COLOR_TUNABLE_BINARY: LOG("%s: - SPECIFIC_TYPE_COLOR_TUNABLE_BINARY \r\n", __FUNCTION__);   break;
    case SPECIFIC_TYPE_IRRIGATION_CONTROLLER: LOG("%s: - SPECIFIC_TYPE_IRRIGATION_CONTROLLER \r\n", __FUNCTION__); break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_SWITCH_MULTILEVEL == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL: LOG("%s: - Window Covering No Position/Endpoint \r\n", __FUNCTION__);      break;
    case SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL: LOG("%s: - Window Covering Endpoint Aware \r\n", __FUNCTION__);            break;
    case SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL: LOG("%s: - Window Covering Position/Endpoint Aware \r\n", __FUNCTION__);   break;
    case SPECIFIC_TYPE_MOTOR_MULTIPOSITION: LOG("%s: - Multiposition Motor \r\n", __FUNCTION__);                         break;
    case SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL: LOG("%s: - Light Dimmer Switch  \r\n", __FUNCTION__);                    break;
    case SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL: LOG("%s: - Multilevel Scene Switch \r\n", __FUNCTION__);                 break;
    case SPECIFIC_TYPE_FAN_SWITCH: LOG("%s: - Fan Switch \r\n", __FUNCTION__);                                           break;
    case SPECIFIC_TYPE_COLOR_TUNABLE_MULTILEVEL: LOG("%s: - SPECIFIC_TYPE_COLOR_TUNABLE_MULTILEVEL \r\n", __FUNCTION__); break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_SWITCH_REMOTE == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_SWITCH_REMOTE_BINARY: LOG("%s: - Binary Remote Switch \r\n", __FUNCTION__);                       break;
    case SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL: LOG("%s: - Multilevel Remote Switch \r\n", __FUNCTION__);               break;
    case SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY: LOG("%s: - Binary Toggle Remote Switch \r\n", __FUNCTION__);         break;
    case SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL: LOG("%s: - Multilevel Toggle Remote Switch \r\n", __FUNCTION__); break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__); break;
   }
  }

  if (GENERIC_TYPE_THERMOSTAT == aucGenericDeviceType)
  {
    switch (aucSpecificDeviceType)
    {
    case SPECIFIC_TYPE_SETBACK_SCHEDULE_THERMOSTAT: LOG("%s: - Setback Schedule Thermostat \r\n", __FUNCTION__); break;
    case SPECIFIC_TYPE_SETBACK_THERMOSTAT: LOG("%s: - Thermostat (Setback) \r\n", __FUNCTION__);                 break;
    case SPECIFIC_TYPE_SETPOINT_THERMOSTAT: LOG("%s: - SPECIFIC_TYPE_SETPOINT_THERMOSTAT \r\n", __FUNCTION__);   break;
    case SPECIFIC_TYPE_THERMOSTAT_GENERAL: LOG("%s: - Thermostat General \r\n", __FUNCTION__);                   break;
    case SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2: LOG("%s: - Thermostat (HVAC) \r\n", __FUNCTION__);                 break;
    case SPECIFIC_TYPE_THERMOSTAT_HEATING: LOG("%s: - Thermostat Heating \r\n", __FUNCTION__);                   break;
    default: LOG("%s: - *** WARNING *** unknown specific device \r\n", __FUNCTION__);  break;
   }
  }



}
// end ZWave_Identify_Specific_Device_Type

/** *****************************************************************************************************************************
  * @brief  Parse received FIFO bytes from Z-Wave controller
  * @param  aucIsACKRequired - TRUE if received frame should be ACKed; FALSE otherwise
  * @retval Result of parsing received bytes from Z-Wave controller
  */
ZWaveRxParseResult_t ZWave_Parse_Rx_Data(uint8_t aucIsACKRequired)
{
  //
  // Based on ZWave_NCP_SerialAPI_Controller_Solution, comm_interface.c, comm_interface_parse_data()
  //

  uint8_t lucRxByte = 0;
  ZWaveRxParseResult_t ltParseResult = ZWAVE_RX_PARSE_IDLE;  // Do not make this a static variable; or at least always initialize to IDLE
  static osStatus_t ltZWaveRxQueueStatus;

  // WHILE result is PARSE_IDLE AND received byte count > 0
  while (ZWAVE_RX_PARSE_IDLE == ltParseResult && osMessageQueueGetCount(ZWaveRxQueueHandle))
  {
    // Read received byte
    ltZWaveRxQueueStatus = osMessageQueueGet(ZWaveRxQueueHandle, &lucRxByte, NULL, 0);
    if (osOK != ltZWaveRxQueueStatus)
    {
      LOG("%s: *** WARNING *** ltZWaveRxQueueStatus = %d\r\n", __FUNCTION__, ltZWaveRxQueueStatus);
    }

    // Process received byte based on ZWave Rx state
    switch (gtZWaveRxInterface.state)
    {
    case ZWAVE_RX_SOF:
      ltParseResult = ZWave_Handle_SOF(lucRxByte);
      break;

    case ZWAVE_RX_LEN:
      ZWave_Handle_LEN(lucRxByte);
      break;

    case ZWAVE_RX_TYPE:
      ZWave_Handle_TYPE(lucRxByte);
      break;

    case ZWAVE_RX_CMD:
      ZWave_Handle_CMD(lucRxByte);
      break;

    case ZWAVE_RX_DATA:
      ZWave_Handle_DATA(lucRxByte);
      break;

    case ZWAVE_RX_CHECKSUM:
      ltParseResult = ZWave_Handle_CHECKSUM(lucRxByte, aucIsACKRequired);
      break;

    default:
      ZWave_Handle_Default();
      break;

    } // end switch
  }
  // END WHILE

  // (check for timeouts if no other events detected)
  if (ZWAVE_RX_PARSE_IDLE == ltParseResult)
  {
    // IF in the middle of receiving data AND byte timeout occurred
    if (gtZWaveRxInterface.rx_active && gtZWaveRxInterface.byte_timeout)
    {
      // Reset byte timeout
      gtZWaveRxInterface.byte_timeout = false;

      // Reset ZWave Rx state to SOF
      //LOG("%s: Transitioning to SOF\r\n", __FUNCTION__);
      gtZWaveRxInterface.state = ZWAVE_RX_SOF;

      // Disable Rx active
      gtZWaveRxInterface.rx_active = false;  // Not really active now...

      // Set result to RX_TIMEOUT
      ltParseResult = ZWAVE_RX_PARSE_RX_TIMEOUT;
    }

    // IF waiting for ACK AND ACK timeout occurred
    if (gtZWaveRxInterface.ack_needed && gtZWaveRxInterface.ack_timeout)
    {
      // Reset ACK timeout
      gtZWaveRxInterface.ack_timeout = false;

      // Reset ZWave Rx state to SOF
      //LOG("%s: Transitioning to SOF\r\n", __FUNCTION__);
      gtZWaveRxInterface.state = ZWAVE_RX_SOF;

      // Reset ACK pending
      gtZWaveRxInterface.ack_needed = false;

      // Set result to TX_TIMEOUT
      ltParseResult = ZWAVE_RX_PARSE_TX_TIMEOUT;
    }

  }



  // Set expected bytes based on ZWave Rx state

  return ltParseResult;
}
// end ZWave_Parse_Rx_Data

/** *****************************************************************************************************************************
  * @brief  Remove the oldest frame from the transmit callback queue (it has been transmitted)
  * @param  None
  * @retval None
  */
void ZWave_Pop_Callback_Queue(void)
{
  // IF the queue count > 0
  if (gstructCallbackQueue.requestCnt)
  {
    // Decrement queue count
    gstructCallbackQueue.requestCnt--;

    // Advance the output pointer to the next frame in the queue
    if (++gstructCallbackQueue.requestOut >= MAX_CALLBACK_QUEUE)
    {
      gstructCallbackQueue.requestOut = 0;
    }
  }
  // ELSE
  else
  {
    // Set the output pointer to the input pointer
    gstructCallbackQueue.requestOut = gstructCallbackQueue.requestIn;
  }
  // ENDIF
}
// end ZWave_Pop_Callback_Queue

/** *****************************************************************************************************************************
  * @brief  Remove the oldest frame from the transmit command queue (it has been transmitted)
  * @param  None
  * @retval None
  */
void ZWave_Pop_Command_Queue(void)
{
  // IF the queue count > 0
  if (gstructCommandQueue.requestCnt)
  {
    // Decrement queue count
    gstructCommandQueue.requestCnt--;

    // Advance the output pointer to the next frame in the queue
    if (++gstructCommandQueue.requestOut >= MAX_UNSOLICITED_QUEUE)
    {
      gstructCommandQueue.requestOut = 0;
    }
  }
  // ELSE
  else
  {
    // Set the output pointer to the input pointer
    gstructCommandQueue.requestOut = gstructCommandQueue.requestIn;
  }
  // ENDIF
}
// end ZWave_Pop_Command_Queue

/** *****************************************************************************************************************************
  * @brief  Read received FIFO bytes from Z-Wave controller
  * @param  *aucResponseBuffer - pointer to buffer into which received bytes will be copied
  * @retval Count of bytes received (possibly 0)
  */
uint16_t ZWave_Receive_Response(uint8_t* aucReceiveBuffer)
{
  static uint16_t luiZWaveRxCount;
  static uint16_t i;
  static uint8_t lucReceivedChar;
  static osStatus_t ltZWaveRxQueueStatus;
  //LOG("%s: START\r\n", __FUNCTION__);

  // Copy bytes (if any) into RX buffer
  luiZWaveRxCount = osMessageQueueGetCount(ZWaveRxQueueHandle);
  if (luiZWaveRxCount > 0)
  {
    LOG("%s: Initial luiZWaveRxCount = %d\r\n", __FUNCTION__, luiZWaveRxCount);
    // Read bytes from ZWave RX queue into RX buffer
    for (i = 0; osMessageQueueGetCount(ZWaveRxQueueHandle) > 0 && i < SERIAL_BUFFER_SIZE-2; ++i)
    {
      ltZWaveRxQueueStatus = osMessageQueueGet(ZWaveRxQueueHandle, &lucReceivedChar, NULL, 0);
      if (osOK != ltZWaveRxQueueStatus)
      {
        LOG("%s: *** WARNING *** ltZWaveRxQueueStatus = %d\r\n", __FUNCTION__, ltZWaveRxQueueStatus);
      }
      aucReceiveBuffer[i] = lucReceivedChar;
    }
    luiZWaveRxCount = i;
    //aucReceiveBuffer[i+1] = 0;
    LOG("%s:   FINAL luiZWaveRxCount = %d\r\n", __FUNCTION__, luiZWaveRxCount);
  }
//  else
//  {
//    LOG("%s: Nothing received from Z-Wave controller\r\n", __FUNCTION__);
//  }

  //LOG("%s: END\r\n", __FUNCTION__);
  return luiZWaveRxCount;
}
// end ZWave_Receive_Response

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x02 FUNC_ID_SERIAL_API_GET_INIT_DATA ZW->HOST: Cmd | ver | capabilities | 29 | nodes[29] | chip_type | chip_version
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_02_Get_Init_Data(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  // ----------------- Version -----------------
  LOG("%s: SerialAPI Version      = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

  // ----------------- Capabilities -----------------
  LOG("%s: SerialAPI Capabilities = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  if (ZWaveSerialFrame->payload[0] & 0x01)
  {
    LOG("%s: - End device API\r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - Controller API \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[0] & 0x02)
  {
    LOG("%s: - Timer functions supported\r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - Timer functions NOT supported\r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[0] & 0x04)
  {
    LOG("%s: - Secondary controller\r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - Primary controller\r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[0] & 0x08)
  {
    LOG("%s: - Controller is SIS\r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - Controller is NOT SIS\r\n", __FUNCTION__);
  }

  // ----------------- MAX_NODES/8 -----------------
  LOG("%s: SerialAPI MAX_NODES/8  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);
  LOG("%s: - should be 29 (0x1D) or possibly 0\r\n", __FUNCTION__);

  // ----------------- Nodes -----------------
  if (ZWaveSerialFrame->payload[2])
  {
    uint8_t lucActiveNode = 0;
    for (int i = 0; i < ZWaveSerialFrame->payload[2]; ++i)
    {
      LOG("%s: SerialAPI node[%02d]  = 0x%02X\r\n", __FUNCTION__, i, ZWaveSerialFrame->payload[3+i]);
      for (int j = 0; j < 8; ++j)
      {
        ++lucActiveNode;
        if (ZWaveSerialFrame->payload[3+i] & (1<<j))
        {
          LOG("%s: - Node %d (0x%04X) is active \r\n", __FUNCTION__, lucActiveNode, lucActiveNode);
        }
      }
    }
  }

  // ----------------- chip_type -----------------
  LOG("%s: SerialAPI chip_type    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3 +  ZWaveSerialFrame->payload[2]]);

  // ----------------- chip_version -----------------
  LOG("%s: SerialAPI chip_version = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4 +  ZWaveSerialFrame->payload[2]]);

  //
  // Validate Z-Wave chip type
  //
  if (0x08 == ZWaveSerialFrame->payload[3 +  ZWaveSerialFrame->payload[2]] &&
      0x00 == ZWaveSerialFrame->payload[4 +  ZWaveSerialFrame->payload[2]]     )
  {
    LOG("%s: ZWave chip identified as ZG23/ZGM230S - OK\r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: **** WARNING *** ZWave chip identified as something other than ZG23/ZGM230S\r\n", __FUNCTION__);
  }
}
// end ZWave_RES_CMD_02_Get_Init_Data

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x05 FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_05_ZW_Get_Controller_Capabilities(void)
{
  CONTROLLER_CONFIGURATION ltCapability;

  LOG("%s: bitmask  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  memcpy(&ltCapability, &ZWaveSerialFrame->payload[0], 1);
  if (ltCapability.controller_is_secondary)          LOG("%s: - controller is secondary\r\n", __FUNCTION__);
  if (ltCapability.controller_on_other_network)      LOG("%s: - controller on other network\r\n", __FUNCTION__);
  if (ltCapability.controller_nodeid_server_present) LOG("%s: - controller NodeID server is present\r\n", __FUNCTION__);
  if (ltCapability.controller_is_real_primary)       LOG("%s: - controller is real primary\r\n", __FUNCTION__);
  if (ltCapability.controller_is_suc)                LOG("%s: - controller is SUC\r\n", __FUNCTION__);
  if (ltCapability.no_nodes_included)                LOG("%s: - no nodes included\r\n", __FUNCTION__);
}
// end ZWave_RES_CMD_05_ZW_Get_Controller_Capabilities

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x07 FUNC_ID_SERIAL_API_GET_CAPABILITIES ZW->HOST: Cmd | data[]
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_07_Serial_API_Get_Capabilities(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  LOG("%s: SERIAL_APP_VERSION                     = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: SERIAL_APP_REVISION                    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  LOG("%s: SERIAL_APP_MANUFACTURER_ID1            = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);
  LOG("%s: SERIAL_APP_MANUFACTURER_ID2            = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
  LOG("%s: SERIAL_APP_MANUFACTURER_PRODUCT_TYPE1  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  LOG("%s: SERIAL_APP_MANUFACTURER_PRODUCT_TYPE2  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
  LOG("%s: SERIAL_APP_MANUFACTURER_PRODUCT_ID1    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);
  LOG("%s: SERIAL_APP_MANUFACTURER_PRODUCT_ID2    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[7]);
  uint8_t lucSupportedCMD = 0;
  for (int i = 0; i < 32; ++i)
  {
    LOG("%s: FUNCID_SUPPORTED_BITMASK[%02d] = 0x%02X\r\n", __FUNCTION__, i, ZWaveSerialFrame->payload[8+i]);
    for (int j=0; j < 8; ++j)
    {
      ++lucSupportedCMD;
      if (ZWaveSerialFrame->payload[8+i] & (1<<j))
      {
        LOG("%s: - API command 0x%02X implemented in Z-Wave controller\r\n", __FUNCTION__, lucSupportedCMD);
      }
    }
  }

}
// end ZWave_RES_CMD_07_Serial_API_Get_Capabilities

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x0A FUNC_ID_SERIAL_API_STARTED ZW->HOST: Cmd | CmdData[]
  * @param  None
  * @retval None
  */
void ZWave_REQ_CMD_0A_Serial_API_Started(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  // Example contents of serial frame (excluding checksum byte)
  // 01 0E 00 0A 00 00 01 02     01 00 00 00 00 00 00
  // 01 is SOF
  // 0E is length of data, including the length byte BUT excluding the SOF and checksum bytes
  // 00 is type: 00 for REQUEST; 01 for RESPONSE
  // 0A is the command FUNC_ID_SERIAL_API_STARTED
  // 00 is wakeup reason
  // 00 is watchdog started
  // 01 is device option mask
  // 02 is generic node type
  // --------
  // 01 is specific node type
  // 00 is command class length: number of command classes in the node information frame (uhhh, 0, so no command classes)
  // 00 is capabilities (e.g. Long Range capable?)
  // 00 is ZPAL_RETENTION_REGISTER_RESET_INFO bits 31-24
  // 00 is ZPAL_RETENTION_REGISTER_RESET_INFO bits 23-16
  // 00 is ZPAL_RETENTION_REGISTER_RESET_INFO bits 15-08
  // 00 is ZPAL_RETENTION_REGISTER_RESET_INFO bits 07-00

  // ----------------- Wakeup reason -----------------
  LOG("%s: Wakeup reason             = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  switch (ZWaveSerialFrame->payload[0])
  {
  case ZPAL_RESET_REASON_PIN:
    LOG("%s: - Reset triggered by reset pin \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_DEEP_SLEEP_WUT:
    LOG("%s: - Reset triggered by wake up by timer from deep sleep state \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_WATCHDOG:
    LOG("%s: - Reset triggered by watchdog \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_DEEP_SLEEP_EXT_INT:
    LOG("%s: - Reset triggered by external interrupt event in deep sleep state \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_POWER_ON:
    LOG("%s: - Reset triggered by power on \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_SOFTWARE:
    LOG("%s: - Reset triggered by software \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_BROWNOUT:
    LOG("%s: - Reset triggered by brownout circuit \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_TAMPER:
    LOG("%s: - Reset triggered by a tamper attempt \r\n", __FUNCTION__);
    break;
  case ZPAL_RESET_REASON_OTHER:
    LOG("%s: - Reset triggered for unknown reason \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** Reset reason 0x%02X is undefined \r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    break;
  }

  // ----------------- Watchdog started -----------------
  LOG("%s: Watchdog started          = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  if (ZWaveSerialFrame->payload[1])
  {
    LOG("%s: - Z-Wave controller's watchdog timer is started and kicked by the Serial API \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - Z-Wave controller's watchdog timer is NOT started \r\n", __FUNCTION__);
  }

  // ----------------- Device option mask -----------------
  LOG("%s: Device option mask        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);
  if (ZWaveSerialFrame->payload[2] & APPLICATION_NODEINFO_LISTENING)
  {
    LOG("%s: - Always On Node, corresponds to Always On (AOS) role type \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[2] & APPLICATION_FREQ_LISTENING_MODE_1000ms)
  {
    LOG("%s: - Frequently Listening, corresponds to FLiRS role type. Wakes up every 1000ms \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[2] & APPLICATION_FREQ_LISTENING_MODE_250ms)
  {
    LOG("%s: - Frequently Listening, corresponds to FLiRS role type. Wakes up every 250ms \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[2] & 0x02)
  {
    LOG("%s: - Optional functionality supported \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[2] & ~(APPLICATION_NODEINFO_LISTENING | APPLICATION_FREQ_LISTENING_MODE_1000ms | APPLICATION_FREQ_LISTENING_MODE_250ms | 0x02) )
  {
    LOG("%s: - *** WARNING *** unknown device options enabled \r\n", __FUNCTION__);
  }

  // ----------------- Generic node type -----------------
  LOG("%s: Generic node type         = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
  ZWave_Identify_Generic_Device_Type(ZWaveSerialFrame->payload[3]);

  // ----------------- Specific node type -----------------
  LOG("%s: Specific node type        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  ZWave_Identify_Specific_Device_Type(ZWaveSerialFrame->payload[3], ZWaveSerialFrame->payload[4]);

  // ----------------- Command class list length -----------------
  LOG("%s: Command class list length = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);

  // ----------------- Command class list -----------------
  int i = ZWaveSerialFrame->payload[5];
  if (i)
  {
    LOG("-----------------------  Command class list START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[6], i, false, 0);
    LOG("-----------------------  Command class list  END  -----------------------\r\n");
  }

  // ----------------- Capabilities -----------------
  LOG("%s: Capabilities              = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6+i]);
  if (ZWaveSerialFrame->payload[6+i] & ((uint8_t)SERIAL_API_STARTED_CAPABILITIES_L0NG_RANGE) )
  {
    LOG("%s: - Long Range is supported \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - *** WARNING *** Long Range is NOT supported \r\n", __FUNCTION__);
  }

  // ----------------- Retained reset info  -----------------
uint32_t lulZpalRetentionResetInfo = (0x1000000*ZWaveSerialFrame->payload[7+i]) +
                                       (  0x10000*ZWaveSerialFrame->payload[8+i]) +
                                       (    0x100*ZWaveSerialFrame->payload[9+i]) +
                                       (          ZWaveSerialFrame->payload[10+i]) ;
  LOG("%s: Retained reset info       = 0x%08X\r\n", __FUNCTION__, lulZpalRetentionResetInfo);

  #if ENABLE_ZWAVE_CONTROLLER_HOST
//  ///////////////////////////////////////////////////////////////////////////
//  //// TEST MAB 2026.01.06
//  //// Set the controller to its defaults
//  //// (erase pre-existing nodes, networks, etc.)
//  gucSessionID = ZWave_SessionID_Randomize();
//  ZWave_Send_REQ_CMD_42_Set_Default();
//  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////
  //// TEST MAB 2025.11.12
  //// When the Serial API Started request is received,
  //// send the Serial API Setup request to the ZWave controller
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET, IGNORE, IGNORE);
  /// Send Memory Get ID to get the HomeID and NodeID values
  ZWave_Send_REQ_CMD_20_Memory_Get_ID();
  /// Send Serial API Get Capabilities
  ZWave_Send_REQ_CMD_07_Serial_API_Get_Capabilities();
  // Send Serial API Get LR Nodes
  ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes();
  // Send Serial API Get Init Data
  ZWave_Send_REQ_CMD_02_Serial_API_Get_Init_Data();
  // Send Get Controller Capabilities
  ZWave_Send_REQ_CMD_05_Get_Controller_Capabilities();
  // Send Get SUC Node ID
  ZWave_Send_REQ_CMD_56_Get_SUC_Node_ID();

  // Set DCDC mode
  ZWave_Send_REQ_CMD_DF_Set_DCDC_Config(EDCDCMODE_AUTO);
  // Send Get DCDC Config
  ZWave_Send_REQ_CMD_DE_Get_DCDC_Config();

  // Send Get Radio PTI
  ZWave_Send_REQ_CMD_E8_Get_Radio_PTI();

  // Set region
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_US, IGNORE); // OK
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_EU, IGNORE); // OK
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_US_LR, IGNORE); // OK
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_DEPRECATED_48, IGNORE); // FAILS; region unchanged
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_JP, IGNORE); // OK
  // Set TX power level
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT, 200, 0); // OK
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT, 205, 0); // FAILS; TX power unchanged
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT, 200, 5); // OK
  // Set LR TX power level
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET, 200, IGNORE); // OK
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET, 190, IGNORE); // OK
  //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET, 205, IGNORE); // FAILS; LR TX power unchanged


  // Send other Serial API Setup requests
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_SUPPORTED, IGNORE, IGNORE);
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT, IGNORE, IGNORE);
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET, IGNORE, IGNORE);
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_GET, IGNORE, IGNORE);
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE, IGNORE, IGNORE);
  ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE, IGNORE, IGNORE);

  // Get NVR values (public/private keys)
  ZWave_Send_REQ_CMD_28_NVR_Get_Value(NVR_PUK_OFFSET, 32);
  ZWave_Send_REQ_CMD_28_NVR_Get_Value(NVR_PRK_OFFSET, 32);
  ///////////////////////////////////////////////////////////////////////////
  #endif // ENABLE_ZWAVE_CONTROLLER_HOST

}
// end ZWave_REQ_CMD_0A_Serial_API_Started

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x0B FUNC_ID_SERIAL_API_SETUP ZW->HOST: Cmd | CmdRes[]
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_0B_Serial_API_Setup(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  // Example contents of serial frame (excluding checksum byte)
  // 01 05 01 0B 80 01
  // 80 is the SerialAPI Setup command byte - in this case SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET
  // 01 for this example is the command result - in this case, OK (requested node ID base type successfully set)

  // ----------------- SerialAPI command -----------------
  LOG("%s: SerialAPI Setup command = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  switch (ZWaveSerialFrame->payload[0])
  {
  case SERIAL_API_SETUP_CMD_UNSUPPORTED:
    LOG("%s: - SERIAL_API_SETUP_CMD_UNSUPPORTED \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_SUPPORTED:
    LOG("%s: - SERIAL_API_SETUP_CMD_SUPPORTED \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_STATUS_REPORT:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_STATUS_REPORT \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_RF_REGION_GET:
    LOG("%s: - SERIAL_API_SETUP_CMD_RF_REGION_GET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_RF_REGION_SET:
    LOG("%s: - SERIAL_API_SETUP_CMD_RF_REGION_SET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET:
    LOG("%s: - SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET:
    LOG("%s: - SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET:
    LOG("%s: - SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT:
    LOG("%s: - SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_GET_SUPPORTED_REGION:
    LOG("%s: - SERIAL_API_SETUP_CMD_GET_SUPPORTED_REGION \r\n", __FUNCTION__);
    break;
  case SERIAL_API_SETUP_CMD_GET_REGION_INFO:
    LOG("%s: - SERIAL_API_SETUP_CMD_GET_REGION_INFO \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** SerialAPI Setup command 0x%02X is UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    break;
  }

  // ----------------- SERIAL_API_SETUP_CMD_SUPPORTED -----------------
  if (SERIAL_API_SETUP_CMD_SUPPORTED == ZWaveSerialFrame->payload[0])
  {
    LOG("%s: Supported flags  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    uint8_t lucSetupCMD = 0;
    for (int i = 0; i <= SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET / 8; ++i)
    {
      LOG("%s: Supported bitmask[%02d]  = 0x%02X\r\n", __FUNCTION__, i, ZWaveSerialFrame->payload[2+i]);
      for (int j = 0; j < 8; ++j)
      {
        ++lucSetupCMD;
        if (ZWaveSerialFrame->payload[2+i] & (1<<j))
        {
          LOG("%s: - Setup command %03d (0x%02X) is supported \r\n", __FUNCTION__, lucSetupCMD, lucSetupCMD);
        }
      }
    }
  }

  // ----------------- SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET -----------------
  if (SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET == ZWaveSerialFrame->payload[0])
  {
    LOG("%s: SerialAPI Setup result  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    if (ZWaveSerialFrame->payload[1])
    {
      LOG("%s: - Requested Long Range TX power level successfully set \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Requested Long Range TX powerlevel setting FAILED \r\n", __FUNCTION__);
    }
  }

  // ----------------- SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET -----------------
  if (SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET == ZWaveSerialFrame->payload[0])
  {
    uint16_t luiReadout        = 0x100*ZWaveSerialFrame->payload[1] + ZWaveSerialFrame->payload[2];
    LOG("%s: Readout   = 0x%04X\r\n", __FUNCTION__, luiReadout);
  }

  // ----------------- SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE -----------------
  if (SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE == ZWaveSerialFrame->payload[0])
  {
    uint8_t lucMaxPayloadSize   = ZWaveSerialFrame->payload[1];
    LOG("%s: Max payload size   = 0x%02X\r\n", __FUNCTION__, lucMaxPayloadSize);
  }

  // ----------------- SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE -----------------
  if (SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE == ZWaveSerialFrame->payload[0])
  {
    uint8_t lucMaxLRPayloadSize   = ZWaveSerialFrame->payload[1];
    LOG("%s: Max LR payload size   = 0x%02X\r\n", __FUNCTION__, lucMaxLRPayloadSize);
  }

  // ----------------- SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT -----------------
  if (SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT == ZWaveSerialFrame->payload[0])
  {
    LOG("%s: SerialAPI Setup result  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    if (ZWaveSerialFrame->payload[1])
    {
      LOG("%s: - Requested TX power level successfully set \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Requested TX powerlevel setting FAILED \r\n", __FUNCTION__);
    }
  }

  // ----------------- SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT -----------------
  if (SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT == ZWaveSerialFrame->payload[0])
  {
    uint16_t luiPowerLevel        = 0x100*ZWaveSerialFrame->payload[1] + ZWaveSerialFrame->payload[2];
    uint16_t luiPower0dbmMeasured = 0x100*ZWaveSerialFrame->payload[3] + ZWaveSerialFrame->payload[4];
    LOG("%s: Power level                = 0x%04X\r\n", __FUNCTION__, luiPowerLevel);
    LOG("%s: Power level 0 dbm measured = 0x%04X\r\n", __FUNCTION__, luiPower0dbmMeasured);
  }

  // ----------------- SERIAL_API_SETUP_CMD_RF_REGION_GET -----------------
  if (SERIAL_API_SETUP_CMD_RF_REGION_GET == ZWaveSerialFrame->payload[0])
  {
    uint8_t lucRfRegion = ZWaveSerialFrame->payload[1];
    switch (lucRfRegion)
    {
    case REGION_EU:
      LOG("%s: RF region = 0x%02X (EU) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_US:
      LOG("%s: RF region = 0x%02X (USA) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_ANZ:
      LOG("%s: RF region = 0x%02X (Australia/New Zealand) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_HK:
      LOG("%s: RF region = 0x%02X (Hong Kong) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_US_LR:
      LOG("%s: RF region = 0x%02X (USA - Long Range) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_EU_LR:
      LOG("%s: RF region = 0x%02X (EU - Long Range) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_IN:
      LOG("%s: RF region = 0x%02X (India) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_IL:
      LOG("%s: RF region = 0x%02X (Israel) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_RU:
      LOG("%s: RF region = 0x%02X (Russia) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_CN:
      LOG("%s: RF region = 0x%02X (China) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_JP:
      LOG("%s: RF region = 0x%02X (Japan) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_KR:
      LOG("%s: RF region = 0x%02X (Korea) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_UNDEFINED:
      LOG("%s: RF region = 0x%02X (undefined) \r\n", __FUNCTION__, lucRfRegion);
      break;
    case REGION_DEFAULT:
      LOG("%s: RF region = 0x%02X (default - EU) \r\n", __FUNCTION__, lucRfRegion);
      break;
    default:
      LOG("%s: RF region = 0x%02X (unknown) \r\n", __FUNCTION__, lucRfRegion);
      break;
    }
  }

  // ----------------- SERIAL_API_SETUP_CMD_RF_REGION_SET -----------------
  if (SERIAL_API_SETUP_CMD_RF_REGION_SET == ZWaveSerialFrame->payload[0])
  {
    LOG("%s: SerialAPI Setup result  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    if (ZWaveSerialFrame->payload[1])
    {
      LOG("%s: - Requested Region type successfully set \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Requested Region FAILED \r\n", __FUNCTION__);
    }
  }

  // ----------------- SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET -----------------
  if (SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET == ZWaveSerialFrame->payload[0])
  {
    LOG("%s: SerialAPI Setup result  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    if (ZWaveSerialFrame->payload[1])
    {
      LOG("%s: - Requested Node ID Base type successfully set \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Requested Node ID Base FAILED, set to default 8-bit values \r\n", __FUNCTION__);
    }
  }

}
// end ZWave_RES_CMD_0B_Serial_API_Setup

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x12 FUNC_ID_ZW_SEND_NODE_INFORMATION ZW->HOST: RES | Cmd | retVal; REQ | Cmd | funcID | txStatus
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_12_ZW_Send_Node_Information(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: NodeID       = 0x%04X\r\n", __FUNCTION__, guiNodeID);
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - expect a callback REQUEST with assorted data...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - No further data are expected\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    /* ZW->HOST: funcID | txStatus */
    LOG("%s: NodeID    = 0x%04X\r\n", __FUNCTION__, guiNodeID);
    LOG("%s: SessionID = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: txStatus  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case TRANSMIT_COMPLETE_OK:
      LOG("%s: - transmit OK \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_NO_ACK:
      LOG("%s: - transmit ERROR (no ACK received) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_FAIL:
      LOG("%s: - transmit ERROR (FAIL; network busy or jammed) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_ROUTING_NOT_IDLE:
      LOG("%s: - transmit ERROR (routing not idle) \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** txStatus value UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }
}
// end ZWave_RSQ_CMD_12_ZW_Send_Node_Information

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x13 FUNC_ID_ZW_SEND_DATA ZW->HOST: RES | Cmd | retVal; REQ | Cmd | data[]
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_13_ZW_Send_Data(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - expect a callback REQUEST with assorted data...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - No further data are expected\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    ZWave_Display_Tx_Report();
  }
}
// end ZWave_RSQ_CMD_13_ZW_Send_Data

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x15 FUNC_ID_ZW_GET_VERSION ZW->HOST: Cmd | version
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_15_ZW_Get_Version(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  // Example contents of serial frame (excluding checksum byte)
  // 01 10 01 15 5A 2D 57 61     76 65 20 37 2E 32 34 00
  // 07
  // 2D 57 61     76 65 20 37 2E 32 34 00 is a null-terminated string: "Z-Wave 7.24"
  // 07 is protocol_info ->eLibraryType; see ZW_application_transport_interface.h, ELibraryType

  // ----------------- Version string -----------------
  LOG("%s: Version string is '%s' \r\n", __FUNCTION__, ZWaveSerialFrame->payload);

  // ----------------- Library -----------------
  LOG("%s: Library byte is 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[strlen(ZWaveSerialFrame->payload)+1]);
  // The following is from ZW_application_transport_interface.h, ELibraryType
  switch (ZWaveSerialFrame->payload[strlen(ZWaveSerialFrame->payload)+1])
  {
  case 3:
    LOG("%s: - ELIBRARYTYPE_SLAVE \r\n", __FUNCTION__);
    break;
  case 7:
    LOG("%s: - ELIBRARYTYPE_CONTROLLER \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** Library byte 0x%02X is unknown or deprecated \r\n", __FUNCTION__, ZWaveSerialFrame->payload[strlen(ZWaveSerialFrame->payload)+1]);
    break;
  }
}
// end ZWave_RES_CMD_15_ZW_Get_Version

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x20 FUNC_ID_MEMORY_GET_ID ZW->HOST: Cmd | HomeID (4 bytes) | NodeID (2 bytes)
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_20_Memory_Get_ID(void)
{
  uint32_t lulHomeID;
  uint16_t luiNodeID;

  lulHomeID = 0x1000000*ZWaveSerialFrame->payload[0] +
                0x10000*ZWaveSerialFrame->payload[1] +
                  0x100*ZWaveSerialFrame->payload[2] +
                        ZWaveSerialFrame->payload[3];
  luiNodeID =     0x100*ZWaveSerialFrame->payload[4] +
                        ZWaveSerialFrame->payload[5];
  LOG("%s: HomeID = 0x%08X \r\n", __FUNCTION__, lulHomeID);
  LOG("%s: NodeID =     0x%04X \r\n", __FUNCTION__, luiNodeID);

  gulZWaveHomeID = lulHomeID;
  guiZWaveNodeID = luiNodeID;

  #if ENABLE_ZWAVE_CONTROLLER_HOST
  ///////////////////////////////////////////////////////////////////////////
  /// TEST MAB 2025.11.13
  /// Send Get Node Protocol Info for the Z-Wave controller's NodeID
  ZWave_Send_REQ_CMD_41_Get_Node_Protocol_Info(guiZWaveNodeID);
  /// TEST MAB 2025.11.14
  /// Try an invalid NodeID
  //ZWave_Send_REQ_CMD_41_Get_Node_Protocol_Info(guiZWaveNodeID+1);

  // Start listening for SmartStart Prime commands, report to host application
  LOG("%s: Start listening for SmartStart Prime commands, report to host application \r\n", __FUNCTION__);
  gucSessionID = ZWave_SessionID_Randomize(); // range [1, 255]
  ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_SMART_START, gucSessionID, NULL);
  //ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_OPTION_LR|ADD_NODE_SMART_START, gucSessionID, NULL);
  ///////////////////////////////////////////////////////////////////////////
  #endif // ENABLE_ZWAVE_CONTROLLER_HOST
}
// end ZWave_RES_CMD_20_Memory_Get_ID

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x28 FUNC_ID_NVR_GET_VALUE ZW->HOST: Cmd | NVRdata[]
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_28_NVR_Get_Value(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  LOG("%s: Retrieved NVR values: \r\n", __FUNCTION__);
  PrintBytes(ZWaveSerialFrame->payload, ZWaveSerialFrame->len - 3, false, 0);

  // Save controller's public or private key if present
  if ( NVR_PUK_OFFSET==gucNVROffset && 32==(ZWaveSerialFrame->len - 3) )
  {
    LOG("%s: ------------------------------ \r\n", __FUNCTION__);
    LOG("%s: Saving controller's public key \r\n", __FUNCTION__);
    LOG("%s: ------------------------------ \r\n", __FUNCTION__);
    memcpy(gucControllerPublicKey, ZWaveSerialFrame->payload, 32);
  }
  if ( NVR_PRK_OFFSET==gucNVROffset && 32==(ZWaveSerialFrame->len - 3) )
  {
    LOG("%s: ------------------------------- \r\n", __FUNCTION__);
    LOG("%s: Saving controller's private key \r\n", __FUNCTION__);
    LOG("%s: ------------------------------- \r\n", __FUNCTION__);
    memcpy(gucControllerPrivateKey, ZWaveSerialFrame->payload, 32);
  }

  // Done with NVR value, so clear the offset to avoid confusion
  gucNVROffset = NVR_UNUSED_OFFSET;
}
// end ZWave_RES_CMD_28_NVR_Get_Value

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x3F FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK ZW->HOST
  * @param  None
  * @retval None
  */
void ZWave_REQ_CMD_3F_ZW_Remove_Node_ID_From_Network(void)
{
  // The host should be receiving the callback (request) data frames
  LOG("%s: Session ID             = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: Status                 = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  switch (ZWaveSerialFrame->payload[1])
  {
  case REMOVE_NODE_STATUS_LEARN_READY:
    LOG("%s: - Network Exclusion started \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_NODE_FOUND:
    LOG("%s: - Node found \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_REMOVING_SLAVE:
    LOG("%s: - Exclusion ongoing (for End node) \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_REMOVING_CONTROLLER:
    LOG("%s: - Exclusion ongoing (for Controller node) \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_DONE:
    LOG("%s: - Exclusion completed \r\n", __FUNCTION__);
    gucIsExclusionFinished = TRUE;
    break;
  case REMOVE_NODE_STATUS_FAILED:
    LOG("%s: - Exclusion FAILED \r\n", __FUNCTION__);
    gucIsExclusionFailed = TRUE;
    break;
  case ADD_NODE_STATUS_NOT_PRIMARY:
    LOG("%s: - NOT Primary \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** unrecognized or reserved status \r\n", __FUNCTION__);
    break;
  }

  LOG("%s: NodeID                 = 0x%04X\r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[2]) + ZWaveSerialFrame->payload[3]);
  LOG("%s: Data length            = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  if (ZWaveSerialFrame->payload[4])
  {
    LOG("%s: Basic device type      = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
    ZWave_Identify_Basic_Device_Type(ZWaveSerialFrame->payload[5]);

    LOG("%s: Generic device type    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);
    ZWave_Identify_Generic_Device_Type(ZWaveSerialFrame->payload[6]);

    LOG("%s: Specific device type   = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[7]);
    ZWave_Identify_Specific_Device_Type(ZWaveSerialFrame->payload[6], ZWaveSerialFrame->payload[7]);
    LOG("-----------------------  Supported Command Classes START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[8], ZWaveSerialFrame->payload[4] - 3, false, 0);
    LOG("-----------------------  Supported Command Classes  END  -----------------------\r\n");
  }
}
// end ZWave_REQ_CMD_3F_ZW_Remove_Node_ID_From_Network

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x41 FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO ZW->HOST: Cmd | NodeInfo[]
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info(void)
{
  // Display received frame data
  //ZWave_Display_Received_Frame_Data();

  // Example contents of serial frame (excluding checksum byte)
  // 01 0A 01 41 D3 96 01 02     02 01 00
  // D3 is network capabilities
  // 96 is network security
  // 01 is reserved
  // 02 is nodeType - basic device type
  // 02 is nodeType - generic device type
  // 01 is nodeType - specific device type
  // 00 is extInfo

  // ----------------- Network capabilities -----------------
  LOG("%s: NodeID               = 0x%04X\r\n", __FUNCTION__, guiNodeID);
  LOG("%s: Network capabilities = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  if (ZWaveSerialFrame->payload[0] & NODEINFO_LISTENING_SUPPORT)
  {
    LOG("%s: - Node is listening \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[0] & NODEINFO_ROUTING_SUPPORT)
  {
    LOG("%s: - Node supports routing \r\n", __FUNCTION__);
  }

  // ----------------- Network security -----------------
  LOG("%s: Network security     = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  if (ZWaveSerialFrame->payload[1] & NODEINFO_OPTIONAL_FUNC_SUPPORT)
  {
    LOG("%s: - Node supports optional functionality \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[1] & NODEINFO_ZWAVE_SENSOR_MODE_WAKEUP_1000)
  {
    LOG("%s: - Node supports wakeup 1000 msec \r\n", __FUNCTION__);
  }
  if (ZWaveSerialFrame->payload[1] & NODEINFO_ZWAVE_SENSOR_MODE_WAKEUP_250)
  {
    LOG("%s: - Node supports wakeup  250 msec \r\n", __FUNCTION__);
  }

  // ----------------- Reserved -----------------
  LOG("%s: Reserved byte        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);

  // ----------------- Basic node type -----------------
  LOG("%s: Basic    device type = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
  ZWave_Identify_Basic_Device_Type(ZWaveSerialFrame->payload[3]);

  // ----------------- Generic node type -----------------
  LOG("%s: Generic  device type = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  ZWave_Identify_Generic_Device_Type(ZWaveSerialFrame->payload[4]);

  // ----------------- Specific node type -----------------
  LOG("%s: Specific device type = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
  ZWave_Identify_Specific_Device_Type(ZWaveSerialFrame->payload[4], ZWaveSerialFrame->payload[5]);

  LOG("%s: extInfo              = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);

}
// end ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x42 FUNC_ID_ZW_SET_DEFAULT ZW->HOST
  * @param  None
  * @retval None
  */
void ZWave_REQ_CMD_42_ZW_Set_Default(void)
{
  LOG("%s: Session ID = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
}
// end ZWave_REQ_CMD_42_ZW_Set_Default

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x46 FUNC_ID_ZW_ASSIGN_RETURN_ROUTE ZW->HOST: RES | Cmd | retVal; REQ | Cmd | sessionID | txStatus
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_46_ZW_Assign_Return_Route(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - expect a callback REQUEST with assorted data...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - No further data are expected\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    LOG("%s: Session ID = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: TX status  = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case TRANSMIT_COMPLETE_OK:
      LOG("%s: - transmit OK \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_NO_ACK:
      LOG("%s: - transmit ERROR (no ACK received) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_FAIL:
      LOG("%s: - transmit ERROR (FAIL; network busy or jammed) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_ROUTING_NOT_IDLE:
      LOG("%s: - transmit ERROR (routing not idle) \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** txStatus value UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }
}
//end ZWave_RSQ_CMD_46_ZW_Assign_Return_Route

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x47 FUNC_ID_ZW_DELETE_RETURN_ROUTE ZW->HOST: RES | Cmd | retVal; REQ | Cmd | sessionID | txStatus
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_47_ZW_Delete_Return_Route(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - expect a callback REQUEST with assorted data...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - No further data are expected\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    LOG("%s: Session ID = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: TX status  = 0x%02X \r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case TRANSMIT_COMPLETE_OK:
      LOG("%s: - transmit OK \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_NO_ACK:
      LOG("%s: - transmit ERROR (no ACK received) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_FAIL:
      LOG("%s: - transmit ERROR (FAIL; network busy or jammed) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_ROUTING_NOT_IDLE:
      LOG("%s: - transmit ERROR (routing not idle) \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** txStatus value UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }
}
//end ZWave_RSQ_CMD_47_ZW_Delete_Return_Route

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x49 FUNC_ID_ZW_APPLICATION_UPDATE ZW->HOST: various
  * @param  None
  * @retval None
  */
void ZWave_REQ_CMD_49_ZW_Application_Update(void)
{
  static uint8_t lucEvent;
  static uint8_t lucCandidateDSKIndex;
  static uint8_t lucFrameLength;
  static uint32_t lulNWIHomeID;
  static uint8_t lucCommandClassListLength;
  static uint8_t lucBasicDeviceType;
  static uint8_t lucGenericDeviceType;
  static uint8_t lucSpecificDeviceType;

  lucEvent = ZWaveSerialFrame->payload[0];
  LOG("%s: Event                = 0x%02X\r\n", __FUNCTION__, lucEvent);
  switch (lucEvent)
  {
  case UPDATE_STATE_SUC_ID:
    LOG("%s: - UPDATE_STATE_SUC_ID \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_DELETE_DONE:
    LOG("%s: - UPDATE_STATE_DELETE_DONE \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NEW_ID_ASSIGNED:
    LOG("%s: - UPDATE_STATE_NEW_ID_ASSIGNED \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_ROUTING_PENDING:
    LOG("%s: - UPDATE_STATE_ROUTING_PENDING \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NODE_INFO_REQ_FAILED:
    LOG("%s: - UPDATE_STATE_NODE_INFO_REQ_FAILED \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NODE_INFO_REQ_DONE:
    LOG("%s: - UPDATE_STATE_NODE_INFO_REQ_DONE \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NOP_POWER_RECEIVED:
    LOG("%s: - UPDATE_STATE_NOP_POWER_RECEIVED \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NODE_INFO_RECEIVED:
    LOG("%s: - UPDATE_STATE_NODE_INFO_RECEIVED \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED:
    LOG("%s: - UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED:
    LOG("%s: - UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED \r\n", __FUNCTION__);
    break;
  case UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED_LR:
    LOG("%s: - UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED_LR \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** Application Update event is undefined \r\n", __FUNCTION__);
    break;
  }

  LOG("%s: Remote node ID       = 0x%04X\r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[1]) + ZWaveSerialFrame->payload[2]);

  // At this point there are 3 different data formats
  // - SmartStart Prime data frame (UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED or UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED_LR)
  // - SmartStart INIF data frame  (UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED)
  // - Unsolicited data frame      (events NOT covered by the above exceptions)

  if (lucEvent == UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED    ||
      lucEvent == UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED_LR    )
  {
    // SmartStart Prime data frame

    lucFrameLength = ZWaveSerialFrame->payload[3];
    lulNWIHomeID = (0x1000000 * ZWaveSerialFrame->payload[4]) +
                   (  0x10000 * ZWaveSerialFrame->payload[5]) +
                   (    0x100 * ZWaveSerialFrame->payload[6]) +
                   (            ZWaveSerialFrame->payload[7]);
    lucCommandClassListLength = ZWaveSerialFrame->payload[8];
    lucBasicDeviceType        = ZWaveSerialFrame->payload[9];
    lucGenericDeviceType      = ZWaveSerialFrame->payload[10];
    lucSpecificDeviceType     = ZWaveSerialFrame->payload[11];
    LOG("%s: NWI HomeID           = 0x%08X\r\n", __FUNCTION__, lulNWIHomeID);
    LOG("%s: - (a.k.a. DSK bytes 9-12, but MSB | 0xC0 and LSB & 0xFE) \r\n", __FUNCTION__);
    lucCandidateDSKIndex = ZWave_Scan_ProvisioningList_For_NWIHomeID(lulNWIHomeID);
    if (lucCandidateDSKIndex != DSK_UNAVAILABLE && UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED_LR == lucEvent)
    {
      // Check if any DSKs are currently being processed, if none then change DSK status to DETECTED
      if (ZWave_DSK_IsProcessing())
      {
        LOG("%s: *** WARNING *** A DSK is already being processed\r\n", __FUNCTION__);
      }
      else
      {
        LOG("%s: Transitioning DSK %d from READY to DETECTED \r\n", __FUNCTION__, lucCandidateDSKIndex);
        gtNodeProvisioningList[lucCandidateDSKIndex].status = SMARTSTART_DETECTED;
        gucProcessingDSK = lucCandidateDSKIndex;
      }
    }
    LOG("%s: Frame length         = 0x%02X\r\n", __FUNCTION__, lucFrameLength);
    LOG("%s: Basic device type    = 0x%02X\r\n", __FUNCTION__, lucBasicDeviceType);
    ZWave_Identify_Basic_Device_Type(lucBasicDeviceType);

    LOG("%s: Generic device type  = 0x%02X\r\n", __FUNCTION__, lucGenericDeviceType);
    ZWave_Identify_Generic_Device_Type(lucGenericDeviceType);

    LOG("%s: Specific device type = 0x%02X\r\n", __FUNCTION__, lucSpecificDeviceType);
    ZWave_Identify_Specific_Device_Type(lucGenericDeviceType, lucSpecificDeviceType);

    LOG("%s: CC list length       = 0x%02X\r\n", __FUNCTION__, lucCommandClassListLength);
    LOG("----------------------- Supported Command Class list START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[12], lucCommandClassListLength, false, 0);
    LOG("----------------------- Supported Command Class list  END  -----------------------\r\n");
  }
  else if (lucEvent == UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED)
  {
    // SmartStart INIF data frame

    LOG("%s: Reserved byte        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
    LOG("%s: Rx status            = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
    lulNWIHomeID = (0x1000000 * ZWaveSerialFrame->payload[5]) +
                   (  0x10000 * ZWaveSerialFrame->payload[6]) +
                   (    0x100 * ZWaveSerialFrame->payload[7]) +
                   (            ZWaveSerialFrame->payload[8]);
    LOG("%s: NWI HomeID           = 0x%08X\r\n", __FUNCTION__, lulNWIHomeID);
  }
  else
  {
    // Unsolicited data frame

    lucCommandClassListLength = ZWaveSerialFrame->payload[3];
    lucBasicDeviceType        = ZWaveSerialFrame->payload[4];
    lucGenericDeviceType      = ZWaveSerialFrame->payload[5];
    lucSpecificDeviceType     = ZWaveSerialFrame->payload[6];
    LOG("%s: CC list length       = 0x%02X\r\n", __FUNCTION__, lucCommandClassListLength);
    LOG("%s: Basic device type    = 0x%02X\r\n", __FUNCTION__, lucBasicDeviceType);
    ZWave_Identify_Basic_Device_Type(lucBasicDeviceType);

    LOG("%s: Generic device type  = 0x%02X\r\n", __FUNCTION__, lucGenericDeviceType);
    ZWave_Identify_Generic_Device_Type(lucGenericDeviceType);

    LOG("%s: Specific device type = 0x%02X\r\n", __FUNCTION__, lucSpecificDeviceType);
    ZWave_Identify_Specific_Device_Type(lucGenericDeviceType, lucSpecificDeviceType);

    LOG("----------------------- Supported Command Class list START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[7], lucCommandClassListLength - 3, false, 0);
    //PrintBytes(&ZWaveSerialFrame->payload[4], lucCommandClassListLength, false, 0);
    LOG("----------------------- Supported Command Class list  END  -----------------------\r\n");

//    //////////////////////////////////////////////////////////////
//    //// TEST MAB 2026.01.21
//    //// Trigger end of BOOTSTRAP
//    if (lucEvent == UPDATE_STATE_NODE_INFO_RECEIVED)
//    {
//      gucIsBootstrapFinished = TRUE;
//    }
//    //////////////////////////////////////////////////////////////
  }

}
// end ZWave_REQ_CMD_49_ZW_Application_Update

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x4A FUNC_ID_ZW_ADD_NODE_TO_NETWORK ZW->HOST: various
  * @param  None
  * @retval None
  */
void ZWave_REQ_CMD_4A_ZW_Add_Node_To_Network(void)
{
  // The host should be receiving the callback (request) data frames
  LOG("%s: Session ID             = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: Status                 = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  switch (ZWaveSerialFrame->payload[1])
  {
  case ADD_NODE_STATUS_LEARN_READY:
    LOG("%s: - Network Inclusion started \r\n", __FUNCTION__);
    break;
  case ADD_NODE_STATUS_NODE_FOUND:
    LOG("%s: - Node found \r\n", __FUNCTION__);
    break;
  case ADD_NODE_STATUS_ADDING_SLAVE:
    LOG("%s: - Inclusion ongoing (for End node) \r\n", __FUNCTION__);
    break;
  case ADD_NODE_STATUS_ADDING_CONTROLLER:
    LOG("%s: - Inclusion ongoing (for Controller node) \r\n", __FUNCTION__);
    break;
  case ADD_NODE_STATUS_PROTOCOL_DONE:
    LOG("%s: - Inclusion completed (protocol part) \r\n", __FUNCTION__);
    gucIsInclusionJoiningNodeFinished = TRUE;
    break;
  case ADD_NODE_STATUS_DONE:
    LOG("%s: - Inclusion completed \r\n", __FUNCTION__);
    gucIsInclusionIncludingNodeFinished = TRUE;
    break;
  case ADD_NODE_STATUS_FAILED:
    LOG("%s: - Inclusion FAILED \r\n", __FUNCTION__);
    gucIsInclusionFailed = TRUE;
    break;
  case ADD_NODE_STATUS_FIND_NEIGHBORS_DONE:
    LOG("%s: - Find neighbors completed (SFLND was TRUE) \r\n", __FUNCTION__);
    break;
  case ADD_NODE_STATUS_NOT_PRIMARY:
    LOG("%s: - NOT Primary \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** unrecognized or reserved status \r\n", __FUNCTION__);
    break;
  }

  LOG("%s: NodeID                 = 0x%04X\r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[2]) + ZWaveSerialFrame->payload[3]);
  if (ADD_NODE_STATUS_ADDING_SLAVE == ZWaveSerialFrame->payload[1] && gucProcessingDSK < NODE_PROVISIONING_LIST_COUNT)
  {
    guiNodeID = (0x100*ZWaveSerialFrame->payload[2]) + ZWaveSerialFrame->payload[3];
    LOG("%s: Saving DSK %d NodeID 0x%04X \r\n", __FUNCTION__, gucProcessingDSK, guiNodeID);
    gtNodeProvisioningList[gucProcessingDSK].NodeID = guiNodeID;
  }
  LOG("%s: Data length            = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  if (ZWaveSerialFrame->payload[4])
  {
    LOG("%s: Basic device type      = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
    ZWave_Identify_Basic_Device_Type(ZWaveSerialFrame->payload[5]);

    LOG("%s: Generic device type    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);
    ZWave_Identify_Generic_Device_Type(ZWaveSerialFrame->payload[6]);

    LOG("%s: Specific device type   = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[7]);
    ZWave_Identify_Specific_Device_Type(ZWaveSerialFrame->payload[6], ZWaveSerialFrame->payload[7]);
    LOG("-----------------------  Supported Command Classes START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[8], ZWaveSerialFrame->payload[4] - 3, false, 0);
    LOG("-----------------------  Supported Command Classes  END  -----------------------\r\n");
  }
}
// end ZWave_REQ_CMD_4A_ZW_Add_Node_To_Network

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x4B FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK ZW->HOST: various
  * @param  None
  * @retval None
  */
void ZWave_REQ_CMD_4B_ZW_Remove_Node_From_Network(void)
{
  // The host should be receiving the callback (request) data frames
  LOG("%s: Session ID             = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: Status                 = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  switch (ZWaveSerialFrame->payload[1])
  {
  case REMOVE_NODE_STATUS_LEARN_READY:
    LOG("%s: - Network Exclusion started \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_NODE_FOUND:
    LOG("%s: - Node found \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_REMOVING_SLAVE:
    LOG("%s: - Exclusion ongoing (for End node) \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_REMOVING_CONTROLLER:
    LOG("%s: - Exclusion ongoing (for Controller node) \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_DONE:
    LOG("%s: - Exclusion completed \r\n", __FUNCTION__);
    break;
  case REMOVE_NODE_STATUS_FAILED:
    LOG("%s: - Exclusion FAILED \r\n", __FUNCTION__);
    break;
  case ADD_NODE_STATUS_NOT_PRIMARY:
    LOG("%s: - NOT Primary \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** unrecognized or reserved status \r\n", __FUNCTION__);
    break;
  }

  LOG("%s: NodeID                 = 0x%04X\r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[2]) + ZWaveSerialFrame->payload[3]);
  LOG("%s: Data length            = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  if (ZWaveSerialFrame->payload[4])
  {
    LOG("%s: Basic device type      = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
    ZWave_Identify_Basic_Device_Type(ZWaveSerialFrame->payload[5]);

    LOG("%s: Generic device type    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);
    ZWave_Identify_Generic_Device_Type(ZWaveSerialFrame->payload[6]);

    LOG("%s: Specific device type   = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[7]);
    ZWave_Identify_Specific_Device_Type(ZWaveSerialFrame->payload[6], ZWaveSerialFrame->payload[7]);
    LOG("-----------------------  Supported Command Classes START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[8], ZWaveSerialFrame->payload[4] - 3, false, 0);
    LOG("-----------------------  Supported Command Classes  END  -----------------------\r\n");
  }
}
// end ZWave_REQ_CMD_4B_ZW_Remove_Node_From_Network

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x50 FUNC_ID_ZW_SET_LEARN_MODE ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_50_ZW_Set_Learn_Mode(void)
{
  LOG("%s: Set Learn Mode result  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  if (ZWaveSerialFrame->payload[0])
  {
    LOG("%s: - Requested Learn Mode successfully set \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - *** WARNING *** Requested Learn Mode setting FAILED \r\n", __FUNCTION__);
  }
}
// end ZWave_RES_CMD_50_ZW_Set_Learn_Mode

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x51 FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_51_ZW_Assign_SUC_Return_Route(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - Assign SUC Return Route has started...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Assign SUC Return Route is already in progress\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    /* ZW->HOST: sessionID | txStatus */
    LOG("%s: Session ID = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: txStatus   = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case TRANSMIT_COMPLETE_OK:
      LOG("%s: - transmit OK \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_NO_ACK:
      LOG("%s: - transmit ERROR (no ACK received) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_FAIL:
      LOG("%s: - transmit ERROR (FAIL; network busy or jammed) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_ROUTING_NOT_IDLE:
      LOG("%s: - transmit ERROR (routing not idle) \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** txStatus value UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }
}
// end ZWave_RSQ_CMD_51_ZW_Assign_SUC_Return_Route

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x54 FUNC_ID_ZW_SET_SUC_NODE_ID
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_54_ZW_Set_SUC_NodeID(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - Set SUC NodeID successful\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Set SUC NodeID unaccepted or failed\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    /* ZW->HOST: sessionID | txStatus */
    LOG("%s: Session ID     = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: Set SUC status = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case ZW_SUC_SET_SUCCEEDED:
      LOG("%s: - Set SUC OK \r\n", __FUNCTION__);
      break;
    case ZW_SUC_SET_FAILED:
      LOG("%s: - Set SUC FAILED \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** Set SUC value UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }
}
// end ZWave_RSQ_CMD_54_ZW_Set_SUC_NodeID

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x55 FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_55_ZW_Delete_SUC_Return_Route(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - Delete SUC Return Route has started...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** Delete SUC Return Route is already in progress\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    /* ZW->HOST: sessionID | txStatus */
    LOG("%s: Session ID = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: txStatus   = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case TRANSMIT_COMPLETE_OK:
      LOG("%s: - transmit OK \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_NO_ACK:
      LOG("%s: - transmit ERROR (no ACK received) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_COMPLETE_FAIL:
      LOG("%s: - transmit ERROR (FAIL; network busy or jammed) \r\n", __FUNCTION__);
      break;
    case TRANSMIT_ROUTING_NOT_IDLE:
      LOG("%s: - transmit ERROR (routing not idle) \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** txStatus value UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }
}
// end ZWave_RSQ_CMD_55_ZW_Delete_SUC_Return_Route

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x56 FUNC_ID_ZW_GET_SUC_NODE_ID ZW->HOST: Cmd | SUC nodeID
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_56_ZW_Get_SUC_Node_ID(void)
{
  uint16_t luiSUCNodeID = (0x100*ZWaveSerialFrame->payload[0]) + ZWaveSerialFrame->payload[1];
  LOG("%s: SUC NodeID = 0x%04X\r\n", __FUNCTION__, luiSUCNodeID);
}
// end ZWave_RES_CMD_56_ZW_Get_SUC_Node_ID

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x60 FUNC_ID_ZW_REQUEST_NODE_INFO ZW->HOST: Cmd | commandStatusw
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_60_ZW_Request_Node_Info(void)
{
  if (ZWaveSerialFrame->payload[0])
  {
    LOG("%s: Request for Node Information was accepted \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: *** WARNING *** Request for Node Information was DENIED \r\n", __FUNCTION__);
  }
}
// end ZWave_RES_CMD_60_ZW_Request_Node_Info

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x61 FUNC_ID_ZW_REMOVE_FAILED_NODE_ID
  * @param  None
  * @retval None
  */
void ZWave_RSQ_CMD_61_ZW_Remove_Failed_Node(void)
{
  static uint8_t lucStatus;

  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    lucStatus = ZWaveSerialFrame->payload[0];
    LOG("%s: NodeID       = 0x%04X\r\n", __FUNCTION__, guiNodeID);
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, lucStatus);
    switch (lucStatus)
    {
    case ZW_FAILED_NODE_REMOVE_STARTED:
      LOG("%s: - ZW_FAILED_NODE_REMOVE_STARTED \r\n", __FUNCTION__);
      break;
    case ZW_NOT_PRIMARY_CONTROLLER:
      LOG("%s: - ZW_NOT_PRIMARY_CONTROLLER \r\n", __FUNCTION__);
      break;
    case ZW_NO_CALLBACK_FUNCTION:
      LOG("%s: - ZW_NO_CALLBACK_FUNCTION \r\n", __FUNCTION__);
      break;
    case ZW_FAILED_NODE_NOT_FOUND:
      LOG("%s: - ZW_FAILED_NODE_NOT_FOUND \r\n", __FUNCTION__);
      break;
    case ZW_FAILED_NODE_REMOVE_PROCESS_BUSY:
      LOG("%s: - ZW_FAILED_NODE_REMOVE_PROCESS_BUSY \r\n", __FUNCTION__);
      break;
    case ZW_FAILED_NODE_REMOVE_FAIL:
      LOG("%s: - ZW_FAILED_NODE_REMOVE_FAIL \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** status 0x%02X is unknown/undefined \r\n", __FUNCTION__, lucStatus);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    /* ZW->HOST: sessionID | Status */
    LOG("%s: NodeID                              = 0x%04X\r\n", __FUNCTION__, guiNodeID);
    LOG("%s: Session ID                          = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

    LOG("%s: Remove Failed Node Operation status = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
    switch (ZWaveSerialFrame->payload[1])
    {
    case ZW_NODE_OK:
      LOG("%s: - ZW_NODE_OK The node is working properly (removed from the failed nodes list )\r\n", __FUNCTION__);
      break;
    case ZW_FAILED_NODE_REMOVED:
      LOG("%s: - ZW_FAILED_NODE_REMOVED The failed node was removed from the failed nodes list\r\n", __FUNCTION__);
      break;
    case ZW_FAILED_NODE_NOT_REMOVED:
      LOG("%s: - ZW_FAILED_NODE_NOT_REMOVED The failed node was not removed from the failing nodes list\r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** Remove Failed Node Operation status UNKNOWN \r\n", __FUNCTION__);
      break;
    }
  }

}
// end ZWave_RES_CMD_61_ZW_Remove_Failed_Node

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0x62 FUNC_ID_ZW_IS_FAILED_NODE_ID ZW->HOST: Cmd | failedNodeIDPresence
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_62_ZW_Is_Failed_Node(void)
{
  if (ZWaveSerialFrame->payload[0])
  {
    //LOG("%s: *** WARNING *** NodeID 0x%04X is indeed present in the controller failed NodeID list \r\n", __FUNCTION__, gtNodeProvisioningList[gucProcessingDSK].NodeID);
    LOG("%s: *** WARNING *** NodeID 0x%04X is indeed present in the controller failed NodeID list \r\n", __FUNCTION__, guiNodeID);
  }
  else
  {
    //LOG("%s: NodeID 0x%04X is NOT present in the controller failed NodeID list \r\n", __FUNCTION__, gtNodeProvisioningList[gucProcessingDSK].NodeID);
    LOG("%s: NodeID 0x%04X is NOT present in the controller failed NodeID list \r\n", __FUNCTION__, guiNodeID);
  }
}
// end ZWave_RES_CMD_60_ZW_Request_Node_Info

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xA6 FUNC_ID_ZW_IS_VIRTUAL_NODE ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_A6_ZW_Is_Virtual_Node(void)
{
  // MAB 2025.11.14 Virtual nodes not implemented for LR nodes?

  LOG("%s: IsVirtualNode      = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  if (ZWaveSerialFrame->payload[0])
  {
    LOG("%s: - Node is a virtual slave node \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - Node is NOT a virtual slave node \r\n", __FUNCTION__);
  }
}
// end ZWave_RES_CMD_A6_ZW_Is_Virtual_Node

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xA8 FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_A8_Application_Command_Handler_Bridge(void)
{
  /* ZW->HOST: REQ | 0xA8 | rxStatus | destNode | sourceNode | cmdLength
   *          | pCmd[] | multiDestsOffset_NodeMaskLen | multiDestsNodeMask[] | rssiVal
   *          | securityKey | bSourceTxPower | bSourceNoiseFloor */
  LOG("%s: RX status                              = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: Destination node ID                    = 0x%04X\r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[1]) + ZWaveSerialFrame->payload[2]);
  LOG("%s: Source node ID                         = 0x%04X\r\n", __FUNCTION__, (0x100*ZWaveSerialFrame->payload[3]) + ZWaveSerialFrame->payload[4]);
  LOG("%s: Payload length                         = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
  int i = ZWaveSerialFrame->payload[5]; // Payload length
  if (i)
  {
    LOG("-----------------------  Command data START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[6], i, false, 0);
    // Invoke the command class handler
    pgucCCBuffer = &ZWaveSerialFrame->payload[6];
    gucCCBufferLength = i;
    gtZWave_CC_Handler[pgucCCBuffer[0]]();
    LOG("-----------------------  Command data  END  -----------------------\r\n");
  }
  LOG("%s: Multicast destination node mask length = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6+i]);
  int j = ZWaveSerialFrame->payload[6+i]; // Multicast destination node mask length
  if (j)
  {
    LOG("-----------------------  Multicast destination node masks START -----------------------\r\n");
    PrintBytes(&ZWaveSerialFrame->payload[7+i], j, false, 0);
    LOG("-----------------------  Multicast destination node masks  END  -----------------------\r\n");
  }
  LOG("%s: Received RSSI                          = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[7+i+j]);
  LOG("%s: Security key                           = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[8+i+j]);
  LOG("%s: Source Tx power                        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[9+i+j]);
  LOG("%s: Source noise floor                     = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[10+i+j]);

}
// end ZWave_RES_CMD_A8_Application_Command_Handler_Bridge

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xA9 FUNC_ID_ZW_SEND_DATA_BRIDGE
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_A9_ZW_Send_Data_Bridge(void)
{
  // This routine handles BOTH cases when Z-Wave controller sends the RESPONSE with a return value, and if the return value
  // is TRUE, the controller sends a callback REQUEST with assorted data.

  if (RESPONSE == ZWaveSerialFrame->type)
  {
    //////////////////////////////////////////////////////
    // Handle the RESPONSE with single return value
    //////////////////////////////////////////////////////
    LOG("%s: Return value = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
    if (ZWaveSerialFrame->payload[0])
    {
      LOG("%s: - expect a callback REQUEST with assorted data...\r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - No further data are expected\r\n", __FUNCTION__);
    }
  }
  else
  {
    //////////////////////////////////////////////////////
    // Handle the REQUEST with assorted data
    //////////////////////////////////////////////////////
    ZWave_Display_Tx_Report();
 }
}
// end ZWave_RES_CMD_A9_ZW_Send_Data_Bridge

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xDA FUNC_ID_SERIAL_API_GET_LR_NODES ZW->HOST: Cmd | MORE_NODES | BITMASK_OFFSET | BITMASK_LEN | BITMASK_ARRAY
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_DA_Serial_API_Get_LR_Nodes(void)
{
  LOG("%s: MORE_NODES      = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  LOG("%s: BITMASK_OFFSET  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  LOG("%s: BITMASK_LEN     = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);
  LOG("%s: BITMASK_ARRAY: \r\n", __FUNCTION__);
  PrintBytes(&ZWaveSerialFrame->payload[3], MAX_LR_NODEMASK_LENGTH, false, 0);
  uint16_t luiNodeID;
  uint8_t lucOffset = ZWaveSerialFrame->payload[1];
  uint8_t lucIsAnyNodeActive = FALSE;
  LOG("%s: -------------------------------------- \r\n", __FUNCTION__);
  for (uint8_t lucByteIndexJ = 0; lucByteIndexJ < ZWaveSerialFrame->payload[2]; ++lucByteIndexJ)
  {
    for (uint8_t lucBitIndexI = 0; lucBitIndexI < 8; ++lucBitIndexI)
    {
      if ( ZWaveSerialFrame->payload[3+lucByteIndexJ] & (0x01 << lucBitIndexI) )
      {
        // Bit is set, it represents an active NodeID
        lucIsAnyNodeActive = TRUE;
        luiNodeID = 256 + (8*lucByteIndexJ) + lucBitIndexI + (128*8*lucOffset);
        LOG("%s: Node %d (0x%04X) is active \r\n", __FUNCTION__, luiNodeID, luiNodeID);
      }
    }
  }
  if (!lucIsAnyNodeActive) LOG("%s: No nodes active \r\n", __FUNCTION__);
  LOG("%s: -------------------------------------- \r\n", __FUNCTION__);
  ////////////////////////////////////////////////////////////////////////////////////////
  //// TEST MAB 2026.01.07
  //// Save the LR nodes bitmask array
  memcpy(gucLRNodes, &ZWaveSerialFrame->payload[3], MAX_LR_NODEMASK_LENGTH);
  //LOG("%s: Copy of BITMASK_ARRAY: \r\n", __FUNCTION__);
  //PrintBytes(gucLRNodes, MAX_LR_NODEMASK_LENGTH, false, 0);
  ////////////////////////////////////////////////////////////////////////////////////////
}
// end ZWave_RES_CMD_DA_Serial_API_Get_LR_Nodes

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xDE FUNC_ID_GET_DCDC_CONFIG ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_DE_Get_DCDC_Config(void)
{
  LOG("%s: DC/DC configuration = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  switch (ZWaveSerialFrame->payload[0])
  {
  case EDCDCMODE_AUTO:
    LOG("%s: - Auto \r\n", __FUNCTION__);
    break;
  case EDCDCMODE_BYPASS:
    LOG("%s: - Bypass \r\n", __FUNCTION__);
    break;
  case EDCDCMODE_DCDC_LOW_NOISE:
    LOG("%s: - DC/DC Low Noise \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** unknown value\r\n", __FUNCTION__);
    break;
  }
}
// end ZWave_RES_CMD_DE_Get_DCDC_Config

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xDF FUNC_ID_SET_DCDC_CONFIG ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_DF_Set_DCDC_Config(void)
{
  LOG("%s: Set DCDC Config result  = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);
  if (ZWaveSerialFrame->payload[0])
  {
    LOG("%s: - Requested DCDC Config successfully set \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - *** WARNING *** Requested DCDC Config FAILED \r\n", __FUNCTION__);
  }
}
// end ZWave_RES_CMD_DF_Set_DCDC_Config

/** *****************************************************************************************************************************
  * @brief  Command handler for CMD 0xE8 FUNC_ID_GET_RADIO_PTI ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_E8_Get_Radio_PTI(void)
{
  LOG("%s: Radio PTI status = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);
  if (ZWaveSerialFrame->payload[0])
  {
    LOG("%s: - PTI Zniffer is ENABLED \r\n", __FUNCTION__);
  }
  else
  {
    LOG("%s: - PTI Zniffer is DISABLED \r\n", __FUNCTION__);
  }
}
// end ZWave_RES_CMD_E8_Get_Radio_PTI

/** *****************************************************************************************************************************
  * @brief  Command class handler for CC 0x26 COMMAND_CLASS_SWITCH_MULTILEVEL_V4
  * @param  None
  * @retval None
  */
void ZWave_Rx_CC_26_Switch_Multilevel_V4(void)
{
  // Assume pgucCCBuffer and gucCCBufferLength have already been assigned

  LOG("%s: Command class   = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[0]);
  LOG("%s: - COMMAND_CLASS_SWITCH_MULTILEVEL_V4 \r\n", __FUNCTION__);

  LOG("%s: Command         = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[1]);
  switch (pgucCCBuffer[1])
  {
  case SWITCH_MULTILEVEL_SET:
    LOG("%s: - SWITCH_MULTILEVEL_SET \r\n", __FUNCTION__);
    break;
  case SWITCH_MULTILEVEL_GET:
    LOG("%s: - SWITCH_MULTILEVEL_GET \r\n", __FUNCTION__);
    break;
  case SWITCH_MULTILEVEL_REPORT:
    LOG("%s: - SWITCH_MULTILEVEL_REPORT \r\n", __FUNCTION__);
    break;
  case SWITCH_MULTILEVEL_START_LEVEL_CHANGE:
    LOG("%s: - SWITCH_MULTILEVEL_START_LEVEL_CHANGE \r\n", __FUNCTION__);
    break;
  case SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE:
    LOG("%s: - SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** Switch Multilevel command UNKNOWN \r\n", __FUNCTION__);
    break;
  }

  if ( SWITCH_MULTILEVEL_SET    == pgucCCBuffer[1] )
  {
    LOG("%s: Value           = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
  }

  if ( SWITCH_MULTILEVEL_REPORT    == pgucCCBuffer[1] )
  {
    LOG("%s: Current Value   = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
    if (0x00 == pgucCCBuffer[2])
    {
      LOG("%s: - OFF \r\n", __FUNCTION__)
    }
    else if (0x01 <= pgucCCBuffer[2] && pgucCCBuffer[2] <= 0x63)
    {
      LOG("%s: - ON, %02d %% \r\n", __FUNCTION__, pgucCCBuffer[2])
    }
    else if (0xFF == pgucCCBuffer[2])
    {
      LOG("%s: - ON; restored to most recent level \r\n", __FUNCTION__)
    }
    else
    {
      LOG("%s: - *** WARNING *** reserved value \r\n", __FUNCTION__);
    }
    LOG("%s: Target  Value   = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[3]);
    LOG("%s: Duration        = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[4]);
  }

  if ( SWITCH_MULTILEVEL_START_LEVEL_CHANGE    == pgucCCBuffer[1] )
  {
    LOG("%s: Options         = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[3]);
    LOG("%s: Start level     = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[4]);
  }
}
// end ZWave_Rx_CC_26_Switch_Multilevel_V4

/** *****************************************************************************************************************************
  * @brief  Command class handler for CC 0x9F COMMAND_CLASS_SECURITY_2_V2
  * @param  None
  * @retval None
  */
void ZWave_Rx_CC_9F_Security_2_V2(void)
{
  // Assume pgucCCBuffer and gucCCBufferLength have already been assigned

  LOG("%s: Command class   = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[0]);
  LOG("%s: - COMMAND_CLASS_SECURITY_2_V2 \r\n", __FUNCTION__);

  LOG("%s: Security header = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[1]);
  switch (pgucCCBuffer[1])
  {
  case SECURITY_2_NONCE_GET_V2:
    LOG("%s: - SECURITY_2_NONCE_GET_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_NONCE_REPORT_V2:
    LOG("%s: - SECURITY_2_NONCE_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_MESSAGE_ENCAPSULATION_V2:
    LOG("%s: - SECURITY_2_MESSAGE_ENCAPSULATION_V2 \r\n", __FUNCTION__)
    break;
  case KEX_GET_V2:
    LOG("%s: - KEX_GET_V2 \r\n", __FUNCTION__)
    break;
  case KEX_REPORT_V2:
    LOG("%s: - KEX_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case KEX_SET_V2:
    LOG("%s: - KEX_SET_V2 \r\n", __FUNCTION__)
    break;
  case KEX_FAIL_V2:
    LOG("%s: - KEX_FAIL_V2 \r\n", __FUNCTION__)
    break;
  case PUBLIC_KEY_REPORT_V2:
    LOG("%s: - PUBLIC_KEY_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_NETWORK_KEY_GET_V2:
    LOG("%s: - SECURITY_2_NETWORK_KEY_GET_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_NETWORK_KEY_REPORT_V2:
    LOG("%s: - SECURITY_2_NETWORK_KEY_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_NETWORK_KEY_VERIFY_V2:
    LOG("%s: - SECURITY_2_NETWORK_KEY_VERIFY_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_TRANSFER_END_V2:
    LOG("%s: - SECURITY_2_TRANSFER_END_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_COMMANDS_SUPPORTED_GET_V2:
    LOG("%s: - SECURITY_2_COMMANDS_SUPPORTED_GET_V2 \r\n", __FUNCTION__)
    break;
  case SECURITY_2_COMMANDS_SUPPORTED_REPORT_V2:
    LOG("%s: - SECURITY_2_COMMANDS_SUPPORTED_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case NLS_NODE_LIST_GET_V2:
    LOG("%s: - NLS_NODE_LIST_GET_V2 \r\n", __FUNCTION__)
    break;
  case NLS_NODE_LIST_REPORT_V2:
    LOG("%s: - NLS_NODE_LIST_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case NLS_STATE_GET_V2:
    LOG("%s: - NLS_STATE_GET_V2 \r\n", __FUNCTION__)
    break;
  case NLS_STATE_REPORT_V2:
    LOG("%s: - NLS_STATE_REPORT_V2 \r\n", __FUNCTION__)
    break;
  case NLS_STATE_SET_V2:
    LOG("%s: - NLS_STATE_SET_V2 \r\n", __FUNCTION__)
    break;
  default:
    LOG("%s: - *** WARNING *** Security header UNKNOWN \r\n", __FUNCTION__)
    break;
  }

  if (SECURITY_2_NONCE_GET_V2             == pgucCCBuffer[1])
  {
    gucIsNonceGetReceived = TRUE;
    LOG("%s: Sequence number = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
  }


  if (SECURITY_2_NONCE_REPORT_V2          == pgucCCBuffer[1])
  {
    LOG("%s: Sequence number = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
    LOG("%s: Sync flags      = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[3]);
    uint8_t lucSPANOutOfSync;
    if (pgucCCBuffer[3] & SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK_V2)
    {
      LOG("%s: - SPAN out of sync \r\n", __FUNCTION__);
      lucSPANOutOfSync = true;
    }
    else
    {
      LOG("%s: - SPAN in sync, decrypted latest SECURITY_2_MESSAGE_ENCAPSULATION_V2 OK \r\n", __FUNCTION__);
      lucSPANOutOfSync = false;
    }
    if (pgucCCBuffer[3] & SECURITY_2_NONCE_REPORT_PROPERTIES1_MOS_BIT_MASK_V2)
    {
      LOG("%s: - MPAN out of sync \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - MPAN in sync \r\n", __FUNCTION__);
    }
    if (lucSPANOutOfSync)
    {
      LOG("%s: Receiver's Entropy Input (REI) \r\n", __FUNCTION__);
      PrintBytes(&pgucCCBuffer[4], 16, false, 0);
    }
  }

  if (SECURITY_2_MESSAGE_ENCAPSULATION_V2 == pgucCCBuffer[1])
  {
    uint8_t lucExtensionOffset = 0;

    LOG("%s: Sequence number = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
    LOG("%s: Extensions      = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[3]);
    uint8_t lucExtensionsPresent;
    if (pgucCCBuffer[3] & SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_EXTENSION_BIT_MASK_V2)
    {
      LOG("%s: - One or more extensions are present \r\n", __FUNCTION__);
      lucExtensionsPresent = true;
    }
    else
    {
      LOG("%s: - No extensions present \r\n", __FUNCTION__);
      lucExtensionsPresent = false;
    }
    //uint8_t lucEncryptedExtensionsPresent;
    if (pgucCCBuffer[3] & SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_ENCRYPTED_EXTENSION_BIT_MASK_V2)
    {
      LOG("%s: - One or more encrypted extensions are present \r\n", __FUNCTION__);
      //lucEncryptedExtensionsPresent = true;
    }
    else
    {
      LOG("%s: - No encrypted extensions present \r\n", __FUNCTION__);
      //lucEncryptedExtensionsPresent = false;
    }

    if (lucExtensionsPresent)
    {
      uint8_t lucExtensionCount = 0;
      uint8_t lucNextExtensionPresent = true;
      while (lucNextExtensionPresent)
      {
        ++lucExtensionCount;

        LOG("%s: Extension %d length      = 0x%02X \r\n", __FUNCTION__, lucExtensionCount, pgucCCBuffer[4+lucExtensionOffset]);
        LOG("%s: - this includes the length byte itself and the options byte following \r\n", __FUNCTION__);
        LOG("%s: Extension %d options     = 0x%02X \r\n", __FUNCTION__, lucExtensionCount, pgucCCBuffer[5+lucExtensionOffset]);
        switch (pgucCCBuffer[5+lucExtensionOffset] & 0x3F)
        {
        case 0x01:
          LOG("%s: - SPAN extension - not encrypted \r\n", __FUNCTION__);
          break;
        case 0x02:
          LOG("%s: - MPAN extension - encrypted \r\n", __FUNCTION__);
          break;
        case 0x03:
          LOG("%s: - MGRP extension - not encrypted \r\n", __FUNCTION__);
          break;
        case 0x04:
          LOG("%s: - MOS extension - not encrypted \r\n", __FUNCTION__);
          break;
        default:
          LOG("%s: *** WARNING *** extension type UNKNOWN \r\n", __FUNCTION__);
          break;
        }
        if (pgucCCBuffer[5+lucExtensionOffset] & 0x40)
        {
          LOG("%s: - Critical bit set \r\n", __FUNCTION__);
        }
        lucNextExtensionPresent = false;
        if (pgucCCBuffer[5+lucExtensionOffset] & 0x80)
        {
          LOG("%s: - 'More to follow' bit set \r\n", __FUNCTION__);
          lucNextExtensionPresent = true;
        }
        LOG("%s: Extension %d... \r\n", __FUNCTION__, lucExtensionCount);
        PrintBytes(&pgucCCBuffer[6+lucExtensionOffset], pgucCCBuffer[4+lucExtensionOffset] - 2, false, 0);
        ////////////////////////////////////////////////////////////////////////
        //// TEST MAB 2026.02.03
        //// This is the Sender's Entropy Input; save it (if 16 bytes)
        //// NOTE: this is either for the Temporary Key or for the Network Key,
        ////       so save appropriately
        if ( 16 == (pgucCCBuffer[4+lucExtensionOffset] - 2) )
        {
          if (BOOTSTRAP_TEMP_NONCE_SET == geBootstrapState)
          {
            LOG("%s: - saving Sender's Entropy Input (SEI) for TEMPORARY KEY \r\n", __FUNCTION__);
            memcpy(gucTemporarySEI, &pgucCCBuffer[6+lucExtensionOffset], 16);
          }
          if (BOOTSTRAP_NETWORK_VERIFY == geBootstrapState)
          {
            LOG("%s: - saving Sender's Entropy Input (SEI) for DSK %d \r\n", __FUNCTION__, gucProcessingDSK);
            memcpy(gtNodeProvisioningList[gucProcessingDSK].SEI, &pgucCCBuffer[6+lucExtensionOffset], 16);
          }
        }
        ////////////////////////////////////////////////////////////////////////

        // Offset to next extension (or to CCM Ciphertext object if no more extensions)
        lucExtensionOffset += pgucCCBuffer[4+lucExtensionOffset];
      } // end while
    } // end if (lucExtensionsPresent)

    // TODO parse encrypted extensions (if present - and if necessary: the encrypted extensions might follow seamlessly with the unencrypted extensions)

    uint8_t lucCCMOffset = 4 + lucExtensionOffset;
    LOG("%s: CCM Ciphertext object... \r\n", __FUNCTION__);
    PrintBytes(&pgucCCBuffer[lucCCMOffset], gucCCBufferLength - lucCCMOffset, false, 0);

  } // end if (SECURITY_2_MESSAGE_ENCAPSULATION_V2 == pgucCCBuffer[1])

  if (KEX_REPORT_V2 == pgucCCBuffer[1])
  {
    gucIsKEXReportReceived = TRUE;

    LOG("%s: KEX options     = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
    if (pgucCCBuffer[2] & KEX_REPORT_PROPERTIES1_ECHO_BIT_MASK_V2)
    {
      LOG("%s: - Echo bit set \r\n", __FUNCTION__);
    }
    if (pgucCCBuffer[2] & KEX_REPORT_PROPERTIES1_REQUEST_CSA_BIT_MASK_V2)
    {
      LOG("%s: - Request CSA bit set \r\n", __FUNCTION__);
    }
    if (pgucCCBuffer[2] & KEX_REPORT_PROPERTIES1_NLS_SUPPORT_BIT_MASK_V2)
    {
      LOG("%s: - NLS Support bit set \r\n", __FUNCTION__);
    }

    LOG("%s: KEX schemes     = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[3]);
    if (pgucCCBuffer[3] & 0x02)
    {
      LOG("%s: - KEX Scheme 1 supported \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** KEX Scheme 1 is NOT supported \r\n", __FUNCTION__);
    }

    LOG("%s: ECDH profiles   = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[4]);
    if (pgucCCBuffer[4] & 0x01)
    {
      LOG("%s: - ECDH Profile Curve25519 supported \r\n", __FUNCTION__);
    }
    else
    {
      LOG("%s: - *** WARNING *** ECDH Profile Curve25519 is NOT supported \r\n", __FUNCTION__);
    }

    LOG("%s: Requested keys  = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[5]);
    if (pgucCCBuffer[5] & SECURITY_KEY_S2_ACCESS_BIT)
    {
      LOG("%s: - S2 Access Control Class supported \r\n", __FUNCTION__);
    }
    if (pgucCCBuffer[5] & SECURITY_KEY_S2_AUTHENTICATED_BIT)
    {
      LOG("%s: - S2 Authenticated Class supported \r\n", __FUNCTION__);
    }
    if (pgucCCBuffer[5] & SECURITY_KEY_S2_UNAUTHENTICATED_BIT)
    {
      LOG("%s: - S2 Unauthenticated Class supported \r\n", __FUNCTION__);
    }
    if (pgucCCBuffer[5] & SECURITY_KEY_S0_BIT)
    {
      LOG("%s: - S0 Secure legacy devices supported \r\n", __FUNCTION__);
    }
    //////////////////////////////////////////////////////////
    //// MAB 2026.01.19
    //// If this is a freshly included node, save requested keys
    if (gucProcessingDSK <= NODE_PROVISIONING_LIST_COUNT && SMARTSTART_BOOTSTRAP == gtNodeProvisioningList[gucProcessingDSK].status)
    {
      LOG("%s: Saving requested keys for DSK %d \r\n", __FUNCTION__, gucProcessingDSK);
      gtNodeProvisioningList[gucProcessingDSK].requested_keys = pgucCCBuffer[5];
    }
    //////////////////////////////////////////////////////////
    // Put out a warning if no S2 levels are supported
    gucIsS2Supported = TRUE;
    if ( 0 == pgucCCBuffer[5] & (SECURITY_KEY_S2_ACCESS_BIT|SECURITY_KEY_S2_AUTHENTICATED_BIT|SECURITY_KEY_S2_UNAUTHENTICATED_BIT) )
    {
      gucIsS2Supported = FALSE;
      LOG("%s: *** WARNING *** no S2 security levels are supported, security bootstrap will fail \r\n", __FUNCTION__);
    }
  }

  if (KEX_FAIL_V2 == pgucCCBuffer[1])
  {
    LOG("%s: KEX Fail type   = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
    switch (pgucCCBuffer[2])
    {
    case KEX_FAIL_KEX_KEY_V2:
      LOG("%s: - KEX_FAIL_KEX_KEY_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_KEX_SCHEME_V2:
      LOG("%s: - KEX_FAIL_KEX_SCHEME_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_KEX_CURVES_V2:
      LOG("%s: - KEX_FAIL_KEX_CURVES_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_DECRYPT_V2:
      LOG("%s: - KEX_FAIL_DECRYPT_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_CANCEL_V2:
      LOG("%s: - KEX_FAIL_CANCEL_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_AUTH_V2:
      LOG("%s: - KEX_FAIL_AUTH_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_KEY_GET_V2:
      LOG("%s: - KEX_FAIL_KEY_GET_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_KEY_VERIFY_V2:
      LOG("%s: - KEX_FAIL_KEY_VERIFY_V2 \r\n", __FUNCTION__);
      break;
    case KEX_FAIL_KEY_REPORT_V2:
      LOG("%s: - KEX_FAIL_KEY_REPORT_V2 \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** unknown KEX Fail type  \r\n", __FUNCTION__ );
      break;
    }
  }

  if (PUBLIC_KEY_REPORT_V2 == pgucCCBuffer[1])
  {
    gucIsPublicKeyReportReceived = TRUE;

    LOG("%s: Options         = 0x%02X \r\n", __FUNCTION__, pgucCCBuffer[2]);
    if (pgucCCBuffer[2] & PUBLIC_KEY_REPORT_PROPERTIES1_INCLUDING_NODE_BIT_MASK_V2)
    {
      LOG("%s: - *** WARNING *** Including Node bit set: sent by the including node; security bootstrap will fail \r\n", __FUNCTION__);
      gucIsIncludingNode = TRUE;
    }
    else
    {
      LOG("%s: - Including Node bit clear: sent by the joining node \r\n", __FUNCTION__);
    }

    LOG("%s: ECDH Public key (1st 16 bytes: DSK with some bytes possibly obfuscated with 0x00)... \r\n", __FUNCTION__);
    PrintBytes(&pgucCCBuffer[3], 32, false, 0);

    ////////////////////////////////////////////////
    //// MAB 2026.01.02
    //// Try to detect DSK in node provisioning list
    uint8_t lucDetectedDSKIndex;
    #define SCAN_FOR_DSK_INCOMPLETE_MATCH_ACCEPTABLE (FALSE)
    #define SCAN_FOR_DSK_FULL_MATCH_REQUIRED (TRUE)
    lucDetectedDSKIndex = ZWave_Scan_ProvisioningList_For_DSK(&pgucCCBuffer[3], SCAN_FOR_DSK_INCOMPLETE_MATCH_ACCEPTABLE);
    if (lucDetectedDSKIndex < NODE_PROVISIONING_LIST_COUNT)
    {
      // Copy the ECDH Public Key into the appropriate DSK
      LOG("%s: Copying ECDH Public Key for DSK %d \r\n", __FUNCTION__, lucDetectedDSKIndex);
      memcpy(&gtNodeProvisioningList[lucDetectedDSKIndex].ECDHPublicKey[ 0], gtNodeProvisioningList[lucDetectedDSKIndex].dsk, DSK_LENGTH_BYTES);
      memcpy(&gtNodeProvisioningList[lucDetectedDSKIndex].ECDHPublicKey[16], &pgucCCBuffer[3+DSK_LENGTH_BYTES],               DSK_LENGTH_BYTES);
      //PrintBytes(gtNodeProvisioningList[lucDetectedDSKIndex].ECDHPublicKey, 32, false, 0);
    }
    ////////////////////////////////////////////////

  }

}
// end ZWave_Rx_CC_9F_Security_2_V2

/** *****************************************************************************************************************************
  * @brief  Dummy command handler for unsupported Z-Wave command class
  * @param  None
  * @retval None
  */
void ZWave_Rx_CC_XX_Unsupported(void)
{
  //PrintBytes(pgucCCBuffer, gucCCBufferLength, false, 0);
  LOG("%s: *** WARNING *** Command class 0x%02X not supported (yet)... \r\n", __FUNCTION__, pgucCCBuffer[0]);
}
// end ZWave_Rx_CC_XX_Unsupported

/** *****************************************************************************************************************************
  * @brief  Dummy command handler for unsupported Z-Wave command
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_XX_Unsupported(void)
{
  LOG("%s: *** WARNING *** Serial API command 0x%02X not supported (yet)... \r\n", __FUNCTION__, ZWaveSerialFrame->cmd);
}
// end ZWave_RES_CMD_XX_Unsupported

/** *****************************************************************************************************************************
  * @brief  Scan Node Provisioning List for matching DSK
  * @param  uint8_t* paucDSKBuffer - pointer to a byte buffer containing the 16-byte DSK
  * @param  uint8_t  aucNeedExactMatch - TRUE for exact 16-byte DSK match required; FALSE for "close enough" match
  * @retval DSK index if matching DSK is found; 0xFF if no matching DSKs found
  */
uint8_t ZWave_Scan_ProvisioningList_For_DSK(uint8_t* paucDSKBuffer, uint8_t aucNeedExactMatch)
{
  uint8_t lucReturnValue = DSK_UNAVAILABLE; // Assume no matching DSKs are present until proven otherwise
  uint8_t lucIsDSKMatch;
  uint8_t lucDSKStartByte;

  // An exact match compares all 16 bytes of a DSK
  // A "close enough" match skips the first 2 bytes of a DSK
  if (aucNeedExactMatch)
  {
    lucDSKStartByte = 0;
  }
  else
  {
    lucDSKStartByte = 2;
  }

  for (uint8_t lucDSKIndex = 0; lucDSKIndex < NODE_PROVISIONING_LIST_COUNT; ++lucDSKIndex)
  {
    // Assume this given DSK is a match until proven otherwise
    lucIsDSKMatch = TRUE;

    // Check each byte of this DSK in the provisioning list
    for (uint8_t lucDSKByte = lucDSKStartByte; lucDSKByte < DSK_LENGTH_BYTES; ++lucDSKByte)
    {
      if (paucDSKBuffer[lucDSKByte] != gtNodeProvisioningList[lucDSKIndex].dsk[lucDSKByte])
      {
        lucIsDSKMatch = FALSE;
      }
    }

    // If we've found a matching DSK, return the DSK index
    if (lucIsDSKMatch)
    {
      LOG("%s: DSK %d is a match \r\n", __FUNCTION__, lucDSKIndex);
      lucReturnValue = lucDSKIndex;
      break;  // Quit searching for a matching DSK
    }
  }

  return lucReturnValue;
}
// end ZWave_Scan_ProvisioningList_For_DSK

/** *****************************************************************************************************************************
  * @brief  Scan Node Provisioning List for any/all matching DSKs for NodeID
  * @param  uint16_t auiNodeID - 2-byte NodeID to search in provisioning list
  * @retval DSK index if any matching DSKs are found; 0xFF if no matching DSKs found
  */
uint8_t ZWave_Scan_ProvisioningList_For_NodeID(uint16_t auiNodeID)
{
  uint8_t lucReturnValue = DSK_UNAVAILABLE;

  for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
  {
    if (gtNodeProvisioningList[i].NodeID == auiNodeID)
    {
      lucReturnValue = i;
      LOG("%s: DSK %d appears to be a match! \r\n", __FUNCTION__, i);
    }
  }

  return lucReturnValue;
}
// end ZWave_Scan_ProvisioningList_For_NodeID

/** *****************************************************************************************************************************
  * @brief  Scan Node Provisioning List for any/all matching DSKs for NWI HomeID
  * @param  uint32_t aulNWIHomeID - NWI HomeID to search in provisioning list
  * @retval DSK index if any matching DSKs are found; 0xFF if no matching DSKs found
  */
uint8_t ZWave_Scan_ProvisioningList_For_NWIHomeID(uint32_t aulNWIHomeID)
{
  uint8_t lucReturnValue = DSK_UNAVAILABLE;
  uint32_t lulCandidateNWIHomeID;

  // NOTE: NWI HomeID is derived from DSK bytes 8-11 (0-indexed),
  //       with 2 most significant bits set and least significant bit cleared
  for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
  {
    lulCandidateNWIHomeID =  0x1000000 * (gtNodeProvisioningList[i].dsk[8] | 0xC0);
    lulCandidateNWIHomeID +=   0x10000 * (gtNodeProvisioningList[i].dsk[9]);
    lulCandidateNWIHomeID +=     0x100 * (gtNodeProvisioningList[i].dsk[10]);
    lulCandidateNWIHomeID +=             (gtNodeProvisioningList[i].dsk[11] & 0xFE);

    if (lulCandidateNWIHomeID == aulNWIHomeID)
    {
      lucReturnValue = i;
      LOG("%s: DSK %d appears to be a match! \r\n", __FUNCTION__, i);
    }
  }

  return lucReturnValue;
}
// end ZWave_Scan_ProvisioningList_For_NWIHomeID

#if ENABLE_ZWAVE_CONTROLLER_HOST

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 02 Serial API Get Init Data
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_02_Serial_API_Get_Init_Data(void)
{
  ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_GET_INIT_DATA, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_SERIAL_API_GET_INIT_DATA\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_02_Serial_API_Get_Init_Data

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 03 Serial API Application Node Information
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_03_Serial_API_Application_Node_Information(void)
{
  // The PC ZWave Controller application at one point (I think I was trying to
  // set up the Learn Mode as Smart Start) sent this out
  // 0x01 SOF
  // 0x18 length
  // 0x03 command FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION
  // Payload:
  // 03 02 01 11 5E 22 85 70 56 7A 72 73 8A 55 86 59 5A 6C 74 98 9F
  // 0x14 checksum

  // Payload breakdown
  // 0x03 Device option mask  - in this case, possibly Always On mode + Optional Functionality supported
  // 0x02 Node type, generic  - in this case, GENERIC_TYPE_STATIC_CONTROLLER
  // 0x01 Node type, specific - in this case, SPECIFIC_TYPE_PC_CONTROLLER
  // 0x11 Parameter length    - in this case, 17 bytes
  // 5E 22 85 70 56 7A 72 73    8A 55 86 59 5A 6C 74 98    9F
  //      Parameter data - in this case, 17 bytes of unsecure included command classes

  // ChatGPT claims that since this is the controller and the controller doesn't need to
  // advertise its supported command classes, this command class list is actually from
  // a Node Information Frame (NIF) *template*, for backwards compatibility, and
  // may be safely ignored for the controller.
}
// end ZWave_Send_REQ_CMD_03_Serial_API_Application_Node_Information

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 05 Get Controller Capabilities
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_05_Get_Controller_Capabilities(void)
{
  ZWave_Enqueue_Request(FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_05_Get_Controller_Capabilities

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 07 Serial API Get Capabilities
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_07_Serial_API_Get_Capabilities(void)
{
  ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_GET_CAPABILITIES, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_SERIAL_API_GET_CAPABILITIES\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_07_Serial_API_Get_Capabilities

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 08 Serial API Soft Reset
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_08_Serial_API_Soft_Reset(void)
{
  ZWave_Enqueue_Request_Unsolicited(FUNC_ID_SERIAL_API_SOFT_RESET, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_SERIAL_API_SOFT_RESET\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_08_Serial_API_Soft_Reset

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 0B Serial API Setup
  * @param  aeSetupCommand - SerialAPI setup command
  * @retval None
  */
void ZWave_Send_REQ_CMD_0B_Serial_API_Setup(eSerialAPISetupCmd aeSetupCommand, int16_t aiParameter1, int16_t aiParameter2)
{
  if (SERIAL_API_SETUP_CMD_SUPPORTED == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 1);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_SUPPORTED\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    gucZWaveWorkbuf[1]  = (uint8_t)(aiParameter1 / 0x100);
    gucZWaveWorkbuf[2]  = (uint8_t)(aiParameter1 &  0xFF);
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 3);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 1);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 1);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 1);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    gucZWaveWorkbuf[1]  = (uint8_t)(aiParameter1 / 0x100);
    gucZWaveWorkbuf[2]  = (uint8_t)(aiParameter1 &  0xFF);
    gucZWaveWorkbuf[3]  = (uint8_t)(aiParameter2 / 0x100);
    gucZWaveWorkbuf[4]  = (uint8_t)(aiParameter2 &  0xFF);
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 5);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 1);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_RF_REGION_GET == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 1);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_RF_REGION_GET\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_RF_REGION_SET == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    gucZWaveWorkbuf[1]  = (zpal_radio_region_t)aiParameter1; // REGION_EU, REGION_US, etc.
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 2);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET\r\n", __FUNCTION__);
  }

  if (SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET == aeSetupCommand)
  {
    gucZWaveWorkbuf[0]  = aeSetupCommand;
    gucZWaveWorkbuf[1]  = SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT;  // We're gonna use LR, so must use 16-bit, so hard-code the value
    ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_SETUP, gucZWaveWorkbuf, 2);
    LOG("%s: Sending SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET\r\n", __FUNCTION__);
  }



}
// end ZWave_Send_REQ_CMD_0B_Serial_API_Setup

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 12 Send Node Information
  * @param  uint16_t auiNodeID    - 2-byte destination NodeID
  * @param  uint8_t  aucTxOption  - Explore | No route | Reserved | Auto route | Low power | ACK
  * @param  uint8_t  aucSessionID - session ID
  * @retval None
  */
void ZWave_Send_REQ_CMD_12_Send_Node_Information(uint16_t auiNodeID, uint8_t aucTxOption, uint8_t aucSessionID)
{
  gucZWaveWorkbuf[0] = (uint8_t)(auiNodeID / 0x100);
  gucZWaveWorkbuf[1] = (uint8_t)(auiNodeID & 0xFF);
  gucZWaveWorkbuf[2] = aucTxOption;
  gucZWaveWorkbuf[3] = aucSessionID;

  ZWave_Enqueue_Request(FUNC_ID_ZW_SEND_NODE_INFORMATION, gucZWaveWorkbuf, 4);
  LOG("%s: Sending FUNC_ID_ZW_SEND_NODE_INFORMATION\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_12_Send_Node_Information

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 13 Send Data
  * @param  uint16_t auiNodeID    - 2-byte destination NodeID
  * @param  uint8_t  aucLength    - data length [1, 255]
  * @param  uint8_t* paucData     - pointer to data buffer to be sent
  * @param  uint8_t  aucTxOption  - Explore | No route | Reserved | Auto route | Low power | ACK
  * @param  uint8_t  aucSessionID - session ID
  * @retval None
  */
void ZWave_Send_REQ_CMD_13_Send_Data(uint16_t auiNodeID, uint8_t aucLength, uint8_t* paucData, uint8_t aucTxOption, uint8_t aucSessionID)
{
  gucZWaveWorkbuf[0] = (uint8_t)(auiNodeID / 0x100);
  gucZWaveWorkbuf[1] = (uint8_t)(auiNodeID & 0xFF);
  gucZWaveWorkbuf[2] = aucLength;
  if (aucLength)
  {
    memcpy(&gucZWaveWorkbuf[3], paucData, aucLength);
  }
  gucZWaveWorkbuf[3+aucLength] = aucTxOption;
  gucZWaveWorkbuf[4+aucLength] = aucSessionID;

  ZWave_Enqueue_Request(FUNC_ID_ZW_SEND_DATA, gucZWaveWorkbuf, 5+aucLength);
  LOG("%s: Sending FUNC_ID_ZW_SEND_DATA\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_13_Send_Data

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 20 Memory Get ID
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_20_Memory_Get_ID(void)
{
  ZWave_Enqueue_Request(FUNC_ID_MEMORY_GET_ID, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_MEMORY_GET_ID\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_20_Memory_Get_ID

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 28 NVR Get Value
  * @param  uint8_t  aucOffset - memory offset [0x00, 0xEF]
  * @param  uint8_t  aucLength - data length [1, 255]
  * @retval None
  */
void ZWave_Send_REQ_CMD_28_NVR_Get_Value(uint8_t aucOffset, uint8_t aucLength)
{
  gucZWaveWorkbuf[0] = aucOffset;
  gucZWaveWorkbuf[1] = aucLength;

  ZWave_Enqueue_Request(FUNC_ID_NVR_GET_VALUE, gucZWaveWorkbuf, 2);
  LOG("%s: Sending FUNC_ID_NVR_GET_VALUE\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_28_NVR_Get_Value

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 3F Remove Specific Node from Network
  * @param  uint8_t aucOptions     - Power | NWI | Protocol | SFLND | Mode (4 bits)
  * @param  uint8_t aucSessionID   - session ID
  * @param  uint16_t auiNodeID     - 2-byte NodeID to be removed from network
  * @retval None
  */
void ZWave_Send_REQ_CMD_3F_Remove_Specific_Node_from_Network(uint8_t aucOptions, uint8_t aucSessionID, uint16_t auiNodeID)
{
  gucZWaveWorkbuf[0] = aucOptions;
  gucZWaveWorkbuf[1] = (uint8_t)(auiNodeID / 0x100);
  gucZWaveWorkbuf[2] = (uint8_t)(auiNodeID & 0xFF);
  gucZWaveWorkbuf[3] = aucSessionID;

  ZWave_Enqueue_Request(FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK, gucZWaveWorkbuf, 4);
  LOG("%s: Sending FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_3F_Remove_Specific_Node_from_Network

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 41 Get Node Protocol Info
  * @param  uint16_t auiNodeID - 2-byte NodeID
  * @retval None
  */
void ZWave_Send_REQ_CMD_41_Get_Node_Protocol_Info(uint16_t auiNodeID)
{
  // IF NodeID is nonzero
  if (auiNodeID)
  {
    gucZWaveWorkbuf[0]  = (uint8_t)(auiNodeID/0x100);
    gucZWaveWorkbuf[1]  = (uint8_t)(auiNodeID & 0xFF);
    ZWave_Enqueue_Request(FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO, gucZWaveWorkbuf, 2);
    LOG("%s: Sending FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO, NodeID = 0x%04X\r\n", __FUNCTION__, auiNodeID);
  }
  // ELSE
  else
  {
    LOG("%s: *** WARNING *** NodeID is still 0x0000, expected a nonzero value. FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO aborted.\r\n", __FUNCTION__);
  }
  // ENDIF
}
// end ZWave_Send_REQ_CMD_41_Get_Node_Protocol_Info

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 42 Set Default
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_42_Set_Default(void)
{
  gucSessionID = ZWave_SessionID_Update(gucSessionID);
  gucZWaveWorkbuf[0] = gucSessionID;
  ZWave_Enqueue_Request(FUNC_ID_ZW_SET_DEFAULT, gucZWaveWorkbuf, 1);
  LOG("%s: Sending FUNC_ID_ZW_SET_DEFAULT\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_42_SetDefault

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 4A Add Node to Network
  * @param  uint8_t aucOptions     - Power | NWI | Protocol | SFLND | Mode (4 bits)
  * @param  uint8_t aucSessionID   - session ID
  * @param  uint8_t* paucNWIAuthID - pointer to buffer containing NWI and Auth HomeIDs (8 bytes total)
  * @retval None
  */
void ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(uint8_t aucOptions, uint8_t aucSessionID, uint8_t* paucNWIAuthID)
{
  uint8_t lucBufferLength;

  gucZWaveWorkbuf[0] = aucOptions;
  gucZWaveWorkbuf[1] = aucSessionID;
  lucBufferLength = 2;

  if ( ADD_NODE_HOME_ID == (aucOptions & 0x0F) )
  {
    // Copy the 4 NWI HomeID and 4 Auth HomeID bytes (8 total)
    memcpy(&gucZWaveWorkbuf[2], paucNWIAuthID, 8);
    lucBufferLength += 8;
  }

  ZWave_Enqueue_Request(FUNC_ID_ZW_ADD_NODE_TO_NETWORK, gucZWaveWorkbuf, lucBufferLength);
  LOG("%s: Sending FUNC_ID_ZW_ADD_NODE_TO_NETWORK\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_4A_Add_Node_to_Network

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 56 Get SUC Node ID
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_56_Get_SUC_Node_ID(void)
{
  ZWave_Enqueue_Request(FUNC_ID_ZW_GET_SUC_NODE_ID, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_ZW_GET_SUC_NODE_ID\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_56_Get_SUC_Node_ID

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 61 Is Node Failed
  * @param  uint8_t aucSessionID   - session ID
  * @param  uint16_t auiNodeID     - 2-byte NodeID to be checked if in failed node list
  * @retval None
  */
void ZWave_Send_REQ_CMD_61_Remove_Failed_Node(uint8_t aucSessionID, uint16_t auiNodeID)
{
  gucZWaveWorkbuf[0] = (uint8_t)(auiNodeID / 0x100);
  gucZWaveWorkbuf[1] = (uint8_t)(auiNodeID & 0xFF);
  gucZWaveWorkbuf[2] = aucSessionID;

  ZWave_Enqueue_Request(FUNC_ID_ZW_REMOVE_FAILED_NODE_ID, gucZWaveWorkbuf, 3);
  LOG("%s: Sending FUNC_ID_ZW_REMOVE_FAILED_NODE_ID\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_61_Remove_Failed_Node_from_Network

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD 62 Is Node Failed
  * @param  uint16_t auiNodeID     - 2-byte NodeID to be checked if in failed node list
  * @retval None
  */
void ZWave_Send_REQ_CMD_62_Is_Node_Failed(uint16_t auiNodeID)
{
  gucZWaveWorkbuf[0] = (uint8_t)(auiNodeID / 0x100);
  gucZWaveWorkbuf[1] = (uint8_t)(auiNodeID & 0xFF);

  ZWave_Enqueue_Request(FUNC_ID_ZW_IS_FAILED_NODE_ID, gucZWaveWorkbuf, 2);
  LOG("%s: Sending FUNC_ID_ZW_IS_FAILED_NODE_ID\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_62_Is_Node_Failed

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD DA Serial API Get LR Nodes
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes(void)
{
  gucZWaveWorkbuf[0]  = 0; // BITMASK_OFFSET; currently 0 is the only realistic value
  ZWave_Enqueue_Request(FUNC_ID_SERIAL_API_GET_LR_NODES, gucZWaveWorkbuf, 1);
  LOG("%s: Sending FUNC_ID_SERIAL_API_GET_LR_NODES\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD DE Get DCDC Config
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_DE_Get_DCDC_Config(void)
{
  ZWave_Enqueue_Request(FUNC_ID_GET_DCDC_CONFIG, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_GET_DCDC_CONFIG\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_DE_Get_DCDC_Config

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD DF Set DCDC Config
  * @param  atDCDCMode
  * @retval None
  */
void ZWave_Send_REQ_CMD_DF_Set_DCDC_Config(sl_dcdc_config_t atDCDCMode)
{
  gucZWaveWorkbuf[0]  = (uint8_t)atDCDCMode;  // For now, hard-code the value for Auto
  ZWave_Enqueue_Request(FUNC_ID_SET_DCDC_CONFIG, gucZWaveWorkbuf, 1);
  LOG("%s: Sending FUNC_ID_SET_DCDC_CONFIG\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_DF_Set_DCDC_Config

/** *****************************************************************************************************************************
  * @brief  Prepare and send REQ CMD E8 Get Radio PTI
  * @param  None
  * @retval None
  */
void ZWave_Send_REQ_CMD_E8_Get_Radio_PTI(void)
{
  ZWave_Enqueue_Request(FUNC_ID_GET_RADIO_PTI, gucZWaveWorkbuf, 0);
  LOG("%s: Sending FUNC_ID_GET_RADIO_PTI\r\n", __FUNCTION__);
}
// end ZWave_Send_REQ_CMD_E8_Get_Radio_PTI

#endif // ENABLE_ZWAVE_CONTROLLER_HOST

/** *****************************************************************************************************************************
  * @brief  Z-Wave SerialAPI state machine
  * @param  stateMachineCommand - INITIALIZE, RUN or STATE
  * @retval Present state
  */
/***********************************************
ZWave_SerialAPI_StateMachine

  IF command is INITIALIZE
    Clear elapsed time
    Initialize subordinate state machines
    Set state to IDLE

  ELSE IF command is RUN
    Update elapsed time

    IF state is IDLE
      IF a frame has been received
        Set state to FRAME_PARSE
      ELSE IF any callback requests are pending
        Transmit request
        Set state to CALLBACK_TX_SERIAL
      ELSE IF any command requests are pending
        Transmit request
        Set state to COMMAND_TX_SERIAL
      ENDIF

    ELSE IF state is FRAME_PARSE
      Invoke the handler for the received command
      Set state to IDLE

    ELSE IF state is TX_SERIAL
      IF received a frame
        Invoke the handler for the received command
      ELSE IF the response was ACKed
        Reset retry count
        Set state to IDLE
      ELSE IF TX timeout
        Increment retry count
        IF retry count < maximum retry count
          Retransmit the response
        ELSE
          Reset retry count
          Set state to IDLE
        ENDIF
      ENDIF

    ELSE IF state is CALLBACK_TX_SERIAL
      IF received a frame
        Invoke the handler for the received command
      ELSE IF the callback was ACKed
        Pop the request from the callback queue
        Reset retry count
        Set state to IDLE
      ELSE IF TX timeout
        Increment retry count
        IF retry count < maximum retry count
          Retransmit the request
        ELSE
          Pop the request from the callback queue
          Reset retry count
          Set state to IDLE
        ENDIF
      ENDIF

    ELSE IF state is COMMAND_TX_SERIAL
      IF received a frame
        Invoke the handler for the received command
      ELSE IF the command was ACKed
        Pop the request from the command queue
        Reset retry count
        Set state to IDLE
      ELSE IF TX timeout
        Increment retry count
        IF retry count < maximum retry count
          Retransmit the request
        ELSE
          Pop the request from the command queue
          Reset retry count
          Set state to IDLE
        ENDIF
      ENDIF

    ELSE
      Flag illegal state
      Set state to IDLE

    ENDIF (state)

  ELSE IF command is STATE
    Do nothing (present state will be returned)

  ELSE
    Flag faulty state machine call
  ENDIF (command)

  Return present state

END ZWave_SerialAPI_StateMachine
************************************************/
ZWaveState ZWave_SerialAPI_StateMachine(ZWaveStateMachineCommand stateMachineCommand)
{
  static ZWaveState leZWaveState = ZWAVE_IDLE;
  static uint32_t lulElapsedTime_sec = 0;
  static uint8_t lucOldSecond = 100; // Nonsense initial value guarantees update when RTC first read
  ZWaveRxParseResult_t ltParseResult = ZWAVE_RX_PARSE_IDLE;

  //////////////////////////////////////////////////////////////////////////
  // IF command is INITIALIZE
  if (ZWAVE_SM_CMD_INITIALIZE == stateMachineCommand)
  {
    LOG("%s: initializing\r\n", __FUNCTION__);

    // Clear elapsed time
    lulElapsedTime_sec = 0;

    // Initialize subordinate state machines

    // Set state to IDLE
    LOG("%s: Transitioning from initialization to IDLE\r\n", __FUNCTION__);
    leZWaveState = ZWAVE_IDLE;
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE IF command is RUN
  else if (ZWAVE_SM_CMD_RUN == stateMachineCommand)
  {
    //  Update elapsed time
    // (update only when the RTC seconds update, i.e. update once per second)
    if (lucOldSecond != sMainRTCTime.Seconds)
    {
      lucOldSecond = sMainRTCTime.Seconds;

      ++lulElapsedTime_sec;
    }

    //-------------------------------------------------------
    // IF state is IDLE
    if (ZWAVE_IDLE == leZWaveState)
    {
      // IF a frame has been received
      if (ZWave_Parse_Rx_Data(true) == ZWAVE_RX_PARSE_FRAME_RECEIVED)
      {
        // Set state to FRAME_PARSE
        LOG("%s: Transitioning from IDLE to FRAME_PARSE\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_FRAME_PARSE;
      }
      // ELSE IF any callback requests are pending
      else if (gstructCallbackQueue.requestCnt)
      {
        // Transmit request
        ZWave_Transmit_Frame(
                             gstructCallbackQueue.requestQueue[gstructCallbackQueue.requestOut].wCmd,
                             REQUEST,
                             (uint8_t*)gstructCallbackQueue.requestQueue[gstructCallbackQueue.requestOut].wBuf,
                             gstructCallbackQueue.requestQueue[gstructCallbackQueue.requestOut].wLen
                            );

        // Set state to CALLBACK_TX_SERIAL
        LOG("%s: Transitioning from IDLE to CALLBACK_TX_SERIAL\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_CALLBACK_TX_SERIAL;
      }
      // ELSE IF any command requests are pending
      else if (gstructCommandQueue.requestCnt)
      {
        // Transmit request
        ZWave_Transmit_Frame(
                             gstructCommandQueue.requestQueue[gstructCommandQueue.requestOut].wCmd,
                             REQUEST,
                             (uint8_t*)gstructCommandQueue.requestQueue[gstructCommandQueue.requestOut].wBuf,
                             gstructCommandQueue.requestQueue[gstructCommandQueue.requestOut].wLen
                            );

        // Set state to COMMAND_TX_SERIAL
        LOG("%s: Transitioning from IDLE to COMMAND_TX_SERIAL\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_COMMAND_TX_SERIAL;
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is FRAME_PARSE
    else if (ZWAVE_FRAME_PARSE == leZWaveState)
    {
      // Invoke the handler for the received command
      //LOG("%s: Invoke the handler for the received frame (from the Z-Wave controller)...\r\n", __FUNCTION__);
      gtZWave_CMD_Handler[ZWaveSerialFrame->cmd]();

      // Set state to IDLE
      LOG("%s: Transitioning from FRAME_PARSE to IDLE\r\n", __FUNCTION__);
      leZWaveState = ZWAVE_IDLE;
    }

    //-------------------------------------------------------
    // ELSE IF state is TX_SERIAL
    else if (ZWAVE_TX_SERIAL == leZWaveState)
    {
      ////////////////////////////////////////////////////////////////////////////////////
      // MAB 2025.11.12
      // Note that this state is reached when there's a need for transmitting
      // an *immediate* RESPONSE to a received REQUEST. Since this state machine
      // typically deals with queued callback or command frames, and since this
      // state wasn't reached by a queued frame, there's no need to pop a frame
      // from a queue.
      // Will there ever be a need for this ZWave host to send an
      // immediate response to the ZWave controller?
      ////////////////////////////////////////////////////////////////////////////////////

      ltParseResult = ZWave_Parse_Rx_Data(TRUE);

      // IF received a frame
      if (ZWAVE_RX_PARSE_FRAME_RECEIVED == ltParseResult)
      {
        // Invoke the handler for the received command
        //LOG("%s: Invoke the handler for the received frame (from the Z-Wave controller)...\r\n", __FUNCTION__);
        gtZWave_CMD_Handler[ZWaveSerialFrame->cmd]();
      }
      // ELSE IF the response was ACKed
      else if (ZWAVE_RX_PARSE_FRAME_SENT == ltParseResult)
      {
        // Reset retry count
        gucRetryCount = 0;

        // Set state to IDLE
        LOG("%s: Transitioning from TX_SERIAL to IDLE\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_IDLE;
      }
      // ELSE IF TX timeout
      else if (ZWAVE_RX_PARSE_TX_TIMEOUT == ltParseResult)
      {
        // Increment retry count
        ++gucRetryCount;

        // IF retry count < maximum retry count
        if (gucRetryCount < MAX_SERIAL_RETRY)
        {
          // Retransmit the response
          LOG("%s: RETRANSMITTING response...\r\n", __FUNCTION__);
          ZWave_Transmit_Frame(0, RESPONSE, NULL, 0);
        }
        // ELSE
        else
        {
          // Reset retry count
          gucRetryCount = 0;

          // Set state to IDLE
          LOG("%s: Transitioning from TX_SERIAL to IDLE\r\n", __FUNCTION__);
          leZWaveState = ZWAVE_IDLE;
        }
        // ENDIF
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is CALLBACK_TX_SERIAL
    else if (ZWAVE_CALLBACK_TX_SERIAL == leZWaveState)
    {
      ltParseResult = ZWave_Parse_Rx_Data(TRUE);

      // IF received a frame
      if (ZWAVE_RX_PARSE_FRAME_RECEIVED == ltParseResult)
      {
        // Invoke the handler for the received command
        //LOG("%s: Invoke the handler for the received frame (from the Z-Wave controller)...\r\n", __FUNCTION__);
        gtZWave_CMD_Handler[ZWaveSerialFrame->cmd]();
      }
      // ELSE IF the callback was ACKed
      else if (ZWAVE_RX_PARSE_FRAME_SENT == ltParseResult)
      {
        // Pop the request from the callback queue
        ZWave_Pop_Callback_Queue();

        // Reset retry count
        gucRetryCount = 0;

        // Set state to IDLE
        LOG("%s: Transitioning from CALLBACK_TX_SERIAL to IDLE\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_IDLE;
      }
      // ELSE IF TX timeout
      else if (ZWAVE_RX_PARSE_TX_TIMEOUT == ltParseResult)
      {
        // Increment retry count
        ++gucRetryCount;

        // IF retry count < maximum retry count
        if (gucRetryCount < MAX_SERIAL_RETRY)
        {
          // Retransmit the request
          LOG("%s: RETRANSMITTING request...\r\n", __FUNCTION__);
          ZWave_Transmit_Frame(0, REQUEST, NULL, 0);
        }
        // ELSE
        else
        {
          LOG("%s: *** WARNING *** too many retries; popping the request from the callback queue\r\n", __FUNCTION__);
          // Pop the request from the callback queue
          ZWave_Pop_Callback_Queue();

          // Reset retry count
          gucRetryCount = 0;

          // Set state to IDLE
          LOG("%s: Transitioning from CALLBACK_TX_SERIAL to IDLE\r\n", __FUNCTION__);
          leZWaveState = ZWAVE_IDLE;
        }
        // ENDIF
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is COMMAND_TX_SERIAL
    else if (ZWAVE_COMMAND_TX_SERIAL == leZWaveState)
    {
      ltParseResult = ZWave_Parse_Rx_Data(TRUE);

      // IF received a frame
      if (ZWAVE_RX_PARSE_FRAME_RECEIVED == ltParseResult)
      {
        // Invoke the handler for the received command
        //LOG("%s: Invoke the handler for the received frame (from the Z-Wave controller)...\r\n", __FUNCTION__);
        gtZWave_CMD_Handler[ZWaveSerialFrame->cmd]();
      }
      // ELSE IF the command was ACKed
      else if (ZWAVE_RX_PARSE_FRAME_SENT == ltParseResult)
      {
        // Pop the request from the command queue
        ZWave_Pop_Command_Queue();

        // Reset retry count
        gucRetryCount = 0;

        // Set state to IDLE
        LOG("%s: Transitioning from COMMAND_TX_SERIAL to IDLE\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_IDLE;
      }
     // ELSE IF TX timeout
      else if (ZWAVE_RX_PARSE_TX_TIMEOUT == ltParseResult)
      {
        // Increment retry count
        ++gucRetryCount;

        // IF retry count < maximum retry count
        if (gucRetryCount < MAX_SERIAL_RETRY)
        {
          // Retransmit the request
          LOG("%s: RETRANSMITTING request...\r\n", __FUNCTION__);
          ZWave_Transmit_Frame(0, REQUEST, NULL, 0);
        }
        // ELSE
        else
        {
          LOG("%s: *** WARNING *** too many retries; popping the request from the command queue\r\n", __FUNCTION__);
          // Pop the request from the command queue
          ZWave_Pop_Command_Queue();

          // Reset retry count
          gucRetryCount = 0;

          // Set state to IDLE
          LOG("%s: Transitioning from COMMAND_TX_SERIAL to IDLE\r\n", __FUNCTION__);
          leZWaveState = ZWAVE_IDLE;
        }
        // ENDIF
      }
     // ENDIF
    }

    //-------------------------------------------------------
    // ELSE
    else
    {
      // Flag illegal state
      LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
      LOG("\r\n%s: *** ERROR *** Invalid state: leZWaveState = %d\r\n", __FUNCTION__, leZWaveState);
      LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");

      // Set state to IDLE
      LOG("%s: Transitioning from invalid state to IDLE\r\n", __FUNCTION__);
      leZWaveState = ZWAVE_IDLE;
   }

    // ENDIF (state)
    //-------------------------------------------------------
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE IF command is STATE
  else if (ZWAVE_SM_CMD_STATE == stateMachineCommand)
  {
    // Do nothing (present state will be returned)
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE
  else
  {
    // Flag faulty state machine call
    LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    LOG("\r\n%s: *** ERROR *** Invalid state machine command: stateMachineCommand = %d\r\n", __FUNCTION__, stateMachineCommand);
    LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
  }
  // ENDIF (command)
  //////////////////////////////////////////////////////////////////////////

  // Return present state
  return leZWaveState;
}
// end ZWave_SerialAPI_StateMachine

/** *****************************************************************************************************************************
  * @brief  Create a random SessionID in the range [1, 255]
  * @param  None
  * @retval Random SessionID in range [1, 255]
  */
uint8_t ZWave_SessionID_Randomize(void)
{
  uint8_t lucSessionID = 0;

  while (0 == lucSessionID)
  {
    lucSessionID = RandomValue() % 256;
  }

  return lucSessionID;
}
// end ZWave_SessionID_Randomize

/** *****************************************************************************************************************************
  * @brief  Update a SessionID in the range [1, 255]
  * @param  uint8_t aucSessionID - current SessionID value
  * @retval Updated SessionID in range [1, 255]
  */
uint8_t ZWave_SessionID_Update(uint8_t aucSessionID)
{
  uint8_t lucSessionID = aucSessionID;

  do
  {
    ++lucSessionID;
  } while (0 == lucSessionID);

  return lucSessionID;
}
// end ZWave_SessionID_Update

/** *****************************************************************************************************************************
  * @brief  SmartStart state machine
  * @param  stateMachineCommand - INITIALIZE, RUN or STATE
  * @retval Present state
  */
/***********************************************
ZWave_SmartStart_StateMachine

  IF command is INITIALIZE
    Clear elapsed time
    Initialize subordinate state machines
    Set state to EMPTY

  ELSE IF command is RUN
    Update elapsed time
    IF a DSK is being processed
      Set state to the processing DSK state
    ELSE
      Set state to EMPTY
    ENDIF

    IF state is EMPTY
      Do nothing
    ELSE IF state is READY
      Do nothing
    ELSE IF state is DETECTED
      Start node inclusion for the active DSK
      Set state to INCLUSION
    ELSE IF state is INCLUSION
      Update elapsed inclusion time
      IF node inclusion failed OR timed out
        Remove this specific node from the network
        Set state to EXCLUSION
      ELSE IF node inclusion has completed
        Stop node inclusion
        Set state to BOOTSTRAP
      ENDIF
    ELSE IF state is EXCLUSION
      Update elapsed exclusion time
      IF node exclusion completed, failed OR timed out
        Resume listening for SmartStart Prime commands, report to host application
        Set state to READY
      ENDIf
    ELSE IF state is BOOTSTRAP
      Update elapsed bootstrap time
      Run Bootstrap state machine
      IF S2 bootstrap failed OR timed out
        Set state to INCLUSION
      ELSE IF S2 bootstrap has completed
        Resume listening for SmartStart Prime commands, report to host application
        Set state to ACTIVE
      ENDIF
    ELSE IF state is ACTIVE
      IF active DSK has been removed
        Set state to REMOVED
      ENDIF
    ELSE IF state is REMOVED
      Zeroize active DSK
      Set state to EMPTY
    ENDIF

  ELSE IF command is STATE
    Do nothing (present state will be returned)

  ELSE
    Flag faulty state machine call
  ENDIF (command)

  Return present state

END ZWave_SmartStart_StateMachine
************************************************/
SmartStartState ZWave_SmartStart_StateMachine(SmartStartStateMachineCommand stateMachineCommand)
{
  static SmartStartState leSmartStartState = SMARTSTART_EMPTY;
  static uint32_t lulElapsedTime_sec = 0;
  static uint8_t lucOldSecond = 100; // Nonsense initial value guarantees update when RTC first read
  static uint32_t lulElapsedTime_Inclusion_msec;
  static uint32_t lulElapsedTime_Exclusion_msec;
  static uint32_t lulElapsedTime_Bootstrap_msec;
  #define INCLUSION_TIMEOUT_MSEC (30000)
  #define EXCLUSION_TIMEOUT_MSEC (30000)
  #define BOOTSTRAP_TIMEOUT_MSEC (120000)
  static uint8_t lucNWIAuthHomeIDBuffer[8];

  //////////////////////////////////////////////////////////////////////////
  // IF command is INITIALIZE
  if (SMARTSTART_SM_CMD_INITIALIZE == stateMachineCommand)
  {
    LOG("%s: initializing\r\n", __FUNCTION__);
    // Clear elapsed time
    lulElapsedTime_sec = 0;

    // Initialize subordinate state machines

    // Set state to EMPTY
    LOG("%s: Transitioning from initialization to EMPTY\r\n", __FUNCTION__);
    leSmartStartState = SMARTSTART_EMPTY;
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE IF command is RUN
  else if (SMARTSTART_SM_CMD_RUN == stateMachineCommand)
  {
    // Update elapsed time
    // (update only when the RTC seconds update, i.e. update once per second)
    if (lucOldSecond != sMainRTCTime.Seconds)
    {
      lucOldSecond = sMainRTCTime.Seconds;

      ++lulElapsedTime_sec;
    }

    // IF a DSK is being processed
    if (ZWave_DSK_IsProcessing())
    {
      // Set state to the processing DSK state
      leSmartStartState = gtNodeProvisioningList[gucProcessingDSK].status;
    }
    // ELSE
    else
    {
      // Set state to EMPTY
      leSmartStartState = SMARTSTART_EMPTY;
    }
    // ENDIF


    //-------------------------------------------------------
    // IF state is EMPTY
    if (SMARTSTART_EMPTY == leSmartStartState)
    {
      // Do nothing
    }

    //-------------------------------------------------------
    // ELSE IF state is READY
    else if (SMARTSTART_READY == leSmartStartState)
    {
      // Do nothing
    }

    //-------------------------------------------------------
    // ELSE IF state is DETECTED
    else if (SMARTSTART_DETECTED == leSmartStartState)
    {
      // Start node inclusion for the active DSK
      #if ENABLE_ZWAVE_CONTROLLER_HOST
      LOG("%s: Starting node inclusion for DSK %d \r\n", __FUNCTION__, gucProcessingDSK);
      ZWave_DSK_Extract_NWIAuthHomeID(gucProcessingDSK, lucNWIAuthHomeIDBuffer);
      gucSessionID = ZWave_SessionID_Update(gucSessionID);
      ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_OPTION_LR|ADD_NODE_HOME_ID, gucSessionID, lucNWIAuthHomeIDBuffer);
      #endif // ENABLE_ZWAVE_CONTROLLER_HOST

      // Set state to INCLUSION
      lulElapsedTime_Inclusion_msec = 0;
      gucIsInclusionFailed                = FALSE;
      gucIsInclusionJoiningNodeFinished   = FALSE;
      gucIsInclusionIncludingNodeFinished = FALSE;
      LOG("%s: Transitioning DSK %d from DETECTED to INCLUSION\r\n", __FUNCTION__, gucProcessingDSK);
      leSmartStartState                               = SMARTSTART_INCLUSION;
      gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_INCLUSION;
    }

    //-------------------------------------------------------
    // ELSE IF state is INCLUSION
    else if (SMARTSTART_INCLUSION == leSmartStartState)
    {
      // Update elapsed inclusion time
      lulElapsedTime_Inclusion_msec += ZWAVE_TASK_PERIOD;

      // IF node inclusion failed OR timed out
      if ( gucIsInclusionFailed || lulElapsedTime_Inclusion_msec > INCLUSION_TIMEOUT_MSEC)
      {
        if (gucIsInclusionFailed)
        {
          LOG("%s: *** WARNING *** Inclusion for DSK %d failed \r\n", __FUNCTION__, gucProcessingDSK);
        }
        if (lulElapsedTime_Inclusion_msec > INCLUSION_TIMEOUT_MSEC)
        {
          LOG("%s: *** WARNING *** Inclusion for DSK %d timed out \r\n", __FUNCTION__, gucProcessingDSK);
        }

        #if ENABLE_ZWAVE_CONTROLLER_HOST
        // Remove this specific node from the network
        gucSessionID = ZWave_SessionID_Update(gucSessionID);
        LOG("%s: Will attempt to delete DSK %d NodeID=0x%04X from network... \r\n", __FUNCTION__, gucProcessingDSK, gtNodeProvisioningList[gucProcessingDSK].NodeID);
        // By erasing the NodeID from the provisioning list, the NodeID will be deleted
        // from the active node list by the periodic "node clean-up" function in the ZWave task
        gtNodeProvisioningList[gucProcessingDSK].NodeID = NODE_ID_UNAVAILABLE;
        #endif

        // Set state to EXCLUSION
        LOG("%s: Transitioning DSK %d from INCLUSION to EXCLUSION\r\n", __FUNCTION__, gucProcessingDSK);
        gucIsExclusionFinished = FALSE;
        gucIsExclusionFailed   = FALSE;
        leSmartStartState                               = SMARTSTART_EXCLUSION;
        gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_EXCLUSION;
      }
      // ELSE IF joining node inclusion has completed
      else if (gucIsInclusionJoiningNodeFinished)
      {
        LOG("%s: Joining node inclusion for DSK %d completed in %d msec \r\n", __FUNCTION__, gucProcessingDSK, lulElapsedTime_Inclusion_msec);
        gucIsInclusionJoiningNodeFinished = FALSE;
        lulElapsedTime_Inclusion_msec = 0;

        #if ENABLE_ZWAVE_CONTROLLER_HOST
        // Stop joining node inclusion
        //////////////////////////////////////
        /// TEST MAB 2026.01.23
        //osDelay(5000);
        //////////////////////////////////////
        LOG("%s: Stopping joining node inclusion \r\n", __FUNCTION__);
        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_STOP, gucSessionID, NULL);
        #endif
      }
      // ELSE IF including node inclusion has completed
      else if (gucIsInclusionIncludingNodeFinished)
      {
        LOG("%s: Including node inclusion for DSK %d completed in %d msec \r\n", __FUNCTION__, gucProcessingDSK, lulElapsedTime_Inclusion_msec);

        #if ENABLE_ZWAVE_CONTROLLER_HOST
        // Stop node inclusion
        //////////////////////////////////////
        /// TEST MAB 2026.01.22
        //osDelay(5000);
        //////////////////////////////////////
        LOG("%s: Stopping node inclusion \r\n", __FUNCTION__);
        //ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_STOP, gucSessionID, NULL);
        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_STOP, 0, NULL);
        #endif

        // Set state to BOOTSTRAP
        lulElapsedTime_Bootstrap_msec = 0;
        gucIsBootstrapFailed   = FALSE;
        gucIsBootstrapFinished = FALSE;
        ZWave_Bootstrap_StateMachine(BOOTSTRAP_SM_CMD_INITIALIZE);
        LOG("%s: Transitioning DSK %d from INCLUSION to BOOTSTRAP\r\n", __FUNCTION__, gucProcessingDSK);
        leSmartStartState                               = SMARTSTART_BOOTSTRAP;
        gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_BOOTSTRAP;
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is EXCLUSION
    else if (SMARTSTART_EXCLUSION == leSmartStartState)
    {
      // Update elapsed exclusion time
      lulElapsedTime_Exclusion_msec += ZWAVE_TASK_PERIOD;

      // IF node exclusion completed, failed OR timed out
      if (gucIsExclusionFinished || gucIsExclusionFailed || lulElapsedTime_Exclusion_msec > EXCLUSION_TIMEOUT_MSEC)
      {
        #if ENABLE_ZWAVE_CONTROLLER_HOST
        // Resume listening for SmartStart Prime commands, report to host application
        LOG("%s: Resume listening for SmartStart Prime commands, report to host application \r\n", __FUNCTION__);
        gucSessionID = ZWave_SessionID_Update(gucSessionID);
        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_SMART_START, gucSessionID, NULL);
        //ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_OPTION_LR|ADD_NODE_SMART_START, gucSessionID, NULL);
        #endif

        // Set state to READY
        LOG("%s: Transitioning DSK %d from EXCLUSION to READY\r\n", __FUNCTION__, gucProcessingDSK);
        leSmartStartState                               = SMARTSTART_READY;
        gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_READY;
        gucProcessingDSK = DSK_UNAVAILABLE;
      }
      // ENDIf
    }

    //-------------------------------------------------------
    // ELSE IF state is BOOTSTRAP
    else if (SMARTSTART_BOOTSTRAP == leSmartStartState)
    {
      // Update elapsed bootstrap time
      lulElapsedTime_Bootstrap_msec += ZWAVE_TASK_PERIOD;

      // Run Bootstrap state machine
      ZWave_Bootstrap_StateMachine(BOOTSTRAP_SM_CMD_RUN);

      // IF S2 bootstrap failed OR timed out
      if ( gucIsBootstrapFailed || lulElapsedTime_Bootstrap_msec > BOOTSTRAP_TIMEOUT_MSEC)
      {
        if (gucIsBootstrapFailed)
        {
          LOG("%s: *** WARNING *** S2 Bootstrap for DSK %d failed \r\n", __FUNCTION__, gucProcessingDSK);
        }
        if (lulElapsedTime_Bootstrap_msec > BOOTSTRAP_TIMEOUT_MSEC)
        {
          LOG("%s: *** WARNING *** S2 Bootstrap for DSK %d timed out \r\n", __FUNCTION__, gucProcessingDSK);
        }

        // Set state to INCLUSION
        lulElapsedTime_Inclusion_msec = 0;
        gucIsInclusionFailed                = FALSE;
        gucIsInclusionJoiningNodeFinished   = FALSE;
        gucIsInclusionIncludingNodeFinished = FALSE;
        LOG("%s: Transitioning DSK %d from BOOTSTRAP to INCLUSION\r\n", __FUNCTION__, gucProcessingDSK);
        leSmartStartState                               = SMARTSTART_INCLUSION;
        gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_INCLUSION;
//        ///////////////////////////////////////////////////////////////////////////////////////////////////
//        //// TEST MAB 2026.01.22
////        #if ENABLE_ZWAVE_CONTROLLER_HOST
////        // Stop node inclusion
////        LOG("%s: Stopping node inclusion \r\n", __FUNCTION__);
////        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_STOP, gucSessionID, NULL);
////        #endif
//
//        #if ENABLE_ZWAVE_CONTROLLER_HOST
//        // Resume listening for SmartStart Prime commands, report to host application
//        LOG("%s: Resume listening for SmartStart Prime commands, report to host application \r\n", __FUNCTION__);
//        gucSessionID = ZWave_SessionID_Update(gucSessionID);
//        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_SMART_START, gucSessionID, NULL);
//        //ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_OPTION_LR|ADD_NODE_SMART_START, gucSessionID, NULL);
//        #endif
//
//        // Set state to ACTIVE
//        LOG("%s: Transitioning DSK %d from BOOTSTRAP to ACTIVE\r\n", __FUNCTION__, gucProcessingDSK);
//        leSmartStartState                               = SMARTSTART_ACTIVE;
//        gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_ACTIVE;
//        ///////////////////////////////////////////////////////////////////////////////////////////////////
      }
      // ELSE IF S2 bootstrap has completed
      else if (gucIsBootstrapFinished)
      {
//        #if ENABLE_ZWAVE_CONTROLLER_HOST
//        // Stop node inclusion
//        LOG("%s: Stopping node inclusion \r\n", __FUNCTION__);
//        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_STOP, gucSessionID, NULL);
//        #endif

        #if ENABLE_ZWAVE_CONTROLLER_HOST
        // Resume listening for SmartStart Prime commands, report to host application
        LOG("%s: Resume listening for SmartStart Prime commands, report to host application \r\n", __FUNCTION__);
        gucSessionID = ZWave_SessionID_Update(gucSessionID);
        ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_SMART_START, gucSessionID, NULL);
        //ZWave_Send_REQ_CMD_4A_Add_Node_to_Network(ADD_NODE_OPTION_NETWORK_WIDE|ADD_NODE_OPTION_LR|ADD_NODE_SMART_START, gucSessionID, NULL);
        #endif

        // Set state to ACTIVE
        LOG("%s: Transitioning DSK %d from BOOTSTRAP to ACTIVE\r\n", __FUNCTION__, gucProcessingDSK);
        leSmartStartState                               = SMARTSTART_ACTIVE;
        gtNodeProvisioningList[gucProcessingDSK].status = SMARTSTART_ACTIVE;
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is ACTIVE
    else if (SMARTSTART_ACTIVE == leSmartStartState)
    {
    }

    //-------------------------------------------------------
    // ELSE IF state is REMOVED
    else if (SMARTSTART_REMOVED == leSmartStartState)
    {
    }

  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE IF command is STATE
  else if (SMARTSTART_SM_CMD_STATE == stateMachineCommand)
  {
    // Do nothing (present state will be returned)
  }

  //////////////////////////////////////////////////////////////////////////
  // ELSE
  else
  {
    // Flag faulty state machine call
    LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    LOG("\r\n%s: Invalid state machine command: stateMachineCommand = %d\r\n", __FUNCTION__, stateMachineCommand);
    LOG("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
  }
  // ENDIF (command)
  //////////////////////////////////////////////////////////////////////////

  // Return present state
  return leSmartStartState;
}
// end ZWave_SmartStart_StateMachine

/** *****************************************************************************************************************************
  * @brief  Generate temporary symmetric key from ECDH Shared Secret
  * @param  None; relies on global arrays containing public/private keys; output to gucTemporarySymmetricKey[]
  * @retval 0 if all OK; nonzero otherwise
  */
int ZWave_Temporary_Key_Generate(void)
{
  #define TEMP_KEY_LEN 16
  #define PRK_LEN      32   // SHA-256 output size
  #define SS_LEN       32   // X25519 shared secret size
  static int liReturnValue;
  static curve25519_key ltControllerPrivateKey, ltRemotePublicKey;
  static uint8_t lucECDHSharedSecret[SS_LEN];
  static unsigned int luiSharedLen;
  static uint8_t lucMsgExtract[32+32+32];
  static uint8_t lucPRK[16];
  static uint8_t lucMsgExpand[16];
  static unsigned int luiSize;

  ///////////////////////////////////////
  // Generate G, the ECDH Shared Secret
  ///////////////////////////////////////
  LOG("%s: Generating ECDH Shared Secret \r\n", __FUNCTION__);
  wc_curve25519_init(&ltControllerPrivateKey);
  wc_curve25519_init(&ltRemotePublicKey);
  LOG("%s: - importing controller private key \r\n", __FUNCTION__);
  liReturnValue = wc_curve25519_import_private_ex(gucControllerPrivateKey,
                                                      32,
                                                      &ltControllerPrivateKey,
                                                      EC25519_LITTLE_ENDIAN);
  if (liReturnValue != 0) goto exit;
  LOG("%s: - importing remote public key \r\n", __FUNCTION__);
  liReturnValue = wc_curve25519_import_public_ex(gtNodeProvisioningList[gucProcessingDSK].ECDHPublicKey,
                                                     32,
                                                     &ltRemotePublicKey,
                                                     EC25519_LITTLE_ENDIAN);
  if (liReturnValue != 0) goto exit;
  LOG("%s: - generating ECDH Shared Secret \r\n", __FUNCTION__);
  luiSharedLen = SS_LEN;
  liReturnValue = wc_curve25519_shared_secret_ex(&ltControllerPrivateKey,
                                                     &ltRemotePublicKey,
                                                     lucECDHSharedSecret,
                                                     &luiSharedLen,
                                                     EC25519_LITTLE_ENDIAN);
  if (liReturnValue != 0 || luiSharedLen != SS_LEN) goto exit;
  // If no errors, lucECDHSharedSecret[] has the ECDH Shared Secret
  //PrintBytes(lucECDHSharedSecret, sizeof(lucECDHSharedSecret), false, 0);


  /////////////////////////////////////////////////
  // TempExtract: PRK = CMAC(c33, G||PubA||PubB)
  /////////////////////////////////////////////////
  LOG("%s: Generating PRK \r\n", __FUNCTION__);
  memcpy(lucMsgExtract,    lucECDHSharedSecret,                                    32);
  memcpy(lucMsgExtract+32, gucControllerPublicKey,                                 32);
  memcpy(lucMsgExtract+64, gtNodeProvisioningList[gucProcessingDSK].ECDHPublicKey, 32);
  luiSize = sizeof(lucPRK);
  liReturnValue = wc_AesCmacGenerate(lucPRK, &luiSize,
                                     lucMsgExtract, sizeof(lucMsgExtract),
                                     CKDF_TEMP_EXTRACT_KEY_C, sizeof(CKDF_TEMP_EXTRACT_KEY_C));
  if (liReturnValue != 0 || luiSize != 16)
  {
    // Make sure return value is *something* other than 0
    if (0==liReturnValue) liReturnValue = -1;
    goto exit;
  }
  // If no errors, lucPRK[] has the PRK
  //PrintBytes(lucPRK, sizeof(lucPRK), false, 0);


  /////////////////////////////////////////////////
  // TempExpand: TempKeyCCM = CMAC(PRK, c88||0x01)
  /////////////////////////////////////////////////
  LOG("%s: Generating Temporary Symmetric Key \r\n", __FUNCTION__);
  memcpy(lucMsgExpand, CKDF_TEMP_EXPAND_C, 15);
  lucMsgExpand[15] = 0x01;
  luiSize = 16;
  liReturnValue = wc_AesCmacGenerate(gucTemporarySymmetricKey, &luiSize,
                                       lucMsgExpand, sizeof(lucMsgExpand),
                                       lucPRK, sizeof(lucPRK));
  // gucTemporarySymmetricKey[] has the Temporary Symmetric Key
  //PrintBytes(gucTemporarySymmetricKey, sizeof(gucTemporarySymmetricKey), false, 0);


exit:
  wc_curve25519_free(&ltRemotePublicKey);
  wc_curve25519_free(&ltControllerPrivateKey);

  if (liReturnValue != 0) LOG("%s: *** WARNING *** return value = %d \r\n", __FUNCTION__, liReturnValue);
  return liReturnValue;
}
// end ZWave_Temporary_Key_Generate

/** *****************************************************************************************************************************
  * @brief  Transit a Z-Wave frame
  * @param  uint8_t  aucCMD      - Z-Wave command
  * @param  uint8_t  aucType     - 0=REQUEST 1=RESPONSE
  * @param  uint8_t* paucPayload - pointer to data buffer
  * @param  uint8_t  aucLength   - length of data buffer in bytes (includes length, type and command bytes plus payload but not SOF nor checksum bytes)
  * @retval None
  */
void ZWave_Transmit_Frame(uint8_t aucCMD, uint8_t aucType, const uint8_t* paucPayload, uint8_t aucLength)
{
  static ZWaveTxFrame_t ltFrame =
  {
      .sof=SOF
  };
  static uint8_t lucCMD;
  static uint8_t lucType;
  static uint8_t lucLength;
  static uint8_t lucChecksum;
  static const uint8_t* plucPayload;

  // Stop ACK, byte and buffer check timers
  gtZWaveRxInterface.ack_timeout_ms = 0; // stop the timer
  gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer

  // Disable ACK and byte timeouts
  gtZWaveRxInterface.ack_timeout = false;
  gtZWaveRxInterface.byte_timeout = false;

  // IF a payload is present
  if (paucPayload != NULL)
  {
    // Set up a new frame to transmit
    ltFrame.sof = SOF;
    ltFrame.len = aucLength+3;
    ltFrame.type = aucType;
    ltFrame.cmd = aucCMD;
    memcpy(ltFrame.payload, paucPayload, aucLength);
    ltFrame.payload[aucLength] = ZWave_XOR_Checksum(0xFF, &ltFrame.len, ltFrame.len);

    // Save the new frame for possible retransmission
    lucLength = aucLength;
    lucType = aucType;
    lucCMD = aucCMD;
    plucPayload = paucPayload;
    lucChecksum = ltFrame.payload[aucLength];
  }
  // ELSE
  else
  {
    // Retransmit the previous frame
    ltFrame.sof = SOF;
    ltFrame.len = lucLength + 3;
    ltFrame.type = lucType;
    ltFrame.cmd = lucCMD;
    if (plucPayload)
    {
      memcpy(ltFrame.payload, plucPayload, lucLength);
    }
    ltFrame.payload[lucLength] = lucChecksum;
  }
  // ENDIF

  // Enable ACK needed
  gtZWaveRxInterface.ack_needed = TRUE;

  // Set expected bytes to ACK

  // Transmit the frame
  HAL_UART_Transmit(&huart2, (uint8_t *)&ltFrame, ltFrame.len + 2, 100);
  LOG("%s: Transmitting to Z-Wave controller\r\n", __FUNCTION__);
  LOG("******************** Transmitting to Z-Wave controller START ******************** \r\n");
  PrintBytes((uint8_t *)&ltFrame, ltFrame.len + 2, false, 0);
  LOG("******************** Transmitting to Z-Wave controller  END  ******************** \r\n");

  // Start the ACK and buffer check timers
  gtZWaveRxInterface.ack_timeout_ms = 1; // start the timer

  //////////////////////////////////////////////////////////////////////////////////////////////
  //// MAB 2026.01.27
  //// Perform any command-specific housekeeping on frame transmit
  if (FUNC_ID_NVR_GET_VALUE==aucCMD)
  {
    // Save NVR offset
    //LOG("%s: aucLength = 0x%02X \r\n", __FUNCTION__, aucLength);
    //LOG("%s: Saving NVR offset 0x%02X \r\n", __FUNCTION__, paucPayload[0]);
    gucNVROffset = paucPayload[0];
  }
  //////////////////////////////////////////////////////////////////////////////////////////////
}
// end ZWave_Transmit_Frame

/** *****************************************************************************************************************************
  * @brief  Calculate Z-Wave checksum
  * @param  uint8_t  aucInitialValue - initial checksum value (for a Z-Wave frame, must be 0xFF)
  * @param  uint8_t* paucDataBuffer  - pointer to data buffer over which the checksum is calculated
  * @param  uint8_t  aucLength       - length of data buffer in bytes
  * @retval uint8_t checksum value
  */
uint8_t ZWave_XOR_Checksum(uint8_t aucInitialValue, const uint8_t *paucDataBuffer, uint8_t aucLength)
{
  uint8_t lucChecksum = aucInitialValue;

  for (int i = 0; i < aucLength; ++i)
  {
    lucChecksum ^= paucDataBuffer[i];
  }

  return lucChecksum;
}
// end ZWave_XOR_Checksum

/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */
/* ***************************************************************************************************************************** */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_MainTask */
/** *****************************************************************************************************************************
  * @brief  Function implementing the Main thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MainTask */
void MainTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  static uint8_t lucOldSecond = 100; // Nonsense initial value guarantees update when RTC first read
  static uint8_t lucOldMinute = 100; // Nonsense initial value guarantees update when RTC first read
  static uint8_t lucOldHour   = 100; // Nonsense initial value guarantees update when RTC first read
  static uint16_t luiMessageQueueBuffer;
  static uint16_t luiCurrentZWaveZone;
  static uint16_t luiDiagnosticRxCount = 0;
  static HAL_StatusTypeDef ltHALStatus;

  PrintStartupBanner();
  LOG("%s: initializing...\r\n", __FUNCTION__);

  // Disable Diagnostic serial interrupt
  //__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
  // Disable Z-Wave controller serial interrupt
  //__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);

  /////////////////////////////////////
  // Initialize RTC
  /////////////////////////////////////
  HAL_RTC_GetTime(&hrtc, &sMainRTCTime, RTC_FORMAT_BIN);
  // workaround for HAL_RTC_GetTime() failure to update: read date immediately afterwards
  HAL_RTC_GetDate(&hrtc, &sMainRTCDate, RTC_FORMAT_BIN);
  lucOldMinute = sMainRTCTime.Minutes;
  lucOldHour = sMainRTCTime.Hours;
  // Display RTC
  LOG("%s: RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC\r\n",
      __FUNCTION__,
      sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
      sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds);

  /////////////////////////////////////////////////////////
  // Arm the Diagnostic interrupt for the first byte
  /////////////////////////////////////////////////////////
  ltHALStatus = HAL_UART_Receive_IT(&huart1, &gucReceivedByteFromDiagnostic, 1);
  //__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  if (ltHALStatus != HAL_OK)
  {
    LOG("%s: *** WARNING *** HAL_UART_Receive_IT(&huart1) (for Diagnostic) returned 0x%02X\r\n", __FUNCTION__, ltHALStatus);
  }
  else
  {
    //LOG("%s: HAL_UART_Receive_IT(&huart1) (for Diagnostic) returned HAL_OK\r\n", __FUNCTION__);
  }

  /////////////////////////////////////////////////////////
  // Initialize wolfSSL
  /////////////////////////////////////////////////////////
  // For SSL/TLS projects
  //wolfSSL_Init();
  // For wolfCrypt-only projects
  wolfCrypt_Init();
  LOG("%s: wolfSSL initialized \r\n", __FUNCTION__);

//  /////////////////////////////////////////////////////////
//  // TEST MAB 2025.12.26
//  // Test the random number generator
//  // TEST MAB 2026.01.28
//  // Test the wolfSSL random number generator
//
//  uint16_t luiRandomValue;
//  uint16_t luiRandomBinCount[16];
//  uint8_t lucRandomBinIndex;
//  uint8_t lucRandomValues[256];
//
//  LOG("%s: Testing random number generator...\r\n", __FUNCTION__);
//
//  // Initialize wolfSSL random number generator
//  wc_InitRng(&gtWolfSSLRng);
//
//  // Initialize random bin counters
//  memset(luiRandomBinCount, 0x00, sizeof(luiRandomBinCount));
//
//  // Generate N random numbers
////  for (int i = 0; i < 20; ++i)
////  {
////    for (int j = 0; j < 10; ++j)
////    {
////      luiRandomValue = RandomValue() % 0xFFFF;
////      LOG("0x%04X \t", luiRandomValue);
////      lucRandomBinIndex = luiRandomValue / 0x1000;
////      ++luiRandomBinCount[lucRandomBinIndex];
////    }
////    LOG("\r\n");
////  }
//  wc_RNG_GenerateBlock(&gtWolfSSLRng, lucRandomValues, sizeof(lucRandomValues));
//  wc_FreeRng(&gtWolfSSLRng);
//  PrintBytes(lucRandomValues, sizeof(lucRandomValues), FALSE, 0);
//
//  // Determine bin counts
//  for (uint16_t i = 0; i < sizeof(lucRandomValues); ++i)
//  {
//    lucRandomBinIndex = lucRandomValues[i] / 0x10;
//    ++luiRandomBinCount[lucRandomBinIndex];
//  }
//
//  // Display bin counts
//  LOG("%s: Random bin counts...\r\n", __FUNCTION__);
//  for (int i = 0; i < 16; ++i)
//  {
//    if (i == 8) LOG("     ");
//    LOG("0x%02X ", luiRandomBinCount[i]);
//  }
//  LOG("\r\n");
//  /////////////////////////////////////////////////////////

  /* ************************************************** Infinite loop ************************************************** */
  for(;;)
  {
    //
    // Fetch RTC time
    //
    HAL_RTC_GetTime(&hrtc, &sMainRTCTime, RTC_FORMAT_BIN);
    // workaround for HAL_RTC_GetTime() failure to update: read date immediately afterwards
    HAL_RTC_GetDate(&hrtc, &sMainRTCDate, RTC_FORMAT_BIN);
    ///////////////////////////////
    // updates on seconds
    if (lucOldSecond != sMainRTCTime.Seconds)
    {
      lucOldSecond = sMainRTCTime.Seconds;

      // Get UNIX timestamp of present RTC
      sUNIXTime = ConvertTimeToUNIX(sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
            sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds);

      // Check if UNIX timestamp is updating every second
      if (0 != sOldUNIXTime && (sUNIXTime-sOldUNIXTime) > 1)
      {
        LOG("%s: *** WARNING *** UNIX timestamp: %d incremented by %d\r\n", __FUNCTION__, sUNIXTime, (sUNIXTime-sOldUNIXTime));
      }
//      if (0 != sOldUNIXTime && (sUNIXTime-sOldUNIXTime) == 1)
//      {
//        LOG("%s:                 UNIX timestamp: %d\r\n", __FUNCTION__, sUNIXTime);
//      }
      sOldUNIXTime = sUNIXTime;

      // Display time
//      LOG("%s: RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC \tSensaphone timestamps: %d  %d\r\n",
//          __FUNCTION__,
//          sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
//          sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds,
//    rtc_calculate_encoded_date(sMainRTCTime.Seconds, sMainRTCTime.Minutes, sMainRTCTime.Hours,
//             sMainRTCDate.Date,    sMainRTCDate.Month,   sMainRTCDate.Year),
//    rtc_get_encoded_date() );
//      LOG("%s: RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC\r\n",
//          __FUNCTION__,
//          sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
//          sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds);
//      LOG("%s: RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC \t UNIX is %d\r\n",
//          __FUNCTION__,
//          sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
//          sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds,
//          (uint_32)sUNIXTime);

      // Run delayed board reset state machine
      //Delay_Reset_Board_state_machine(RESET_BOARD_SM_CMD_RUN);

//      // Monitor if acked and unacked alarms are present
//      // Inform Output task of changes
//      // (It shouldn't happen often, but Output task should accommodate the possibility
//      //  of unacked alarms present BUT no acked alarms present.)
//      // Do NOT include Alarm On Return alarms for blinking/flickering Alarm LED
//      lucIsAlarmPresent = Input_AlarmExists();
//      if (lucOldIsAlarmPresent != lucIsAlarmPresent)
//      {
//        lucOldIsAlarmPresent = lucIsAlarmPresent;
//        if (lucIsAlarmPresent)
//        {
//          // Inform Output task an alarm exists
//          osMessagePut(OutputQueueHandle, msgid_MAIN_OUTPUT_ALARM_EXISTS, 1000);
//        }
//        else
//        {
//          // Inform Output task there are no (acknowledged) alarms
//          osMessagePut(OutputQueueHandle, msgid_MAIN_OUTPUT_NO_ALARM, 1000);
//        }
//      }
//      lucIsUnackedAlarmPresent = Input_UnackedAlarmExists();
//      if (lucOldIsUnackedAlarmPresent != lucIsUnackedAlarmPresent)
//      {
//        lucOldIsUnackedAlarmPresent = lucIsUnackedAlarmPresent;
//        if (lucIsUnackedAlarmPresent)
//        {
//          // Inform Output task an unacked alarm exists
//          osMessagePut(OutputQueueHandle, msgid_MAIN_OUTPUT_UNACKED_ALARM_EXISTS, 1000);
//        }
//        else
//        {
//          // Inform Output task there are no unacknowledged alarms
//          osMessagePut(OutputQueueHandle, msgid_MAIN_OUTPUT_NO_UNACKED_ALARM, 1000);
//        }
//      }

      // Elapsed runtime (since startup)
      ++gulElapsedTime_Runtime_sec;

      //////////////////////////////////////////////////////////////////////////////////
      //// TEST MAB 2025.09.29
      //// Test of the CMSIS v2 message queuing, and data sent from Main to TouchGFX
      if (gulElapsedTime_Runtime_sec%15 == 0)
      {
        luiMessageQueueBuffer = msgid_MAIN_PRINT_ELAPSED_TIME;
        osMessageQueuePut(MainQueueHandle, &luiMessageQueueBuffer, 0, 1);
      }
      if (gulElapsedTime_Runtime_sec%5 == 0)
      {
        ++luiCurrentZWaveZone;
        if (luiCurrentZWaveZone >50) luiCurrentZWaveZone = 1;  // limit zones to [1, 50]
        luiMessageQueueBuffer = msgid_MAIN_TOUCHGFX_SET_CURRENT_ZONE;
        osMessageQueuePut(TouchGFXQueueHandle, &luiMessageQueueBuffer, 0, 1);
        osMessageQueuePut(TouchGFXQueueHandle, &luiCurrentZWaveZone, 0, 1);
      }
      //////////////////////////////////////////////////////////////////////////////////

    } // updates on seconds
    ///////////////////////////////
    // updates on minutes
    if (lucOldMinute != sMainRTCTime.Minutes)
    {
      lucOldMinute = sMainRTCTime.Minutes;

//      LOG("%s: ............... RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC ...............\r\n",
//          __FUNCTION__,
//          sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
//          sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds);
      LOG("%s: ............... RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC; runtime %d seconds ...............\r\n",
          __FUNCTION__,
          sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
          sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds,
          gulElapsedTime_Runtime_sec);

//      // Display UNTIMED Standby state if active
//      luiStandbyState = getSystemStandbyState();
//      if ((uint16)STANDBY_NO_COUNTDOWN==luiStandbyState)
//      {
//        LOGV1("%s: Standby State: UNTIMED\r\n", __FUNCTION__);
//      }

      // Update Standby if active
      if (eSTANDBY_CONFIG==gucStandbyMode && gulStandbyCounter>0)
      {
        --gulStandbyCounter;
        LOG("%s: Standby countdown at %d minutes\r\n", __FUNCTION__, (int)gulStandbyCounter);
        if (0==gulStandbyCounter)
        {
          gucStandbyMode = eSTANDBY_OFF;
          LOG("%s: *** WARNING *** Standby mode OFF, readings and alarms active again\r\n", __FUNCTION__);
        }
      }

//      ///////////////////////////////////////////////////////////////////////////
//      //// TEST MAB 2025.10.14
//      //// To test ZWave_Receive_Response(), stuff the ZWave Rx FIFO
//      static uint8_t lucSimulatedZWaveRxByte;
//      LOG("%s: *** WARNING *** Simulating ZWave receive bytes...\r\n", __FUNCTION__);
//      for (int i = 0; i < 16; ++i)
//      {
//        lucSimulatedZWaveRxByte = i;
//        osMessageQueuePut(ZWaveRxQueueHandle, &lucSimulatedZWaveRxByte, 0, 1);
//      }
//      ///////////////////////////////////////////////////////////////////////////

    } // updates on minutes
    ///////////////////////////////
    // updates on hours
    if (lucOldHour != sMainRTCTime.Hours)
    {
      lucOldHour = sMainRTCTime.Hours;
      osMutexWait(DiagnosticMutexHandle, 1000);
      LOG_NOW("=================================<=>=================================\r\n");
      LOG_NOW("                     Sensaphone Z-Wave Sentinel                      \r\n");
      LOG_NOW("                              v%s.%s.%s.%c \r\n", VERSION_A,VERSION_B,VERSION_C,BOARD_REVISION);
      LOG_NOW("=================================<=>=================================\r\n");
      osMutexRelease(DiagnosticMutexHandle);

      //LOG("%s: TIME_FORMAT_LONG  \t%s\r\n", __FUNCTION__, rtc_get_timestamp_string(rtc_get_encoded_date(), TIME_FORMAT_LONG));
    }
    // updates on hours
    ///////////////////////////////

    //////////////////////////////////////////////
    //
    // Pull messages from the queue and process
    //
    //////////////////////////////////////////////
    uint8_t lucMainQueueCount = osMessageQueueGetCount(MainQueueHandle);
    uint16_t luiMainQueueEventID;
    //uint16_t luiMainQueuePayload;
    if (lucMainQueueCount > 0)
    {
      osStatus_t ltMainQueueStatus = osMessageQueueGet(MainQueueHandle, &luiMainQueueEventID, NULL, 0);
      if (ltMainQueueStatus  != osOK) LOG("%s: Message queue osMessageQueueGet() returned = %d\r\n", __FUNCTION__, ltMainQueueStatus);

      switch (luiMainQueueEventID)
      {
        //////////////////////////////////////////////////////////////////////////////////
        //// TEST MAB 2025.09.29
        //// Test of the CMSIS v2 message queuing, and data exchange between Main and TouchGFX
        case msgid_MAIN_PRINT_ELAPSED_TIME:
          //LOG("%s: Elapsed time: %d seconds\r\n", __FUNCTION__, gulElapsedTime_Runtime_sec);
          break;
        case msgid_TOUCHGFX_MAIN_SET_CURRENT_ZONE:
          osMessageQueueGet(MainQueueHandle, &luiCurrentZWaveZone, NULL, 0);
          LOG("%s: TouchGFX set new zone number to %d\r\n", __FUNCTION__, luiCurrentZWaveZone);
          break;
        //////////////////////////////////////////////////////////////////////////////////

        case msgid_INPUT_MAIN_RESET_BUTTON_PRESSED:
          LOG("%s: Reset-To-Defaults button is pressed\r\n", __FUNCTION__);
          break;

        case msgid_NETWORK_MAIN_ONLINE_STATE:
          break;

        case msgid_MAIN_FLASH_APP_CRC:
          break;

        case msgid_MAIN_FLASH_COPY_APP:
          break;

        case msgid_MAIN_REBOOT:
          break;

        default:
          if (luiMainQueueEventID == msgid_NOP)
          {
            // do nothing
          }
          else
          {
            LOG("%s: ***ERROR*** Invalid command %d\r\n", __FUNCTION__, luiMainQueueEventID)
          }
          break;
      }
      // end switch MainQueueEvent
    }

    //////////////////////////////////////////////
    //
    // Check for any received bytes from the Diagnostic port
    //
    //////////////////////////////////////////////
    memset(gucDiagnosticRxBuffer, 0x00, sizeof(gucDiagnosticRxBuffer));
    luiDiagnosticRxCount = Diagnostic_Receive_Response(gucDiagnosticRxBuffer);
    if (luiDiagnosticRxCount > 0)
    {
      LOG("-----------------------  Diagnostic received bytes START -----------------------\r\n");
      PrintBytes(gucDiagnosticRxBuffer, luiDiagnosticRxCount, false, 0);
      LOG("-----------------------  Diagnostic received bytes  END  -----------------------\r\n");

      /////////////////////////////////////////////////////////////////////////////////
      //// TEST MAB 2025.10.16
      //// As a test of the ZWave serial port, transmit the received
      //// Diagnostic bytes out the ZWave serial port
      //// (ZWave Tx doesn't appear active either)
      HAL_UART_Transmit(&huart2, (uint8_t *)gucDiagnosticRxBuffer, luiDiagnosticRxCount, luiDiagnosticRxCount);
      /////////////////////////////////////////////////////////////////////////////////
    }

    //////////////////////////////////////////////
    //
    // Task loop delay
    //
    //////////////////////////////////////////////
    osDelay(MAIN_TASK_PERIOD);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_TouchGFX_Task */
/** *****************************************************************************************************************************
* @brief Function implementing the TouchGFXTask thread.
* @param argument: Not used
* @retval None
* ************************************
* *** WARNING *** This is NOT the "real" TouchGFX task!!! The real one is in app_touchgfx.c
* ************************************
*/
/* USER CODE END Header_TouchGFX_Task */
__weak void TouchGFX_Task(void *argument)
{
  /* USER CODE BEGIN TouchGFX_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TouchGFX_Task */
}

/* USER CODE BEGIN Header_InputTask */
/** *****************************************************************************************************************************
* @brief Function implementing the Input thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_InputTask */
void InputTask(void *argument)
{
  /* USER CODE BEGIN InputTask */
  LOG("%s: initializing...\r\n", __FUNCTION__);
  /* Infinite loop */
  for(;;)
  {
    osDelay(INPUT_TASK_PERIOD);
  }
  /* USER CODE END InputTask */
}

/* USER CODE BEGIN Header_OutputTask */
/** *****************************************************************************************************************************
* @brief Function implementing the Output thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OutputTask */
void OutputTask(void *argument)
{
  /* USER CODE BEGIN OutputTask */
  LOG("%s: initializing...\r\n", __FUNCTION__);
  uint32_t lulBlinkLedRedInterval_msec = 0;

  /* Infinite loop */
  for(;;)
  {
    // Toggle the LED2/LED_RED/LD6 LED
    lulBlinkLedRedInterval_msec += OUTPUT_TASK_PERIOD;
    if (lulBlinkLedRedInterval_msec > 250)
    {
      lulBlinkLedRedInterval_msec = 0;
      BSP_LED_Toggle(LED_RED);
    }

    osDelay(OUTPUT_TASK_PERIOD);
  }
  /* USER CODE END OutputTask */
}

/* USER CODE BEGIN Header_DatalogTask */
/** *****************************************************************************************************************************
* @brief Function implementing the Datalog thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DatalogTask */
void DatalogTask(void *argument)
{
  /* USER CODE BEGIN DatalogTask */
  LOG("%s: initializing...\r\n", __FUNCTION__);
  /* Infinite loop */
  for(;;)
  {
    osDelay(DATALOG_TASK_PERIOD);
  }
  /* USER CODE END DatalogTask */
}

/* USER CODE BEGIN Header_NetworkTask */
/** *****************************************************************************************************************************
* @brief Function implementing the Network thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_NetworkTask */
void NetworkTask(void *argument)
{
  /* USER CODE BEGIN NetworkTask */
  LOG("%s: initializing...\r\n", __FUNCTION__);
  /* Infinite loop */
  for(;;)
  {
    osDelay(NETWORK_TASK_PERIOD);
  }
  /* USER CODE END NetworkTask */
}

/* USER CODE BEGIN Header_ZWaveTask */
/** *****************************************************************************************************************************
* @brief Function implementing the ZWave thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ZWaveTask */
void ZWaveTask(void *argument)
{
  /* USER CODE BEGIN ZWaveTask */
  //static uint16_t luiZWaveRxCount = 0;
  static HAL_StatusTypeDef ltHALStatus;
  static uint8_t lucOldSecond = 100; // Nonsense initial value guarantees update when RTC first read
  static uint8_t lucOldMinute = 100; // Nonsense initial value guarantees update when RTC first read
  static uint8_t lucOldHour   = 100; // Nonsense initial value guarantees update when RTC first read
  static uint8_t lucCountdownToRepeatedZWaveCommands_minutes = 30;
  #define NODE_PL_DISPLAY_INTERVAL_MINUTES (5)
  static uint8_t lucCountdownToNodeProvisioningListStatus_minutes = NODE_PL_DISPLAY_INTERVAL_MINUTES;
  static uint32_t lulCountdownToCheckAbandonedNodeID_seconds = 120;
  static uint8_t lucCountdownToUpdateNodeIDList_minutes = 60;

  LOG("%s: initializing...\r\n", __FUNCTION__);

  //////////////////////////////////////////////////////////////////
  // Arm the Z-Wave controller interrupt for the first byte
  //////////////////////////////////////////////////////////////////
  ltHALStatus = HAL_UART_Receive_IT(&huart2, &gucReceivedByteFromZWave, 1);
  //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  if (ltHALStatus != HAL_OK)
  {
    LOG("%s: *** WARNING *** HAL_UART_Receive_IT(&huart2) (for Z-Wave) returned 0x%02X\r\n", __FUNCTION__, ltHALStatus);
  }
  else
  {
    //LOG("%s: HAL_UART_Receive_IT(&huart2) (for Z-Wave) returned HAL_OK\r\n", __FUNCTION__);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Initialize received command handler arrays
  // (the command-indexed jump table of handler routines for received commands)
  //////////////////////////////////////////////////////////////////////////////
  // First, initialize the entire jump table with the "unsupported command" routines
  for (int i = 0; i < 256; ++i)
  {
    gtZWave_CMD_Handler[i] = ZWave_RES_CMD_XX_Unsupported;
    gtZWave_CC_Handler[i]  = ZWave_Rx_CC_XX_Unsupported;
  }

  // Now fill in the entries for supported command handler routines
  gtZWave_CMD_Handler[FUNC_ID_SERIAL_API_GET_INIT_DATA]           = ZWave_RES_CMD_02_Get_Init_Data;
  gtZWave_CMD_Handler[FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES]     = ZWave_RES_CMD_05_ZW_Get_Controller_Capabilities;
  gtZWave_CMD_Handler[FUNC_ID_SERIAL_API_GET_CAPABILITIES]        = ZWave_RES_CMD_07_Serial_API_Get_Capabilities;
  gtZWave_CMD_Handler[FUNC_ID_SERIAL_API_STARTED]                 = ZWave_REQ_CMD_0A_Serial_API_Started;
  gtZWave_CMD_Handler[FUNC_ID_SERIAL_API_SETUP]                   = ZWave_RES_CMD_0B_Serial_API_Setup;
  gtZWave_CMD_Handler[FUNC_ID_ZW_SEND_NODE_INFORMATION]           = ZWave_RSQ_CMD_12_ZW_Send_Node_Information;
  gtZWave_CMD_Handler[FUNC_ID_ZW_SEND_DATA]                       = ZWave_RSQ_CMD_13_ZW_Send_Data;
  gtZWave_CMD_Handler[FUNC_ID_ZW_GET_VERSION]                     = ZWave_RES_CMD_15_ZW_Get_Version;
  gtZWave_CMD_Handler[FUNC_ID_MEMORY_GET_ID]                      = ZWave_RES_CMD_20_Memory_Get_ID;
  gtZWave_CMD_Handler[FUNC_ID_NVR_GET_VALUE]                      = ZWave_RES_CMD_28_NVR_Get_Value;
  gtZWave_CMD_Handler[FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK]     = ZWave_REQ_CMD_3F_ZW_Remove_Node_ID_From_Network;
  gtZWave_CMD_Handler[FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO]          = ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info;
  gtZWave_CMD_Handler[FUNC_ID_ZW_SET_DEFAULT]                     = ZWave_REQ_CMD_42_ZW_Set_Default;
  gtZWave_CMD_Handler[FUNC_ID_ZW_ASSIGN_RETURN_ROUTE]             = ZWave_RSQ_CMD_46_ZW_Assign_Return_Route;
  gtZWave_CMD_Handler[FUNC_ID_ZW_DELETE_RETURN_ROUTE]             = ZWave_RSQ_CMD_47_ZW_Delete_Return_Route;
  gtZWave_CMD_Handler[FUNC_ID_ZW_APPLICATION_UPDATE]              = ZWave_REQ_CMD_49_ZW_Application_Update;
  gtZWave_CMD_Handler[FUNC_ID_ZW_ADD_NODE_TO_NETWORK]             = ZWave_REQ_CMD_4A_ZW_Add_Node_To_Network;
  gtZWave_CMD_Handler[FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK]        = ZWave_REQ_CMD_4B_ZW_Remove_Node_From_Network;
  gtZWave_CMD_Handler[FUNC_ID_ZW_SET_LEARN_MODE]                  = ZWave_RES_CMD_50_ZW_Set_Learn_Mode;
  gtZWave_CMD_Handler[FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE]         = ZWave_RSQ_CMD_51_ZW_Assign_SUC_Return_Route;
  gtZWave_CMD_Handler[FUNC_ID_ZW_SET_SUC_NODE_ID]                 = ZWave_RSQ_CMD_54_ZW_Set_SUC_NodeID;
  gtZWave_CMD_Handler[FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE]         = ZWave_RSQ_CMD_55_ZW_Delete_SUC_Return_Route;
  gtZWave_CMD_Handler[FUNC_ID_ZW_GET_SUC_NODE_ID]                 = ZWave_RES_CMD_56_ZW_Get_SUC_Node_ID;
  gtZWave_CMD_Handler[FUNC_ID_ZW_REQUEST_NODE_INFO]               = ZWave_RES_CMD_60_ZW_Request_Node_Info;
  gtZWave_CMD_Handler[FUNC_ID_ZW_REMOVE_FAILED_NODE_ID]           = ZWave_RSQ_CMD_61_ZW_Remove_Failed_Node;
  gtZWave_CMD_Handler[FUNC_ID_ZW_IS_FAILED_NODE_ID]               = ZWave_RES_CMD_62_ZW_Is_Failed_Node;
  gtZWave_CMD_Handler[FUNC_ID_ZW_IS_VIRTUAL_NODE]                 = ZWave_RES_CMD_A6_ZW_Is_Virtual_Node;
  gtZWave_CMD_Handler[FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE] = ZWave_RES_CMD_A8_Application_Command_Handler_Bridge;
  gtZWave_CMD_Handler[FUNC_ID_ZW_SEND_DATA_BRIDGE]                = ZWave_RES_CMD_A9_ZW_Send_Data_Bridge;
  gtZWave_CMD_Handler[FUNC_ID_SERIAL_API_GET_LR_NODES]            = ZWave_RES_CMD_DA_Serial_API_Get_LR_Nodes;
  gtZWave_CMD_Handler[FUNC_ID_GET_DCDC_CONFIG]                    = ZWave_RES_CMD_DE_Get_DCDC_Config;
  gtZWave_CMD_Handler[FUNC_ID_SET_DCDC_CONFIG]                    = ZWave_RES_CMD_DF_Set_DCDC_Config;
  gtZWave_CMD_Handler[FUNC_ID_GET_RADIO_PTI]                      = ZWave_RES_CMD_E8_Get_Radio_PTI;
  // Here's a handy template when implementing command handlers in the future
  //gtZWave_CMD_Handler[xxxxxxxxxxxxxxxxxx] = xxxxxxxxxxxxxxxxx;

  // Also fill in the entries for supported command class handler routines
  gtZWave_CC_Handler[COMMAND_CLASS_SWITCH_MULTILEVEL_V4]          = ZWave_Rx_CC_26_Switch_Multilevel_V4;
  gtZWave_CC_Handler[COMMAND_CLASS_SECURITY_2_V2]                 = ZWave_Rx_CC_9F_Security_2_V2;
  // Here's a handy template when implementing command class handlers in the future
  //gtZWave_CC_Handler[xxxxxxxxxxxxxxxxxx] = xxxxxxxxxxxxxxxxx;

  //////////////////////////////////////////////////////////////////////////////
  // Initialize Z-Wave SerialAPI state machine
  //////////////////////////////////////////////////////////////////////////////
  ZWave_SerialAPI_StateMachine(ZWAVE_SM_CMD_INITIALIZE);

  //////////////////////////////////////////////////////////////////////////////
  // Initialize Z-Wave SmartStart state machine
  //////////////////////////////////////////////////////////////////////////////
  ZWave_SmartStart_StateMachine(SMARTSTART_SM_CMD_INITIALIZE);

  //////////////////////////////////////////////
  // Reset the EFR32ZG23 Z-Wave controller
  //////////////////////////////////////////////
  // TEST MAB 2025.11.13
  // Soft reset the ZWave controller
  #if ENABLE_ZWAVE_CONTROLLER_HOST
  ZWave_Send_REQ_CMD_08_Serial_API_Soft_Reset();
  #endif

  //////////////////////////////////////////////////////////////////////////////
  // Initialize Z-Wave node provisioning list
  //////////////////////////////////////////////////////////////////////////////
  static char lucIntegerString[10];
  static char lucDSKString[60];
  static uint16_t luiTempDSKInt;
  static char lucDSKHexString[60];
  LOG("%s: --------- Initializing Node Provisioning list... ---------\r\n", __FUNCTION__);
  // ----- Zeroize DSKs (initialize any valid DSKs later)
  LOG("%s: Zeroized DSKs\r\n", __FUNCTION__);
  for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
  {
    ZWave_DSK_Zeroize(i);
  }
  for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
  {
    memset(lucDSKString, 0x00, sizeof(lucDSKString));
    ZWave_DSK_Write_To_String(i, lucDSKString);
    LOG("%s: DSK %d: %s\r\n", __FUNCTION__, i, lucDSKString);
  }
  // ----- Initialize with random values for DSKs (initialize any valid DSKs later)
  uint8_t lucDSKBuffer[DSK_LENGTH_BYTES];
  LOG("%s: Randomized DSKs\r\n", __FUNCTION__);
  for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
  {
    gtNodeProvisioningList[i].NodeID = NODE_ID_UNAVAILABLE; // initialize NodeID
    memset(lucDSKString, 0x00, sizeof(lucDSKString));
    for (int j = 0; j < DSK_LENGTH_BYTES; ++j)
    {
      lucDSKBuffer[j] = (uint8_t)RandomValue();
    }
    ZWave_DSK_Write(i, lucDSKBuffer);
    ZWave_DSK_Write_To_String(i, lucDSKString);
    LOG("%s: DSK %d: %s\r\n", __FUNCTION__, i, lucDSKString);
  }
  // ----- Set up a valid DSK for existing End node
  LOG("%s: Valid DSKs\r\n", __FUNCTION__);
  uint8_t lucAvailableDSKIndex;
  // Zeroize a random DSK
  lucAvailableDSKIndex = RandomValue() % NODE_PROVISIONING_LIST_COUNT;
  ZWave_DSK_Zeroize(lucAvailableDSKIndex);
  // END Zeroize a random DSK
  lucAvailableDSKIndex = ZWave_DSK_Find_Zeroized();
  if (lucAvailableDSKIndex != DSK_UNAVAILABLE)
  {
    gtNodeProvisioningList[lucAvailableDSKIndex].NodeID = NODE_ID_UNAVAILABLE; // initialize NodeID
    ZWave_DSK_Write_From_String(lucAvailableDSKIndex, "15522-62065-59566-53879-58322-31565-50017-40262");
    memset(lucDSKString, 0x00, sizeof(lucDSKString));
    ZWave_DSK_Write_To_String(lucAvailableDSKIndex, lucDSKString);
    LOG("%s: DSK %d: %s\r\n", __FUNCTION__, lucAvailableDSKIndex, lucDSKString);
  }
  LOG("%s: --------- END Initializing Node Provisioning list ---------\r\n", __FUNCTION__);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //// TEST MAB 2025.12.30
  //// Test DSK string validation
//  ZWave_DSK_Write_From_String(0, "12345-12345-12345-12345-12345-12345-12345-12345"); // OK
//  ZWave_DSK_Write_From_String(0, "65536-12345-12345-12345-12345-12345-12345-12345");  // Fail - a number exceeds 65535
//  ZWave_DSK_Write_From_String(0, "12345-12345-12345-12345-12345-12345-12345-12345-"); // Fail - DSK string isn't 47
//  ZWave_DSK_Write_From_String(0, "12345-12345-12345-12345-12345-12345-12345-123"); // Fail - DSK string isn't 47
//  ZWave_DSK_Write_From_String(0, "12345 12345 12345 12345 12345 12345 12345 12345"); // OK
//  ZWave_DSK_Write_From_String(0, "12w45-12345-12345-12345-12345-12345-12345-12345"); // Fail - non-digit char detected
//  ZWave_DSK_Validate_String("12345-12345-12345-12345-12345-12345-12345-12345"); // OK
//  ZWave_DSK_Validate_String("65536-12345-12345-12345-12345-12345-12345-12345");  // Fail - a number exceeds 65535
//  ZWave_DSK_Validate_String("12345-12345-12345-12345-12345-12345-12345-12345-"); // Fail - DSK string isn't 47
//  ZWave_DSK_Validate_String("12345-12345-12345-12345-12345-12345-12345-123"); // Fail - DSK string isn't 47
//  ZWave_DSK_Validate_String("12345 12345 12345 12345 12345 12345 12345 12345"); // OK
//  ZWave_DSK_Validate_String("12w45-12345-12345-12345-12345-12345-12345-12345"); // Fail - non-digit char detected
 ///////////////////////////////////////////////////////////////////////////////////////////////////////

//  ///////////////////////////////////////////////////////////////////////////////////////////////////////
//  //// TEST MAB 2026.01.07
//  //// Test LR node byte/bit detection
//  uint16_t luiNodeID;
//  memset(gucLRNodes, 0x00, MAX_LR_NODEMASK_LENGTH);
//  //memset(gucLRNodes, 0xFF, MAX_LR_NODEMASK_LENGTH);
////  for (luiNodeID = 0x0100; luiNodeID < 0x0200; ++luiNodeID)
////  {
////    ZW_NodeMaskSetBit(NULL, luiNodeID);
////  }
//  uint16_t luiRandomNodeIDCount = RandomValue() % 0x10;
//  for (uint16_t i = 0; i < luiRandomNodeIDCount; ++i)
//  {
//    luiNodeID = 0x100 + RandomValue()%0x400;
//    //LOG("%s: randomized NodeID = 0x%04X \r\n", __FUNCTION__, luiNodeID);
//    ZW_NodeMaskSetBit(gucLRNodes, luiNodeID);
//    //ZW_NodeMaskClearBit(gucLRNodes, luiNodeID);
//  }
//  PrintBytes(gucLRNodes, MAX_LR_NODEMASK_LENGTH, false, 0);
//  ZW_NodeMaskBitsIn(gucLRNodes, MAX_LR_NODEMASK_LENGTH);
//
//  // Detect all active nodes
//  uint16_t luiActiveNodeIDCount = 0;
//  luiNodeID = 0;
//  do
//  {
//    luiNodeID = ZW_NodeMaskGetNextNodeIndex(luiNodeID, gucLRNodes);
//    if (luiNodeID) ++luiActiveNodeIDCount;
//  }
//  while (luiNodeID);
//  LOG("%s: %d active NodeIDs detected \r\n", __FUNCTION__, luiActiveNodeIDCount);
//  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  /* Infinite loop */
  for(;;)
  {
    ///////////////////////////////
    // updates on seconds
    if (lucOldSecond != sMainRTCTime.Seconds)
    {
      lucOldSecond = sMainRTCTime.Seconds;

      if (lulCountdownToCheckAbandonedNodeID_seconds) --lulCountdownToCheckAbandonedNodeID_seconds;

    } // updates on seconds
    ///////////////////////////////
    // updates on minutes
    if (lucOldMinute != sMainRTCTime.Minutes)
    {
      lucOldMinute = sMainRTCTime.Minutes;

      if (lucCountdownToRepeatedZWaveCommands_minutes) --lucCountdownToRepeatedZWaveCommands_minutes;
      if (lucCountdownToNodeProvisioningListStatus_minutes) --lucCountdownToNodeProvisioningListStatus_minutes;
      if (lucCountdownToUpdateNodeIDList_minutes) --lucCountdownToUpdateNodeIDList_minutes;

    } // updates on minutes
    ///////////////////////////////
    // updates on hours
    if (lucOldHour != sMainRTCTime.Hours)
    {
      lucOldHour = sMainRTCTime.Hours;
    } // updates on hours
    ///////////////////////////////

    //////////////////////////////////////////////
    //
    // Update msec-based timing variables
    //
    //////////////////////////////////////////////
    if (gtZWaveRxInterface.ack_timeout_ms  > 0 && gtZWaveRxInterface.ack_timeout_ms  < DEFAULT_ACK_TIMEOUT_MS)  gtZWaveRxInterface.ack_timeout_ms  += ZWAVE_TASK_PERIOD;
    if (gtZWaveRxInterface.byte_timeout_ms > 0 && gtZWaveRxInterface.byte_timeout_ms < DEFAULT_BYTE_TIMEOUT_MS) gtZWaveRxInterface.byte_timeout_ms += ZWAVE_TASK_PERIOD;
    // Trigger any timeouts
    if (gtZWaveRxInterface.ack_timeout_ms >= DEFAULT_ACK_TIMEOUT_MS)
    {
      gtZWaveRxInterface.ack_timeout_ms = 0; // stop the timer
      gtZWaveRxInterface.ack_timeout = TRUE; // timeout occurred
      LOG("%s: *** WARNING *** ACK timeout occurred\r\n", __FUNCTION__);
    }
    if (gtZWaveRxInterface.byte_timeout >= DEFAULT_BYTE_TIMEOUT_MS)
    {
      gtZWaveRxInterface.byte_timeout_ms = 0; // stop the timer
      gtZWaveRxInterface.byte_timeout = TRUE; // timeout occurred
      LOG("%s: *** WARNING *** byte timeout occurred\r\n", __FUNCTION__);
    }

//    //////////////////////////////////////////////
//    //
//    // Check for any received bytes from the Z-Wave controller
//    //
//    //////////////////////////////////////////////
//    memset(gucZWaveRxBuffer, 0x00, sizeof(gucZWaveRxBuffer));
//    luiZWaveRxCount = ZWave_Receive_Response(gucZWaveRxBuffer);
//    if (luiZWaveRxCount > 0)
//    {
//      LOG("-----------------------  Z-Wave received bytes START -----------------------\r\n");
//      PrintBytes(gucZWaveRxBuffer, luiZWaveRxCount, false, 0);
//      LOG("-----------------------  Z-Wave received bytes  END  -----------------------\r\n");
//    }


    //////////////////////////////////////////////
    //
    // Run the Z-Wave state machine
    //
    //////////////////////////////////////////////
    ZWave_SerialAPI_StateMachine(ZWAVE_SM_CMD_RUN);

    //////////////////////////////////////////////
    //
    // Run the SmartStart state machine
    //
    //////////////////////////////////////////////
    if (ZWave_DSK_IsProcessing())
    {
      ZWave_SmartStart_StateMachine(SMARTSTART_SM_CMD_RUN);
    }

//    ////////////////////////////////////////////////////////////////////////
//    //// TEST MAB 2025.11.14
//    //// Every N minutes, requeue a bunch of commands to send to ZWave controller
//    if (lucCountdownToRepeatedZWaveCommands_minutes == 0)
//    {
//      lucCountdownToRepeatedZWaveCommands_minutes = 30;
//
//      /// Send Memory Get ID to get the HomeID and NodeID values
//      ZWave_Send_REQ_CMD_20_Memory_Get_ID();
//      /// Send Serial API Get Capabilities
//      ZWave_Send_REQ_CMD_07_Serial_API_Get_Capabilities();
//      // Send Serial API Get LR Nodes
//      ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes();
//      // Send Serial API Get Init Data
//      ZWave_Send_REQ_CMD_02_Serial_API_Get_Init_Data();
//      // Send Get Controller Capabilities
//      ZWave_Send_REQ_CMD_05_Get_Controller_Capabilities();
//      // Send Get SUC Node ID
//      ZWave_Send_REQ_CMD_56_Get_SUC_Node_ID();
//
//      // Set DCDC mode
//      ZWave_Send_REQ_CMD_DF_Set_DCDC_Config(EDCDCMODE_AUTO);
//      // Send Get DCDC Config
//      ZWave_Send_REQ_CMD_DE_Get_DCDC_Config();
//
//      // Send Get Radio PTI
//      ZWave_Send_REQ_CMD_E8_Get_Radio_PTI();
//
//      // Set region
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_US, IGNORE); // OK
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_EU, IGNORE); // OK
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_US_LR, IGNORE); // OK
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_DEPRECATED_48, IGNORE); // FAILS; region unchanged
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_SET, REGION_JP, IGNORE); // OK
//      // Set TX power level
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT, 200, 0); // OK
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT, 205, 0); // FAILS; TX power unchanged
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT, 200, 5); // OK
//      // Set LR TX power level
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET, 200, IGNORE); // OK
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET, 190, IGNORE); // OK
//      //ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET, 205, IGNORE); // FAILS; LR TX power unchanged
//
//
//      // Send other Serial API Setup requests
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_SUPPORTED, IGNORE, IGNORE);
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT, IGNORE, IGNORE);
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET, IGNORE, IGNORE);
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_RF_REGION_GET, IGNORE, IGNORE);
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE, IGNORE, IGNORE);
//      ZWave_Send_REQ_CMD_0B_Serial_API_Setup(SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE, IGNORE, IGNORE);
//    }
//    ////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    //// TEST MAB 2025.12.29
    //// Every N minutes display the node provisioning list
    //// TEST MAB 2025.12.30
    //// Also check for the 1st zeroized DSK, if any
    uint8_t lucNWIAuthHomeIDBuffer[8];
    uint32_t lulNWIHomeID;
    uint32_t lulAuthHomeID;
    if (lucCountdownToNodeProvisioningListStatus_minutes == 0)
    {
      lucCountdownToNodeProvisioningListStatus_minutes = NODE_PL_DISPLAY_INTERVAL_MINUTES;

      for (int i = 0; i < NODE_PROVISIONING_LIST_COUNT; ++i)
      {
        if (!ZWave_DSK_IsZeroized(i))
        {
          // Display DSK
          memset(lucDSKString,    0x00, sizeof(lucDSKString));
          ZWave_DSK_Write_To_String(i, lucDSKString);
          memset(lucDSKHexString, 0x00, sizeof(lucDSKHexString));
          for (int j = 0; j < DSK_LENGTH_BYTES; j+=2)
          {
            luiTempDSKInt = 0x100*gtNodeProvisioningList[i].dsk[j] + gtNodeProvisioningList[i].dsk[j+1];
            memset(lucIntegerString, 0x00, sizeof(lucIntegerString));
            sprintf(lucIntegerString, "%04X", luiTempDSKInt);
            strcat(lucDSKHexString, lucIntegerString);
            if (j < DSK_LENGTH_BYTES-2) strcat(lucDSKHexString, "--");
          }
          LOG("%s: DSK %d: %s\r\n", __FUNCTION__, i, lucDSKString);
          LOG("%s:         %s\r\n", __FUNCTION__,    lucDSKHexString);
          // END Display DSK

          // Display NWI and Auth HomeIDs
          if (ZWave_DSK_Extract_NWIAuthHomeID(i, lucNWIAuthHomeIDBuffer))
          {
            lulNWIHomeID =  (0x1000000 * lucNWIAuthHomeIDBuffer[0]) +
                            (  0x10000 * lucNWIAuthHomeIDBuffer[1]) +
                            (    0x100 * lucNWIAuthHomeIDBuffer[2]) +
                            (            lucNWIAuthHomeIDBuffer[3]);
            lulAuthHomeID = (0x1000000 * lucNWIAuthHomeIDBuffer[4]) +
                            (  0x10000 * lucNWIAuthHomeIDBuffer[5]) +
                            (    0x100 * lucNWIAuthHomeIDBuffer[6]) +
                            (            lucNWIAuthHomeIDBuffer[7]);
            LOG("%s: - NWI  HomeID:                   %08X \r\n",             __FUNCTION__, lulNWIHomeID);
            LOG("%s: - Auth HomeID:                               %08X \r\n", __FUNCTION__, lulAuthHomeID);
          }
          // END Display NWI and Auth HomeIDs

          // Test if zeroized
          if (ZWave_DSK_IsZeroized(i))
          {
            LOG("%s: - Zeroized \r\n", __FUNCTION__);
          }

          // Display LR-capable or Mesh Only
          switch (gtNodeProvisioningList[i].lr_capable)
          {
          case ZWAVE_NODE_PROVISIONING_LIST_LR_CAPABLE:
            LOG("%s: - Long Range capable \r\n", __FUNCTION__);
            break;
          case ZWAVE_NODE_PROVISIONING_LIST_MESH_ONLY:
            LOG("%s: - Mesh only\r\n", __FUNCTION__);
            break;
          default:
            LOG("%s: - *** WARNING *** lr_capable value %d is UNKNOWN\r\n", __FUNCTION__, gtNodeProvisioningList[i].lr_capable);
            break;
          }
          // END Display LR-capable or Mesh Only

          // Display boot mode
          switch (gtNodeProvisioningList[i].boot_mode)
          {
          case ZWAVE_NODE_PROVISIONING_LIST_SMARTSTART:
            LOG("%s: - SmartStart \r\n", __FUNCTION__);
            break;
          case ZWAVE_NODE_PROVISIONING_LIST_S2_MANUAL:
            LOG("%s: - S2 manual \r\n", __FUNCTION__);
            break;
          default:
            LOG("%s: - *** WARNING *** boot_mode value %d is UNKNOWN\r\n", __FUNCTION__, gtNodeProvisioningList[i].boot_mode);
            break;
          }
          // END Display boot mode

          // Display status
          switch (gtNodeProvisioningList[i].status)
          {
          case SMARTSTART_EMPTY:
            LOG("%s: - EMPTY - DSK not written \r\n", __FUNCTION__);
            break;
          case SMARTSTART_READY:
            LOG("%s: - READY - DSK written to node provisioning list; end node not connected \r\n", __FUNCTION__);
            break;
          case SMARTSTART_DETECTED:
            LOG("%s: - DETECTED - End node with correlated DSK detected \r\n", __FUNCTION__);
            break;
          case SMARTSTART_INCLUSION:
            LOG("%s: - INCLUSION - End node joining home network \r\n", __FUNCTION__);
            break;
          case SMARTSTART_EXCLUSION:
            LOG("%s: - EXCLUSION - End node removed from home network \r\n", __FUNCTION__);
            break;
          case SMARTSTART_BOOTSTRAP:
            LOG("%s: - BOOTSTRAP - End node sharing key information \r\n", __FUNCTION__);
            break;
          case SMARTSTART_ACTIVE:
            LOG("%s: - ACTIVE - End node fully connected, including security \r\n", __FUNCTION__);
            break;
          case SMARTSTART_REMOVED:
            LOG("%s: - REMOVED - End node being removed from home network; DSK being erased from node provisioning list \r\n", __FUNCTION__);
            break;
          default:
            LOG("%s: - *** WARNING *** status value %d is UNKNOWN\r\n", __FUNCTION__, gtNodeProvisioningList[i].status);
            break;
          }
          // END Display status

          // Display NodeID (may be 0x0000)
          LOG("%s: - NodeID: 0x%04X \r\n", __FUNCTION__, gtNodeProvisioningList[i].NodeID);
          // END Display NodeID
        }
        // END not zeroized DSK
      }
      // END for NODE_PROVISIONING_LIST_COUNT

      // Also check for the first zeroized DSK, if any
      uint8_t lucFirstZeroizedDSKIndex;
      lucFirstZeroizedDSKIndex = ZWave_DSK_Find_Zeroized();
      if (lucFirstZeroizedDSKIndex == DSK_UNAVAILABLE)
      {
        LOG("%s: *** WARNING *** no zeroized DSK is available in the node provisioning list \r\n", __FUNCTION__);
      }
      else
      {
        LOG("%s: DSK %d is zeroized \r\n", __FUNCTION__, lucFirstZeroizedDSKIndex);
      }
    }
    // END display the node provisioning list
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    //// TEST MAB 2026.01.08
    //// Periodically update the local copy of LR node list
    if (0 == lucCountdownToUpdateNodeIDList_minutes)
    {
      lucCountdownToUpdateNodeIDList_minutes = 60;

      #if ENABLE_ZWAVE_CONTROLLER_HOST
      // Fetch latest LR node list
      ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes();
      #endif
    }
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    //// TEST MAB 2026.01.07
    //// Periodically check the next active NodeID if it is abandoned and if so,
    //// remove the NodeID from the network.
    static uint16_t luiAbandonedNodeID = 0;
    static uint8_t lucSendDataBuffer[10];
    static uint8_t lucIsAbandonedNodeIDSearchActive;
    if (0 == lulCountdownToCheckAbandonedNodeID_seconds)
    //if (0 == lulCountdownToCheckAbandonedNodeID_seconds && !ZWave_DSK_IsProcessing())
    {
      //lulCountdownToCheckAbandonedNodeID_seconds = 300 + RandomValue()%60; // 5-6 minutes
      //lulCountdownToCheckAbandonedNodeID_seconds = 60 + RandomValue()%60; // 1-2 minutes
      lulCountdownToCheckAbandonedNodeID_seconds = 30 + RandomValue()%10; // 30-40 seconds
      //lulCountdownToCheckAbandonedNodeID_seconds = 10 + RandomValue()%10; // 10-20 seconds

      // Look for a NodeID in the node list that IS NOT an active NodeID
      // IF such a NodeID is found, assume it's a "failed" node and
      // delete it from the node list
      lucIsAbandonedNodeIDSearchActive = TRUE;
      while (lucIsAbandonedNodeIDSearchActive)
      {
        luiAbandonedNodeID = ZW_NodeMaskGetNextNodeIndex(luiAbandonedNodeID, gucLRNodes);
        if (0==luiAbandonedNodeID)
        {
          // No further NodeIDs found in node list; search is done
          lucIsAbandonedNodeIDSearchActive = FALSE;
        }
        else
        {
          // Check if candidate NodeID is actually an active one
          if ( DSK_UNAVAILABLE == ZWave_Scan_ProvisioningList_For_NodeID(luiAbandonedNodeID) )
          {
            // Candidate NodeID is NOT in the node provisioning list, so consider it failed. Search is done
            lucIsAbandonedNodeIDSearchActive = FALSE;
          }
        }
      } // end while lucIsAbandonedNodeIDSearchActive

      // Delete the abandoned NodeID
      if (luiAbandonedNodeID)
      {
        // (we've found the next abandoned NodeID in the current LR node list)
        // "Ping" the node
        // (Attempt to communicate with the failed node: failure -> failed NodeID added to failed node list)
        guiNodeID = luiAbandonedNodeID;
        gucSessionID = ZWave_SessionID_Randomize();
        lucSendDataBuffer[0] = COMMAND_CLASS_SECURITY_2_V2;
        lucSendDataBuffer[1] = SECURITY_2_NONCE_GET_V2;
        lucSendDataBuffer[2] = gucSessionID;
        #if ENABLE_ZWAVE_CONTROLLER_HOST
        ZWave_Send_REQ_CMD_13_Send_Data(luiAbandonedNodeID, 3, lucSendDataBuffer, TRANSMIT_OPTION_ACK, gucSessionID);
        #endif

        // Check if the specified NodeID is in the controller's failed nodes list
        guiNodeID = luiAbandonedNodeID;
        #if ENABLE_ZWAVE_CONTROLLER_HOST
        ZWave_Send_REQ_CMD_62_Is_Node_Failed(luiAbandonedNodeID);
        #endif

        // Remove the failed node from the network
        gucSessionID = ZWave_SessionID_Randomize();
        //gucSessionID = ZWave_SessionID_Update(gucSessionID);
        guiNodeID = luiAbandonedNodeID;
        #if ENABLE_ZWAVE_CONTROLLER_HOST
        ZWave_Send_REQ_CMD_61_Remove_Failed_Node(gucSessionID, luiAbandonedNodeID);
        #endif
      }
      ////////////////////////////////////////
      // MAB 2026.01.14
      // During Z-Wave Sentinel development, when abandoned NodeIDs may be plentiful,
      // will need to update the local copy of the NodeID list more frequently.
      // In the final deliverable firmware probably don't need this ELSE case,
      // can rely on the separate periodic update of the local copy of the NodeID list.
      else
      {
        // (no more NodeIDs to check in the current LR node list)
        // Fetch latest LR node list
        #if ENABLE_ZWAVE_CONTROLLER_HOST
        ZWave_Send_REQ_CMD_DA_Serial_API_Get_LR_Nodes();
        #endif
      }
      ////////////////////////////////////////

    } // endif time to check for abandoned NodeIDs

    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////
    //
    // Task loop delay
    //
    //////////////////////////////////////////////
    osDelay(ZWAVE_TASK_PERIOD);
  }
  /* USER CODE END ZWaveTask */
}

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_1MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128MB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  LOG("%s: *** ERROR *** some unusual error occurred, watchdog reset likely...\r\n", __FUNCTION__);
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
