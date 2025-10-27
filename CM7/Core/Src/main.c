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
uint8_t ZWaveRxQueueBuffer[ 1024 * sizeof( uint8_t ) ];
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

// Received bytes from Z-Wave controller and Diagnostic ports
uint8_t gucReceivedByteFromZWave;
uint8_t gucReceivedByteFromDiagnostic;

/* Queue for frames transmitted to Z-Wave controller (EFR32ZG23) */
#define MAX_CALLBACK_QUEUE  8
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
// Dispatch tables for Z-Wave command callbacks
//
typedef void (*zwave_cmd_handler_t)(void);

zwave_cmd_handler_t gtZWave_CMD_Callback[256];

/////////////////////////////

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
void MainTask(void *argument);
void TouchGFX_Task(void *argument);
void InputTask(void *argument);
void OutputTask(void *argument);
void DatalogTask(void *argument);
void NetworkTask(void *argument);
void ZWaveTask(void *argument);

/* USER CODE BEGIN PFP */

void ZWave_REQ_CMD_0A_Serial_API_Started(void);
void ZWave_RES_CMD_02_Get_Init_Data(void);
void ZWave_RES_CMD_05_ZW_Get_Controller_Capabilities(void);
void ZWave_RES_CMD_07_Serial_API_Get_Capabilities(void);
void ZWave_RES_CMD_0B_Serial_API_Setup(void);
void ZWave_RES_CMD_15_ZW_Get_Version(void);
void ZWave_RES_CMD_20_Memory_Get_ID(void);
void ZWave_RES_CMD_28_NVR_Get_Value(void);
void ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info(void);
void ZWave_RES_CMD_56_ZW_Get_SUC_Node_ID(void);
void ZWave_RES_CMD_A6_ZW_Is_Virtual_Node(void);
void ZWave_RES_CMD_DA_Serial_API_Get_LR_Nodes(void);
void ZWave_RES_CMD_DE_Get_DCDC_Config(void);
void ZWave_RES_CMD_XX_Unsupported(void);

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
  ZWaveRxQueueHandle = osMessageQueueNew (1024, sizeof(uint8_t), &ZWaveRxQueue_attributes);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  * @brief  Display received frame data (to debug port)
  * @param  None
  * @retval None
  */
void ZWave_Display_Received_Frame_Data(void)
{
  // Display received frame data
  LOG("Received frame:\r\n");
  LOG("------------------------------------------------------------------------------\r\n");
  PrintBytes(ZWaveSerialFrame, ZWaveSerialFrame->len + 1, false, 0); // Length doesn't include SOF so need to increment length
  LOG("------------------------------------------------------------------------------\r\n");
}
// end ZWave_Display_Received_Frame_Data

/** *****************************************************************************************************************************
  * @brief  Parse received byte from Z-Wave controller when ZWave Rx state is CHECKSUM
  * @param  aucRxByte - received byte from ZWave controller
  * @param  aucIsACKRequired - TRUE if ACK/NAK should be sent based on checksum; FALSE otherwise
  * @retval Result of validating checksum
  */
