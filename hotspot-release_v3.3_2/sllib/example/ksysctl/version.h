/***********************************************************
*   chip : silan8925A
************************************************************
*   product   | function                  |   version :
*   HSV892    | one more                  | 0.0.x
*   HSV893    | SWITCH control            | 0.1.x
*   HSV562    | KVM control + one more    | 2.0.x
*   HSV592    | KVM control + more one    | 2.1.x
*   HSV701    | WEB control + distribute  | 1.0.x
*   HSV761    | WEB control + centralize  | 1.1.x
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

#define PRODUCT_NAME   "HSV592"     //product name
#define VER_ID1      "2"          //major version id 
#define VER_ID2      "1"          //minor version id 
#define VER_ID3      "4"             //sbuversion id 
#define BUILDING      "20190903"        //date
#define CURRTNE_STATE "alpha"  //test version 

#define PRINT_VERSION    "HSV592 V2.1.4.190903_ALPHA"
#define WEB_VERSION      "V2.1.4.190903"
#define OSD_VERSION      "V2.1.4"

/*
 * V2.1.3 : 
 * V2.1.4 : debug RX jam issue when TX input 32khz audio.

*/


#endif


