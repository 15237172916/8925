#ifndef __API_SOCKET_H__
#define __API_SOCKET_H__


#define API_PORT  8111
#define BUFFER_SIZE 1024
#define PASSWORD_SIZE   20
#define LENGTH_OF_LISTEN_QUEUE 20
#define WELCOME    "\n\r\n          -- Welcome to EXP-2496 --\r\n\r\n\0"
#define CMD_READY    "\nEXP-2496>\0"
#define PASSWORD    "Password:\0"
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
#define HUB "HUB\0"
#define EXT "EXT\0"
#define DISPLAY_GROUD "displaygroup\0"
#define OSD "OSD\0"
#define REBOOT "reboot\0"
#define NOTIFY  "notify\0"
#define SINK_POWER "sinkpower\0"
#define DEVICE  "device\0"
#define STATUS "status\0"
#define OFF_STR "off\0"
#define ON_STR  "on\0"
#define ALIAS   "alias\0"
#define PASS "123456\r\n\0"
#define PASS_ERROR "password error\n\r\0"

//accurate sample
#define SAMPLE_MATRIX  "parametric error ! \r\n\
accurate format:\r\n\
    matrix get <RX> <TX> \r\n\
    matrix get <TX> <RX1> <RX2> ... <RXn> \r\n\
    matrix set <TX> <RX1> <RX2> .... <RXn> \r\n\0"

#define SAMPLE_SCENE    "parametric error ! \r\n\
accurate format:\r\n\
    scene set <S> \r\n\0"

#define SAMPLE_GROUP   "parametric error ! \r\n\
accurate format:\r\n\
    displaygroup set <DG> <TX> \r\n\0"

#define SAMPLE_CONFIG   "parametric error ! \r\n\
accurate format:\r\n\
    config get TX status <TX> \r\n\
    config get RX status <RX> \r\n\
    config get HUB status \r\n\
    config get EXT status <EXT> \r\n\
    config set device sinkpower <on|off> <RX1> <RX2> ... <RXn> \r\n\
    config set displaygroup sinkpower <on|off> <GD1> <GD2> .... <GDn> \r\n\
    config set OSD <on|off> \r\n\
    config set reboot \r\n\
    config set notify <on|off> \r\n\0"
    

//error alert
#define ALERT_PARA_ERROR "parametric error ! \r\n\0"
#define ALERT_CHECK_FORMAT "please check format ! \r\n\0"
#define ALERT_PARA_MORE "the numbers of parametric are more than provide ! \r\n\0"
#define ALERT_NO_FIND "no find target, please check your alias ! \r\n\0"







void *API_server(void);



#endif