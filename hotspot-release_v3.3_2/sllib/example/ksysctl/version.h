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

#define PRODUCT_NAME   "HSV893"     //product name
#define VER_ID1      "0"          //major version id 
#define VER_ID2      "1"          //minor version id 
#define VER_ID3      "4"             //sbuversion id 
#define BUILDING      "20190829"        //date
#define CURRTNE_STATE "release" 

#define PRINT_VERSION    "HSV893 V0.1.4.190829_R"
#define WEB_VERSION      "V0.1.4.190829"
#define OSD_VERSION      "V0.1.4"

/* history version explained
 * V0.1.1 : base version
 * V0.1.2 : add checksum of audio
 * 	    add RX's display if no input of TX
 * V0.1.3 : debug audio error 
 * V0.1.4 : debug rx carsh of 24h timeout  
 *
 *
 * */


#endif


