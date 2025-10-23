/*
 * File:        	$Id: gconfig.h,v 1.308 2019/12/13 15:18:49 mbersalona Exp $
 * Authors:     	Derek Ramsey
 * Maintainers:		Derek Ramsey
 * Description:  	This file contains the global configuration options used for the product.
 *
 * Revisions:		See the CVS repository
 */
#ifndef _GCONFIG_H_
#define _GCONFIG_H_

//#if !defined(PRODUCT_TYPE_SENSAPHONE_ZWAVE_SENTINEL)
//#include "buildconfig.h"
//#include "IMSoptions.h"
//#endif



//
// Note: MQX_PROJ if defined in the CW10 projects settings
//
#ifdef	MQX_PROJ
 #define ONLY_DEBUG_OPTIONS	1	// Define this in source files that only need the DEBUG options, without including the MQX headers which conflict with Sensaphone code
 #include "mss_ssl_client.h"
 #undef	ONLY_DEBUG_OPTIONS
#endif



/*******************************************************************************
 * Configuration Defines
 ******************************************************************************/
#define IS_DEMO_VERSION         (0) 

#define REVERSE_OUTPUT_POLARITY (0)   // 0 for Web600 output relay normally open; 1 for Web600 output relay normally closed

//
// Product Type:
//
#define	SENSAPHONE_WIRELESS_NODE        (1)
#define SENSAPHONE_WEB600               (2)
#define SENSAPHONE_WEB800               (3)
#define SENSAPHONE_SAT6                 (4)
#define SENSAPHONE_WEB_WIRELESS         (5) // WSG30
#define SENSAPHONE_WIRED_NODE           (6)
#define SENSAPHONE_VIDEO_OVLY_MONITOR	(8)
#define SENSAPHONE_DRY_CONTACT_NODE     (9)
#define WINDCHIME_BASELINE              (10)
#define WINDCHIME_MODBUS		(11)
#define WINDCHIME_STRATUS               (12)
#define SENSAPHONE_400_CELLULAR         (13)
#define SENSAPHONE_ZWAVE_SENTINEL       (14)
	
#if defined(PRODUCT_TYPE_SENSAPHONE_400_CELLULAR)
  #define PRODUCT_TYPE (SENSAPHONE_400_CELLULAR)
#endif
//#if defined(PRODUCT_TYPE_SENSAPHONE_ZWAVE_SENTINEL)
  #define PRODUCT_TYPE (SENSAPHONE_ZWAVE_SENTINEL)
//#endif

#if PRODUCT_TYPE == WINDCHIME_BASELINE || PRODUCT_TYPE == WINDCHIME_MODBUS || PRODUCT_TYPE == WINDCHIME_STRATUS
#define VERSION_A		  "1"
#define VERSION_B		  "7"
#define VERSION_C		  "21"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x01071500
#elif PRODUCT_TYPE == SENSAPHONE_WEB600 && REVERSE_OUTPUT_POLARITY==1
#define VERSION_A		  "9"
#define VERSION_B		  "6"
#define VERSION_C		  "3"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x01060200
#elif PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS
#define VERSION_A                 "1"
#define VERSION_B                 "7"
#define VERSION_C                 "3"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x01070300
#elif PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
# ifdef SENSAPHONE_400_CELLULAR_BOOTLOADER
#define VERSION_A                 "0"
#define VERSION_B                 "1"
#define VERSION_C                 "6"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x00010600
#define VERSION_DATE             "2020.02.21"
# else // Application version, not bootloader version
#define VERSION_A                 "1"
#define VERSION_B                 "1"
#define VERSION_C                 "2"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x01010200
#define VERSION_DATE             "2025.08.22"
# endif
#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
# ifdef SENSAPHONE_ZWAVE_SENTINEL_BOOTLOADER
#define VERSION_A                 "0"
#define VERSION_B                 "1"
#define VERSION_C                 "6"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x00010600
#define VERSION_DATE             "2020.02.21"
# else // Application version, not bootloader version
#define VERSION_A                 "0"
#define VERSION_B                 "0"
#define VERSION_C                 "1"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x00000100
#define VERSION_DATE             "2025.10.23"
# endif
#else
#define VERSION_A		  "1"
#define VERSION_B		  "6"
#define VERSION_C		  "4"
#define VERSION_D                 "" __DATE__ " " __TIME__
#define VERSION_ENCODED           0x01060400
#endif
#if PRODUCT_TYPE != SENSAPHONE_400_CELLULAR
#ifdef DEBUG_LEVEL
# if DEBUG_LEVEL > 0
#  undef VERSION_A
#  define VERSION_A "0"
# endif
#endif
#endif // if PRODUCT_TYPE != SENSAPHONE_400_CELLULAR
#define SOFTWARE_VERSION          "v." VERSION_A "." VERSION_B "." VERSION_C

// Any version less than this will be reset-to-defaults
#if PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
#define VERSION_RESET_TO_DEFAULTS   0x00010900
#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
#define VERSION_RESET_TO_DEFAULTS   0x00000100
#else
#define VERSION_RESET_TO_DEFAULTS   0x01050604
#endif

// For 400 Cellular, define product name, company name, web name, etc.
#if PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
#define PRODUCT_NAME        "Sentry"
#define PRODUCT_COMPANY     "Sensaphone"
#define PRODUCT_WEB_NAME    PRODUCT_NAME
#define PRODUCT_WEB_COMPANY PRODUCT_COMPANY
#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
#define PRODUCT_NAME        "ZWave Sentinel"
#define PRODUCT_COMPANY     "Sensaphone"
#define PRODUCT_WEB_NAME    PRODUCT_NAME
#define PRODUCT_WEB_COMPANY PRODUCT_COMPANY
#endif

// Default Authorization UUID
#define DEFAULT_AUTHORIZATION_UUID ""

//
// Born on Date
//
#define BORN_ON_DAY           1
#define BORN_ON_DAY_BCD       0x01
#define BORN_ON_DAY_OF_WEEK   0x01 // 1 = Sunday
#define BORN_ON_MONTH         1
#define BORN_ON_MONTH_BCD     0x01
#define BORN_ON_YEAR          2012
#define BORN_ON_YEAR_BCD      0x12

#define SENSAPHONE_MIN	(SENSAPHONE_WIRELESS_NODE)
#define SENSAPHONE_MAX	(SENSAPHONE_ZWAVE_SENTINEL)

#ifndef ASM_SOURCE
# ifndef PRODUCT_TYPE
#  error The product type was not defined!
#  define PRODUCT_TYPE	(SENSAPHONE_WEB600)
# endif
# if PRODUCT_TYPE == SENSAPHONE_WIRED_NODE
//#  undef PRODUCT_TYPE
//#  define PRODUCT_TYPE SENSAPHONE_DRY_CONTACT_NODE
# endif
#endif


//
// Board Revision Configuration
//
#if PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE || PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS
# define BOARD_REVISION            'C'
#elif PRODUCT_TYPE == SENSAPHONE_WEB600
# define BOARD_REVISION            'H'
#elif PRODUCT_TYPE == SENSAPHONE_WEB800
# define BOARD_REVISION            'A'
#elif PRODUCT_TYPE == SENSAPHONE_SAT6
# define BOARD_REVISION            'A'
#elif PRODUCT_TYPE == SENSAPHONE_WIRED_NODE
# define BOARD_REVISION            'A'
#elif PRODUCT_TYPE == SENSAPHONE_DRY_CONTACT_NODE
# define BOARD_REVISION            'B'
#elif PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR
  #define BOARD_REVISION            'B'
#elif PRODUCT_TYPE == WINDCHIME_BASELINE
  #define BOARD_REVISION            'F'			/* AD mux remap */
#elif PRODUCT_TYPE == WINDCHIME_MODBUS
  #define BOARD_REVISION            'B'			/* 32MB flash; MODBUS serial baud from processor; new buzzer */
#elif PRODUCT_TYPE == WINDCHIME_STRATUS
  #define BOARD_REVISION            'B'
#elif PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
  #define BOARD_REVISION            'E'
  //// TEST MAB 2024.01.10 #define BOARD_REVISION            'D'
#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
  #define BOARD_REVISION            'A'
#endif

//
// Specific Build Types (API Portability)
//
#define IS400		  	0
#define IS400800 		0
#define IS1400 			0
#define IS1800			0
#define IS4BY4			0
#if PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE  || \
    PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS   || \
    PRODUCT_TYPE == SENSAPHONE_WEB800
# define USE_WIRELESS		1
#else
# define USE_WIRELESS		0
#endif
#define HAS_PHONE_STATUS	0
#define FOURCHANNELDIALER	0		/* Is product type: 400/800/1400 Family? */
#define IS_8051           0   /* Is this an 8051 processor? */
#define IS_COLDFIRE       0   /* Is this the Coldfire processor? */
#define IS_STM32          1   /* Is this a STM32 processor? */
#define	IS_MULTI_THREADED	1   /* Are we using a RTOS? */
#define IS_DEBUG_MODE     0   /* Are we using a debug mode? */


//
// Choose which build type this is
//
#define STANDARD_BUILD		0
#define DIAGNOSTIC_BUILD	1
#define TEST_MODE		STANDARD_BUILD
		//
		//   0 = Normal Build (STANDARD_BUILD)
    //   1 = Diagnostic Build (DIAGNOSTIC_BUILD)
		//

//
// Only allow diagnostic modes in the standard build
//
#if TEST_MODE != STANDARD_BUILD
# undef ENABLE_DIAGNOSTICS
# define ENABLE_DIAGNOSTICS 0
#endif

//
// Define the system interrupt intervals
//
#define	PIT_INTERVAL_MS		(10)

#ifdef	MQX_PROJ
 #define RTOS_INTERVAL_MS	(5)
#else
 #define RTOS_INTERVAL_MS	(10)
#endif

#if PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
// Task periods, msec
#define MAIN_TASK_PERIOD     50
#define INPUT_TASK_PERIOD    50
#define OUTPUT_TASK_PERIOD   50
#define NETWORK_TASK_PERIOD  50
#endif

#if PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
// Task periods, msec
#define MAIN_TASK_PERIOD     50
#define INPUT_TASK_PERIOD   100
#define OUTPUT_TASK_PERIOD  100
#define NETWORK_TASK_PERIOD 500
#define DATALOG_TASK_PERIOD 500
#define ZWAVE_TASK_PERIOD    50
#endif


#define DO_ETHERNET_AUTONEGOTIATE

#define RUN_NONDESTRUCTIVE_SPI_FLASH_TEST 0  // if 1, disable MODBUS task and run SPI Flash test on startup

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Task Configuration
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NO_TASK                 -1

//
// Task Assignments
//
#define UTASKER_TASK            0
#if PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS
  // ********************
  // Standalone Wireless
  // ********************
  #define ALARM_HANDLER_TASK     1
  #define ADC_TASK               NO_TASK
  #define ZIGBEE_WIRELESS_TASK   2
  #define LCD_TASK               4
  #define DATALOG_TASK           3
  #define RTC_TASK               5
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   7
  #else
  # define MAX_PROCESSES	7  // Total Number of tasks
  #endif
#elif PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE
  // ********************
  // Wireless Node
  // ********************
  #define ZIGBEE_WIRELESS_TASK   1
  #define LCD_TASK               2
  #define ALARM_HANDLER_TASK     3
  #define ADC_TASK               NO_TASK
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           NO_TASK
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          4
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   5
  #else
  # define MAX_PROCESSES	5  // Total Number of tasks
  #endif
#elif PRODUCT_TYPE == SENSAPHONE_WIRED_NODE
  // ********************
  // Wired Node
  // ********************
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define ALARM_HANDLER_TASK     1
  #define ADC_TASK               2
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           NO_TASK
  #define IMS_CONSOLE_TASK       3
  #define IP_ALARM_TASK          4
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   5
  #else
  # define MAX_PROCESSES	5  // Total Number of tasks
  #endif
#elif PRODUCT_TYPE == SENSAPHONE_DRY_CONTACT_NODE
  // ********************
  // Dry Contact Node
  // ********************
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define ALARM_HANDLER_TASK     1
  #define ADC_TASK               2
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           NO_TASK
  #define IMS_CONSOLE_TASK       3
  #define IP_ALARM_TASK          4
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   5
  #else
  # define MAX_PROCESSES	5  // Total Number of tasks
  #endif
#elif PRODUCT_TYPE == SENSAPHONE_WEB600
  // ********************
  // Web600
  // ********************
  #define ALARM_HANDLER_TASK     1
  #define ADC_TASK               4
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               3
  #define DATALOG_TASK           2
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            ((BOARD_REVISION >= 'G') ? 5 : NO_TASK)
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
     MAX_PROCESSES      .equ   6
  #else
  # define MAX_PROCESSES ((BOARD_REVISION >= 'G') ? 6 : 6)  // Total Number of tasks
  #endif
#elif PRODUCT_TYPE == SENSAPHONE_SAT6
  // ********************
  // SAT6
  // ********************
  #define ALARM_HANDLER_TASK     1
  #define ADC_TASK               NO_TASK
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           2
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   3
  #else
  # define MAX_PROCESSES	3  // Total Number of tasks
  #endif
#elif PRODUCT_TYPE == WINDCHIME_BASELINE
  // ********************
  // Windchime Baseline, i.e. Sentinel
  // ********************
  #define INIT_TASK              1
  #define ALARM_HANDLER_TASK     NO_TASK
  #define ADC_TASK               4
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           3
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
#if RUN_NONDESTRUCTIVE_SPI_FLASH_TEST==1
  #define MODBUS_TASK            NO_TASK
#else
  #define MODBUS_TASK            6
#endif
  #define EEPROM_V2_TASK         NO_TASK // TODO: Figure out what needs to be done here
  // SSL Tasks
  #define MAIN_TASK   		 5  // Processor Expert startup task for SSL

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Note: keep the following define in sync with the mss_ssl_client.h.
//		We cannot include that file here without causing errors
//		Set  USE_CMD_SHELL 1 for shell debugging in the MQX projects
/////////////////////////////////////////////////////////////////////////////////////////////////////
          
////  #define	USE_CMD_SHELL	0
  #if	USE_CMD_SHELL==0
		#define SSL_TASK							7
  	#ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   7
  	#else
					#define MAX_PROCESSES           8  // Total Number of tasks
  	#endif
	#else
		#ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   6
		#else
			#define MAX_PROCESSES     8  // Total Number of tasks
		#endif
	#endif

	#ifndef	__MQX__
		#define	__MQX__		1
	#endif
          
#elif PRODUCT_TYPE == WINDCHIME_MODBUS || PRODUCT_TYPE == WINDCHIME_STRATUS
  // ********************
  // Sentinel Pro
  // ********************
  #define INIT_TASK              1
  #define ALARM_HANDLER_TASK     NO_TASK
  #define ADC_TASK               4
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           3
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
#if RUN_NONDESTRUCTIVE_SPI_FLASH_TEST==1
  #define MODBUS_TASK            NO_TASK
#else
  #define MODBUS_TASK            6
#endif
  #define EEPROM_V2_TASK         NO_TASK // TODO: Figure out what needs to be done here
  // SSL Tasks
  #define MAIN_TASK   		 5			// Processor Expert startup task for SSL

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Note: keep the following define in sync with the mss_ssl_client.h.
//		We cannot include that file here without causing errors
//		Set  USE_CMD_SHELL 1 for shell debugging in the MQX projects
/////////////////////////////////////////////////////////////////////////////////////////////////////
          
