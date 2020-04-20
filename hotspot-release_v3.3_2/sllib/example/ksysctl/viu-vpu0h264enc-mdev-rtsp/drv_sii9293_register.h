#ifndef __DRV_SII9293_REGISTER_H__
#define __DRV_SII9293_REGISTER_H__

#define SET_BITS 			0xFF
#define CLEAR_BITS		0x00
#define BIT0 					0x01
#define BIT1					0x02
#define BIT2					0x04
#define BIT3					0x08
#define BIT4					0x10
#define BIT5					0x20
#define BIT6					0x40
#define BIT7					0x80


/********************************I2C Device Addr*************************************/
/********************Page1***************/
//0x64
#define SYSTEM				0x64		//0x00-0x19
#define HDCP_B				0x64		//0x1A-0x38 & 0xD0-0xF2
#define VIDEO					0x64		//0x3A-0x6F
#define AUTO_CONTROL	0x64		//0xB5-0xBA
#define ECC						0x64		//0xBB-0xCE
#define POWER_A				0x64		//0x08[0]

/*******************Page2****************/
//0x68
#define ACR						0x68		//0x00-0x18
#define AUDIO_OUT			0x68		//0x26-0x39
#define POWER_B				0x68		//0x3C-0x3F
#define PACKETS				0x68		//0x40-0xFF

/***************************************/
//0x74
#define HDCP_A				0x74		//0x00-0xFF

/***************************************/
//0x80
#define CEC						0x80		//0x00-0x0C

/***************************************/
//0xC0
#define HML_CBUS			0xC0		//0x00-0xFF

/***************************************/
//0xD0
#define GPIO					0xD0		//0x01-0x03, 0x06
#define RX						0xD0		//0x6C-0x70

/*************************************/
//0xE0
#define XVYCC					0xE0		//0x00-0x7F

/************************************************************************************/

/*****************************HDCP Register Address*******************************/
///////////////////////////////////0x00-0xFF////////////////////////////////////////
/*********************-BKSV-*******************/
#define BKSV1					0x00
#define BKSV2					0x01
#define BKSV3					0x02
#define BKSV4					0x03
#define BKSV5					0x04
/********************-R-***********************/
#define R_1						0x08
#define R_2						0x09
/******************-AKSV-**********************/
#define AKSV1					0x10
#define AKSV2					0x11
#define AKSV3					0x12
#define AKSV4					0x13
#define AKSV5					0x14
/*******************-AN-**************************/
#define AN1						0x18
#define AN2						0x19
#define AN3						0x1A
#define AN4						0x1B
#define AN5						0x1C
#define AN6						0x1D
#define AN7						0x1E
#define AN8						0x1F
/*******************-BCAPS-**************************/
#define BCAPS					0x40
/*******************-BSTATUS-***********************/
#define BSTATUS1			0x41
#define BSTATUS2			0x42
/*******************-V.H-**************************/
#define V_H0_0				0x20
#define V_H0_1				0x21
#define V_H0_2				0x22
#define V_H0_3				0x23
#define V_H1_0				0x24
#define V_H1_1				0x25
#define V_H1_2				0x26
#define V_H1_3				0x27
#define V_H2_0				0x28
#define V_H2_1				0x29
#define V_H2_2				0x2A
#define V_H2_3				0x2B
#define V_H3_0				0x2C
#define V_H3_1				0x2D
#define V_H3_2				0x2E
#define V_H3_3				0x2F
#define V_H4_0				0x30
#define V_H4_1				0x31
#define V_H4_2				0x32
#define V_H4_3				0x33
/*******************-KSV_FIFO-*********************/
#define	KSV_FIFO			0x43

/*****************************SYSTEM Register Address*******************************/
///////////////////////////////////0x00-0x19////////////////////////////////////////

/**********ID and System Initialization Registers************/
#define VND_IDL				0x00
#define VND_IDH				0x01
#define DEV_IDL				0x02
#define DEV_IDH				0x03
#define DEV_REV				0x04

/*************************-Software Reset Register-************************/
#define SRST					0x05	//Software Reset Register
#define STATE					0x06	//System Status Register
#define RX_SRST2			0x07	//Software Reset Register 2
#define SYS_CTRL1			0x08 	//System Control Register 1
#define SYS_SWTCHC		0x09	//SYS Switch Configuration Register
#define SYS_PSTOP			0x0A	//SYSPCLK Stop Register
#define HOT_PLUG_CTRL	0x0B	//Hot Plug Control Register
#define KSV_FIFO_CTRL	0x0D	//KSV FIFO Control Register
#define SYS_TMDS_CH_MAP	0x0E	//SYS TMDS CH Map
#define PCLK_CNT_MAX	0x0F	//PCLK Auto Stop Register

