#include "tcp_ip_to_can/Tcp_ip_to_can.h"
#include "console_bridge/console.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

Tcp_ip_to_can::Tcp_ip_to_can() : connected_(false)
{
  waited.tv_sec = 0;
  waited.tv_usec = 5000;
  FD_ZERO(&read_flags);
  FD_ZERO(&write_flags);
}

Tcp_ip_to_can::~Tcp_ip_to_can()
{
}

void Tcp_ip_to_can::connect(std::string host, int port)
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

void Tcp_ip_to_can::disconnect()
{
    if (connected_)
    {
        close(socket_fd_);//close在<unistd.h>
        connected_ = false;
    }
}

bool Tcp_ip_to_can::isConnected()
{
  return connected_;
}

int Tcp_ip_to_can::read_from_can(int __fd, void *__buf, size_t __nbytes)
{
  FD_ZERO(&read_flags);
  FD_SET(__fd, &read_flags);
  err = select(__fd + 1,&read_flags,&write_flags,(fd_set*)0,&waited);
  if(err < 0) return -1;
  if(FD_ISSET(__fd, &read_flags))
        {
            //Socket ready for reading
            FD_CLR(__fd, &read_flags);
            //if (read(thefd, buf, sizeof(buf)-1) <= 0)
            if (read(__fd, __buf, __nbytes) <= 0)
            {
                close(__fd);
                return -2;
            }
            // else
            //     printf("byte[%x %x %x %x %x %x %x %x]\n",__buf[6],__buf[7],__buf[8],__buf[9],__buf[10],__buf[11],__buf[12],__buf[13]);
        }
}