////  #define	USE_CMD_SHELL	0
  #if	USE_CMD_SHELL==0
		#define SSL_TASK							7
  	#ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   7
  	#else
					#define MAX_PROCESSES           8  // Total Number of tasks
  	#endif
	#else
		#ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   6
		#else
			#define MAX_PROCESSES     8  // Total Number of tasks
		#endif
	#endif

	#ifndef	__MQX__
		#define	__MQX__		1
	#endif
          
#elif PRODUCT_TYPE == SENSAPHONE_WEB800
  // ********************
  // Web800
  // ********************
  #define ALARM_HANDLER_TASK     2
  #define ADC_TASK               1
  #define ZIGBEE_WIRELESS_TASK   NO_TASK // TBD: Enable this task
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               4
  #define DATALOG_TASK           5
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          3
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK // TBD: Enable this task
  #define EEPROM_V2_TASK         NO_TASK // TBD: Enable this task
  #ifdef ASM_SOURCE
          MAX_PROCESSES      .equ   6
  #else
  # define MAX_PROCESSES	 6  // Total Number of tasks
  #endif

#elif PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR
  // ********************
  // Video Overlay Monitor
  #define ALARM_HANDLER_TASK     1
  #define ADC_TASK               3
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               4
  #define DATALOG_TASK           2
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             5
  #define OUTPUT_TASK            NO_TASK
  #define MODBUS_TASK            NO_TASK
  #define EEPROM_V2_TASK         NO_TASK
  #ifdef ASM_SOURCE
        MAX_PROCESSES      .equ   6
  #else
  # define MAX_PROCESSES	 6  // Total Number of tasks
  #endif
         
#elif PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
  // ********************
  // 400 Cellular
  // ********************
  #define INIT_TASK              NO_TASK
  #define NETWORK_TASK           2
  #define ALARM_HANDLER_TASK     NO_TASK
  #define ADC_TASK               3
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           NO_TASK
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            4
  #if RUN_NONDESTRUCTIVE_SPI_FLASH_TEST==1
  #define MODBUS_TASK            NO_TASK
  #else
  #define MODBUS_TASK            NO_TASK
  #endif
  #define EEPROM_V2_TASK         NO_TASK
  // SSL Tasks
  #define MAIN_TASK              1     // Processor Expert startup task for SSL
  #ifdef ASM_SOURCE
        MAX_PROCESSES      .equ   6
  #else
  # define MAX_PROCESSES          4  // Total Number of tasks
  #endif

#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
  // ********************
  // Z-Wave Sentinel
  // ********************
  #define INIT_TASK              NO_TASK
  #define NETWORK_TASK           2
  #define ALARM_HANDLER_TASK     NO_TASK
  #define ADC_TASK               NO_TASK
  #define ZIGBEE_WIRELESS_TASK   NO_TASK
  #define LCD_TASK               NO_TASK
  #define RTC_TASK               NO_TASK
  #define DATALOG_TASK           NO_TASK
  #define IMS_CONSOLE_TASK       NO_TASK
  #define IP_ALARM_TASK          NO_TASK
  #define BOB_4_TASK             NO_TASK
  #define OUTPUT_TASK            4
  #if RUN_NONDESTRUCTIVE_SPI_FLASH_TEST==1
  #define MODBUS_TASK            NO_TASK
  #else
  #define MODBUS_TASK            NO_TASK
  #endif
  #define EEPROM_V2_TASK         NO_TASK
  // SSL Tasks
  #define MAIN_TASK              1     // Processor Expert startup task for SSL
  #ifdef ASM_SOURCE
        MAX_PROCESSES      .equ   6
  #else
  # define MAX_PROCESSES          4  // Total Number of tasks
  #endif

#elif !defined(BOOTLOADER)
# warning No Tasks Defined for this Target!
#endif


//
// UART -> Task Assignments
//
#if PRODUCT_TYPE == SENSAPHONE_WIRED_NODE || PRODUCT_TYPE == SENSAPHONE_DRY_CONTACT_NODE
# define TASK_USING_UART0       IMS_CONSOLE_TASK
#else
# define TASK_USING_UART0       NO_TASK
#endif

#if PRODUCT_TYPE == SENSAPHONE_WEB800
# define TASK_USING_UART1       MODBUS_TASK
#elif PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR
# define TASK_USING_UART1       BOB_4_TASK
#elif PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS   || \
      PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE
# define TASK_USING_UART1       ZIGBEE_WIRELESS_TASK
#else
# define TASK_USING_UART1       NO_TASK
#endif

#if PRODUCT_TYPE == SENSAPHONE_WEB800 || PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR
# define TASK_USING_UART2       ZIGBEE_WIRELESS_TASK
#elif PRODUCT_TYPE == WINDCHIME_BASELINE || PRODUCT_TYPE == WINDCHIME_MODBUS || PRODUCT_TYPE == WINDCHIME_STRATUS
# define TASK_USING_UART2       MODBUS_TASK
#else
# define TASK_USING_UART2       NO_TASK
#endif


#define IMS_CONSOLE_SERIAL_PORT (TASK_USING_UART0 == IMS_CONSOLE_TASK     ? SERIAL_PORT_1 : \
                                 TASK_USING_UART1 == IMS_CONSOLE_TASK     ? SERIAL_PORT_2 : \
                                 TASK_USING_UART2 == IMS_CONSOLE_TASK     ? SERIAL_PORT_3 : \
                                                                            SERIAL_PORT)
#define WIRELESS_SERIAL_PORT    (TASK_USING_UART0 == ZIGBEE_WIRELESS_TASK ? SERIAL_PORT_1 : \
                                 TASK_USING_UART1 == ZIGBEE_WIRELESS_TASK ? SERIAL_PORT_2 : \
                                 TASK_USING_UART2 == ZIGBEE_WIRELESS_TASK ? SERIAL_PORT_3 : \
                                                                            SERIAL_PORT)
#define DEBUG_SERIAL_PORT       (TASK_USING_UART0 == NO_TASK              ? SERIAL_PORT_1 : \
                                 TASK_USING_UART1 == NO_TASK              ? SERIAL_PORT_2 : \
                                 TASK_USING_UART2 == NO_TASK              ? SERIAL_PORT_3 : \
                                                                            SERIAL_PORT)
#define BOB_4_SERIAL_PORT       (TASK_USING_UART0 == BOB_4_TASK           ? SERIAL_PORT_1 : \
                                 TASK_USING_UART1 == BOB_4_TASK           ? SERIAL_PORT_2 : \
                                 TASK_USING_UART2 == BOB_4_TASK           ? SERIAL_PORT_3 : \
                                                                            SERIAL_PORT)
#define MODBUS_SERIAL_PORT      (TASK_USING_UART0 == MODBUS_TASK          ? SERIAL_PORT_1 : \
                                 TASK_USING_UART1 == MODBUS_TASK          ? SERIAL_PORT_2 : \
                                 TASK_USING_UART2 == MODBUS_TASK          ? SERIAL_PORT_3 : \
                                                                            SERIAL_PORT)

        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Feature Selection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Note: to disbale IMS protocol etc, set
// BUILD_SUPPORT_IMS_PROTOCOL == false below and undef USE_IMS in config.h
#if PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS
  // ********************
  // Standalone Wireless
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   true
 #define BUILD_SUPPORT_LCD            (LCD_TASK != NO_TASK)
 #define BUILD_SUPPORT_KEYPAD         (LCD_TASK != NO_TASK)
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   true
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     true
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      (DEBUG_LEVEL == 0) // Disable this to save code space in debug mode
 #define BUILD_SNMP_AGENT_SUPPORT     (DEBUG_LEVEL == 0) //
 #define BUILD_SNMP_SERVER_SUPPORT    (DEBUG_LEVEL == 0) //
 #define BUILD_SMTP_SUPPORT           true
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   true
 #define BUILD_MODBUS_MASTER_SUPPORT  false
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     true
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         true
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       true
 #define BUILD_CALCULATIONS           false 
#elif PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE
  // ********************
  // Wireless Node
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            true
 #define BUILD_SUPPORT_KEYPAD         true
 #define BUILD_SUPPORT_IMS_PROTOCOL   true
 #define BUILD_RTC_SUPPORT            false
 #define BUILD_EXTERNAL_RTC_SUPPORT   false
 #define BUILD_INT_FLASH_SUPPORT      true
 #define BUILD_SPI_FLASH_SUPPORT      false
 #define BUILD_SPI_EEPROM_SUPPORT     true
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           false
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         false
 #define BUILD_IP_ALARM_SUPPORT       true
 #define BUILD_WIRELESS_SUPPORT       true
 #define BUILD_CALCULATIONS           false 
#elif PRODUCT_TYPE == SENSAPHONE_WIRED_NODE
  // ********************
  // Wired Node
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   true
 #define BUILD_RTC_SUPPORT            false
 #define BUILD_EXTERNAL_RTC_SUPPORT   false
 #define BUILD_INT_FLASH_SUPPORT      true
 #define BUILD_SPI_FLASH_SUPPORT      false
 #define BUILD_SPI_EEPROM_SUPPORT     true
 #define BUILD_SPI_ADC_SUPPORT        true
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           false
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         false
 #define BUILD_IP_ALARM_SUPPORT       true
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           false 
#elif PRODUCT_TYPE == SENSAPHONE_DRY_CONTACT_NODE
  // ********************
  // Dry Contact Node
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   true
 #define BUILD_RTC_SUPPORT            false
 #define BUILD_EXTERNAL_RTC_SUPPORT   false
 #define BUILD_INT_FLASH_SUPPORT      true
 #define BUILD_SPI_FLASH_SUPPORT      false
 #define BUILD_SPI_EEPROM_SUPPORT     true
 #define BUILD_SPI_ADC_SUPPORT        true
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           false
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         false
 #define BUILD_IP_ALARM_SUPPORT       true
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           false 
#elif PRODUCT_TYPE == SENSAPHONE_WEB600 
  // ********************
  // Web600
  // ******************** 
 #define BUILD_LOCAL_ALARM_DELIVERY   true
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   true
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     true
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      true
 #define BUILD_SNMP_AGENT_SUPPORT     true
 #define BUILD_SNMP_SERVER_SUPPORT    true
 #define BUILD_SMTP_SUPPORT           true
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   true
 #define BUILD_MODBUS_MASTER_SUPPORT  ((BOARD_REVISION >= 'G') ? true : false)
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     true
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         true
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           ((BOARD_REVISION >= 'G') ? true : false)
#elif PRODUCT_TYPE == WINDCHIME_BASELINE
  // ********************
  // WindChime Baseline, i.e. Sentinel
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   false // We use the internal battery-backed RTC instead
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     false
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           true
 #define BUILD_HTTP_SUPPORT           false
 #define BUILD_MODBUS_SLAVE_SUPPORT   true
 #define BUILD_MODBUS_MASTER_SUPPORT  true
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         true
//// #define BUILD_IP_ALARM_SUPPORT       true
#define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           true
#elif PRODUCT_TYPE == WINDCHIME_MODBUS || PRODUCT_TYPE == WINDCHIME_STRATUS
  // ********************
  // WindChime MODBUS, i.e. Sentinel Pro; Windchime STRATUS, i.e. Stratus EMS
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   false // We use the internal battery-backed RTC instead
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     false
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           true
 #define BUILD_HTTP_SUPPORT           false
 #define BUILD_MODBUS_SLAVE_SUPPORT   true
 #define BUILD_MODBUS_MASTER_SUPPORT  true
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         true
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           true
#elif PRODUCT_TYPE == SENSAPHONE_WEB800
  // ********************
  // Web800
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   true
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   false // We use the internal battery-backed RTC instead
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     false
 #define BUILD_SPI_ADC_SUPPORT        true
 #define BUILD_SNMP_TRAP_SUPPORT      true
 #define BUILD_SNMP_AGENT_SUPPORT     true
 #define BUILD_SNMP_SERVER_SUPPORT    true
 #define BUILD_SMTP_SUPPORT           true
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   false // true
 #define BUILD_MODBUS_MASTER_SUPPORT  false // true
 #define BUILD_MODBUS_SCAN_SUPPORT    false // true
 #define BUILD_UPLOAD_PROGRAMMING     true
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         true
 #define BUILD_IP_ALARM_SUPPORT       true
 #define BUILD_WIRELESS_SUPPORT       true
 #define BUILD_CALCULATIONS           true 
#elif PRODUCT_TYPE == SENSAPHONE_SAT6
  // ********************
  // SAT6
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            false
 #define BUILD_EXTERNAL_RTC_SUPPORT   false
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      false
 #define BUILD_SPI_EEPROM_SUPPORT     false
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           false
 #define BUILD_HTTP_SUPPORT           false
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      false
 #define BUILD_LOGGER_SUPPORT         false
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           false
#elif PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR 
  // ********************
  // Video Overlay Monitor
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   true
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   false
 #define BUILD_INT_FLASH_SUPPORT      false	//	No Web pages in internal flash
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     true
 #define BUILD_SPI_ADC_SUPPORT        false // true
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           true
 #define BUILD_HTTP_SUPPORT           true
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false // true
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         true
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false // true*
 #define BUILD_CALCULATIONS           false
          
#elif PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
  // ********************
  // 400 Cellular
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   false // We use the internal battery-backed RTC instead
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     false
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           false
 #define BUILD_HTTP_SUPPORT           false
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false  // change to false eventually
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         false
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           false

#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
  // ********************
  // ZWave Sentinel
  // ********************
 #define BUILD_LOCAL_ALARM_DELIVERY   false
 #define BUILD_SUPPORT_LCD            false
 #define BUILD_SUPPORT_KEYPAD         false
 #define BUILD_SUPPORT_IMS_PROTOCOL   false
 #define BUILD_RTC_SUPPORT            true
 #define BUILD_EXTERNAL_RTC_SUPPORT   false // We use the internal battery-backed RTC instead
 #define BUILD_INT_FLASH_SUPPORT      false
 #define BUILD_SPI_FLASH_SUPPORT      true
 #define BUILD_SPI_EEPROM_SUPPORT     false
 #define BUILD_SPI_ADC_SUPPORT        false
 #define BUILD_SNMP_TRAP_SUPPORT      false
 #define BUILD_SNMP_AGENT_SUPPORT     false
 #define BUILD_SNMP_SERVER_SUPPORT    false
 #define BUILD_SMTP_SUPPORT           false
 #define BUILD_HTTP_SUPPORT           false
 #define BUILD_MODBUS_SLAVE_SUPPORT   false
 #define BUILD_MODBUS_MASTER_SUPPORT  false  // change to false eventually
 #define BUILD_MODBUS_SCAN_SUPPORT    false
 #define BUILD_UPLOAD_PROGRAMMING     false
 #define BUILD_GENERIC_INTERFACE      true
 #define BUILD_LOGGER_SUPPORT         false
 #define BUILD_IP_ALARM_SUPPORT       false
 #define BUILD_WIRELESS_SUPPORT       false
 #define BUILD_CALCULATIONS           false

#elif !defined(BOOTLOADER)
# warning No Configuration Defined for this Target!
#endif

