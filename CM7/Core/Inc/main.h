/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DIAG_TX_Pin GPIO_PIN_6
#define DIAG_TX_GPIO_Port GPIOB
#define LED_GREEN1_Pin GPIO_PIN_3
#define LED_GREEN1_GPIO_Port GPIOD
#define DIAG_RX_Pin GPIO_PIN_7
#define DIAG_RX_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_13
#define LED_RED_GPIO_Port GPIOI
#define FRAME_RATE_Pin GPIO_PIN_3
#define FRAME_RATE_GPIO_Port GPIOG
#define TP_IRQ_Pin GPIO_PIN_2
#define TP_IRQ_GPIO_Port GPIOG
#define TP_IRQ_EXTI_IRQn EXTI2_IRQn
#define MCU_ACTIVE_Pin GPIO_PIN_6
#define MCU_ACTIVE_GPIO_Port GPIOA
#define LED_GREEN2_Pin GPIO_PIN_2
#define LED_GREEN2_GPIO_Port GPIOJ

/* USER CODE BEGIN Private defines */

// Let LOG debug macros work outside main.c/.h
extern UART_HandleTypeDef huart1;
extern char gucDiagnosticOutput[2048];
extern osMutexId DiagnosticMutexHandle;
extern osMutexId DiagnosticRxQueueHandle;
extern osMutexId ZWaveRxQueueHandle;

#define LOG(...) {\
  osMutexWait(DiagnosticMutexHandle, 5000);\
  sprintf(gucDiagnosticOutput, __VA_ARGS__);\
  HAL_UART_Transmit(&huart1, (uint8_t *)gucDiagnosticOutput, strlen(gucDiagnosticOutput), strlen(gucDiagnosticOutput));\
  osMutexRelease(DiagnosticMutexHandle);\
}

#define LOG_NOW(...) {\
  sprintf(gucDiagnosticOutput, __VA_ARGS__);\
  HAL_UART_Transmit(&huart1, (uint8_t *)gucDiagnosticOutput, strlen(gucDiagnosticOutput), strlen(gucDiagnosticOutput));\
}

#define LOGV1 LOG
#define LOGV2

//
// Intertask message IDs
//
typedef enum Intertask_messages
{
  msgid_NOP = 0,

  // for Z-Wave Sentinel
  msgid_EXTI_MAIN_MAINS_POWER_TRIGGERED,  // no payload
  msgid_MAIN_PRINT_ELAPSED_TIME,      // no payload
  msgid_MAIN_INPUT_START_CALIBRATION, // no payload
  msgid_MAIN_INPUT_ENABLE_ALARM_CHECKS,// no payload
  msgid_MAIN_OUTPUT_POWER_LED_ON,     // no payload
  msgid_MAIN_OUTPUT_POWER_LED_OFF,    // no payload
  msgid_MAIN_OUTPUT_ONLINE_LED_ON,    // no payload
  msgid_MAIN_OUTPUT_ONLINE_LED_OFF,   // no payload
  msgid_MAIN_OUTPUT_ALARM_LED_ON,     // no payload
  msgid_MAIN_OUTPUT_ALARM_LED_OFF,    // no payload
  msgid_MAIN_OUTPUT_LAMP_TEST_ON,     // no payload
  msgid_MAIN_OUTPUT_LAMP_TEST_OFF,    // no payload
  msgid_MAIN_OUTPUT_RTC_STOPPED,      // no payload
  msgid_MAIN_OUTPUT_ALARM_EXISTS,         // no payload
  msgid_MAIN_OUTPUT_NO_ALARM,             // no payload
  msgid_MAIN_OUTPUT_UNACKED_ALARM_EXISTS, // no payload
  msgid_MAIN_OUTPUT_NO_UNACKED_ALARM,     // no payload
  msgid_MAIN_OUTPUT_RESET_IN_PROGRESS, // no payload
  msgid_MAIN_TOUCHGFX_SET_CURRENT_ZONE,   // 1 payload: new current zone (within zone limits)
  msgid_MAIN_TOUCHGFX_STANDBY_UPDATE,   // no payload
  msgid_INPUT_CHECK_MAINS_CONNECTED,  // no payload
  msgid_INPUT_MAIN_RESET_BUTTON_PRESSED, // no payload
  msgid_MAIN_NETWORK_UPDATE_RTC,      // no payload
  msgid_MAIN_NETWORK_PING,            // no payload
  msgid_MAIN_NETWORK_UPDATE_POST,     // no payload
  msgid_MAIN_FLASH_DIAGNOSTIC_ON,     // no payload
  msgid_MAIN_FLASH_DIAGNOSTIC_OFF,    // no payload
  msgid_MAIN_FLASH_COPY_APP,          // no payload
  msgid_MAIN_FLASH_WRITE_APP,         // no payload
  msgid_MAIN_FLASH_APP_CRC,           // no payload
  msgid_MAIN_FLASH_ERASE,             // no payload
  msgid_MAIN_REBOOT,                  // no payload
  msgid_NETWORK_MAIN_ONLINE_STATE,      // 1 payload: online state
  msgid_NETWORK_OUTPUT_ONLINE_STATE,    // 1 payload: online state
  msgid_NETWORK_OUTPUT_CONNECTION_ERROR,// 1 payload: network connection error code
  msgid_NETWORK_OUTPUT_INITIALIZE_ERROR,// no payload
  msgid_TOUCHGFX_MAIN_SET_CURRENT_ZONE,   // 1 payload: new current zone (within zone limits)
} Intertask_message_ID;