/*********************-Slave Address Mapping Registers-***********/
#define RX_SLAVE_ADDR7	0x10	//Page 7 Slave Address Register
#define RX_SLAVE_ADDR2	0x11	//Page 2 Slave Address Register
#define TX_SLAVE_ADDR4	0x12	//Page 4 Slave Address Register
#define RX_SLAVE_ADDR1	0x13	//Page 1 Slave Address Register
#define RX_SLAVE_CEC		0x14	//Page CEC Slave Address Register
#define RX_SLAVE_CBUS		0x15	//RX_SLAVE_CBUS
#define RX_SLAVE_ADDR9	0x16	//Page 9 Slave Address Register
#define RX_SLAVE_ADDR5	0x18	//Page 5 Slave Address Register
#define RX_SLAVE_ADDR6	0x19	//Page 6 Slave Address Register

/*************************HDCP Registers**********************/
#define HDCP_BKSV1			0x1A	//HDCP Shadow BKSV Registers
#define HDCP_BKSV2			0x1B
#define HDCP_BKSV3			0x1C
#define HDCP_BKSV4			0x1D
#define HDCP_BKSV5			0x1E
#define HDCP_RI1				0x1F	//HDCP Shadow Ri Registers
#define HDCP_RI2				0x20
#define HDCP_AKSV1			0x21
#define HDCP_AKSV2			0x22
#define HDCP_AKSV3			0x23
#define HDCP_AKSV4			0x24
#define HDCP_AKSV5			0x25
#define HDCP_AN1				0x26	//HDCP Shadow AN Registers
#define HDCP_AN2				0x27
#define HDCP_AN3				0x28
#define HDCP_AN4				0x29
#define HDCP_AN5				0x2A
#define HDCP_AN6				0x2B
#define HDCP_AN7				0x2C
#define HDCP_AN8				0x2D
#define HDCP_BCAPS_SET	0x2E	//HDCP BCAPS_SET Register
#define SHD_BSTAT1			0x2F	//HDCP BSTATUS_SET Register 1
#define SHD_BSTAT2			0x30	//HDCP BSTATUS_SET Register 2
#define HDCPCTRL				0x31	//HDCP Debug Register
#define HDCP_STAT				0x32	//HDCP Status Register
#define KSVSHA_ST1			0x33	//HDCP KSV/SHA Start Register
#define KSVSHA_ST2			0x33
#define SHA_LEN1				0x35	//HDCP SHA Length Register
#define SHA_LEN2				0x36
#define SHA_CTRL				0x37	//HDCP SHA Control Register
#define HDCP_KSV_FIFO				0x38	//HDCP Repeater KSV FIFO Register

/****************************Video Input Registers*************************/
#define H_RESL					0x3A	//Video H Resolution Registers
#define H_RESH					0x3B
#define V_RESL					0x3C	//Video V Refresh Registers
#define V_RESH					0x3D
#define DE_PIX1					0x4E	//Video DE Pixels Registers
#define DE_PIX2					0x4F
#define DE_LINL					0x50	//Video DE Line Registers
#define DE_LINH					0x51
#define VID_VTAVL				0x52	//Video VSYNC to Active Video Lines Register
#define VID_VFP					0x53	//Video Vertical Front Porch Register
#define VID_STAT				0x55	//Video Status Register
#define VID_HFP1				0x59	//Video Horizontal Front Porch Registers
#define VID_HFP2				0x5A
#define VID_HSWID1			0x5B	//Video Hsync Active Width Registers
#define VID_HSWID2			0x5C
#define VID_XPCNT1			0x6E	//Video Pixel Clock Timing Registers
#define VID_XPCNT2			0x6F
#define VID_XPM_MODE		0x68	//Video Channel PCLK Count Multiplication Register
#define VID_XPM_BASE		0x69	//Video Channel PCLK Count Base Register
#define VID_XPCLK_EN		0x6A	//Video Channel 0 XCLK To PCLK Update Register
#define VID_XPC_BASE1		0x6B	//Video Channel 0 Pixel Clock Count Base Registers
#define VID_XPC_BASE2		0x6C	//
#define VID_XPC_THRESH	0x6D	//Video Channel 0 Pixel Clock Count Base Registers