#define FEATURE_BITMAP_1         (BUILD_LOCAL_ALARM_DELIVERY  == true ? 0x00000001 : 0x00000000)
#define FEATURE_BITMAP_2         (BUILD_SUPPORT_LCD           == true ? 0x00000002 : 0x00000000)
#define FEATURE_BITMAP_3         (BUILD_SUPPORT_KEYPAD        == true ? 0x00000004 : 0x00000000)
#define FEATURE_BITMAP_4         (BUILD_SUPPORT_IMS_PROTOCOL  == true ? 0x00000008 : 0x00000000)
#define FEATURE_BITMAP_5         (BUILD_RTC_SUPPORT           == true ? 0x00000010 : 0x00000000)
#define FEATURE_BITMAP_6         (BUILD_EXTERNAL_RTC_SUPPORT  == true ? 0x00000020 : 0x00000000)
#define FEATURE_BITMAP_7         (BUILD_INT_FLASH_SUPPORT     == true ? 0x00000040 : 0x00000000)
#define FEATURE_BITMAP_8         (BUILD_SPI_FLASH_SUPPORT     == true ? 0x00000080 : 0x00000000)
#define FEATURE_BITMAP_9         (BUILD_SPI_EEPROM_SUPPORT    == true ? 0x00000100 : 0x00000000)
#define FEATURE_BITMAP_10        (BUILD_SPI_ADC_SUPPORT       == true ? 0x00000200 : 0x00000000)
#define FEATURE_BITMAP_11        (BUILD_SNMP_TRAP_SUPPORT     == true ? 0x00000400 : 0x00000000)
#define FEATURE_BITMAP_12        (BUILD_SNMP_AGENT_SUPPORT    == true ? 0x00000800 : 0x00000000)
#define FEATURE_BITMAP_13        (BUILD_SNMP_SERVER_SUPPORT   == true ? 0x00001000 : 0x00000000)
#define FEATURE_BITMAP_14        (BUILD_SMTP_SUPPORT          == true ? 0x00002000 : 0x00000000)
#define FEATURE_BITMAP_15        (BUILD_HTTP_SUPPORT          == true ? 0x00004000 : 0x00000000)
#define FEATURE_BITMAP_16        (BUILD_MODBUS_SLAVE_SUPPORT  == true ? 0x00008000 : 0x00000000)
#define FEATURE_BITMAP_17        (BUILD_MODBUS_MASTER_SUPPORT == true ? 0x00010000 : 0x00000000)
#define FEATURE_BITMAP_18        (BUILD_UPLOAD_PROGRAMMING    == true ? 0x00020000 : 0x00000000)
#define FEATURE_BITMAP_19        (BUILD_GENERIC_INTERFACE     == true ? 0x00040000 : 0x00000000)
#define FEATURE_BITMAP_20        (BUILD_LOGGER_SUPPORT        == true ? 0x00080000 : 0x00000000)
#define FEATURE_BITMAP_21        (BUILD_IP_ALARM_SUPPORT      == true ? 0x00100000 : 0x00000000)
#define FEATURE_BITMAP_22        (BUILD_WIRELESS_SUPPORT      == true ? 0x00200000 : 0x00000000)
#define PRODUCT_FEATURE_BITMAP   (FEATURE_BITMAP_1 | FEATURE_BITMAP_2 | FEATURE_BITMAP_3 | FEATURE_BITMAP_4 | FEATURE_BITMAP_5 | FEATURE_BITMAP_6 | FEATURE_BITMAP_7 | FEATURE_BITMAP_8 | FEATURE_BITMAP_9 | FEATURE_BITMAP_10 | FEATURE_BITMAP_11 | FEATURE_BITMAP_12 | FEATURE_BITMAP_13 | FEATURE_BITMAP_14 | FEATURE_BITMAP_15 | FEATURE_BITMAP_16 | FEATURE_BITMAP_17 | FEATURE_BITMAP_18 | FEATURE_BITMAP_19 | FEATURE_BITMAP_20 | FEATURE_BITMAP_21 | FEATURE_BITMAP_22)

/*******************************************************************************
 * Hardware Peripheral Layout
 ******************************************************************************/



#if PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE || PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS
  // **************************************************************
  // IMS Wireless Node
  // **************************************************************

  // 					          PxPAR            DDRx          PORTx
  // Port/Pin 	        Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------		-------------------------------------
  //
  // PORT AN
  //  [0] AN0 (92)      1 [ADC]          N/A           N/A              A/D Input: V_supply_main
  //  [1] AN1 (91)      1 [ADC]          N/A           N/A              A/D Input: V_battery
  //  [2] AN2 (90)      1 [ADC]          N/A           N/A              A/D Input: On-board Temperature
  //  [3] AN3 (89)      1 [ADC]          N/A           N/A              A/D Input: V_contrast
  //  [4] AN4 (85)      1 [ADC]          N/A           N/A              A/D Input: V_supply_5V
  //  [5] AN5 (86)      1 [ADC]          N/A           N/A
  //  [6] AN6 (87)      1 [ADC]          N/A           N/A
  //  [7] AN7 (88)      1 [ADC]          N/A           N/A
# define DEFAULT_PANPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    DDRAN
# define DEFAULT_PORTAN   PORTAN
  //
  // PORT AS
  //  [0] SCL   (111)   1 [I�C SCL]      N/A           N/A              I�C Clock
  //  [1] SDA   (112)   1 [I�C SDA]      N/A           N/A              I�C Data
  //  [2] SYNCB (27)    0 [GPIO]         1 [Output]    1 [High]         !SPI-WP   [SPI Not Write Protected]
  //  [3] SYNCA (28)    1 [SYNCA]        N/A           N/A              ADC_Sync
# define DEFAULT_PASPAR   0x45 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x04
# define DEFAULT_PORTAS   0x04
  //
  // PORT NQ
  //  [0]               0                0             0
  //  [1] IRQ1 (55)     0 [GPIO]         1 [Output]    1 [High]         !EEPROM Hold      [Not Holding]
  //  [2] IRQ2 (54)     0 [GPIO]         1 [Output]    1 [High]         XBee Sleep        [XBee Sleep Mode]
  //  [3] IRQ3 (53)     0 [GPIO]         1 [Output]    0 [Low]          !RS               [Register Mode]
  //  [4] IRQ4 (41)     0 [GPIO]         1 [Output]    1 [High]         R/!W              [Read Mode]
  //  [5] IRQ5 (20)     0 [GPIO]         1 [Output]    0 [Low]          EN                [Disabled]
  //  [6] IRQ6 (19)     0 [GPIO]         1 [Output]    0 [Low]          LCD On            [LCD Off]
  //  [7] IRQ7 (56)     0 [GPIO]         1 [Output]    1 [High]         [RESERVED]
# define DEFAULT_PNQPAR   0x0000 // Quad Function Pin Assignment Register x 7 pins (lsb not used)
# define DEFAULT_DDRNQ    0xfe
# define DEFAULT_PORTNQ   0x96
  //
  // PORT GP
  //  [0] IRQ8  (10)    0 [GPIO]         0 [Input]     N/A              Commission
  //  [1] IRQ9  (11)    0 [GPIO]         0 [Input]     N/A              Association
  //  [2] IRQ10 (29)    0 [GPIO]         0 [Input]     N/A              !CTS
  //  [3] IRQ11 (57)    0 [GPIO]         0 [Input]     N/A              <ENTER>
  //  [4] IRQ12 (97)    0 [GPIO]         0 [Input]     N/A              <UP>
  //  [5] IRQ13 (98)    0 [GPIO]         0 [Input]     N/A              <MENU>
  //  [6] IRQ14 (105)   0 [GPIO]         0 [Input]     N/A              <DOWN>
  //  [7] IRQ15 (106)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
# define DEFAULT_PGPPAR   0x00 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRGP    0x80
# define DEFAULT_PORTGP   0x00
  //
  // PORT TA
  //  [0] GPT0 (110)    0 [GPIO]         0 [Input]     N/A
  //  [1] GPT1 (109)    0 [GPIO]         0 [Input]     N/A
  //  [2] GPT2 (108)    0 [GPIO]         0 [Input]     N/A              RTC Pulse
  //  [3] GPT3 (107)    0 [GPIO]         0 [Input]     N/A
# define DEFAULT_PTAPAR   0x00 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTA    0x00
# define DEFAULT_PORTTA   PORTTA
  //
  // PORT TC
  //  [0] DTIN0 (8)     0 [GPIO]         0 [Input]     N/A              [External Clock]
  //  [1] DTIN1 (9)     0 [GPIO]         0 [Input]     N/A              [External Clock]
  //  [2] DTIN2 (21)    0 [GPIO]         0 [Input]     N/A              RSSI
  //  [3] DTIN3 (22)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0x80 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTC    0x00
# define DEFAULT_PORTTC   PORTTC
  //
  //
  // PORT TD
  //  [0] PWM1 (99)     1 [PWM1]         N/A           N/A              PWM LCD Contrast
  //  [1] PWM3 (100)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [2] PWM5 (103)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] PWM7 (104)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
# define DEFAULT_PTDPAR   0x01 // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRTD    0x0e
# define DEFAULT_PORTTD   0x00
  //
  // PORT LD
  //  [0] ACTLED (84)   1 [ACTLED]
  //  [1] LNKLED (83)   1 [LNKLED]
  //  [2] SPDLED (81)   0 [GPIO]         1 [Output]    1 [High]         Back Light ON   [On]
  //  [3] DUPLED (59)   0 [GPIO]         1 [Output]    1 [High]         DB4 [High]
  //  [4] COLLED (58)   0 [GPIO]         1 [Output]    1 [High]         DB5 [High]
  //  [5] RXLED  (52)   0 [GPIO]         1 [Output]    1 [High]         DB6 [High]
  //  [6] TXLED  (51)   0 [GPIO]         1 [Output]    1 [High]         DB7 [High]
# define DEFAULT_PLDPAR   0x03 // Dual Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRLD    0x7c
# define DEFAULT_PORTLD   0x7c
  //
  // PORT QS
  //  [0] QSPI_DOUT (35) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (34) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (36) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (37) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 0
  //  [4] QSPI_CS1  (38) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 1
  //  [5] QSPI_CS2  (39) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 2
  //  [6] QSPI_CS3  (40) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 3
# define DEFAULT_PQSPAR   0x1555 // Quad Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRQS    0x78
# define DEFAULT_PORTQS   0x78
  //
  // PORT UA
  //  [0] UTXD0  (31)   1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (30)   1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (25)   0 [GPIO]         1 [Output]    0 [Low]          Battery On
  //  [3] !UCTS0 (26)   0 [GPIO]         1 [Output]    1 [High]         Battery Charging Disabled
# define DEFAULT_PUAPAR   0x05 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0x0c
# define DEFAULT_PORTUA   0x08
  //
  // PORT UB
  //  [0] UTXD1  (33)   1 [TXD1]         N/A           N/A              UART TXD (XBee Module)
  //  [1] URXD1  (32)   1 [RXD1]         N/A           N/A              UART RXD (XBee Module)
  //  [2] !URTS1 (23)   1 [RTS1]         N/A           N/A              UART RTS (XBee Module)
  //  [3] !UCTS1 (24)   1 [CTS1]         N/A           N/A              UART CTS (XBee Module)
# define DEFAULT_PUBPAR   0x55 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUB    DDRUB
# define DEFAULT_PORTUB   PORTUB
  //
  // PORT UC
  //  [0] UTXD2  (63)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [1] URXD2  (62)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [2] !URTS2 (60)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] !UCTS2 (61)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
# define DEFAULT_PUCPAR   0x00 // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRUC    0x0f
# define DEFAULT_PORTUC   0x00


#elif PRODUCT_TYPE == SENSAPHONE_WEB600
  // **************************************************************
  // Network-Ready Sensors: Sensaphone 600
  // **************************************************************

  // 					          PxPAR            DDRx          PORTx
  // Port/Pin 	        Assignment       Direction     Data Value       Description
  // -----------------  ---------------  -----------   -------------		-------------------------------------
  //
  // PORT AN
  //  [0] AN0 (68)      1 [ADC]          N/A           N/A              A/D Input: Input 6
  //  [1] AN1 (67)      1 [ADC]          N/A           N/A              A/D Input: Input 5
  //  [2] AN2 (66)      1 [ADC]          N/A           N/A              A/D Input: Input 4
  //  [3] AN3 (65)      1 [ADC]          N/A           N/A              A/D Input: Input 3
  //  [4] AN4 (61)      1 [ADC]          N/A           N/A              A/D Input: V_supply_main
  //  [5] AN5 (62)      1 [ADC]          N/A           N/A              A/D Input: On-board Temperature
  //  [6] AN6 (63)      1 [ADC]          N/A           N/A              A/D Input: Input 1
  //  [7] AN7 (64)      1 [ADC]          N/A           N/A              A/D Input: Input 2
# define DEFAULT_PANPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    DDRAN
# define DEFAULT_PORTAN   PORTAN
  //
  // PORT AS
  //  [0] SCL   (79)    1 [I�C SCL]      N/A           N/A              I�C Clock
  //  [1] SDA   (80)    1 [I�C SDA]      N/A           N/A              I�C Data
  //  [2] SYNCB (19)    0 [GPIO]         1 [Output]    1 [High]         !SPI-WP   [SPI Not Write Protected]
  //  [3] SYNCA (20)    1 [SYNCA]        N/A           N/A              ADC_Sync
# define DEFAULT_PASPAR   0x45 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x04
# define DEFAULT_PORTAS   0x04
  //
  // PORT NQ
  //  [1] IRQ1 (39)     0 [GPIO]         1 [Output]    1 [High]         !EEPROM Hold      [Not Holding]
  //  [2] IRQ2 ()
  //  [3] IRQ3 ()
  //  [4] IRQ4 (29)     0 [GPIO]         1 [Output]    0 [Low]          A/D Control: Input 1
  //  [5] IRQ5 ()
  //  [6] IRQ6 ()
  //  [7] IRQ7 (40)     0 [GPIO]         1 [Output]    0 [Low]          A/D Control: Input 2
# define DEFAULT_PNQPAR   0x0000 // Quad Function Pin Assignment Register x 7 pins (lsb not used)
# define DEFAULT_DDRNQ    0x92
# define DEFAULT_PORTNQ   0x02
  //
  // PORT GP
  //  [0] IRQ8  ()
  //  [1] IRQ9  ()
  //  [2] IRQ10 ()
  //  [3] IRQ11 (41)    0 [GPIO]         1 [Output]    0 [Low]          [Rev A-B] A/D Control: Input 3
  //                                     0 [Input]     N/A              [Rev C+ ] PWRon
  //  [4] IRQ12 ()
  //  [5] IRQ13 ()
  //  [6] IRQ14 ()
  //  [7] IRQ15 ()
# define DEFAULT_PGPPAR   0x00 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRGP    0x00
# define DEFAULT_PORTGP   0x00
  //
  // PORT TA
  //  [0] GPT0 (78)     0 [GPIO]         1 [Output]    0 [Low]          [Rev C+] A/D Control: Input 3
  //  [1] GPT1 (77)     0 [GPIO]         1 [Output]    0 [Low]          [Rev C+] Battery Charger Enable [Disabled]
  //  [2] GPT2 (76)     1 [GPT2]         0 [Input]     N/A              RTC Pulse
  //  [3] GPT3 (75)     0 [GPIO]         0 [Input]     N/A              Reset to defaults button
