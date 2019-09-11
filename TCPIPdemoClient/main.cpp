#include "sys/socket.h"
#include "netinet/in.h"
#include "stdio.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERV_PORT 8765
int main(int argc, char **argv) {
    if(argc<2){
        printf("./TCPIPdemoClient ser_ip\n");
        return 1;
    }

    //1 建立socket
    int sfd;
    sfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //2 connect链接Server
    struct sockaddr_in ser_addr;
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    inet_pton(sfd,argv[1],&ser_addr.sin_addr.s_addr);
    ser_addr.sin_port = htons(SERV_PORT);
    connect(sfd,(struct sockaddr*)&ser_addr, sizeof(ser_addr));

    int len;
    char buf[4096];
    memset(buf,0, sizeof(buf));

    while(fgets(buf, sizeof(buf),stdin))//从标准输入读
    {
        write(sfd,buf,strlen(buf));//strlen获得字符个数，\0被会略
        len = read(sfd,buf, sizeof(buf));
        write(STDOUT_FILENO,buf,len);//把从socket中读取的内容输出到终端
    }

    return 0;
}