/************************-Video Processing Registers-*****************************/
#define VID_CTRL				0x48	//Video Control Register
#define VID_MODE2				0x49	//Video Mode 2 Register
#define VID_MODE				0x4A	//Video Mode 1 Register
#define VID_BLANK1			0x4B	//Video Digital Blank Value Registers
#define VID_BLANK2			0x4C
#define VID_BLANK3			0x4D
#define VID_F2BPM				0x54	//Video Field 2 Back Porch Mode Register
#define VID_CH_MAP			0x56	//Video Channel Map Register
#define VID_CTRL1				0x57	//Video Control 2 Register
#define GCP_HEADER			0x62	//General Control Packet Header Register
#define PHASE_LUT				0x63	//General Control Packet Phase LUT Register

/************************-Auto Video Configuration Registers-********************/
#define VID_AOF					0x5F	//Auto Output Format Register

/************************-Interrupt Registers-**********************************/
#define INTR_STATE			0x70	//Interrupt State Register
#define INTR1						0x71	//Interrupt Status 1 Register
#define INTR2						0x72	//Interrupt Status 2 Register
#define INTR3						0x73	//Interrupt Status 3 Register
#define INTR4						0x74
#define INTR1_MASK			0x75	//Interrupt 1 Mask Register
#define INTR2_MASK			0x76
#define INTR3_MASK			0x77
#define INTR4_MASK			0x78
#define INTR_CTRL				0x79	//Interrupt Control Register
#define IP_CTRL					0x7A	//Interrupt Packet Control Register
#define INTR5						0x7B	//Interrupt Status 5 Register
#define INTR6						0x7C	//Interrupt Status 6 Register
#define INTR5_MASK			0x7D	//Interrupt 5 Mask Register
#define INTR6_MASK			0x7E
#define RX_INTR7				0x90	//Interrupt Status 7 Register
#define RX_INTR8				0x91	//Interrupt Status 8 Register
#define INTR7_MASK			0x92	//Interrupt 7 Mask Register
#define RX_INTR8_MASK		0x93	//Interrupt 8 Mask Register
#define RX_INTR_STATE_2	0x94	//Interrupt State 2 Register

/************************-Auto Audio and Video Control-**********************************/
#define AEC0_CTRL				0xB5	//Auto Audio and Video Control Register
#define AEC1						0xB4	//AAC Control Register

/************************-Auto Audio Control (AAC) Registers-****************************/
#define AEC_EN1					0xB6	//Audio Exception Enable Registers
#define AEC_EN2 				0xB7
#define AEC_EN3					0xB8
#define AVC_EN1					0xB9	//AVC Enable Registers
#define AVC_EN2					0xBA

/************************-ECC Control Registers-***************************/
#define ECC_CTRL				0xBB	//ECC Control Register
#define BCH_THRESH			0xBC	//ECC BCH Threshold Register
#define T4_THRES				0xBD	//ECC T4 Corrected Threshold Register
#define T4_UNTHRES			0xBE	//ECC T4 Uncorrected Threshold Register
#define PKT_THRESH			0xBF
#define PKT_THRESH2			0xC0
#define T4_PKT_THRES		0xC1
#define T4_PKT_THRES2		0xC2
#define BCH_PKT_THRES		0xC3
#define BCH_PKT_THRES2	0xC4
#define HDCP_THRES			0xC5
#define HDCP_THRES2			0xC6
#define PKT_CNT					0xC7
#define PKT_CNT2				0xC8
#define T4_ERR					0xC9
#define T4_ERR2					0xCA
#define BCH_ERR					0xCB
#define BCH_ERR2				0xCC
#define HDCP_ERR				0xCD
#define HDCP_ERR2				0xCE