# define DEFAULT_PTAPAR   0x10 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTA    0x03
# define DEFAULT_PORTTA   0x00
  //
  // PORT TC
  //  [0] DTIN0 (8)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED: External UART Clock Input]
  //  [1] DTIN1 (9)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED: External UART Clock Input]
  //  [2] DTIN2 (13)    0 [GPIO]         1 [Output]    1 [High]         [Rev F+] !Relay On
  //  [3] DTIN3 (14)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0b10000000 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTC    0b00000111
# define DEFAULT_PORTTC   0b00000100
  //
  //
  // PORT TD
  //  [0] PWM1 ()
  //  [1] PWM3 ()
  //  [2] PWM5 ()
  //  [3] PWM7 ()
# define DEFAULT_PTDPAR   PTDPAR // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRTD    DDRTD
# define DEFAULT_PORTTD   PORTTD
  //
  // PORT LD
  //  [0] ACTLED (60)   1 [ACTLED]
  //  [1] LNKLED (59)   1 [LNKLED]
  //  [2] SPDLED (57)   0 [GPIO]         1 [Output]    0 [Low]          A/D Control: Input 6
  //  [3] DUPLED (43)   0 [GPIO]         1 [Output]    0 [Low]          A/D Control: Input 5
  //  [4] COLLED (42)   0 [GPIO]         1 [Output]    0 [Low]          A/D Control: Input 4
  //  [5] RXLED  ()
  //  [6] TXLED  ()
# define DEFAULT_PLDPAR   0x03 // Dual Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRLD    0x1c
# define DEFAULT_PORTLD   0x00
  //
  // PORT QS
  //  [0] QSPI_DOUT (26) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (25) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (27) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (28) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 0
  //  [4] QSPI_CS1  ()
  //  [5] QSPI_CS2  ()
  //  [6] QSPI_CS3  ()
# define DEFAULT_PQSPAR   0x1555 // Quad Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRQS    0x08
# define DEFAULT_PORTQS   0x08
  //
  // PORT UA
  //  [0] UTXD0  (22)   1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (21)   1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (17)   0 [GPIO]         1 [Output]    0 [Low]          Alarm LED                   [LED Off]
  //  [3] !UCTS0 (18)   0 [GPIO]         1 [Output]    1 [High]         [Rev A-B] Battery Test Off  [Test Off]
  //                                     1 [Output]    1 [High]         [Rev C+ ] Battery Disable   [Disabled]
# define DEFAULT_PUAPAR   0x05 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0x0c
# define DEFAULT_PORTUA   0x08
  //
  // PORT UB
  //  [0] UTXD1  (24)     0 [GPIO]         1 [Output]    0 [Low]        [RESERVED]
  //  [1] URXD1  (23)     0 [GPIO]         1 [Output]    0 [Low]        [RESERVED]
  //  [2] !URTS1 (15)     0 [GPIO]         1 [Output]    0 [Low]        [RESERVED]
  //  [3] !UCTS1 (16)     0 [GPIO]         1 [Output]    0 [Low]        [RESERVED]
# define DEFAULT_PUBPAR   0x00  // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUB    0x0f
# define DEFAULT_PORTUB   0x00
  //
  // PORT UC
  //  [0] UTXD2  ()
  //  [1] URXD2  ()
  //  [2] !URTS2 ()
  //  [3] !UCTS2 ()
# define DEFAULT_PUCPAR   PUCPAR // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRUC    DDRUC
# define DEFAULT_PORTUC   PORTUC


#elif PRODUCT_TYPE == SENSAPHONE_WIRED_NODE
  // **************************************************************
  // IMS Wired Node
  // **************************************************************

  // 					          PxPAR            DDRx          PORTx
  // Port/Pin 	        Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------		-------------------------------------
  //
  // PORT AN
  //  [0] AN0 (68)      1 [ADC]          N/A           N/A              A/D Input: Sound
  //  [1] AN1 (67)      0 [GPIO]         0 [Input]     N/A              IMS Address A1 - Zones 1, 3, 5, 7
  //  [2] AN2 (66)      0 [GPIO]         0 [Input]     N/A              IMS Address A2 - Zones 1, 3, 5, 7
  //  [3] AN3 (65)      0 [GPIO]         0 [Input]     N/A              IMS Address A3 - Zones 1, 3, 5, 7
  //  [4] AN4 (61)      0 [GPIO]         0 [Input]     N/A              IMS Address A0 - Zones 2, 4, 6, 8
  //  [5] AN5 (62)      0 [GPIO]         0 [Input]     N/A              IMS Address A1 - Zones 2, 4, 6, 8
  //  [6] AN6 (63)      0 [GPIO]         0 [Input]     N/A              IMS Address A2 - Zones 2, 4, 6, 8
  //  [7] AN7 (64)      0 [GPIO]         0 [Input]     N/A              IMS Address A3 - Zones 2, 4, 6, 8
# define DEFAULT_PANPAR   0x01 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    0x00
# define DEFAULT_PORTAN   PORTAN
  //
  // PORT AS
  //  [0] SCL   (79)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [1] SDA   (80)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [2] SYNCB (19)    0 [GPIO]         1 [Output]    1 [High]         !SPI-WP   [SPI Not Write Protected]
  //  [3] SYNCA (20)    1 [SYNCA]        N/A           N/A              ADC_Sync
# define DEFAULT_PASPAR   0x40 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x07
# define DEFAULT_PORTAS   0x04
  //
  // PORT NQ
  //  [0]
  //  [1] IRQ1 (39)     0 [GPIO]         1 [Output]    1 [High]         !EEPROM Hold      [Not Holding]
  //  [2] IRQ2 ()
  //  [3] IRQ3 ()
  //  [4] IRQ4 (29)     0 [GPIO]         0 [Input]     N/A              ADC: End-of-Conversion
  //  [5] IRQ5 ()
  //  [6] IRQ6 ()
  //  [7] IRQ7 (40)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
# define DEFAULT_PNQPAR   0x0000 // Quad Function Pin Assignment Register x 7 pins (lsb not used)
# define DEFAULT_DDRNQ    0x82
# define DEFAULT_PORTNQ   0x02
  //
  // PORT GP
  //  [0] IRQ8  ()
  //  [1] IRQ9  ()
  //  [2] IRQ10 ()
  //  [3] IRQ11 (41)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [4] IRQ12 ()
  //  [5] IRQ13 ()
  //  [6] IRQ14 ()
  //  [7] IRQ15 ()
# define DEFAULT_PGPPAR   0x00 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRGP    0x00
# define DEFAULT_PORTGP   0x00
  //
  // PORT TA
  //  [0] GPT0 (78)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [1] GPT1 (77)     0 [GPIO]         1 [Output]    0 [Low]          Battery Charger Enable [Disabled]
  //  [2] GPT2 (76)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] GPT3 (75)     0 [GPIO]         0 [Input]     N/A              Reset to defaults button
# define DEFAULT_PTAPAR   0x00 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTA    0x07
# define DEFAULT_PORTTA   0x00
  //
  // PORT TC
  //  [0] DTIN0 (8)     0 [GPIO]         0 [Input]     N/A              IMS Address A0 - Zones 1, 3, 5, 7
  //  [1] DTIN1 (9)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [2] DTIN2 (13)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] DTIN3 (14)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0x80 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTC    0x06
# define DEFAULT_PORTTC   0x00
  //
  // PORT TD
  //  [0] PWM1 ()
  //  [1] PWM3 ()
  //  [2] PWM5 ()
  //  [3] PWM7 ()
# define DEFAULT_PTDPAR   PTDPAR // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRTD    DDRTD
# define DEFAULT_PORTTD   PORTTD
  //
  // PORT LD
  //  [0] ACTLED (60)   1 [ACTLED]
  //  [1] LNKLED (59)   1 [LNKLED]
  //  [2] SPDLED (57)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] DUPLED (43)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [4] COLLED (42)   0 [GPIO]         1 [Output]    1 [High]         EEPROM SPI Chip Select
  //  [5] RXLED  ()
  //  [6] TXLED  ()
# define DEFAULT_PLDPAR   0x03 // Dual Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRLD    0x1c
# define DEFAULT_PORTLD   0x10
  //
  // PORT QS
  //  [0] QSPI_DOUT (26) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (25) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (27) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (28) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 0
  //  [4] QSPI_CS1  ()
  //  [5] QSPI_CS2  ()
  //  [6] QSPI_CS3  ()
# define DEFAULT_PQSPAR   0x1555 // Quad Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRQS    0x08
# define DEFAULT_PORTQS   0x08
  //
  // PORT UA
  //  [0] UTXD0  (22)   1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (21)   1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (17)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] !UCTS0 (18)   0 [GPIO]         1 [Output]    1 [High]         Battery Disable   [Disabled]
# define DEFAULT_PUAPAR   0x05 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0x0c
# define DEFAULT_PORTUA   0x08
  //
  // PORT UB
  //  [0] UTXD1  (24)     0 [GPIO]         1 [Output]    1 [High]       IMS Address Enable: Zones 1 and 2
  //  [1] URXD1  (23)     0 [GPIO]         1 [Output]    1 [High]       IMS Address Enable: Zones 3 and 4
  //  [2] !URTS1 (15)     0 [GPIO]         1 [Output]    1 [High]       IMS Address Enable: Zones 5 and 6
  //  [3] !UCTS1 (16)     0 [GPIO]         1 [Output]    1 [High]       IMS Address Enable: Zones 7 and 8
# define DEFAULT_PUBPAR   0x00  // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUB    0x0f
# define DEFAULT_PORTUB   0x0f
  //
  // PORT UC
  //  [0] UTXD2  ()
  //  [1] URXD2  ()
  //  [2] !URTS2 ()
  //  [3] !UCTS2 ()
# define DEFAULT_PUCPAR   PUCPAR // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRUC    DDRUC
# define DEFAULT_PORTUC   PORTUC



#elif PRODUCT_TYPE == SENSAPHONE_DRY_CONTACT_NODE
  // **************************************************************
  // IMS Dry Contact Node
  // **************************************************************

  // 					          PxPAR            DDRx          PORTx
  // Port/Pin 	        Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------		-------------------------------------
  //
  // PORT AN
  //  [0] AN0 (68)      1 [ADC]          N/A           N/A              A/D Input: Sound
  //  [1] AN1 (67)      0 [GPIO]         0 [Input]     N/A              IMS Address A1 - Zones 1, 3, 5, 7
  //  [2] AN2 (66)      0 [GPIO]         0 [Input]     N/A              IMS Address A2 - Zones 1, 3, 5, 7
  //  [3] AN3 (65)      0 [GPIO]         0 [Input]     N/A              IMS Address A3 - Zones 1, 3, 5, 7
  //  [4] AN4 (61)      0 [GPIO]         0 [Input]     N/A              IMS Address A0 - Zones 2, 4, 6, 8
  //  [5] AN5 (62)      0 [GPIO]         0 [Input]     N/A              IMS Address A1 - Zones 2, 4, 6, 8
  //  [6] AN6 (63)      0 [GPIO]         0 [Input]     N/A              IMS Address A2 - Zones 2, 4, 6, 8
  //  [7] AN7 (64)      0 [GPIO]         0 [Input]     N/A              IMS Address A3 - Zones 2, 4, 6, 8
# define DEFAULT_PANPAR   0x01 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    0x00
# define DEFAULT_PORTAN   PORTAN
  //
  // PORT AS
  //  [0] SCL   (79)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [1] SDA   (80)    0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [2] SYNCB (19)    0 [GPIO]         1 [Output]    1 [High]         !SPI-WP   [SPI Not Write Protected]
  //  [3] SYNCA (20)    1 [SYNCA]        N/A           N/A              ADC_Sync
# define DEFAULT_PASPAR   0x40 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x07
# define DEFAULT_PORTAS   0x04
  //
  // PORT NQ
  //  [0]
  //  [1] IRQ1 (39)     0 [GPIO]         1 [Output]    1 [High]         !EEPROM Hold      [Not Holding]
  //  [2] IRQ2 ()
  //  [3] IRQ3 ()
  //  [4] IRQ4 (29)     0 [GPIO]         0 [Input]     N/A              ADC: End-of-Conversion
  //  [5] IRQ5 ()
  //  [6] IRQ6 ()
  //  [7] IRQ7 (40)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
# define DEFAULT_PNQPAR   0x0000 // Quad Function Pin Assignment Register x 7 pins (lsb not used)
# define DEFAULT_DDRNQ    0x82
# define DEFAULT_PORTNQ   0x02
  //
  // PORT GP
  //  [0] IRQ8  ()
  //  [1] IRQ9  ()
  //  [2] IRQ10 ()
  //  [3] IRQ11 (41)    0 [GPIO]         0 [Input]     N/A              !PowerOn
  //  [4] IRQ12 ()
  //  [5] IRQ13 ()
  //  [6] IRQ14 ()
  //  [7] IRQ15 ()
# define DEFAULT_PGPPAR   0x00 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRGP    0x00
# define DEFAULT_PORTGP   0x00
  //
  // PORT TA
  //  [0] GPT0 (78)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [1] GPT1 (77)     0 [GPIO]         1 [Output]    0 [Low]          Battery Charger Enable [Disabled]
  //  [2] GPT2 (76)     0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [3] GPT3 (75)     0 [GPIO]         0 [Input]     N/A              Reset to defaults button
# define DEFAULT_PTAPAR   0x00 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTA    0x07
# define DEFAULT_PORTTA   0x00
  //
  // PORT TC
  //  [0] DTIN0 (8)     0 [GPIO]         0 [Input]     N/A              DC Select 3
  //  [1] DTIN1 (9)     0 [GPIO]         0 [Input]     N/A              DC Select 4
  //  [2] DTIN2 (13)    0 [GPIO]         0 [Input]     N/A              DC Select 5
  //  [3] DTIN3 (14)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0x80 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTC    0x00
# define DEFAULT_PORTTC   0x00
  //
  // PORT TD
  //  [0] PWM1 ()
  //  [1] PWM3 ()
  //  [2] PWM5 ()
  //  [3] PWM7 ()
# define DEFAULT_PTDPAR   PTDPAR // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRTD    DDRTD
# define DEFAULT_PORTTD   PORTTD
  //
  // PORT LD
  //  [0] ACTLED (60)   1 [ACTLED]
  //  [1] LNKLED (59)   1 [LNKLED]
  //  [2] SPDLED (57)   0 [GPIO]         0 [Input]     N/A              Dry Contact Zone 1 input
  //  [3] DUPLED (43)   0 [GPIO]         1 [Output]    0 [Low]          [RESERVED]
  //  [4] COLLED (42)   0 [GPIO]         1 [Output]    1 [High]         EEPROM SPI Chip Select
  //  [5] RXLED  ()
  //  [6] TXLED  ()
# define DEFAULT_PLDPAR   0x03 // Dual Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRLD    0x18
# define DEFAULT_PORTLD   0x10
  //
  // PORT QS
  //  [0] QSPI_DOUT (26) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (25) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (27) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (28) 1 [QSPI]        1 [Output]    1 [High]         SPI Chip Select 0
  //  [4] QSPI_CS1  ()
  //  [5] QSPI_CS2  ()
  //  [6] QSPI_CS3  ()
# define DEFAULT_PQSPAR   0x1555 // Quad Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRQS    0x08
# define DEFAULT_PORTQS   0x08
  //
  // PORT UA
  //  [0] UTXD0  (22)     1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (21)     1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (17)     0 [GPIO]         0 [Input]     N/A              DC Select 8
  //  [3] !UCTS0 (18)     0 [GPIO]         1 [Output]    1 [High]         Battery Disable   [Disabled]
