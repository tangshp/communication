#include "swsocketcan_bridge/swsocketcan_bridge.h"
#include "ros/ros.h"
#include "console_bridge/console.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>//uint8_t的头文件

Swsocketcan_bridge::Swsocketcan_bridge() : connected_(false)
{
  waited.tv_sec = 0;
  waited.tv_usec = 5000;
  FD_ZERO(&read_flags);
  FD_ZERO(&write_flags);

  memset(&can_from_base,0,sizeof(can_from_base));
  can_to_base.candevindex = _CANDEVINDEX;
  can_to_base.param = _PARAM;
  can_to_base.ext = _UNEXT;
  can_to_base.rtr = _UNRTR;
  can_msgs_pub.header.frame_id = "0";
  can_msgs_pub.is_rtr = false;
  can_msgs_pub.is_extended = false;
  can_msgs_pub.is_error = false;
  can_msgs_pub.dlc = 8;
  can_msgs_pub.id = 0x00;
  for(int i(0);i<8;i++)
    can_msgs_pub.data[i] = 0x00;
}

Swsocketcan_bridge::~Swsocketcan_bridge()
{
}

void Swsocketcan_bridge::connect(std::string host, int port)
{
  if (!connected_)
  {
    //define variables
    int x;
    logDebug("Creating non-blocking socket.");//logDebug在“console_bridge/console.h” 
    socket_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP在”netinet/in.h“
    // socket_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP在”netinet/in.h“
    x = fcntl(socket_fd_,F_GETFL,0); 
    fcntl(socket_fd_,F_SETFL,x | O_NONBLOCK);
    if (socket_fd_)
    {
      struct sockaddr_in stSockAddr;
      stSockAddr.sin_family = AF_INET;
      // stSockAddr.sin_family = PF_INET;
      stSockAddr.sin_port = htons(port);
      inet_pton(AF_INET, host.c_str(), &stSockAddr.sin_addr);
      // stSockAddr.sin_addr.s_addr = inet_addr(host.c_str());

      logDebug("Connecting socket to TCP/IP_to_CAN.");
      int ret;
      while((ret = ::connect(socket_fd_, (struct sockaddr *) &stSockAddr, sizeof(stSockAddr))) == -1);

      if (ret == 0)
      {
        connected_ = true;
        logDebug("Connected succeeded.");
      }
    }
  }
}

void Swsocketcan_bridge::disconnect()
{
    if (connected_)
    {
        close(socket_fd_);//close在<unistd.h>
        connected_ = false;
    }
}

bool Swsocketcan_bridge::isConnected()
{
  return connected_;
}

int Swsocketcan_bridge::read_from_can(int __fd, void *__buf, size_t __nbytes)
{
  FD_ZERO(&read_flags);
  FD_SET(__fd, &read_flags);
  err = select(__fd + 1,&read_flags,&write_flags,(fd_set*)0,&waited);
  if(err == 0) return 0;
  if(err < 0) return -1;
  if(FD_ISSET(__fd, &read_flags))
        {
            //Socket ready for reading
            FD_CLR(__fd, &read_flags);
            //if (read(thefd, buf, sizeof(buf)-1) <= 0)
            if (read(__fd, __buf, __nbytes) <= 0)//小于等于0 表示没有 数据
            {
                close(__fd);
                return -2;
            }
            // else
            //     printf("byte[%x %x %x %x %x %x %x %x]\n",__buf[6],__buf[7],__buf[8],__buf[9],__buf[10],__buf[11],__buf[12],__buf[13]);
        }
}

void Swsocketcan_bridge::fromROSMsg(NETWORK_CANFRAME& can,const can_msgs::Frame::ConstPtr& roscan)
{
  can.id = roscan->id;
  can.len = roscan->dlc;
  can.ext = roscan->is_extended ? _ISEXT : _UNEXT;
  can.rtr = roscan->is_rtr ? _ISRTR :_UNRTR;
  for(int i(0); i<8; i++)
    can.byte[i] = roscan->data[i];
}

void Swsocketcan_bridge::toROSMsg(NETWORK_CANFRAME& can,can_msgs::Frame& roscan)
{
  roscan.header.frame_id = "1";
  roscan.header.stamp = ros::Time::now();
  roscan.id = (uint8_t)can.id;

  if(can.rtr == _ISRTR) roscan.is_rtr = true;
  else if(can.rtr == _UNRTR) roscan.is_rtr = false;

  if(can.ext == _ISEXT) roscan.is_extended = true;
  else if(can.ext == _UNEXT) roscan.is_extended = false;

  roscan.dlc = (uint8_t)can.len;
  for(int i(0); i<8; i++)
    roscan.data[i] = (uint8_t)can.byte[i];
  // ROS_INFO("byte %x %x %x %x %x %x %x %x",can.byte[0],can.byte[1],can.byte[2],can.byte[3],can.byte[4],can.byte[5],can.byte[6],can.byte[7]);
  // ROS_INFO("data %x %x %x %x %x %x %x %x",roscan.data[0],roscan.data[1],roscan.data[2],roscan.data[3],roscan.data[4],roscan.data[5],roscan.data[6],roscan.data[7]);
}

void Swsocketcan_bridge::clearROSMsg(can_msgs::Frame& roscan)
{
  roscan.header.frame_id = "0";
  roscan.header.stamp = ros::Time::now();
  roscan.id = 0x00;
  roscan.is_rtr = false;
  roscan.is_extended = false;
  roscan.dlc = 0;
  for(int i(0); i<8; i++) 
    roscan.data[i] = 0;
}