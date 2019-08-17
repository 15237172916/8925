#ifndef __API_SOCKET_H__
#define __API_SOCKET_H__


#define API_PORT  8111
#define BUFFER_SIZE 1024
#define LENGTH_OF_LISTEN_QUEUE 20
#define WELCOME    "\n\r\n          -- Welcome to EXP-2496 --\r\n\0"
#define CMD_READY    "\nEXP-2496>\0"
#define MAX_LEN_PART   20
#define MAX_CMD_PART   128

//static parametric
#define NOTIFY "notify\0"
#define CONFIG "config\0"
#define DISPLAY "displaygroup\0"
#define SCENE   "scene\0"
#define MATRIX "matrix\0"
#define UNKNOW "Unknow Command\r\n\0"
#define SET "set\0"
#define GET "get\0"
#define TX "TX\0"
#define RX "RX\0"


//accurate sample
#define SAMPLE_MATRIX  "parametric error ! \r\n\
accurate format:\r\n\
    matrix get <RX> <TX> \r\n\
    matrix get <TX> <RX1> <RX2> ... <RXn> \r\n\
    matrix set <TX> <RX1> <RX2> .... <RXn> \r\n\0"



#define MATRIX_SET "parametric error ! \r\n\
accurate format:\r\n\
    matrix set <TX> <RX1> <RX2> .... <RXn> \r\n\0"


//error alert
//#define ALERT_PARA "parametric error ! \r\n\0"







void *API_server(void);



#endif