# define DEFAULT_PUAPAR   0x05 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0x08
# define DEFAULT_PORTUA   0x08
  //
  // PORT UB
  //  [0] UTXD1  (24)     0 [GPIO]       0 [Input]     N/A              DC Select 1
  //  [1] URXD1  (23)     0 [GPIO]       0 [Input]     N/A              DC Select 2
  //  [2] !URTS1 (15)     0 [GPIO]       0 [Input]     N/A              DC Select 6
  //  [3] !UCTS1 (16)     0 [GPIO]       0 [Input]     N/A              DC Select 7
# define DEFAULT_PUBPAR   0x00  // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUB    0x00
# define DEFAULT_PORTUB   0x00
  //
  // PORT UC
  //  [0] UTXD2  ()
  //  [1] URXD2  ()
  //  [2] !URTS2 ()
  //  [3] !UCTS2 ()
# define DEFAULT_PUCPAR   PUCPAR // Dual Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRUC    DDRUC
# define DEFAULT_PORTUC   PORTUC



#elif PRODUCT_TYPE == SENSAPHONE_WEB800
  // **************************************************************
  // Sensaphone Web800
  // **************************************************************

  // 					          PxPAR            DDRx          PORTx
  // Port/Pin 	        Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------		-------------------------------------
  //
  // PORT AN
  //  [0] AN0 (66)      1 [ADC]          N/A           N/A              A/D Input: V_battery
  //  [1] AN1 (67)      1 [ADC]          N/A           N/A              A/D Input: RTC Lithium Battery Voltage
  //  [2] AN2 (68)      1 [ADC]          N/A           N/A              A/D Input: Supply On (Digital)
  //  [3] AN3 (69)      0 [GPIO]         1 [Output]    0 [Low]          ADC Mux A
  //  [4] AN4 (77)      0 [GPIO]         1 [Output]    0 [Low]          ADC Mux B
  //  [5] AN5 (76)      0 [GPIO]         1 [Output]    0 [Low]          ADC Mux C
  //  [6] AN6 (75)      0 [GPIO]         0 [Input]     N/A              Ethernet Link (On = Good, Blink = Activity)
  //  [7] AN7 (74)      0 [GPIO]         1 [Output]    0 [Low]          RTC Lithium Battery A/D Input On/Off
# define DEFAULT_PANPAR   0x07 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    0xb8
# define DEFAULT_PORTAN   0x00
  //
  // PORT TI
  //  [0] FEC_COL       1 [FEC]          N/A           N/A              FEC
  //  [1] FEC_CRS       1 [FEC]          N/A           N/A              FEC
  //  [2] FEC_RXCLK     1 [FEC]          N/A           N/A              FEC
  //  [3] FEC_RXD0      1 [FEC]          N/A           N/A              FEC
  //  [4] FEC_RXD1      1 [FEC]          N/A           N/A              FEC
  //  [5] FEC_RXD2      1 [FEC]          N/A           N/A              FEC
  //  [6] FEC_RXD3      1 [FEC]          N/A           N/A              FEC
  //  [7] FEC_RXDV      1 [FEC]          N/A           N/A              FEC
# define DEFAULT_PTIPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTI    DDRTI
# define DEFAULT_PORTTI   PORTTI
  //
  // PORT TJ
  //  [0] FEC_RXER      1 [FEC]          N/A           N/A              FEC
  //  [1] FEC_TXCLK     1 [FEC]          N/A           N/A              FEC
  //  [2] FEC_TXD0      1 [FEC]          N/A           N/A              FEC
  //  [3] FEC_TXD1      1 [FEC]          N/A           N/A              FEC
  //  [4] FEC_TXD2      1 [FEC]          N/A           N/A              FEC
  //  [5] FEC_TXD3      1 [FEC]          N/A           N/A              FEC
  //  [6] FEC_TXEN      1 [FEC]          N/A           N/A              FEC
  //  [7] FEC_TXER      0 [GPIO]         0 [Input]     N/A              Ethernet Speed (Off = 10Mbps, On = 100Mbps)
# define DEFAULT_PTJPAR   0x7f // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTJ    0x7f
# define DEFAULT_PORTTJ   PORTTJ
  //
  // PORT AS
  //  [0] SCL (28)      0 [GPIO]         0 [Input]     N/A              Voice RDY/BSYSB
  //  [1] SDA (29)      0 [GPIO]         0 [Input]     N/A              Voice INTB/EOM
  //  [2] FB_ALE (37)   1 [FB]           N/A           N/A              Flex Bus
  //  [3]
# define DEFAULT_PASPAR   0x10 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x00
# define DEFAULT_PORTAS   PORTAS
  //
  // PORT NQ
  //  [0]
  //  [1] IRQ1 (103)    0 [GPIO]         0 [Input]     N/A              Zigbee SRDY
  //  [2]
  //  [3] IRQ3 (129)    2 [FEC]          N/A           N/A              FEC_MDIO
  //  [4]
  //  [5] IRQ5 (128)    2 [FEC]          N/A           N/A              FEC_MDC
  //  [6]
  //  [7] IRQ7 (96)     0 [GPIO]         0 [Input]     N/A              Reset-to-Defaults Button
//// JJR # define DEFAULT_PNQPAR   0x0028 // Quad Function Pin Assignment Register x 7 pins (lsb not used) 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_PNQPAR   0x0880 // Quad Function Pin Assignment Register x 7 pins (lsb not used) 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRNQ    0x00
# define DEFAULT_PORTNQ   PORTNQ
  //
  // PORT QS
  //  [0] QSPI_DOUT (25) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (24) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (26) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (27) 1 [QSPI]        N/A           N/A              SPI Chip Select 0
  //  [4]
  //  [5] QSPI_CS2  (23) 1 [QSPI]        N/A           N/A              SPI Chip Select 2
  //  [6] QSPI_CS3  (22) 1 [QSPI]        N/A           N/A              SPI Chip Select 3
  //  [7]
# define DEFAULT_PQSPAR   0x1455 // Quad Function Pin Assignment Register x 7 pins (msb not used)
# define DEFAULT_DDRQS    DDRQS
# define DEFAULT_PORTQS   PORTQS
  //
  // PORT TA
  //  [0] GPT0 (93)     0 [GPIO]         0 [Input]     N/A              Ring Detect
  //  [1] GPT1 (94)     0 [GPIO]         0 [Input]     N/A              Line Voltage PWM
  //  [2] GPT2 (95)     0 [GPIO]         0 [Input]     N/A              DTMF DStD
  //  [3] GPT3 (58)     0 [GPIO]         0 [Input]     N/A              DTMF SDO
# define DEFAULT_PTAPAR   0x00 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRTA    0x00
# define DEFAULT_PORTTA   PORTTA
  //
  // PORT TC
  //  [0] DTIN0 (11)    1 [DTIN0]        N/A           N/A              External Clock (7.3728MHz)
  //  [1] DTIN1 (12)    1 [DTIN1]        N/A           N/A              External Clock (7.3728MHz)
  //  [2] DTIN2 (65)    1 [DTIN2]        N/A           N/A              External Clock (7.3728MHz)
  //  [3] DTIN3 (19)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0b10010101 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTC    DDRTC
# define DEFAULT_PORTTC   PORTTC
  //
  // PORT UA
  //  [0] UTXD0  (31)   1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (30)   1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (25)   0 [GPIO]         1 [Output]    0 [Low]          Battery On
  //  [3] !UCTS0 (26)   0 [GPIO]         1 [Output]    1 [High]         Battery Charging Disabled
# define DEFAULT_PUAPAR   0x05 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0x0c
# define DEFAULT_PORTUA   0x08
  //
  // PORT UB
  //  [0] UTXD1  (33)   1 [TXD1]         N/A           N/A              UART TXD (RS-485)
  //  [1] URXD1  (32)   1 [RXD1]         N/A           N/A              UART RXD (RS-485)
  //  [2] !URTS1 (23)   1 [RTS1]         N/A           N/A              UART RTS (RS-485)
  //  [3] !UCTS1 (24)   1 [CTS1]         N/A           N/A              UART CTS (RS-485)
# define DEFAULT_PUBPAR   0x55 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUB    0x00
# define DEFAULT_PORTUB   PORTUB
  //
  // PORT UC
  //  [0] UTXD2  (101)  1 [TXD2]         N/A           N/A              UART TXD (XBee Module)
  //  [1] URXD2  (102)  1 [RXD2]         N/A           N/A              UART RXD (XBee Module)
  //  [2] !URTS2 (98)   1 [RTS2]         N/A           N/A              UART RTS (XBee Module)
  //  [3] !UCTS2 (97)   1 [CTS2]         N/A           N/A              UART CTS (XBee Module)
# define DEFAULT_PUCPAR   0x55 // Quad Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRUC    0x00
# define DEFAULT_PORTUC   PORTUC
  //
  // PORT TE
  //  [0] FB_AD0 (57)   1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD1 (56)   1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD2 (55)   1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD3 (54)   1 [FB]          N/A           N/A              Flex Bus
  //  [4] FB_AD4 (53)   1 [FB]          N/A           N/A              Flex Bus
  //  [5] FB_AD5 (36)   1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_AD6 (35)   1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_AD7 (34)   1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTEPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTE    DDRTE
# define DEFAULT_PORTTE   PORTTE
  //
  // PORT TF
  //  [0] FB_AD8  (33)  1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD9  (32)  1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD10 (6)   1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD11 (5)   1 [FB]          N/A           N/A              Flex Bus
  //  [4] FB_AD12 (4)   1 [FB]          N/A           N/A              Flex Bus
  //  [5] FB_AD13 (3)   1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_AD14 (2)   1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_AD15 (136) 1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTFPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTF    DDRTF
# define DEFAULT_PORTTF   PORTTF
  //
  // PORT TG
  //  [0] FB_AD16 (133) 1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD17 (132) 1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD18 (131) 1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD19 (130) 1 [FB]          N/A           N/A              Flex Bus
  //  [4]
  //  [5] FB_!CS0 (52)  1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_!OE (137)  1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_R/!W (45)  1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTGPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTG    DDRTG
# define DEFAULT_PORTTG   PORTTG
  //
  // PORT TH
  //  [0] FB_D2  (47)   0 [GPIO]         1 [Output]    0 [Low]          Relay 2 Control
  //  [1] FB_D3  (46)   0 [GPIO]         1 [Output]    0 [Low]          Relay 1 Control
  //  [2] FB_D4  (1)    0 [GPIO]         1 [Output]    0 [Low]          !Hook
  //  [3] FB_D5  (140)  0 [GPIO]         1 [Output]    0 [Low]          Phone Line In Use LED
  //  [4] FB_D6  (139)  0 [GPIO]         1 [Output]    0 [Low]          DTMF Ack
  //  [5] FB_D7  (138)  0 [GPIO]         1 [Output]    0 [Low]          Zigbee Reset
  //  [6] FB_D0   (51)  0 [GPIO]         1 [Output]    0 [Low]          Alarm LED
  //  [7] FB_D1   (50)  2 [SYNC_A]       N/A           N/A              SYNC_A
# define DEFAULT_PTHPAR   0x8000 // Quad Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTH    0xff
# define DEFAULT_PORTTH   0x00

#elif PRODUCT_TYPE == WINDCHIME_BASELINE
  // **************************************************************
  // Windchime Baseline
  // **************************************************************

  //                    PxPAR            DDRx          PORTx
  // Port/Pin           Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------    -------------------------------------
  //
  // PORT AN
  //  [0] AN0 (66)      0 [GPIO]         1 [Output]    0 [Low]          Zone 1 Configuration
  //  [1] AN1 (67)      1 [ADC]          N/A           N/A              A/D Input: Zones 9 to 12
  //  [2] AN2 (68)      1 [ADC]          N/A           N/A              A/D Input: Zones 1 to 8
  //  [3] AN3 (69)      1 [ADC]          N/A           N/A              A/D Input: NiMH Battery Voltage
  //  [4] AN4 (77)      0 [GPIO]         1 [Output]    0 [Low]          Zone 2 Configuration
  //  [5] AN5 (76)      0 [GPIO]         1 [Output]    0 [Low]          Zone 3 Configuration
  //  [6] AN6 (75)      0 [GPIO]         1 [Output]    0 [Low]          Zone 4 Configuration
  //  [7] AN7 (74)      0 [GPIO]         1 [Output]    0 [Low]          Zone 5 Configuration
# define DEFAULT_PANPAR   0x0e // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    0xf1
# define DEFAULT_PORTAN   0x00
  //
  // PORT TI
  //  [0] FEC_COL       1 [FEC]          N/A           N/A              FEC
  //  [1] FEC_CRS       1 [FEC]          N/A           N/A              FEC
  //  [2] FEC_RXCLK     1 [FEC]          N/A           N/A              FEC
  //  [3] FEC_RXD0      1 [FEC]          N/A           N/A              FEC
  //  [4] FEC_RXD1      1 [FEC]          N/A           N/A              FEC
  //  [5] FEC_RXD2      1 [FEC]          N/A           N/A              FEC
  //  [6] FEC_RXD3      1 [FEC]          N/A           N/A              FEC
  //  [7] FEC_RXDV      1 [FEC]          N/A           N/A              FEC
# define DEFAULT_PTIPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTI    DDRTI
# define DEFAULT_PORTTI   PORTTI
  //
  // PORT TJ
  //  [0] FEC_RXER      1 [FEC]          N/A           N/A              FEC
  //  [1] FEC_TXCLK     1 [FEC]          N/A           N/A              FEC
  //  [2] FEC_TXD0      1 [FEC]          N/A           N/A              FEC
  //  [3] FEC_TXD1      1 [FEC]          N/A           N/A              FEC
  //  [4] FEC_TXD2      1 [FEC]          N/A           N/A              FEC
  //  [5] FEC_TXD3      1 [FEC]          N/A           N/A              FEC
  //  [6] FEC_TXEN      1 [FEC]          N/A           N/A              FEC
  //  [7] FEC_TXER      0 [GPIO]         1 [Output]    0 [Low]          !WiFi Reset
# define DEFAULT_PTJPAR   0x7f // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTJ    0xff
# define DEFAULT_PORTTJ   0x00
  //
  // PORT AS
  //  [0] SCL (28)      0 [GPIO]         1 [Output]    0 [Low]          Zone 10 Configuration
  //  [1] SDA (29)      0 [GPIO]         1 [Output]    0 [Low]          Zone 11 Configuration
  //  [2] FB_ALE (37)   1 [FB]           N/A           N/A              Flex Bus
  //  [3]
# define DEFAULT_PASPAR   0x10 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x03
# define DEFAULT_PORTAS   0x00
  //
  // PORT NQ
  //  [0]
  //  [1] IRQ1 (103)    0 [GPIO]         0 [Input]     N/A              PHY Interrupt
  //  [2]
  //  [3] IRQ3 (129)    2 [FEC]          N/A           N/A              FEC_MDIO
  //  [4]
  //  [5] IRQ5 (128)    2 [FEC]          N/A           N/A              FEC_MDC
  //  [6]
  //  [7] IRQ7 (96)     0 [GPIO]         0 [Input]     N/A              !WiFi Interrupt
