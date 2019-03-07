#ifndef _SL_VPG_H_
#define _SL_VPG_H_

struct SLVPG_InitSurface_in_type 
{
	enum SLVPP_ColorMode ColorMode;
	enum SLVPP_ColorFormat ColorFormat;
	enum SLVPP_ColorSpace ColorSpace;	
	unsigned int Width;
	unsigned int Height;
	unsigned int RLDLen;
};

struct SLVPG_InputWindow 
{
	unsigned int X;
	unsigned int Y;
	unsigned int Width;
	unsigned int Height;
	enum SLVPPWindowValueMode Mode;
};

struct SLVPG_OutputWindow 
{
	unsigned int X;
	unsigned int Y;
	unsigned int Width;
	unsigned int Height;
	enum SLVPPWindowValueMode Mode;
};

typedef struct SLVPG_IOWindow
{
	struct SLVPG_InputWindow in;
	struct SLVPG_OutputWindow out;	   
}SLVPG_IOWindow_t;

#endif
