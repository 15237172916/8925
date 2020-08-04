/***********************************************************
*   chip : silan8925A
************************************************************
*   product   |   function  |   version :
*   HSV892  | one more                  | 0.0.x
*   HSV893  | SWITCH control            | 0.1.x
*   HSV562  | KVM control + one more    | 2.0.x
*   HSV592  | KVM control + more one    | 2.1.x
*   HSV701  | WEB control + distribute  | 1.0.x
*   HSV761  | WEB control + centralize  | 1.1.x
*   HSV900/922  
*************************************************************
*   current state |  abridge
*   release       |    _R
*   beta          |    _BETA
*   baseline      |    _PD_BL
*   alpha         |    _ALPHA
*
**/


#ifndef _VERSION_H_
#define _VERSION_H_


#define VER_ID1      "1"          //major version id 
#define VER_ID2      "1"          //minor version id 
#define VER_ID3      "3"             //sbuversion id 
#define BUILDING      "20200319"        //date
#define CURRTNE_STATE "release" 

#define PRINT_VERSION    "V1.5.3.200319_R"
#define OSD_VERSION      "V1.5.3"

/***************************define*******************************/
#define WATCHDOG
#define WATCHDOG_UART
#define WEB_ENABLE
#define VIDEO_IN
#define AUDIO_IN
#define APP_RTP
#define VIU_DISPLAY
#define IGMP_ENABLE
#define CONFIG_FLASH_TX

#if HSV761
#define HSV761
#define PRODUCT_NAME   "HSV761"     //product name
#define WEB_VERSION      "V1.1.4.0529_logo"
#define SWITCH_KEY
#define CONFIG_EEPROM
#define CONFIG_FLASH_RX
#define BROAD_CONTROL
#define BACKUP
#define DISPLAY_LOGO
#endif /* HSV761 */

#if HSV7011
#define HSV7011
#define PRODUCT_NAME   "HSV7011"     //product name
#define WEB_VERSION      "HSV7011V1.1.3"
#define KEY
#define CONFIG_EEPROM
#define CONFIG_FLASH_RX
#define BROAD_CONTROL
#define DIGIT_LED
#endif /* HSV7011 */

#if HSV7012
#define HSV7012
#define PRODUCT_NAME   "HSV7012"     //product name
#define WEB_VERSION      "HSV7012V1.1.3"
#define KEY
#define CONFIG_EEPROM
#define CONFIG_FLASH_RX
#define DIGIT_LED
#define IR_ENABLE
#endif /* HSV7012 */

#endif