# define DEFAULT_PNQPAR   0x0880 // Quad Function Pin Assignment Register x 7 pins (lsb not used) 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRNQ    0x00
# define DEFAULT_PORTNQ   PORTNQ
  //
  // PORT QS
  //  [0] QSPI_DOUT (25) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (24) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (26) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (27) 0 [GPIO]        1 [Output]    N/A              SPI Chip Select 0: Flash
  //  [4]
  //  [5] QSPI_CS2  (23) 0 [GPIO]        1 [Output]    0 [Low]          Zone 12 Configuration
  //  [6] QSPI_CS3  (22) 1 [QSPI]        N/A           N/A              SPI Chip Select 3: WiFi
  //  [7]
# define DEFAULT_PQSPAR   0x1055 // Quad Function Pin Assignment Register x 7 pins (msb not used)
//// JJR# define DEFAULT_DDRQS    0x48
# define DEFAULT_DDRQS    0x68        
# define DEFAULT_PORTQS   0x00
  //
  // PORT TA
  //  [0] GPT0 (93)     0 [GPIO]         0 [Input]     N/A              Acknowledge Button
  //  [1] GPT1 (94)     0 [GPIO]         1 [Output]    0 [Low]          Monnit APN nRST
  //  [2] GPT2 (95)     0 [GPIO]         0 [Input]     N/A              Monnit APN Activity
  //  [3] GPT3 (58)     0 [GPIO]         0 [Input]     N/A              Reset-to-Defaults Button
# define DEFAULT_PTAPAR   0b00000000 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTA    0b00000010
# define DEFAULT_PORTTA   0b00000000
  //
  // PORT TC
  //  [0] DTIN0 (11)    3 [PWM2]         N/A           N/A              PWM Audio Speaker Out
  //  [1] DTIN1 (12)    0 [GPIO]         0 [Input]     N/A              !Power Supply On
  //  [2] DTIN2 (65)    1 [DTIN2]        N/A           N/A              External Clock (7.3728MHz)
  //  [3] DTIN3 (19)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0b10010011 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTC    0b00000000
# define DEFAULT_PORTTC   PORTTC
  //
  // PORT UA
  //  [0] UTXD0  (16)   1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (17)   1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (18)   0 [GPIO]         1 [Output]    0 [High]         Battery Charging Disabled
  //  [3] !UCTS0 (15)   0 [GPIO]         1 [Output]    0 [Low]          Relay Out
# define DEFAULT_PUAPAR   0b00000101 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0b00001100
//// JJR # define DEFAULT_PORTUA   0b00000000
# define DEFAULT_PORTUA   0b00000100			/* Charger off */
  //
  // PORT UB
  //  [0] UTXD1  (33)   0 [GPIO]         1 [Output]    0 [Low]          Zone 6 Configuration
  //  [1] URXD1  (32)   0 [GPIO]         1 [Output]    0 [Low]          Zone 7 Configuration
  //  [2] !URTS1 (23)   0 [GPIO]         1 [Output]    0 [Low]          Zone 8 Configuration
  //  [3] !UCTS1 (24)   0 [GPIO]         1 [Output]    0 [Low]          Zone 9 Configuration
//// JJR # define DEFAULT_PUBPAR   0x55 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_PUBPAR   0x00 // Quad Function Pin Assignment Register x 4 pins
//// JJR # define DEFAULT_DDRUB    0x00
# define DEFAULT_DDRUB    0x0F
# define DEFAULT_PORTUB   PORTUB
  //
  // PORT UC
  //  [0] UTXD2  (101)  1 [TXD2]         N/A           N/A              UART TXD (Monnit APN Module)
  //  [1] URXD2  (102)  1 [RXD2]         N/A           N/A              UART RXD (Monnit APN Module)
  //  [2] !URTS2 (98)   1 [RTS2]         N/A           N/A              UART RTS (Monnit APN Module)
  //  [3] !UCTS2 (97)   1 [CTS2]         N/A           N/A              UART CTS (Monnit APN Module)
# define DEFAULT_PUCPAR   0x55 // Quad Function Pin Assignment Register x 4 pins (msb not used)
# define DEFAULT_DDRUC    0x00
# define DEFAULT_PORTUC   PORTUC
  //
  // PORT TE
  //  [0] FB_AD0 (57)   1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD1 (56)   1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD2 (55)   1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD3 (54)   1 [FB]          N/A           N/A              Flex Bus
  //  [4] FB_AD4 (53)   1 [FB]          N/A           N/A              Flex Bus
  //  [5] FB_AD5 (36)   1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_AD6 (35)   1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_AD7 (34)   1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTEPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTE    DDRTE
# define DEFAULT_PORTTE   PORTTE
  //
  // PORT TF
  //  [0] FB_AD8  (33)  1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD9  (32)  1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD10 (6)   1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD11 (5)   1 [FB]          N/A           N/A              Flex Bus
  //  [4] FB_AD12 (4)   1 [FB]          N/A           N/A              Flex Bus
  //  [5] FB_AD13 (3)   1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_AD14 (2)   1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_AD15 (136) 1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTFPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTF    DDRTF
# define DEFAULT_PORTTF   PORTTF
  //
  // PORT TG
  //  [0] FB_AD16 (133) 1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD17 (132) 1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD18 (131) 1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD19 (130) 1 [FB]          N/A           N/A              Flex Bus
  //  [4]
  //  [5] FB_!CS0 (52)  1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_!OE (137)  1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_R/!W (45)  1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTGPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTG    DDRTG
# define DEFAULT_PORTTG   PORTTG
  //
  // PORT TH
  //  [0] FB_D2  (47)   0 [GPIO]         1 [Output]    0 [Low]          Analog Mux A
  //  [1] FB_D3  (46)   0 [GPIO]         1 [Output]    0 [Low]          Analog Mux B
  //  [2] FB_D4  (1)    0 [GPIO]         1 [Output]    0 [Low]          Analog Mux C
  //  [3] FB_D5  (140)  0 [GPIO]         1 [Output]    0 [Low]          Red LED
  //  [4] FB_D6  (139)  0 [GPIO]         1 [Output]    0 [Low]          Yellow LED
  //  [5] FB_D7  (138)  0 [GPIO]         1 [Output]    0 [Low]          Green LED
  //  [6] FB_D0   (51)  0 [GPIO]         1 [Output]    0 [Low]          !Battery On
  //  [7] FB_D1   (50)  2 [SYNC_A]       N/A           N/A              SYNC_A
# define DEFAULT_PTHPAR   0x8000 // Quad Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTH    0xff
# define DEFAULT_PORTTH   0x20 /* Green LED ON */

#elif PRODUCT_TYPE == WINDCHIME_MODBUS || PRODUCT_TYPE == WINDCHIME_STRATUS
  // **************************************************************
  // Sentinel Pro
  // **************************************************************

  //                    PxPAR            DDRx          PORTx
  // Port/Pin           Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------    -------------------------------------
  //
  // PORT AN
  //  [0] AN0 (66)      0 [GPIO]         1 [Output]    0 [Low]          Zone 1 Configuration
  //  [1] AN1 (67)      1 [ADC]          N/A           N/A              A/D Input: Zones 9 to 12
  //  [2] AN2 (68)      1 [ADC]          N/A           N/A              A/D Input: Zones 1 to 8
  //  [3] AN3 (69)      0 [GPIO]         1 [Output]    1 [High]         Buzzer
  //  [4] AN4 (77)      0 [GPIO]         1 [Output]    0 [Low]          Zone 2 Configuration
  //  [5] AN5 (76)      0 [GPIO]         1 [Output]    0 [Low]          Zone 3 Configuration
  //  [6] AN6 (75)      0 [GPIO]         1 [Output]    0 [Low]          Zone 4 Configuration
  //  [7] AN7 (74)      0 [GPIO]         1 [Output]    0 [Low]          Zone 5 Configuration
# define DEFAULT_PANPAR   0x06 // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRAN    0xf9
# define DEFAULT_PORTAN   0x08
  //
  // PORT TI
  //  [0] FEC_COL       1 [FEC]          N/A           N/A              FEC
  //  [1] FEC_CRS       1 [FEC]          N/A           N/A              FEC
  //  [2] FEC_RXCLK     1 [FEC]          N/A           N/A              FEC
  //  [3] FEC_RXD0      1 [FEC]          N/A           N/A              FEC
  //  [4] FEC_RXD1      1 [FEC]          N/A           N/A              FEC
  //  [5] FEC_RXD2      1 [FEC]          N/A           N/A              FEC
  //  [6] FEC_RXD3      1 [FEC]          N/A           N/A              FEC
  //  [7] FEC_RXDV      1 [FEC]          N/A           N/A              FEC
# define DEFAULT_PTIPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTI    DDRTI
# define DEFAULT_PORTTI   PORTTI
  //
  // PORT TJ
  //  [0] FEC_RXER      1 [FEC]          N/A           N/A              FEC
  //  [1] FEC_TXCLK     1 [FEC]          N/A           N/A              FEC
  //  [2] FEC_TXD0      1 [FEC]          N/A           N/A              FEC
  //  [3] FEC_TXD1      1 [FEC]          N/A           N/A              FEC
  //  [4] FEC_TXD2      1 [FEC]          N/A           N/A              FEC
  //  [5] FEC_TXD3      1 [FEC]          N/A           N/A              FEC
  //  [6] FEC_TXEN      1 [FEC]          N/A           N/A              FEC
  //  [7] FEC_TXER      0 [GPIO]         1 [Output]    0 [Low]          !WiFi Reset
# define DEFAULT_PTJPAR   0x7f // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTJ    0xff
# define DEFAULT_PORTTJ   0x00
  //
  // PORT AS
  //  [0] SCL (28)      0 [GPIO]         1 [Output]    0 [Low]          Zone 10 Configuration
  //  [1] SDA (29)      0 [GPIO]         1 [Output]    0 [Low]          Zone 11 Configuration
  //  [2] FB_ALE (37)   1 [FB]           N/A           N/A              Flex Bus
  //  [3]
# define DEFAULT_PASPAR   0x10 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRAS    0x03
# define DEFAULT_PORTAS   0x00
  //
  // PORT NQ
  //  [0]
  //  [1] IRQ1 (103)    0 [GPIO]         0 [Input]     N/A              PHY Interrupt
  //  [2]
  //  [3] IRQ3 (129)    2 [FEC]          N/A           N/A              FEC_MDIO
  //  [4]
  //  [5] IRQ5 (128)    2 [FEC]          N/A           N/A              FEC_MDC
  //  [6]
  //  [7] IRQ7 (96)     0 [GPIO]         0 [Input]     N/A              !WiFi Interrupt
# define DEFAULT_PNQPAR   0x0880 // Quad Function Pin Assignment Register x 7 pins (lsb not used) 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRNQ    0x00
# define DEFAULT_PORTNQ   PORTNQ
  //
  // PORT QS
  //  [0] QSPI_DOUT (25) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (24) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (26) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (27) 0 [GPIO]        1 [Output]    N/A              SPI Chip Select 0: Flash
  //  [4]
  //  [5] QSPI_CS2  (23) 0 [GPIO]        1 [Output]    0 [Low]          Zone 12 Configuration
  //  [6] QSPI_CS3  (22) 1 [QSPI]        N/A           N/A              SPI Chip Select 3: WiFi
  //  [7]
# define DEFAULT_PQSPAR   0x1055 // Quad Function Pin Assignment Register x 7 pins (msb not used)
//// JJR# define DEFAULT_DDRQS    0x48
# define DEFAULT_DDRQS    0x68        
# define DEFAULT_PORTQS   0x00
  //
  // PORT TA
  //  [0] GPT0 (93)     0 [GPIO]         0 [Input]     N/A              Acknowledge Button
  //  [1] GPT1 (94)     0 [GPIO]         1 [Output]    0 [Low]          Output Relay #2
  //  [2] GPT2 (95)     0 [GPIO]         1 [Output]    0 [Low]          Standby LED
  //  [3] GPT3 (58)     0 [GPIO]         0 [Input]     N/A              Reset-to-Defaults Button
# define DEFAULT_PTAPAR   0b00000000 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTA    0b00000110
# define DEFAULT_PORTTA   0b00000100
  //
  // PORT TC
  //  [0] DTIN0 (11)    3 [DTOUT0]       N/A           N/A              MODBUS serial baud clock
  //  [1] DTIN1 (12)    0 [GPIO]         0 [Input]     N/A              !Power Supply On
  //  [2] DTIN2 (65)    1 [DTIN2]        N/A           N/A              External Clock (MODBUS serial baud clock)
  //  [3] DTIN3 (19)    2 [DTOUT3]       N/A           N/A              ADC_Sync Output
# define DEFAULT_PTCPAR   0b10010010 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTC    0b00000000
# define DEFAULT_PORTTC   PORTTC
  //
  // PORT UA
  //  [0] UTXD0  (16)   1 [TXD0]         N/A           N/A              UART TXD (Debugging)
  //  [1] URXD0  (17)   1 [RXD0]         N/A           N/A              UART RXD (Debugging)
  //  [2] !URTS0 (18)   0 [GPIO]         1 [Output]    0 [High]         Battery Charging Disabled
  //  [3] !UCTS0 (15)   0 [GPIO]         1 [Output]    0 [Low]          Relay Out
# define DEFAULT_PUAPAR   0b00000101 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUA    0b00001100
//// JJR # define DEFAULT_PORTUA   0b00000000
# define DEFAULT_PORTUA   0b00000100			/* Charger off */
  //
  // PORT UB
  //  [0] UTXD1  (33)   0 [GPIO]         1 [Output]    0 [Low]          Zone 6 Configuration
  //  [1] URXD1  (32)   0 [GPIO]         1 [Output]    0 [Low]          Zone 7 Configuration
  //  [2] !URTS1 (23)   0 [GPIO]         1 [Output]    0 [Low]          Zone 8 Configuration
  //  [3] !UCTS1 (24)   0 [GPIO]         1 [Output]    0 [Low]          Zone 9 Configuration
//// JJR # define DEFAULT_PUBPAR   0x55 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_PUBPAR   0x00 // Quad Function Pin Assignment Register x 4 pins
//// JJR # define DEFAULT_DDRUB    0x00
# define DEFAULT_DDRUB    0x0F
# define DEFAULT_PORTUB   PORTUB
  //
  // PORT UC
  //  [0] UTXD2  (101)  1 [TXD2]         N/A           N/A              RS-485 TxD
  //  [1] URXD2  (102)  1 [RXD2]         N/A           N/A              RS-485 RxD
  //  [2] !URTS2 (98)   1 [RTS2]         N/A           N/A              RS-485 DE /RE
  //  [3] PUC3   (97)   0 [GPIO]         1 [Output]    0 [Low]          Link LED
# define DEFAULT_PUCPAR   0x15 // Quad Function Pin Assignment Register x 4 pins
# define DEFAULT_DDRUC    0x08
# define DEFAULT_PORTUC   PORTUC
  //
  // PORT TE
  //  [0] FB_AD0 (57)   1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD1 (56)   1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD2 (55)   1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD3 (54)   1 [FB]          N/A           N/A              Flex Bus
  //  [4] FB_AD4 (53)   1 [FB]          N/A           N/A              Flex Bus
  //  [5] FB_AD5 (36)   1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_AD6 (35)   1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_AD7 (34)   1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTEPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTE    DDRTE