/************************-HDCP Repeater Registers-***************************/
#define SHA_CTRL2				0xD0	//SHA Control 2 Register
#define V_RDY_STATUS		0xD1	//Value Ready Status Register
#define KSV_SRC_SEL			0xD2	//KSV Source Select Register
#define EDID_CTRL				0xD3	//EDID Control Register
#define DS_BSTAT1				0xD5	//Downstream BSTATUS 1 Register
#define DS_BSTAT2				0xD6	//Downstream BSTATUS 2 Register
#define DS_M0_0					0xD7	//Downstream M0 Registers
#define DS_M0_1					0xD8
#define DS_M0_2					0xD9
#define DS_M0_3					0xDA
#define DS_M0_4					0xDB
#define DS_M0_5					0xDC
#define DS_M0_6					0xDD
#define DS_M0_7					0xDE
#define DS_VH0_0				0xDF	//Downstream VH Registers
#define DS_VH0_1				0xE0
#define DS_VH0_2				0xE1
#define DS_VH0_3				0xE2
#define DS_VH1_0				0xE3
#define DS_VH1_1				0xE4
#define DS_VH1_2				0xE5
#define DS_VH1_3				0xE6
#define DS_VH2_0				0xE7
#define DS_VH2_1				0xE8
#define DS_VH2_2				0xE9
#define DS_VH2_3				0xEA
#define DS_VH3_0				0xEB
#define DS_VH3_1				0xEC
#define DS_VH3_2				0xED
#define DS_VH3_3				0xEE
#define DS_VH4_0				0xEF
#define DS_VH4_1				0xF0
#define DS_VH4_2				0xF1
#define DS_VH4_3				0xF2

/**************************-Extended Color Gamut Registers-************************/
#define GBD_BYTE1							0x61	//GBD Packet Registers
#define GBD_BYTE2							0x62
#define GBD_BYTE3							0x63
#define GBD_BYTE4							0x64
#define GBD_BYTE5							0x65
#define GBD_BYTE6							0x66
#define GBD_BYTE7							0x67
#define GBD_BYTE8							0x68
#define GBD_BYTE9							0x69
#define GBD_BYTE10						0x6A
#define GBD_BYTE11						0x6B
#define GBD_BYTE12						0x6C
#define GBD_BYTE13						0x6D
#define GBD_BYTE14						0x6E
#define GBD_BYTE15						0x6F
#define GBD_BYTE16						0x70
#define GBD_BYTE17						0x71
#define GBD_BYTE18						0x72
#define GBD_BYTE19						0x73
#define GBD_BYTE20						0x74
#define GBD_BYTE21						0x75
#define GBD_BYTE22						0x76
#define GBD_BYTE23						0x77
#define GBD_BYTE24						0x78
#define GBD_BYTE25						0x79
#define GBD_BYTE26						0x7A
#define GBD_BYTE27						0x7B
#define GBD_BYTE28						0x7C
#define GBD_BYTE29						0x7D
#define GBD_BYTE30						0x7E
#define GBD_BYTE31						0x7F

////////////////////////////////////0xE0///////////////////////////////////////////
/**************************-Extended Color Gamut Registers-************************/
#define XVYCC_CTL				0x00	//xvYCC Control Register
#define XVYCC_CTL2			0x19	//xvYCC Control Register 2
#define XVYCC_COEFY2RGB_LOW		0x1A	//xvYCC Y2RGB Coefficient Register
#define XVYCC_COEFY2RGB_HIGH	0x1B	//
#define XVYCC_COEFCR2R_LOW		0x1C	//xvYCC Cr2R Coefficient Register
#define XVYCC_COEFCR2R_HIGH		0x1D
#define XVYCC_COEFCB2B_LOW		0x1E	//xvYCC Cb2B Coefficient Register
#define XVYCC_COEFCB2B_HIGH		0x1F
#define XVYCC_COEFCR2B_LOW		0x1E	//xvYCC Cb2B Coefficient Register
#define XVYCC_COEFCR2B_HIGH		0x1F
#define XVYCC_COEFCR2G_LOW		0x20	//xvYCC Cr2G Coefficient Register
#define XVYCC_COEFCR2G_HIGH		0x21
#define XVYCC_COEFCB2G_LOW		0x22	//xvYCC Cr2G Coefficient Register
#define XVYCC_COEFCB2G_HIGH		0x23
#define XVYCC_YOFFSET_LOW			0x24	//xvYCC Y Offset Register
#define XVYCC_YOFFSET_HIGH		0x25
#define XVYCC_OFFSET1_LOW			0x26	//xvYCC Offset1 Register
#define XVYCC_OFFSET1_MID			0x27
#define XVYCC_OFFSET1_HIGH		0x28
#define XVYCC_OFFSET2_LOW			0x29	//xvYCC Offset2 Register
#define XVYCC_OFFSET2_HIGH		0x2A
#define XVYCC_DCLEVEL_LOW			0x2B	//xvYCC DC-level Register
#define XVYCC_DCLEVEL_HIGH		0x2C
#define AVC_EN3								0x2D	//AVC Enable Register 3
#define RED_MIN_RANGE_LOW			0x2E	//xvYCC Red Min Range Register
#define RED_MIN_RANGE_HIGH		0x2F
#define RED_MAX_RANGE_LOW			0x30	//xvYCC Red Max Range Register
#define RED_MAX_RANGE_HIGH		0x31
#define GREEN_MIN_RANGE_LOW		0x32	//xvYCC Green Min Range Register
#define GREEN_MIN_RANGE_HIGH	0x33
#define GREEN_MAX_RANGE_LOW		0x34	//xvYCC Green Max Range Register
#define GREEN_MAX_RANGE_HIGH	0x35
#define BLUE_MIN_RANGE_LOW		0x36	//xvYCC Blue Min Range Register	
#define BLUE_MIN_RANGE_HIGH		0x37
#define BLUE_MAX_RANGE_LOW		0x38	//xvYCC Blue Max Range Register
#define BLUE_MAX_RANGE_HIGH		0x39
#define XVYCC_REVERSE_BO_EN		0x3A	//xvYCC Reverse Binary Offset Enable Register

