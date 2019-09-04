#ifndef _TCP_IP_TO_CAN_H_
#define _TCP_IP_TO_CAN_H_

#include <string>
#include <sys/time.h>
#include <sys/select.h>
#pragma pack(1)//
typedef struct __attribute_1_NETWORK_CANFRAME
{
    unsigned char candevindex;//CAN端口(1)
    unsigned int id;//CAN包id(4)
    unsigned char len;//length(1)
    unsigned char byte[8];//CAN Data(8)
    unsigned char ext;//1-扩展,0-标准(1)
    unsigned char rtr;//(1)
    unsigned int param1;//备用参数(8)
}NETWORK_CANFRAME;
#pragma pack()/*_USER_PROTCOL_H_*/

#pragma pack(1)//
typedef struct command_velocity_to_base
{
    int linear;//线速度
    int angular;//角速度
}CMD_VEL;
#pragma pack()/*_USER_PROTCOL_H_*/

class Tcp_ip_to_can
{
public:
    Tcp_ip_to_can();
    ~Tcp_ip_to_can();
    void connect(std::string host, int port = 6020);
    void disconnect();
    bool isConnected();
    int read_from_can(int __fd, void *__buf, size_t __nbytes);
    int writer_to_can(int __fd, const void *__buf, size_t n);
    int socket_fd_;
    int err;
    struct timeval waited;
    fd_set read_flags,write_flags;
protected:
    bool connected_;
};
#endif