# define DEFAULT_PORTTE   PORTTE
  //
  // PORT TF
  //  [0] FB_AD8  (33)  1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD9  (32)  1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD10 (6)   1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD11 (5)   1 [FB]          N/A           N/A              Flex Bus
  //  [4] FB_AD12 (4)   1 [FB]          N/A           N/A              Flex Bus
  //  [5] FB_AD13 (3)   1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_AD14 (2)   1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_AD15 (136) 1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTFPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTF    DDRTF
# define DEFAULT_PORTTF   PORTTF
  //
  // PORT TG
  //  [0] FB_AD16 (133) 1 [FB]          N/A           N/A              Flex Bus
  //  [1] FB_AD17 (132) 1 [FB]          N/A           N/A              Flex Bus
  //  [2] FB_AD18 (131) 1 [FB]          N/A           N/A              Flex Bus
  //  [3] FB_AD19 (130) 1 [FB]          N/A           N/A              Flex Bus
  //  [4]
  //  [5] FB_!CS0 (52)  1 [FB]          N/A           N/A              Flex Bus
  //  [6] FB_!OE (137)  1 [FB]          N/A           N/A              Flex Bus
  //  [7] FB_R/!W (45)  1 [FB]          N/A           N/A              Flex Bus
# define DEFAULT_PTGPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTG    DDRTG
# define DEFAULT_PORTTG   PORTTG
  //
  // PORT TH
  //  [0] FB_D2  (47)   0 [GPIO]         1 [Output]    0 [Low]          Analog Mux A
  //  [1] FB_D3  (46)   0 [GPIO]         1 [Output]    0 [Low]          Analog Mux B
  //  [2] FB_D4  (1)    0 [GPIO]         1 [Output]    0 [Low]          Analog Mux C
  //  [3] FB_D5  (140)  0 [GPIO]         1 [Output]    0 [Low]          Red LED
  //  [4] FB_D6  (139)  0 [GPIO]         1 [Output]    0 [Low]          J5-3
  //  [5] FB_D7  (138)  0 [GPIO]         1 [Output]    0 [Low]          J5-2
  //  [6] FB_D0   (51)  0 [GPIO]         1 [Output]    0 [Low]          !Battery On
  //  [7] FB_D1   (50)  2 [SYNC_A]       N/A           N/A              SYNC_A
# define DEFAULT_PTHPAR   0x8000 // Quad Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTH    0xff
# define DEFAULT_PORTTH   0x00

#elif PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR
  // **************************************************************
  // Sensaphone Video Overlay  Monitor
  // **************************************************************
  // 					          PxPAR            DDRx          PORTx
  // Port/Pin 	        Assignment       Direction     Data Value       Description [Default]
  // -----------------  ---------------  -----------   -------------		-------------------------------------
  //
  // PORT AN
  //  [0] AN0 (66)      1 [ADC]          N/A           N/A              Input1
  //  [1] AN1 (67)      1 [ADC]          N/A           N/A              Input2
  //  [2] AN2 (68)      1 [ADC]          N/A           N/A              Input3
  //  [3] AN3 (69)      1 [ADC]          N/A           N/A  		        Input4
  //  [4] AN4 (77)      0 [ADC]          N/A       	              			Battery
  //  [5] AN5 (76)      0 [GPIO]         Output                   			Control Battery Low=Off Hi=Read
  //  [6] AN6 (75)      0 [GPIO]         Output                     		Control1
  //  [7] AN7 (74)      0 [GPIO]         Output         				        Control2
#ifndef ASM_SOURCE
#if BOARD_REVISION=='A'
  # define DEFAULT_PANPAR   0x0F  // Dual Function Pin Assignment Register x 8 pins  0=gpio 1=primary 
  # define DEFAULT_DDRAN    0xF0	// 0=input 1=output
  # define DEFAULT_PORTAN   0x00
#elif BOARD_REVISION=='B'
  # define DEFAULT_PANPAR   0x1F  // Dual Function Pin Assignment Register x 8 pins  0=gpio 1=primary 
  # define DEFAULT_DDRAN    0xE0  // 0=input 1=output
  # define DEFAULT_PORTAN   0x00  //  Drive AN5 low to turn off Q15 
#endif
#endif
  //
  // PORT TI
  //  [0] FEC_COL 	(109)   1 [FEC]      N/A           N/A              FEC
  //  [1] FEC_CRS 	(108)   1 [FEC]      N/A           N/A              FEC
  //  [2] FEC_RXCLK (120)   1 [FEC]      N/A           N/A              FEC
  //  [3] FEC_RXD0  (122)   1 [FEC]      N/A           N/A              FEC
  //  [4] FEC_RXD1  (123)   1 [FEC]      N/A           N/A              FEC
  //  [5] FEC_RXD2  (126)	  1 [FEC]      N/A           N/A              FEC
  //  [6] FEC_RXD3  (127)	  1 [FEC]      N/A           N/A              FEC
  //  [7] FEC_RXDV  (121)   1 [FEC]      N/A           N/A              FEC
# define DEFAULT_PTIPAR   0xff // Dual Function Pin Assignment Register x 8 pins 0=gpio 1=primary 
# define DEFAULT_DDRTI    DDRTI
# define DEFAULT_PORTTI   PORTTI
  //
  // PORT TJ
  //  [0] FEC_RXER (119)    1 [FEC]      N/A           N/A              FEC
  //  [1] FEC_TXCLK(117)    1 [FEC]      N/A           N/A              FEC
  //  [2] FEC_TXD0 (113)    1 [FEC]      N/A           N/A              FEC
  //  [3] FEC_TXD1 (112)    1 [FEC]      N/A           N/A              FEC
  //  [4] FEC_TXD2 (111)    1 [FEC]      N/A           N/A              FEC
  //  [5] FEC_TXD3 (110)    1 [FEC]      N/A           N/A              FEC
  //  [6] FEC_TXEN (116)    1 [FEC]      N/A           N/A              FEC
  //  [7] FEC_TXER (118)    0 [GPIO}     Input         N/A              Ethernet Speed (Off = 10Mbps, On = 100Mbps)
# define DEFAULT_PTJPAR   0x7f // Dual Function Pin Assignment Register x 8 pins 0=gpio 1=primary 
# define DEFAULT_DDRTJ    0x7f // 0=input 1=output
# define DEFAULT_PORTTJ   PORTTJ
  //
  // PORT AS
  //  [0] SCL (28)      0 [GPIO]         Input         N/A              N/A
  //  [1] SDA (29)      0 [GPIO]         Input         N/A              N/A
  //  [2] MB_ALE (37)   1 [FlexBus]      N/A           N/A              Flex Bus
# define DEFAULT_PASPAR   0x10 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRAS    0x00 // 0=input 1=output
# define DEFAULT_PORTAS   PORTAS
  //
  // PORT NQ
  //  [0]
  //  [1] IRQ1 (103)    0 [GPIO]         Input         N/A              N/A
  //  [2]
  //  [3] IRQ3 (129)    2 [FEC]          N/A           N/A              FEC_MDIO
  //  [4]
  //  [5] IRQ5 (128)    2 [FEC]          N/A           N/A              FEC_MDC
  //  [6]
  //  [7] IRQ7 (96)     0 [GPIO]         Input         N/A              Reset-to-Defaults Button
# define DEFAULT_PNQPAR   0x0880 // Quad Function Pin Assignment Register x 7 pins (lsb not used) 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRNQ    0x00	//  0=input 1=output
# define DEFAULT_PORTNQ   PORTNQ
  //
  // PORT QS
  //  [0] QSPI_DOUT (25) 1 [QSPI]        N/A           N/A              SPI Data Out
  //  [1] QSPI_DIN  (24) 1 [QSPI]        N/A           N/A              SPI Data In
  //  [2] QSPI_CLK  (26) 1 [QSPI]        N/A           N/A              SPI Clock
  //  [3] QSPI_CS0  (27) 1 [QSPI]        N/A           N/A              SPI Chip Select 0
  //  [4]
  //  [5] QSPI_CS2  (23) 1 [QSPI]        N/A           N/A              SPI Chip Select 2
  //  [6] QSPI_CS3  (22) 1 [QSPI]        N/A           N/A              SPI Chip Select 3
  //  [7]
# define DEFAULT_PQSPAR   0b0001010001010101 // Quad Function Pin Assignment Register x 7 pins (msb not used) 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRQS    DDRQS	//  0=input 1=output
# define DEFAULT_PORTQS   PORTQS
  //
  // PORT TA
  //  [0] GPT0 (93)     0 [GPIO]         Input         N/A              N/A
  //  [1] GPT1 (94)     0 [GPIO]         Input         N/A              N/A
  //  [2] GPT2 (95)     0 [GPIO]         Input         N/A              N/A
  //  [3] GPT3 (58)     0 [GPIO]         Input         N/A              N/A
# define DEFAULT_PTAPAR   0x00 // Quad Function Pin Assignment Register x 4 pins  0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTA    0x00 // 0=input 1=output
# define DEFAULT_PORTTA   PORTTA
  //
  // PORT TC
  //  [0] DTIN0 (11)    1 [DTIN]          N/A		       N/A		          External Clock (7.3728MHz)
  //  [1] DTIN1 (12)    1 [DTIN]          N/A		       N/A              External Clock (7.3728MHz)
  //  [2] DTIN2 (65)    1 [DTIN]          N/A		       N/A		          External Clock (7.3728MHz)
  //  [3] DTIN3 (19)    0 [GPIO]          N/A          N/A           		N/A
# define DEFAULT_PTCPAR   0b0000000000010101 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRTC    0x00 // 0=input 1=output
# define DEFAULT_PORTTC   PORTTC
  //
  // PORT UA
  //  [0] UTXD0  (16)   1 [TXD0]          N/A			     N/A              UART0 TXD (Debugging)
  //  [1] URXD0  (17)   1 [RXD0]          N/A          N/A              UART0 RXD (Debugging)
  //  [2] !URTS0 (18)   0 [GPIO]          N/A	  		   N/A              N/A
  //  [3] !UCTS0 (15)   0 [GPIO]          N/A 			   N/A              N/A	
# define DEFAULT_PUAPAR   0b0000000000000101 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRUA    0x00  // 0=input 1=output
# define DEFAULT_PORTUA   PORTUA
  //
  // PORT UB
  //  [0] UTXD1  (62)   1 [TXD1]         N/A           N/A              UART1 TXD (BOB)
  //  [1] URXD1  (63)   1 [RXD1]         N/A           N/A              UART1 RXD (BOB)
  //  [2] !URTS1 (64)   1 [RTS1]         N/A           N/A              UART1 RTS (BOB)
  //  [3] !UCTS1 (61)   1 [CTS1]         N/A           N/A              UART1 CTS (BOB)
# define DEFAULT_PUBPAR   0b0000000001010101 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRUB    0x00
# define DEFAULT_PORTUB   PORTUB
  //
  // PORT UC
  //  [0] UTXD2  (101)  1 [TXD2]         N/A           N/A              UART2 TXD (XBee)
  //  [1] URXD2  (102)  1 [RXD2]         N/A           N/A              UART2 RXD (XBee)
  //  [2] !URTS2 (98)   1 [RTS2]         N/A           N/A              UART2 RTS (XBee)
  //  [3] !UCTS2 (97)   1 [CTS2]         N/A           N/A              UART2 CTS (XBee)
# define DEFAULT_PUCPAR   0b0000000001010101 // Quad Function Pin Assignment Register x 4 pins 0=gpio 1=primary 2=alt1 3=alt2
# define DEFAULT_DDRUC    0x00
# define DEFAULT_PORTUC   PORTUC
  //
  // PORT TE
  //  [0] FB_AD0 (57)   1 [MB_A0]        N/A           N/A              FlexBus
  //  [1] FB_AD1 (56)   1 [MB_A1]        N/A           N/A              FlexBus
  //  [2] FB_AD2 (55)   1 [MB_A2]        N/A           N/A              FlexBus
  //  [3] FB_AD3 (54)   1 [MB_A3]        N/A           N/A              FlexBus
  //  [4] FB_AD4 (53)   1 [MB_A4]        N/A           N/A              FlexBus
  //  [5] FB_AD5 (36)   1 [MB_A5]        N/A           N/A              FlexBus
  //  [6] FB_AD6 (35)   1 [MB_A6]        N/A           N/A              FlexBus
  //  [7] FB_AD7 (34)   1 [MB_A7]        N/A           N/A              FlexBus
# define DEFAULT_PTEPAR   0xff // Dual Function Pin Assignment Register x 8 pins 0=gpio 1=primary 
# define DEFAULT_DDRTE    DDRTE
# define DEFAULT_PORTTE   PORTTE
   //
   // PORT TF
   //  [0] FB_AD8  (33)  1 [MB_A8]        N/A           N/A             FlexBus
   //  [1] FB_AD9  (32)  1 [MB_A9]        N/A           N/A             FlexBus
   //  [2] FB_AD10 (6)   1 [MB_A10]       N/A           N/A             FlexBus
   //  [3] FB_AD11 (5)   1 [MB_A11]       N/A           N/A             FlexBus
   //  [4] FB_AD12 (4)   1 [MB_A12]       N/A           N/A             FlexBus
   //  [5] FB_AD13 (3)   1 [MB_A13]       N/A           N/A             FlexBus
   //  [6] FB_AD14 (2)   1 [MB_A14]       N/A           N/A             FlexBus
   //  [7] FB_AD15 (136) 1 [MB_A15]       N/A           N/A             FlexBus
# define DEFAULT_PTFPAR   0xff // Dual Function Pin Assignment Register x 8 pins
# define DEFAULT_DDRTF    DDRTF
# define DEFAULT_PORTTF   PORTTF
   //
   // PORT TG
   //  [0] FB_AD16 (133) 1 [MB_A16]       N/A           N/A             FlexBus
   //  [1] FB_AD17 (132) 1 [MB_A17]       N/A           N/A             FlexBus
   //  [2] FB_AD18 (131) 1 [MB_A18]       N/A           N/A             FlexBus
   //  [3] FB_AD19 (130) 1 [MB_A19]       N/A           N/A             FlexBus
   //  [4]
   //  [5] FB_!CS0 (52)  1 [MB_CS0]       N/A           N/A             FlexBus
   //  [6] FB_!OE (137)  1 [MB_OE]        N/A           N/A             FlexBus
   //  [7] FB_R/!W (45)  1 [MB_RW]        N/A           N/A             FlexBus
# define DEFAULT_PTGPAR   0xff // Dual Function Pin Assignment Register x 8 pins 0=gpio 1=primary 
# define DEFAULT_DDRTG    DDRTG
# define DEFAULT_PORTTG   PORTTG
   //
   // PORT TH
   //  [0] FB_D2  (47)   0 [GPIO]         1 [Output]    0 [Low]           Relay
   //  [1] FB_D3  (46)   0 [GPIO]         0 [Input]     0 [Low]
   //  [2] FB_D4  (1)    0 [GPIO]         0 [Input]     0 [Low]
   //  [3] FB_D5  (140)  0 [GPIO]         0 [Input]     0 [Low]
   //  [4] FB_D6  (139)  0 [GPIO]         0 [Input]     0 [Low]
   //  [5] FB_D7  (138)  0 [GPIO]         0 [Input]     0 [Low]
   //  [6] FB_D0  (51)   0 [GPIO]         1 [Output]    0 [Low]           Alarm LED
   //  [7] FB_D1  (50)   0 [GPIO]         1 [Output]    0 [Low]           BOB \RST
