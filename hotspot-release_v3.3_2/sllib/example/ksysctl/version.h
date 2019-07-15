/***********************************************************
*   chip : silan8925A
************************************************************
*   product      | function                        |   version :
*   HSV892       | one more                        |     0.0.x
*   HSV893       | SWITCH control more             |     0.1.x
*   HSV895/6     | one more + double IR + uart     |     2.2.x
*   HSV562       | KVM control + one more          |     2.0.x
*   HSV592       | KVM control + more one          |     2.1.x
*   HSV701       | WEB control + distribute        |     1.0.x
*   HSV761       | WEB control + centralize        |     1.1.x
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

#define PRODUCT_NAME   "HSV895/6"     //product name
#define VER_ID1      "2"          //major version id 
#define VER_ID2      "2"          //minor version id 
#define VER_ID3      "2"             //sbuversion id 
#define BUILDING      "20190715"        //date
#define CURRTNE_STATE "debug" 

#define PRINT_VERSION    "HSV895/6 V2.2.2.190715_R"
#define WEB_VERSION      "V2.2.2.190715"
#define OSD_VERSION      "V2.2.2"


#endif
/*
update log:
	HSV895/6_V2.2.1 baseline version double UART function  
	HSV895/6_V2.2.2 add change bitrate in HTML

*/