//
// Z-Wave SerialAPI state machine commands
//
typedef enum ZWave_state_machine_commands
{
  ZWAVE_SM_CMD_INITIALIZE,
  ZWAVE_SM_CMD_RUN,
  ZWAVE_SM_CMD_STATE,
} ZWaveStateMachineCommand;

//
// Z-Wave SerialAPI states
//
typedef enum ZWave_state
{
  ZWAVE_IDLE,
  ZWAVE_FRAME_PARSE,
  ZWAVE_TX_SERIAL,
  ZWAVE_CALLBACK_TX_SERIAL,
  ZWAVE_COMMAND_TX_SERIAL,
} ZWaveState;

typedef enum ZWave_Rx_State
{
  ZWAVE_RX_SOF,
  ZWAVE_RX_LEN,
  ZWAVE_RX_TYPE,
  ZWAVE_RX_CMD,
  ZWAVE_RX_DATA,
  ZWAVE_RX_CHECKSUM,
} ZWaveRxState;

#define RECEIVE_BUFFER_SIZE     180
#define FRAME_LENGTH_MIN        3
#define FRAME_LENGTH_MAX        RECEIVE_BUFFER_SIZE

typedef enum ZWave_Rx_Parse_Result
{
  ZWAVE_RX_PARSE_IDLE,             // returned if nothing special has happened
  ZWAVE_RX_PARSE_FRAME_RECEIVED,   // returned when a valid frame has been received
  ZWAVE_RX_PARSE_FRAME_SENT,       // returned if frame was ACKed by the other end
  ZWAVE_RX_PARSE_FRAME_ERROR,      // returned if frame has error in Checksum
  ZWAVE_RX_PARSE_RX_TIMEOUT,       // returned if Rx timeout has happened
  ZWAVE_RX_PARSE_TX_TIMEOUT        // returned if Tx timeout (waiting for ACK) ahs happened
} ZWaveRxParseResult_t;

typedef struct {
  uint8_t ack_timeout;
  uint32_t ack_timeout_ms;
  uint8_t byte_timeout;
  uint32_t byte_timeout_ms;
  ZWaveRxState state;
  uint8_t expect_bytes;
  uint8_t ack_needed;
  uint8_t buffer_len;
  uint8_t buffer[RECEIVE_BUFFER_SIZE];
  uint8_t rx_active;
  uint8_t rx_wait_count;
} ZWaveRxInterface_t;

typedef struct {
  uint8_t sof;
  uint8_t len;
  uint8_t type;
  uint8_t cmd;
  uint8_t payload[UINT8_MAX];
} ZWaveTxFrame_t;

// Z-Wave buffer sizes
#define BUF_SIZE_RX 168
#define BUF_SIZE_TX 168

typedef struct {
  uint8_t sof;
  uint8_t len;
  uint8_t type;
  uint8_t cmd;
  uint8_t payload[RECEIVE_BUFFER_SIZE]; //size defined to fix SonarQube errors
} *ZWaveInterfaceFrame_ptr;

extern ZWaveInterfaceFrame_ptr const ZWaveSerialFrame;

/////////////////////////////
//// TEST MAB 2025.10.03
//// Temporary "saved" IP address bytes
extern uint8_t gucAddress1Saved;
extern uint8_t gucAddress2Saved;
extern uint8_t gucAddress3Saved;
extern uint8_t gucAddress4Saved;
extern uint8_t gucDNS1Saved;
extern uint8_t gucDNS2Saved;
extern uint8_t gucDNS3Saved;
extern uint8_t gucDNS4Saved;
extern uint8_t gucGateway1Saved;
extern uint8_t gucGateway2Saved;
extern uint8_t gucGateway3Saved;
extern uint8_t gucGateway4Saved;
extern uint8_t gucNetmask1Saved;
extern uint8_t gucNetmask2Saved;
extern uint8_t gucNetmask3Saved;
extern uint8_t gucNetmask4Saved;

//// Temporarily saved DHCP
extern uint8_t gucIsDHCPEnabled;

//// Temporary Standby mode and countdown
// JJR-STANDBY
#define DEFAULT_STANDBY_MINS    60
typedef enum  {eSTANDBY_OFF=0,  eSTANDBY_CONFIG,  eSTANDBY_NO_COUNTDOWN} eStandbyMode;
#define DEFAULT_STANDBY_MODE    eSTANDBY_OFF
#define STANDBY_NO_COUNTDOWN    (0xFFFFFFFF)                            /* Do not count down, stay in standby mode */
//  Standby Mode
extern uint8_t gucStandbyMode;
extern uint32_t gulStandbyCounter;  /* In Standby mode when When <> 0 */

// ZWave TX/RX buffers
#define SERIAL_BUFFER_SIZE (4096)
//extern uint8_t gucZWaveTxBuffer[SERIAL_BUFFER_SIZE];
extern uint8_t gucZWaveRxBuffer[SERIAL_BUFFER_SIZE];

/////////////////////////////


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