////////////////////////////////////0x68///////////////////////////////////////////
/**************************-Audio Clock Recovery (ACR) Registers-*******************/
#define ACR_CTRL1							0x00	//ACR Control Register 1
#define ACR_MCLK_SEL					0x01	//ACR MCLK Select Register
#define FREQ_SVAL							0x02	//ACR Audio Frequency Register
#define N_SVAL1								0x03	//ACR N Value Registers
#define N_SVAL2								0x04
#define N_SVAL3								0x05
#define N_HVAL1								0x06
#define N_HVAL2								0x07
#define N_HVAL3								0x08
#define CTS_SVAL1							0x09	// ACR CTS Value Registers
#define CTS_SVAL2							0x0A
#define CTS_SVAL3							0x0B
#define CTS_HVAL1							0x0C
#define CTS_HVAL2							0x0D
#define CTS_HVAL3							0x0E
#define UPLL_SVAL							0x0F	//ACR UPLL Divider Value Registers
#define UPLL_HVAL							0x10
#define POST_SVAL							0x11	//ACR Post Divider Value Registers
#define POST_HVAL							0x12
#define LKWIN_SVAL						0x13	//ACR PLL Lock Window Register
#define LKTHRESH1							0x14	//ACR PLL Lock Threshold Registers
#define LKTHRESH2							0x15
#define LKTHRESH3							0x16
#define TCLK_FS								0x17	//ACR Hardware Extracted Fs Register
#define ACR_CTRL3							0x18	//ACR Control 3 Register
#define ACR_CFG1							0x88	//ACR Configuration Registers
#define ACR_CFG2							0x89

/**************************-Audio Output Formatting Registers-*******************/
#define I2S_CTRL1							0x26	//Audio Out I2S Control Register 1
#define I2S_CTRL2							0x27	//Audio Out I2S Control Register 2
#define I2S_MAP								0x28	//Audio Out I2S Map Register
#define AUD_CTRL							0x29	//Audio Out Control Register
#define CHST1									0x2A	//Audio Out SPDIF Channel Status 1 Register
#define CHST2									0x2B	//Audio Out SPDIF Channel Status 2 Register
#define CHST3									0x2C	//Audio Out SPDIF Channel Status 3 Register
#define TDM_CTRL1							0x23	//RX TDM Control Register 1
#define TDM_CTRL2							0x24	//X TDM Control Register 2
#define MUTE_DIV							0x2D	//Audio Out Soft Mute Divider Register
#define SW_OW									0x2E	//Audio Swap and Overwrite Register
#define OW_CHST5							0x2F	//Audio CHST5 Overwrite Register
#define CHST4									0x30	//Audio Out S/PDIF Channel Status 4 Register
#define CHST5									0x31	//Audio Out S/PDIF Channel Status 5 Register
#define AUDO_MUTE							0x32	//Audio Out Channel Mute Register

/**************************-HDMI Control and Status Registers-*******************/
#define AUDP_STAT							0x34	//HDMI Audio Status Register
#define AUDP_CRIT							0x35	//HDMI Audio Preamble Criteria Register
#define AUDP_FILT							0x36	//HDMI Audio Filter Register
#define AUDP_MUTE							0x37	//HDMI Audio MUTE Register
#define AUDP_CRIT2						0x38	//HDMI Audio HDCP Enable Criteria Register
#define AUDP_FIFO							0x39	//HDMI Audio FIFO Read/Write Pointer Difference Register