ZWaveRxParseResult_t ZWave_Handle_CHECKSUM(uint8_t aucRxByte, uint8_t aucIsACKRequired)
{
  static ZWaveRxParseResult_t ltResult;

  //LOG("%s: CHECKSUM byte is 0x%02X\r\n", __FUNCTION__, aucRxByte);

  // Reset byte timeout
  gtZWaveRxInterface.byte_timeout = false;

  // Stop byte timer

  // Disable Rx active
  gtZWaveRxInterface.rx_active = false;  // Not really active now...

  /* Default values for ack == false */
  /* It means we are in the process of looking for an acknowledge to a callback request */
  // MAB 2025.10.22 - Another way to think of it:
  // comm_interface.c, SerialAPIStateHandler() state is stateTxSerial, stateCallbackTxSerial or stateCommandTxSerial
  /* Drop the new frame we received - we don't have time to handle it. */
  ltResult = ZWAVE_RX_PARSE_IDLE;
  uint8_t lucResponse = CAN;

  /* Do we send ACK/NAK according to checksum... */
  /* if not then the received frame is dropped! */
  if (aucIsACKRequired)
  {
    uint8_t checksum = ZWave_XOR_Checksum(0xFF, &(ZWaveSerialFrame->len), ZWaveSerialFrame->len);
    ltResult = (aucRxByte == checksum) ? ZWAVE_RX_PARSE_FRAME_RECEIVED : ZWAVE_RX_PARSE_FRAME_ERROR;
    lucResponse = (aucRxByte == checksum) ? ACK : NAK;
  }
  switch (lucResponse)
  {
  case ACK:
    //LOG("%s: ACK (checksum OK)\r\n", __FUNCTION__);
    break;
  case NAK:
    LOG("%s: *** WARNING *** NAK (checksum error)\r\n", __FUNCTION__);
    break;
  case CAN:
    LOG("%s:*** WARNING ***  CAN (unable to process received frame: received frame dropped)\r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: *** WARNING *** lucResponse = 0x%02X UNKNOWN\r\n", __FUNCTION__, lucResponse);
    break;
  }

  // At this point the received frame (minus the checksum) has been saved to the Rx buffer
  // Display the received frame
  //LOG("%s: gtZWaveRxInterface.buffer_len=%d \t ZWaveSerialFrame->len=%d\r\n", __FUNCTION__, gtZWaveRxInterface.buffer_len, ZWaveSerialFrame->len);
  LOG("-----------------------  Z-Wave received frame (minus checksum) START -----------------------\r\n");
  PrintBytes(gtZWaveRxInterface.buffer, gtZWaveRxInterface.buffer_len, false, 0);
  //PrintBytes(ZWaveSerialFrame, ZWaveSerialFrame->len, false, 0); // length off by -1 because SOF excluded
  LOG("-----------------------  Z-Wave received frame (minus checksum)  END  -----------------------\r\n");

  // Set ZWave Rx state to SOF
  //LOG("%s: Transitioning from CHECKSUM to SOF\r\n", __FUNCTION__);
  gtZWaveRxInterface.state = ZWAVE_RX_SOF;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Transmit ACK (checksum OK), NAK (checksum error) or CAN (unable to process received frame: received frame dropped)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //HAL_UART_Transmit(&huart2, (uint8_t *)&lucResponse, 1, 1);

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
  //LOG("%s: Transitioning to SOF\r\n", __FUNCTION__);
  gtZWaveRxInterface.state = ZWAVE_RX_SOF;

  // Disable Rx active
  gtZWaveRxInterface.rx_active = false;  // Not really active now...

  // Reset ACK timeout
  gtZWaveRxInterface.ack_timeout = false;

  // Reset byte timeout
  gtZWaveRxInterface.byte_timeout = false;

  // Stop ACK timer
  // Stop byte timer

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
      // Stop byte timer
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
  * @brief  Parse received FIFO bytes from Z-Wave controller
  * @param  aucIsACKRequired - TRUE if received frame should be ACKed; FALSE otherwise
  * @retval Result of parsing received bytes from Z-Wave controller
  */
ZWaveRxParseResult_t ZWave_Parse_Rx_Data(uint8_t aucIsACKRequired)
{
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
  switch (ZWaveSerialFrame->payload[2])
  {
  case APPLICATION_NODEINFO_NOT_LISTENING:
    LOG("%s: - Not listening Node, corresponds to Reporting Sleeping End Device role type \r\n", __FUNCTION__);
    break;
  case APPLICATION_NODEINFO_LISTENING:
    LOG("%s: - Always On Node, corresponds to Always On (AOS) role type \r\n", __FUNCTION__);
    break;
  case APPLICATION_FREQ_LISTENING_MODE_1000ms:
    LOG("%s: - Frequently Listening, corresponds to FLiRS role type. Wakes up every 1000ms \r\n", __FUNCTION__);
    break;
  case APPLICATION_FREQ_LISTENING_MODE_250ms:
    LOG("%s: -  Frequently Listening, corresponds to FLiRS role type. Wakes up every 250ms \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** device option mask 0x%02X is UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);
    break;
  }

  // ----------------- Generic node type -----------------
  LOG("%s: Generic node type         = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
  switch (ZWaveSerialFrame->payload[3])
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
    LOG("%s: - *** WARNING *** generic device type 0x%02X UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
    break;
  }

  // ----------------- Specific node type -----------------
  LOG("%s: Specific node type        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  // MAB 2025.10.24 - For now just list specific types for static controllers
  if (GENERIC_TYPE_STATIC_CONTROLLER == ZWaveSerialFrame->payload[3])
  {
    switch (ZWaveSerialFrame->payload[4])
    {
    case SPECIFIC_TYPE_NOT_USED:
      LOG("%s: - Specific Device Class Not Used \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_PC_CONTROLLER:
      LOG("%s: - Central Controller Device Type \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_SCENE_CONTROLLER:
      LOG("%s: - Scene Controller \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_STATIC_INSTALLER_TOOL:
      LOG("%s: - Static installer tool \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_SET_TOP_BOX:
      LOG("%s: - Set Top Box \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_SUB_SYSTEM_CONTROLLER:
      LOG("%s: - Sub System Controller \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_TV:
      LOG("%s: - Television \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_GATEWAY:
      LOG("%s: - Gateway \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** Specific device type 0x%02X UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
      break;
    }
  }

  // ----------------- Command class list length -----------------
  LOG("%s: Command class list length = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
  int i = ZWaveSerialFrame->payload[9];

  // ----------------- Retained reset info  (doesn't seem to be used) -----------------
uint32_t lulZpalRetentionResetInfo = (0x1000000*ZWaveSerialFrame->payload[6+i]) +
                                       (  0x10000*ZWaveSerialFrame->payload[7+i]) +
                                       (    0x100*ZWaveSerialFrame->payload[8+i]) +
                                       (          ZWaveSerialFrame->payload[9+i]) ;
  LOG("%s: Retained reset info       = 0x%08X\r\n", __FUNCTION__, lulZpalRetentionResetInfo);

}
// end ZWave_REQ_CMD_0A_Serial_API_Started

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
    for (int i = 0; i < ZWaveSerialFrame->payload[2]; ++i)
    {
      LOG("%s: SerialAPI node[%02d]  = 0x%02X\r\n", __FUNCTION__, i, ZWaveSerialFrame->payload[3+i]);
    }
  }

  // ----------------- chip_type -----------------
  LOG("%s: SerialAPI chip_type    = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3 +  ZWaveSerialFrame->payload[2]]);
  switch (ZWaveSerialFrame->payload[3 +  ZWaveSerialFrame->payload[2]])

  // ----------------- chip_version -----------------
  LOG("%s: SerialAPI chip_version = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4 +  ZWaveSerialFrame->payload[2]]);

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
        LOG("%s: - CMD 0x%02X implemented in Z-Wave controller\r\n", __FUNCTION__, lucSupportedCMD);
      }
    }
  }

}
// end ZWave_RES_CMD_07_Serial_API_Get_Capabilities

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

  // ----------------- SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET -----------------
  if (SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET == ZWaveSerialFrame->payload[0])
  {
    uint16_t luiReadout        = 0x100*ZWaveSerialFrame->payload[1] + ZWaveSerialFrame->payload[2];
    LOG("%s: Readout   = 0x%04X\r\n", __FUNCTION__, luiReadout);
  }

  // ----------------- SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT -----------------
  if (SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT == ZWaveSerialFrame->payload[0])
  {
    uint16_t luiPowerLevel        = 0x100*ZWaveSerialFrame->payload[1] + ZWaveSerialFrame->payload[2];
    uint16_t luiPower0dbmMeasured = 0x100*ZWaveSerialFrame->payload[3] + ZWaveSerialFrame->payload[4];
    LOG("%s: Power level                = 0x%04X\r\n", __FUNCTION__, luiPowerLevel);
    LOG("%s: Power level 0 dbm measured = 0x%04X\r\n", __FUNCTION__, luiPower0dbmMeasured);
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
    LOG("%s: - *** WARNING *** Library byte 0x%02X is unknown or deprecated \r\n", __FUNCTION__);
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
}
// end ZWave_RES_CMD_28_NVR_Get_Value

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
  LOG("%s: Network capabilities = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[0]);

  // ----------------- Network security -----------------
  LOG("%s: Network security     = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[1]);

  // ----------------- Reserved -----------------
  LOG("%s: Reserved byte        = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[2]);

  // ----------------- Basic node type -----------------
  LOG("%s: Basic    device type = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
  switch (ZWaveSerialFrame->payload[3])
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
    LOG("%s: - *** WARNING *** Basic node type 0x%02X is UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[3]);
    break;
  }

  // ----------------- Generic node type -----------------
  LOG("%s: Generic  device type = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
  switch (ZWaveSerialFrame->payload[4])
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
    LOG("%s: - *** WARNING *** generic device type 0x%02X UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[4]);
    break;
  }

  // ----------------- Specific node type -----------------
  LOG("%s: Specific device type = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
  // MAB 2025.10.24 - For now just list specific types for static controllers
  if (GENERIC_TYPE_STATIC_CONTROLLER == ZWaveSerialFrame->payload[4])
  {
    switch (ZWaveSerialFrame->payload[5])
    {
    case SPECIFIC_TYPE_NOT_USED:
      LOG("%s: - Specific Device Class Not Used \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_PC_CONTROLLER:
      LOG("%s: - Central Controller Device Type \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_SCENE_CONTROLLER:
      LOG("%s: - Scene Controller \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_STATIC_INSTALLER_TOOL:
      LOG("%s: - Static installer tool \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_SET_TOP_BOX:
      LOG("%s: - Set Top Box \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_SUB_SYSTEM_CONTROLLER:
      LOG("%s: - Sub System Controller \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_TV:
      LOG("%s: - Television \r\n", __FUNCTION__);
      break;
    case SPECIFIC_TYPE_GATEWAY:
      LOG("%s: - Gateway \r\n", __FUNCTION__);
      break;
    default:
      LOG("%s: - *** WARNING *** Specific device type 0x%02X UNKNOWN \r\n", __FUNCTION__, ZWaveSerialFrame->payload[5]);
      break;
    }
  }

  LOG("%s: extInfo              = 0x%02X\r\n", __FUNCTION__, ZWaveSerialFrame->payload[6]);

}
// end ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info

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
  * @brief  Command handler for CMD 0xA6 FUNC_ID_ZW_IS_VIRTUAL_NODE ZW->HOST: Cmd | retVal
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_A6_ZW_Is_Virtual_Node(void)
{
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
  case 0x00:
    LOG("%s: - Auto \r\n", __FUNCTION__);
    break;
  case 0x01:
    LOG("%s: - Bypass \r\n", __FUNCTION__);
    break;
  case 0x02:
    LOG("%s: - DC/DC Low Noise \r\n", __FUNCTION__);
    break;
  default:
    LOG("%s: - *** WARNING *** unknown value\r\n", __FUNCTION__);
    break;
  }
}
// end ZWave_RES_CMD_DE_Get_DCDC_Config

/** *****************************************************************************************************************************
  * @brief  Dummy command handler for unsupported Z-Wave command
  * @param  None
  * @retval None
  */
void ZWave_RES_CMD_XX_Unsupported(void)
{
  LOG("%s: *** WARNING *** Command 0x%02X not supported (yet)... \r\n", __FUNCTION__, ZWaveSerialFrame->cmd);
}
// end ZWave_RES_CMD_XX_Unsupported

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
      IF any callback requests are pending
        Transmit request
        Set state to CALLBACK_TX_SERIAL
      ELSE IF any command requests are pending
        Transmit request
        Set state to COMMAND_TX_SERIAL
      ELSE IF a frame has been received
        Set state to FRAME_PARSE
      ENDIF

    ELSE IF state is FRAME_PARSE
      Invoke the handler for the received command
      Set state to IDLE

    ELSE IF state is TX_SERIAL
      IF the response was ACKed
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
      IF the callback was ACKed
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
       IF the command was ACKed
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
      // IF any callback requests are pending
      if (gstructCallbackQueue.requestCnt)
      {
        // Transmit request

        // Set state to CALLBACK_TX_SERIAL
        LOG("%s: Transitioning from IDLE to CALLBACK_TX_SERIAL\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_CALLBACK_TX_SERIAL;
      }
      // ELSE IF any command requests are pending
      else if (gstructCommandQueue.requestCnt)
      {
        // Transmit request

        // Set state to COMMAND_TX_SERIAL
        LOG("%s: Transitioning from IDLE to COMMAND_TX_SERIAL\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_COMMAND_TX_SERIAL;
      }
      // ELSE IF a frame has been received
      else if (ZWave_Parse_Rx_Data(true) == ZWAVE_RX_PARSE_FRAME_RECEIVED)
      {
        // Set state to FRAME_PARSE
        LOG("%s: Transitioning from IDLE to FRAME_PARSE\r\n", __FUNCTION__);
        leZWaveState = ZWAVE_FRAME_PARSE;
      }
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is FRAME_PARSE
    else if (ZWAVE_FRAME_PARSE == leZWaveState)
    {
      // Invoke the handler for the received command
      //LOG("%s: Invoke the handler (callback routine) for the received frame (from the Z-Wave controller)...\r\n", __FUNCTION__);
      gtZWave_CMD_Callback[ZWaveSerialFrame->cmd]();

      // Set state to IDLE
      LOG("%s: Transitioning from FRAME_PARSE to IDLE\r\n", __FUNCTION__);
      leZWaveState = ZWAVE_IDLE;
    }

    //-------------------------------------------------------
    // ELSE IF state is TX_SERIAL
    else if (ZWAVE_TX_SERIAL == leZWaveState)
    {
      // IF the response was ACKed
        // Reset retry count
        // Set state to IDLE
      // ELSE IF TX timeout
        // Increment retry count
        // IF retry count < maximum retry count
          // Retransmit the response
        // ELSE
          // Reset retry count
          // Set state to IDLE
        // ENDIF
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is CALLBACK_TX_SERIAL
    else if (ZWAVE_CALLBACK_TX_SERIAL == leZWaveState)
    {
      // IF the callback was ACKed
        // Pop the request from the callback queue
        // Reset retry count
        // Set state to IDLE
      // ELSE IF TX timeout
        // Increment retry count
        // IF retry count < maximum retry count
           // Retransmit the request
        // ELSE
          // Pop the request from the callback queue
          // Reset retry count
          // Set state to IDLE
        // ENDIF
      // ENDIF
    }

    //-------------------------------------------------------
    // ELSE IF state is COMMAND_TX_SERIAL
    else if (ZWAVE_COMMAND_TX_SERIAL == leZWaveState)
    {
      // IF the command was ACKed
       // Pop the request from the command queue
       // Reset retry count
       // Set state to IDLE
     // ELSE IF TX timeout
       // Increment retry count
       // IF retry count < maximum retry count
         // Retransmit the request
       // ELSE
         // Pop the request from the command queue
         // Reset retry count
         // Set state to IDLE
       // ENDIF
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

  //
  // Initialize RTC
  //
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

  // Arm the Diagnostic interrupt for the first byte
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

  /* Infinite loop */
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

      LOG("%s: ............... RTC is 20%02d.%02d.%02d %02d:%02d:%02d UTC ...............\r\n",
          __FUNCTION__,
          sMainRTCDate.Year,  sMainRTCDate.Month,   sMainRTCDate.Date,
          sMainRTCTime.Hours, sMainRTCTime.Minutes, sMainRTCTime.Seconds);

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

  LOG("%s: initializing...\r\n", __FUNCTION__);

  // Arm the Z-Wave controller interrupt for the first byte
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

  // Initialize command callback array
  for (int i = 0; i < 256; ++i)
  {
    gtZWave_CMD_Callback[i] = ZWave_RES_CMD_XX_Unsupported;
  }
  gtZWave_CMD_Callback[FUNC_ID_SERIAL_API_GET_INIT_DATA]       = ZWave_RES_CMD_02_Get_Init_Data;
  gtZWave_CMD_Callback[FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES] = ZWave_RES_CMD_05_ZW_Get_Controller_Capabilities;
  gtZWave_CMD_Callback[FUNC_ID_SERIAL_API_GET_CAPABILITIES]    = ZWave_RES_CMD_07_Serial_API_Get_Capabilities;
  gtZWave_CMD_Callback[FUNC_ID_SERIAL_API_STARTED]             = ZWave_REQ_CMD_0A_Serial_API_Started;
  gtZWave_CMD_Callback[FUNC_ID_SERIAL_API_SETUP]               = ZWave_RES_CMD_0B_Serial_API_Setup;
  gtZWave_CMD_Callback[FUNC_ID_ZW_GET_VERSION]                 = ZWave_RES_CMD_15_ZW_Get_Version;
  gtZWave_CMD_Callback[FUNC_ID_MEMORY_GET_ID]                  = ZWave_RES_CMD_20_Memory_Get_ID;
  gtZWave_CMD_Callback[FUNC_ID_NVR_GET_VALUE]                  = ZWave_RES_CMD_28_NVR_Get_Value;
  gtZWave_CMD_Callback[FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO]      = ZWave_RES_CMD_41_ZW_Get_Node_Protocol_Info;
  gtZWave_CMD_Callback[FUNC_ID_ZW_GET_SUC_NODE_ID]             = ZWave_RES_CMD_56_ZW_Get_SUC_Node_ID;
  gtZWave_CMD_Callback[FUNC_ID_ZW_IS_VIRTUAL_NODE]             = ZWave_RES_CMD_A6_ZW_Is_Virtual_Node;
  gtZWave_CMD_Callback[FUNC_ID_SERIAL_API_GET_LR_NODES]        = ZWave_RES_CMD_DA_Serial_API_Get_LR_Nodes;
  gtZWave_CMD_Callback[FUNC_ID_GET_DCDC_CONFIG]                = ZWave_RES_CMD_DE_Get_DCDC_Config;
  //gtZWave_CMD_Callback[FUNC_ID_GET_RADIO_PTI] = xxxxxxxxxxxxxxxxx;
  //gtZWave_CMD_Callback[xxxxxxxxxxxxxxxxxx] = xxxxxxxxxxxxxxxxx;

  // Initialize Z-Wave SerialAPI state machine
  ZWave_SerialAPI_StateMachine(ZWAVE_SM_CMD_INITIALIZE);

  /* Infinite loop */
  for(;;)
  {
    ///////////////////////////////
    // updates on seconds
    if (lucOldSecond != sMainRTCTime.Seconds)
    {
      lucOldSecond = sMainRTCTime.Seconds;

      //////////////////////////////////////////////////////////////////////////////////
      //// TEST MAB 2025.10.17
      //// Every second, transmit to the Z-Wave controller
      //static uint8_t lucZWaveTxBuffer[5];
      //LOG("%s: Transmitting 'Hello' to Z-Wave controller\r\n", __FUNCTION__);
      //HAL_UART_Transmit(&huart2, (uint8_t *)"Hello", 5, 5);
      //LOG("%s: Transmitting NAK to Z-Wave controller\r\n", __FUNCTION__);
      //lucZWaveTxBuffer[0] = 0x15; // NAK
      //HAL_UART_Transmit(&huart2, (uint8_t *)lucZWaveTxBuffer, 1, 1);
      //////////////////////////////////////////////////////////////////////////////////

    } // updates on seconds
    ///////////////////////////////
    // updates on minutes
    if (lucOldMinute != sMainRTCTime.Minutes)
    {
      lucOldMinute = sMainRTCTime.Minutes;
    } // updates on minutes
    ///////////////////////////////
    // updates on hours
    if (lucOldHour != sMainRTCTime.Hours)
    {
      lucOldHour = sMainRTCTime.Hours;
    } // updates on hours
    ///////////////////////////////

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
