#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "netinet/in.h"
#include "arpa/inet.h"
#include <string.h>
#include <sys/socket.h>

#define SERV_PORT 8765
int main() {
    int sfd;
    //1 建立socket对象
    sfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//AF_INET IPPROTO_TCP IPPROTO_TCP都在netinet/in.h中
    //int socket(int domain, int type, int protocol);
    //domain:选择通讯的网络层
    //      AF_INET--IPv4 Internet protocols
    //      AF_INET6--IPv6 Internet protocols
    //type:选择传输层(TCP/UDP)
    //      SOCK_STREAM:流式协议，包括TCP;指定流式协议，并非就是指定类TCP协议
    //      SOCK_DGRAM：报式协议，包括UDP;指报式式协议，并非就是指定类UDP协议
    //protocol:一般使用0，表示使用默认协议;流式协议的默认协议是TCP协议;报式协议默认协议是UDP协议
    //返回值表示文件描述符
    //printf("sfd:%d\n",sfd);sfd=3

    //2 把socket与ip/端口绑定（一台主机有三个网口）
    struct sockaddr_in serv_addr;//定义协议族 ip地址 port,此时是垃圾值(使用sockaddr_in的原因在于，TCP通讯在socket之后才出现)
    bzero(&serv_addr, sizeof(serv_addr));//在string.h中
    serv_addr.sin_family = AF_INET;//domain
    serv_addr.sin_port = htons(SERV_PORT);//端口号 host->net
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//表示接受所有端口是8080的ip
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//把字符串转化为in_addr_t
    bind(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));//sockaddr_in 是ipv4类型  sockaddr_in6 是ipv6类型
    //int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
    //sockfd:socket返回的文件描述符
    //addr：端口与ip

    //3 设置同时连接的客服端的最大个数,并与之3次握手
    listen(sfd,128);

    //4 accept阻塞型
    struct sockaddr_in client_addr;
    socklen_t addr_len(sizeof(client_addr));
    printf("Accepting connections ...\n");
    int cfd = accept(sfd,(struct sockaddr*)&client_addr, &addr_len);
    //accept用来保存来链接的客户端的ip与port号,调用成功后,会返回一个新的文件描述符,用来读接受到的文件
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    //addr:传出参数;存储来链接的客户端的ip和端口号,事先不知道这个变量的大小,所以要定义的大些
    //addrlen:传入传出参数;调用函数前,作为传入参数,是自己定义的addr的大小;调用函数后,是真正链接到的客户端的addr的大小

    char client_ip[128];
    printf("client IP:%s\t%d\n",
            inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_ip, sizeof(client_ip)),ntohs(client_addr.sin_port));
    //inet_ntop把ip地址转化为字符串发在client_ip中，如果成功操作，则返回client_ip的地址;否则返回NULL

    char buf[32];
    memset(&buf,0, sizeof(buf));
    int index(0);
    while(true)//while写在accept之后
    {
        printf("before read!!!\n");
        int len = read(cfd,buf, sizeof(buf));//read writer close都在unistd.h中
        //数据处理
        write(STDOUT_FILENO,buf,len);//输出到终端
        printf("index:%d len:%d\n",index++, len);
        for(int i(0);i<len;i++)
            buf[i] = toupper(buf[i]);//toupper在ctype.h中
        write(cfd,buf, len);//使用的cfd
    }
    close(cfd);

    return 0;
}
//预备知识点：电脑字节序是小端，网络字节序是大端
//int x = 0x12345678;
//printf("sizeof(x) = %lu\n", sizeof(x));
//printf("x:%x\n",x);// 78 56 34 12(地址：小->大)
//printf("x:%x htonl(x)",htonl(x));//htonl(x)的结果是12 34 56 78(地址：小->大)
//uint32_t htonl(uint32_t hostlong);
//uint16_t htons(uint16_t hostshort);
//uint32_t ntohl(uint32_t netlong);
//uint16_t ntohs(uint16_t netshort);