/**************************-System Power Down Registers-*******************/
#define PD_TOT							0x3C		//Power Down Total Register
#define PD_SYS3							0x3D		//System Power Down 3 Register
#define PD_SYS2							0x3E		//System Power Down 2 Register
#define PD_SYS							0x3F		//System Power Down Register
#define AVI_TYPE						0x40		//AVI InfoFrame Registers
#define AVI_VERS						0x41
#define AVI_LEN							0x42
#define AVI_CHSUM						0x43
#define AVI_DBYTE1					0x44
#define AVI_DBYTE2					0x45
#define AVI_DBYTE3					0x46
#define AVI_DBYTE4					0x47
#define AVI_DBYTE5					0x48
#define AVI_DBYTE6					0x49
#define AVI_DBYTE7					0x4A
#define AVI_DBYTE8					0x4B
#define AVI_DBYTE9					0x4C
#define AVI_DBYTE10					0x4D
#define AVI_DBYTE11					0x4E
#define AVI_DBYTE12					0x4F
#define AVI_DBYTE13					0x50
#define AVI_DBYTE14					0x51
#define AVI_DBYTE15					0x52
#define SPD_TYPE						0x60	//SPD InfoFrame Registers
#define SPD_VERS						0x61
#define SPD_LEN							0x62
#define SPD_CHSUM						0x63
#define SPD_DBYTE1					0x64
#define SPD_DBYTE2					0x65
#define SPD_DBYTE3					0x66
#define SPD_DBYTE4					0x67
#define SPD_DBYTE5					0x68
#define SPD_DBYTE6					0x69
#define SPD_DBYTE7					0x6A
#define SPD_DBYTE8					0x6B
#define SPD_DBYTE9					0x6C
#define SPD_DBYTE10					0x6D
#define SPD_DBYTE11					0x6E
#define SPD_DBYTE12					0x6F
#define SPD_DBYTE13					0x70
#define SPD_DBYTE14					0x71
#define SPD_DBYTE15					0x72
#define SPD_DBYTE16					0x73
#define SPD_DBYTE17					0x74
#define SPD_DBYTE18					0x75
#define SPD_DBYTE19					0x76
#define SPD_DBYTE20					0x77
#define SPD_DBYTE21					0x78
#define SPD_DBYTE22					0x79
#define SPD_DBYTE23					0x7A
#define SPD_DBYTE24					0x7B
#define SPD_DBYTE25					0x7C
#define SPD_DBYTE26					0x7D
#define SPD_DBYTE27					0x7E
#define SPD_DEC							0x7F
#define AUDIO_TYPE					0x80	//Audio InfoFrame Registers
#define AUDIO_VERS					0x81
#define AUDIO_LEN						0x82
#define AUDIO_CHSUM					0x83
#define AUDIO_DBYTE1				0x84
#define AUDIO_DBYTE2				0x85
#define AUDIO_DBYTE3				0x86
#define AUDIO_DBYTE4				0x87
#define AUDIO_DBYTE5				0x88
#define AUDIO_DBYTE6				0x89
#define AUDIO_DBYTE7				0x8A
#define AUDIO_DBYTE8				0x8B
#define AUDIO_DBYTE9				0x8C
#define AUDIO_DBYTE10				0x8D
#define MPEG_TYPE						0xA0	//MPEG InfoFrame Registers
#define MPEG_VERS						0xA1
#define MPEG_LEN						0xA2
#define MPEG_CHSUM					0xA3
#define MPEG_DBYTE1					0xA4
#define MPEG_DBYTE2					0xA5
#define MPEG_DBYTE3					0xA6
#define MPEG_DBYTE4					0xA7
#define MPEG_DBYTE5					0xA8
#define MPEG_DBYTE6					0xA9
#define MPEG_DBYTE7					0xAA
#define MPEG_DBYTE8					0xAB
#define MPEG_DBYTE9					0xAC
#define MPEG_DBYTE10				0xAD
#define MPEG_DBYTE11				0xAE
#define MPEG_DBYTE12				0xAF
#define MPEG_DBYTE13				0xB0
#define MPEG_DBYTE14				0xB1
#define MPEG_DBYTE15				0xB2
#define MPEG_DBYTE16				0xB3
#define MPEG_DBYTE17				0xB4
#define MPEG_DBYTE18				0xB5
#define MPEG_DBYTE19				0xB6
#define MPEG_DBYTE20				0xB7
#define MPEG_DBYTE21				0xB8
#define MPEG_DBYTE22				0xB9
#define MPEG_DBYTE23				0xBA
#define MPEG_DBYTE24				0xBB
#define MPEG_DBYTE25				0xBC
#define MPEG_DBYTE26				0xBD
#define MPEG_DBYTE27				0xBE
#define	MPEG_DEC						0xBF	//MPEG Packet Type Decode Register
#define ACP_HB0							0xE0	//Audio Content Protection Packet Registers
#define ACP_HB1							0xE1
#define ACP_HB2							0xE2
#define ACP_DBYTE0					0xE3
#define ACP_DBYTE1					0xE4
#define ACP_DBYTE2					0xE5.
#define ACP_DBYTE3					0xE6
#define ACP_DBYTE4					0xE7
#define ACP_DBYTE5					0xE8
#define ACP_DBYTE6					0xE9
#define ACP_DBYTE7					0xEA
#define ACP_DBYTE8					0xEB
#define ACP_DBYTE9					0xEC
#define ACP_DBYTE10					0xED
#define ACP_DBYTE11					0xEE
#define ACP_DBYTE12					0xEF
#define ACP_DBYTE13					0xF0
#define ACP_DBYTE14					0xF1
#define ACP_DBYTE15					0xF2
#define ACP_DBYTE16					0xF3
#define ACP_DBYTE17					0xF4
#define ACP_DBYTE18					0xF5
#define ACP_DBYTE19					0xF6
#define ACP_DBYTE20					0xF7
#define ACP_DBYTE21					0xF8
#define ACP_DBYTE22					0xF9
#define ACP_DBYTE23					0xFA
#define ACP_DBYTE24					0xFB
#define ACP_DBYTE25					0xFC
#define ACP_DBYTE26					0xFD
#define ACP_DBYTE27					0xFE
#define ACP_DEC							0xFF	// ACP Packet Type Decode Register
#define UNR_DBYTE1					0xC0	//Unrecognized Packet Registers
#define UNR_DBYTE2					0xC1
#define UNR_DBYTE3					0xC2
#define UNR_DBYTE4					0xC3
#define UNR_DBYTE5					0xC4
#define UNR_DBYTE6					0xC5
#define UNR_DBYTE7					0xC6
#define UNR_DBYTE8					0xC7
#define UNR_DBYTE9					0xC8
#define UNR_DBYTE10					0xC9
#define UNR_DBYTE11					0xCA
#define UNR_DBYTE12					0xCB
#define UNR_DBYTE13					0xCC
#define UNR_DBYTE14					0xCD
#define UNR_DBYTE15					0xCE
#define UNR_DBYTE16					0xCF
#define UNR_DBYTE17					0xD0
#define UNR_DBYTE18					0xD1
#define UNR_DBYTE19					0xD2
#define UNR_DBYTE20					0xD3
#define UNR_DBYTE21					0xD4
#define UNR_DBYTE22					0xD5
#define UNR_DBYTE23					0xD6
#define UNR_DBYTE24					0xD7
#define UNR_DBYTE25					0xD8
#define UNR_DBYTE26					0xD9
#define UNR_DBYTE27					0xDA
#define UNR_DBYTE28					0xDB
#define UNR_DBYTE29					0xDC
#define UNR_DBYTE30					0xDD
#define UNR_DBYTE31					0xDE

/**************************-General Control Packet Registers-*******************/
#define GCP_DBYTE1					0xDF	//CP Packet Byte 1 Register

////////////////////////////////////0xD0///////////////////////////////////
/**************************-General Purpose IO Control Registers-**************/
#define GPIO_OUT_EN				0x01
#define GPIO_CO						0x02
#define GPIO_CI						0x03
#define GPIO_MODE					0x06

/**************************-Receiver Control Registers-*******************/
#define RX_CTRL1					0x6C	//Receiver Control Register 1
#define RX_CTRL4					0x6F	//Receiver Control Register 4
#define RX_CTRL5					0x70	//Receiver Control Register 5

/**************************-Analog Core Registers-*******************************/
#define TMDS_TERMCTRL		0x70	//TMDS Termination Control Register

/**************************-MHL and CBUS Control Registers-*********************/





#endif

