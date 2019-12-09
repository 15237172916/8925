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

#define PRODUCT_NAME   "HSV701"     //product name
#define VER_ID1      "1"          //major version id 
#define VER_ID2      "0"          //minor version id 
#define VER_ID3      "3"             //sbuversion id 
#define BUILDING      "20191209"        //date
#define CURRTNE_STATE "release" 

#define PRINT_VERSION    "HSV701 V1.0.3.191209_R"
#define WEB_VERSION      "V1.0.3.191209"
#define OSD_VERSION      "V1.0.3"

/*
V1.0.1 base version 
V1.0.2 repair switch key order
V1.0.3 debug writing config file fail issue
*/

#endif


