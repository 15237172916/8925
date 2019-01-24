#ifndef _SL_PARAM_VTK_DSP_H
#define _SL_PARAM_VTK_DSP_H

#include "sl_param_dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define		MAX_NUM_BLOBS				    32

#if 0
#ifndef BOARD_TYPE_8923
typedef struct PTZCONTROL_INFO_SIMPLE{
	unsigned char     byPtzIndex; 
	unsigned int	  dwTrackId; 
	float       xkn;    //propogate n frame for position
	float		ykn;
	float		xk1;    //propogate 1 frame for speed
	float		yk1;
	float		xk;
	float		yk;
	unsigned short		tw;
	unsigned short		th;
	unsigned char       bFlag;//   bit 0 is posFlag   bit 1 is zoomAndFocus  bit 2 is stopPTZFlag
    unsigned char       TracksFlag;	
    unsigned char       RoiLevel;
}PTZCONTROL_INFO_SIMPLE_T;
typedef   struct    VIDEO_TRACK_TARGET_INFO{
    int  nTotalTrackNum;   
    int  nCurFollowTrackNum;   
    float       deltaT;
    PTZCONTROL_INFO_SIMPLE_T            tPtzCtrl[8];//ptz control info come from Track Algm.
}VIDEO_TRACK_TARGET_INFO_T;
#endif
#if 0
typedef struct {
	int		xkn;
	int		ykn;
	int		xk1;
	int		yk1;
	int		xk;
	int		yk;
	int		tw;
	int		th;
	unsigned char bFlag;
}ALTAIRTTM_PTZCONTROL_PARAM_STRUCT;
#endif
typedef struct 
{
    int         iDeviceId;
    int         dwTrackId;
    int         dwTargetId;
    int         iTargetWidth;
    int         iTargetHeight;
    int         iInRoiLevel;
    int         iTargetType;
    int         targetObsX;
    int         targetObsY;     
    int         iTripWireNo;
    int         dbTargetPosX;
    int         dbTargetPosY;
    int         dbTargetSpeedX;
    int         dbTargetSpeedY;
    int         sigma_x_sqr;
    int         sigma_y_sqr;
    int         sigma_xy;
    int         bSelectedTrack;
    int         bFollowedNow;
    int         bConfirmedTrack;
    int         dwTrackCreateTime;
    int         dwStartTrackTime;
    long        lTrackContext;
    int         dwTargetColor;
    char        chTargetId[36];
//  char        chPictureUrl[_MAX_PATH];
    int         iInRoiNum;
}ALTAIRTTM_TRACKINFO_STRUCT;


typedef  struct  VIDEO_TRACK_RESULT{
    int     TargetNum;
    ALTAIRTTM_TRACKINFO_STRUCT   tTargetInfo[MAX_NUM_BLOBS]; 
	//ALTAIRTTM_PTZCONTROL_PARAM_STRUCT ptzControlInfo;
	VIDEO_TRACK_TARGET_INFO_T	ptzControlInfo;
	int 	isNeedPTZControl;
	int		deltaT;			//in unit ms
}VIDEO_TRACK_RESULT_T;
#else

typedef unsigned short Bool;

typedef struct {
	unsigned char     byPtzIndex; 
	unsigned int	  dwTrackId; 
	float       xkn;    //propogate n frame for position
	float		ykn;
	float		xk1;    //propogate 1 frame for speed
	float		yk1;
	float		xk;
	float		yk;
	unsigned short		tw;
	unsigned short		th;
	unsigned char       bFlag;//   bit 0 is posFlag   bit 1 is zoomAndFocus  bit 2 is stopPTZFlag
    unsigned char       TracksFlag;	
    unsigned char       RoiLevel;
}ALTAIRTTM_PTZCONTROL_PARAM_STRUCT;

typedef   struct    VIDEO_TRACK_TARGET_INFO{
    int  nTotalTrackNum;   
    int  nCurFollowTrackNum;   
    float       deltaT;
    ALTAIRTTM_PTZCONTROL_PARAM_STRUCT       tPtzCtrl[8];//ptz control info come from Track Algm.
}VIDEO_TRACK_TARGET_INFO_T;


/*°èÏßÉèÖÃ*/
typedef struct ALTAIRTTM_TRIPWIRE_STRUCT
{
	int x1;
	int y1;
	int x2;
	int y2;
	int direction; // "-1" from left to right, "+1" from right to left, "0" both directions
}ALTAIRTTM_TRIPWIRE_STRUCT;

typedef struct 
{
  	int			iDeviceId;
	int 		      dwTrackId;
	int 		      dwTargetId;
	int			iTargetWidth;
	int			iTargetHeight;
	int			iInRoiLevel;
	int			iTargetType;
	int			targetObsX;
	int			targetObsY;		
	int			iTripWireNo;
	float 		dbTargetPosX;
	float	             dbTargetPosY;
	float		      dbTargetSpeedX;
	float		      dbTargetSpeedY;
	float		      sigma_x_sqr;
	float  		sigma_y_sqr;
	float 		sigma_xy;
	Bool		      bSelectedTrack;
	Bool		      bFollowedNow;
	Bool		      bConfirmedTrack;
	int 		      dwTrackCreateTime;
	int		      dwStartTrackTime;
	long		      lTrackContext;
	int 		     dwTargetColor;
	char		     chTargetId[36];
	int		     iInRoiNum;
}ALTAIRTTM_TRACKINFO_STRUCT;

typedef  struct  VIDEO_TRACK_RESULT{
    int     TargetNum;
    ALTAIRTTM_TRACKINFO_STRUCT   tTargetInfo[MAX_NUM_BLOBS]; 
	//ALTAIRTTM_PTZCONTROL_PARAM_STRUCT ptzControlInfo;
	VIDEO_TRACK_TARGET_INFO_T	ptzControlInfo;
	int 	isNeedPTZControl;
	int		deltaT;			//in unit ms
}VIDEO_TRACK_RESULT_T;
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VTK_DSP_H */
