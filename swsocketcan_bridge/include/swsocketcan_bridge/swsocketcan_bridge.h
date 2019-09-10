#ifndef _SWSOCKETCAN_BRIDGE_H_
#define _SWSOCKETCAN_BRIDGE_H_

#include <string>
#include <sys/time.h>
#include <sys/select.h>
#include "can_msgs/Frame.h"

#pragma pack(1)//
typedef struct socket_NETWORK_CANFRAME
{
    unsigned char candevindex;//CAN端口(1)
    unsigned int id;//CAN包id(4)
    unsigned char len;//length(1)
    unsigned char byte[8];//CAN Data(8)
    unsigned char ext;//1-扩展,0-标准(1)
    unsigned char rtr;//(1)
    unsigned int param;//备用参数(4)
}NETWORK_CANFRAME;
#pragma pack()/*_USER_PROTCOL_H_*/

#define _CANDEVINDEX 0x00
#define _PARAM 0x00000000
#define _ISEXT 0x01//扩展帧
#define _UNEXT 0x00//标准帧
#define _ISRTR 0x01
#define _UNRTR 0x00
class Swsocketcan_bridge
{
public:
    Swsocketcan_bridge();
    ~Swsocketcan_bridge();
    void connect(std::string host, int port = 6020);
    void disconnect();
    bool isConnected();
    int read_from_can(int __fd, void *__buf, size_t __nbytes);
    int writer_to_can(int __fd, const void *__buf, size_t n);
    void fromROSMsg(NETWORK_CANFRAME& can,const can_msgs::Frame::ConstPtr& roscan);
    void toROSMsg(NETWORK_CANFRAME& can,can_msgs::Frame& roscan);
    void clearROSMsg(can_msgs::Frame& roscan);
    int socket_fd_;
    int err;
    struct timeval waited;
    fd_set read_flags,write_flags;

    //can_frame
    NETWORK_CANFRAME can_to_base;//下发给二代控制器的can_frame
    NETWORK_CANFRAME can_from_base;//来自二代控制器的can_frame
    can_msgs::Frame can_msgs_pub;

protected:
    bool connected_;
};
#endif
