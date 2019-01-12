#include "stdio.h"
void main(void)
{
 //   system("rm /user/update/bin/viu-vpu0h264enc-mdev-rtsp");


        printf("content-Type:text/xml\n\n");
        
      if(remove("/user/update/bin/viu-vpu0h264enc-mdev-rtsp")==0)
             printf("succeed");
      else
            printf("fail");
}