# define DEFAULT_PTHPAR   0x0000          // Quad Function Pin Assignment Register x 8 pins 0=gpio 1=primary 2=alt1 3=alt2

#ifndef ASM_SOURCE
#if BOARD_REVISION=='A'
  # define DEFAULT_DDRTH    0b11000001      // 0=input 1=output
  # define DEFAULT_PORTTH   0x80            // Disable \BOB_RST
#elif BOARD_REVISION=='B'
  # define DEFAULT_DDRTH    0b01001111      // 0=input 1=output
  # define DEFAULT_PORTTH   0x02            // Disable \BOB_RST
#endif        
#endif
        
        
#elif PRODUCT_TYPE == SENSAPHONE_400_CELLULAR
  // **************************************************************
  // 400 Cellular
  // **************************************************************

  // Processor configuration for the STM32F101 processor is defined
  // through the STM32CubeMX tool of the STM32CubeIDE.
        
#elif PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
  // **************************************************************
  // ZWave Sentinel
  // **************************************************************

  // Processor configuration for the STM32H743 processor is defined
  // through the STM32CubeMX tool of the STM32CubeIDE.

#elif !defined(BOOTLOADER)
# warning Unsupported hardware configuration!
# define DEFAULT_PANPAR   PANPAR
# define DEFAULT_DDRAN    DDRAN
# define DEFAULT_PORTAN   PORTAN
# define DEFAULT_PASPAR   PASPAR
# define DEFAULT_DDRAS    DDRAS
# define DEFAULT_PORTAS   PORTAS
# define DEFAULT_PNQPAR   PNQPAR
# define DEFAULT_DDRNQ    DDRNQ
# define DEFAULT_PORTNQ   PORTNQ
# define DEFAULT_PGPPAR   PGPPAR
# define DEFAULT_DDRGP    DDRGP
# define DEFAULT_PORTGP   PORTGP
# define DEFAULT_PTAPAR   PTAPAR
# define DEFAULT_DDRTA    DDRTA
# define DEFAULT_PORTTA   PORTTA
# define DEFAULT_PTCPAR   PTCPAR
# define DEFAULT_DDRTC    DDRTC
# define DEFAULT_PORTTC   PORTTC
# define DEFAULT_PTDPAR   PTDPAR
# define DEFAULT_DDRTD    DDRTD
# define DEFAULT_PORTTD   PORTTD
# define DEFAULT_PLDPAR   PLDPAR
# define DEFAULT_DDRLD    DDRLD
# define DEFAULT_PORTLD   PORTLD
# define DEFAULT_PQSPAR   PQSPAR
# define DEFAULT_DDRQS    DDRQS
# define DEFAULT_PORTQS   PORTQS
# define DEFAULT_PUAPAR   PUAPAR
# define DEFAULT_DDRUA    DDRUA
# define DEFAULT_PORTUA   PORTUA
# define DEFAULT_PUBPAR   PUBPAR
# define DEFAULT_DDRUB    DDRUB
# define DEFAULT_PORTUB   PORTUB
# define DEFAULT_PUCPAR   PUCPAR
# define DEFAULT_DDRUC    DDRUC
# define DEFAULT_PORTUC   PORTUC
#endif


/////////////////////////////////////////////////////////////////////////////////////
//  Ethernet PHY implementation
/////////////////////////////////////////////////////////////////////////////////////        
#if PRODUCT_TYPE == SENSAPHONE_WEB800 || PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR || PRODUCT_TYPE == WINDCHIME_BASELINE || PRODUCT_TYPE == WINDCHIME_MODBUS || PRODUCT_TYPE == WINDCHIME_STRATUS
//  These products use the National DP83848 PHY chip, which does not support interrupt,
//  therefore we must poll it to update the link status etc
  #define DP83848_PHY
#endif
        
        
        
        
        
 /**********************************************************************************************************************
 * Portability Definitions
 */
#ifndef ASM_SOURCE

//
// Setup the boolean types that are not C-standard
//
#ifndef false
# define false            0
#endif
#ifndef true
# define true             1
#endif
#ifndef TRUE
# define TRUE             true
#endif
#ifndef FALSE
# define FALSE            false
#endif

//
// The following type definitions are platform dependent and must be set before porting the programming API to a
// different platform or compiler.
//
typedef char                int8;
typedef signed char         sint8;
typedef unsigned char       uint8;
typedef unsigned char       ubyte;
typedef unsigned char       sboolean;

typedef short               int16;
typedef signed short        sint16;
typedef unsigned short      uint16;

typedef int                 int32;
typedef signed int          sint32;
typedef signed int          sbyte4;
typedef unsigned int        uint32;

// A word is at least 16-bits and matches most efficient representation for a given platform.
// On a coldfire this is 32-bits, but on an 8-bit 8051, this must be 16-bits even though 8-bits is more natural.
typedef int                 word_t;
typedef signed int          sword_t;
typedef unsigned int        uword_t;

// A halfword is at least 8-bits and matches most efficient representation for a given platform.
// This is equivelent to a word unless this is an 8-bit platform
typedef int                 hword_t;
typedef signed int          shword_t;
typedef unsigned int        uhword_t;
typedef unsigned int        wboolean;

typedef long long           int64;
typedef signed long long    sint64;
typedef unsigned long long  uint64;

#if PRODUCT_TYPE != SENSAPHONE_400_CELLULAR && PRODUCT_TYPE != SENSAPHONE_ZWAVE_SENTINEL
typedef unsigned long       time_t;
#endif


#if PRODUCT_TYPE == SENSAPHONE_400_CELLULAR || PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
typedef unsigned char       uint_8;
typedef unsigned short      uint_16;
typedef int                 int_32;
typedef unsigned int        uint_32;
typedef unsigned char       boolean;
typedef char* char_ptr;    /* signed character       */
typedef signed char         sbyte;
typedef unsigned int        ubyte4;

// from mqx.h
typedef struct time_struct
{

   /*! \brief The number of seconds in the time. */
   uint_32     SECONDS;

   /*! \brief The number of milliseconds in the time. */
   uint_32     MILLISECONDS;

} TIME_STRUCT/*, _PTR_ TIME_STRUCT_PTR*/;

typedef struct date_struct
{
   /*! \brief Range from 1970 to 2099. */
   uint_16 YEAR;

   /*! \brief Range from 1 to 12. */
   uint_16 MONTH;

   /*! \brief Range from 1 to 31 (depending on month). */
   uint_16 DAY;

   /*! \brief Range from 0 to 23. */
   uint_16 HOUR;

   /*! \brief Range from 0 to 59. */
   uint_16 MINUTE;

   /*! \brief Range from 0 to 59. */
   uint_16 SECOND;

   /*! \brief Range from 0 to 999. */
   uint_16 MILLISEC;

} DATE_STRUCT/*, _PTR_ DATE_STRUCT_PTR*/;


// from rtcs.h
typedef uint_32 _ip_address;


// from ipcfg.h
typedef struct ipcfg_ip_address_data
{
    _ip_address ip;
    _ip_address mask;
    _ip_address gateway;
} IPCFG_IP_ADDRESS_DATA/*, _PTR_ IPCFG_IP_ADDRESS_DATA_PTR*/;


#endif


union integer_size_checks{
  char char_is_size_1[sizeof(char) == 1 ? 1 : -1];
  char short_is_size_2[sizeof(short) == 2 ? 1 : -1];
  char int_is_size_4[sizeof(int) == 4 ? 1 : -1];
  char long_is_size_4[sizeof(long) == 4 ? 1 : -1];
  char long_long_is_size_8[sizeof(long long) == 8 ? 1 : -1];
  char word_size_is_at_least_2[sizeof(word_t) >= 2 ? 1 : -1];
};


#define UINT_STR(size)     (size == 1 ? 3 : (size == 2 ? 5 : 10))
#define UINT_STRSZ(type)   (UINT_STR(sizeof(type)))
#define SINT_STR(size)     (UINT_STR(size) + 1)
#define SINT_STRSZ(type)   (UINT_STR(sizeof(type)) + 1)
#define STRSZ(str)         (sizeof(str) - 1)

#endif // ASM_SOURCE

/*******************************************************************************
 * Automatic Configuration (Use the above to cause these changes)
 ******************************************************************************/
#if IS_MULTI_THREADED != 0
# define MULTITHREADED
# define USE_MUTEXES
# ifdef ASM_SOURCE
          USE_RTOS .equ 1
# else
#  if PRODUCT_TYPE != SENSAPHONE_400_CELLULAR && PRODUCT_TYPE != SENSAPHONE_ZWAVE_SENTINEL
#  define USE_RTOS
#  endif
# endif

# define MUTEX_TIMERS		0    /* ptimers.c mutex */
# define MUTEX_PIO		1    /* pio.c mutex */
# define MUTEX_IO		2  	 /* coldio.c mutex */
# define MUTEX_ALARMS		3    /* inputs.c mutex */
# define MUTEX_EEPROM		4    /* EEPROM high level mutex */
# define MUTEX_EEPROM_LOWLEVEL	5    /* EEPROM low level mutex */
# define MUTEX_DMA     		6    /* DMA hardware Mutex */
# define MUTEX_RTC              7    /* Realtime Clock Mutex */
#ifndef ASM_SOURCE
#if defined(BUILD_SUPPORT_IMS_PROTOCOL) && BUILD_SUPPORT_IMS_PROTOCOL == true
# define MUTEX_IMS_ALARM_QUE    8    /* IMS Alarm Message Que Mutex */
# define TOTAL_MUTEXES		9 /* Total Number of Mutexes available */
#else
# define TOTAL_MUTEXES		8 /* Total Number of Mutexes available */
#endif
#endif
# define NUM_NONEXCLUSIVE_TIMERS 2
# define NUM_EXCLUSIVE_TIMERS	 4 + MAX_PROCESSES /* 1: PIO, 2: Wireless, 3: Console, 4: IP Alarms */
#else
# define TOTAL_MUTEXES		 0
# define MAX_PROCESSES		 1
#endif

#if IS_DEBUG_MODE > 0
# define USE_DEBUG
#endif

//
// Coldfire Battery, Power, and Charging Levels:
//	 Voltage Circuitry:
//			100k/400k * Vbat = Vad -> Vbat = (1/4096) * 3.3v * (400k/100k)
//      1 A/D Count = .000806v (Reference) -> .003223v (Device)
//   At ~xxxv, the 3.3v signal on the board drops out of spec (TBD: Verify this for coldfire!)
//   At ~yyyv, the board enters into a oscillating reset      (TBD: Verify this for coldfire!)
//
// WARNING: THESE A/D VALUES MUST BE IN A 12-BIT SCALE.  IF THIS CHANGES, "inputs.c" MUST BE UPDATED!
//
# define NO_AD				(78)	//  0.25v
# define HALF_VOLT_AD			(155)	//  0.50v
# define BATTERY_POWER_CHARGE_DIFF_AD   (78)	//  0.25v
# define HIB_DEADBAND_AD		(155)	//  0.50v
# define LOW_BATTERY_AD            	(1862)	//  6.00v - Low Battery Level
# define LOW_BATTERY_FIXED         	(600)	//  6.00v - Low Battery Level
# define NO_BATTERY_AD			(776) 	//  2.50v - No Battery Level (LED)
# define MIN_BATTERY_AD			(155)	//  0.50v - Minimum Charging Battery Level
# define HIB_ON_BATTERY_AD             	(1706)  //  5.50v - When to turn hibernation on
# define HIB_OFF_BATTERY_AD            	((HIB_ON_BATTERY_AD) + (HIB_DEADBAND_AD))
#if PRODUCT_TYPE == SENSAPHONE_WIRELESS_NODE  || \
    PRODUCT_TYPE == SENSAPHONE_WEB_WIRELESS   || \
    PRODUCT_TYPE == SENSAPHONE_DRY_CONTACT_NODE     || \
    PRODUCT_TYPE == SENSAPHONE_WIRED_NODE
# define   LOW_POWER_AD			(1939)    //  6.25v - Low Power Level
# define   LOW_POWER_FIXED              (625)     //  6.25v - Low Power Level
# define   NO_POWER_AD 			(1086)    //  3.50v - No Power Level (LED)
# define   HIB_ON_POWER_AD              (1862)    //  6.00v - When to turn hibernation on
# define   HIB_OFF_POWER_AD             ((HIB_ON_POWER_AD)   + (HIB_DEADBAND_AD))
#elif PRODUCT_TYPE == SENSAPHONE_WEB600         || \
      PRODUCT_TYPE == SENSAPHONE_WEB800         || \
      PRODUCT_TYPE == WINDCHIME_BASELINE        || \
      PRODUCT_TYPE == WINDCHIME_MODBUS          || \
      PRODUCT_TYPE == WINDCHIME_STRATUS         || \
      PRODUCT_TYPE == SENSAPHONE_VIDEO_OVLY_MONITOR
# define   LOW_POWER_AD			(1319)    //  4.25v - Low Power Level
# define   LOW_POWER_FIXED              (425)     //  4.25v - Low Power Level
# define   NO_POWER_AD 			(1086)    //  3.50v - No Power Level (LED)
# define   HIB_ON_POWER_AD              (1241)    //  4.00v - When to turn hibernation on
# define   BAT_OFFSET_AD                (22)      //  0.0089775v on 3.3v scale - Switch Voltage Loss
# define   HIB_OFF_POWER_AD             ((HIB_ON_POWER_AD)   + (HIB_DEADBAND_AD))
#elif PRODUCT_TYPE == SENSAPHONE_400_CELLULAR || PRODUCT_TYPE == SENSAPHONE_ZWAVE_SENTINEL
// 400 Cellular uses an STM32 processor
# define   LOW_POWER_FIXED              (330)     //  3.30v - Low Power Level
#elif !defined(BOOTLOADER)
# warning No Power Settings Defined for this Target
#endif
#if PRODUCT_TYPE != SENSAPHONE_400_CELLULAR && PRODUCT_TYPE != SENSAPHONE_ZWAVE_SENTINEL
# define   MAX_BATTERY_CHARGE_ON_AD		(2172)		//  7.00v
# define   MAX_BATTERY_CHARGE_OFF_AD		(2125)    //  6.85v
# define   HIGH_LCD_AD				(2172)		//  7.00v
# define   LOW_LCD_AD				(1427)		//  4.60v
#endif

/*******************************************************************************
 * System Includes
 ******************************************************************************/
#if IS_MULTI_THREADED != 0 && defined(USE_RTOS) && !defined(BOOTLOADER) && PRODUCT_TYPE!=SENSAPHONE_400_CELLULAR && PRODUCT_TYPE != SENSAPHONE_ZWAVE_SENTINEL
# include "RTOS.h"
#endif

/*******************************************************************************
 * Miscellaneous Definitions
 ******************************************************************************/
 
#ifdef DEBUG_LEVEL
  #if defined(DEBUG_LEVEL) && (DEBUG_LEVEL > 0)
    #define ABORT(msg)        while(1) {}
    #define ASSERT(Condition) if  (!(Condition))  while(1) {}
    #define CT_ASSERT(pred) switch(0){case 0:case pred:;}
  #else
    #define ABORT(msg)
	  #define	ASSERT(Condition)
  #endif
#endif


#define POWER_UP(x)
#define POWER_DOWN(x)

#if defined(IS_DEMO_VERSION) && IS_DEMO_VERSION == 1
# undef VERSION_A
# define VERSION_A	"x"
#endif

#endif /* GCONFIG_H_ */
