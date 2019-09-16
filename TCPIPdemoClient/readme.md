# TCPIOdemoClient
1 strlen
```c++
//size_t strlen (const char *__s);
   
char buf[4096]="hello";
printf("strlen(buf):%lu\n",strlen(buf));
//运行结果
strlen(buf):5
```
2 Server 会调用bind()函数，会把ip与port绑定在相应的socket上;但是client使用bind(),所以connect()会分配一个随机的端口;服务器的端口必须是固定的;
#注意事项
```bash
mkdir cmakefile && cd cmakefile
cmake ..
make
```
## 2.查看进程
### 同时开启server与client时：
```shell script
➜  TCPIPdemoServer sudo netstat -apn | grep 8765    
[sudo] sw 的密码： 
tcp        0      0 127.0.0.1:8765          0.0.0.0:*               LISTEN      19551/TCPIPdemoServ
tcp        0      0 127.0.0.1:53172         127.0.0.1:8765          ESTABLISHED 19747/TCPIPdemoClie
tcp        0      0 127.0.0.1:8765          127.0.0.1:53172         ESTABLISHED 19551/TCPIPdemoServ
#                   自己的ip与port            对方的ip与port           连接状态
#LISTEN是在调用accept后server出现的状态
#ESTABLISHED数据传输状态
```

### ctrl+c server
```shell script
➜  TCPIPdemoServer sudo netstat -apn | grep 8765
tcp        1      0 127.0.0.1:53172         127.0.0.1:8765          CLOSE_WAIT  19747/TCPIPdemoClie
tcp        0      0 127.0.0.1:8765          127.0.0.1:53172         FIN_WAIT2   -   
```

### ctrl+c client
```shell script
➜  TCPIPdemoServer sudo netstat -apn | grep 8765
tcp        0      0 127.0.0.1:8765          127.0.0.1:53202         TIME_WAIT   - 
# server还挂着，所以port[8765]还被占用;此时在此启动server的话，会bind失败
# 只有主动关闭的一端，才会有TIME_WAIT状态，2个MLS时间长度
``` 

### TCP状态转移

### listen()函数
```C
int listen(int sockfd, int backlog);
sockfd:
    socket文件描述符
backlog:
    同时处于三次握手时，最多链接的客户端个数
```
    
查看默认的backlog值
```shell
➜  ~ cat /proc/sys/net/ipv4/tcp_max_syn_backlog
256
```

### read()返回值
```c
ssize_t Read(int fd, void *ptr, size_t nbytes)
读取 > 请求，返回请求数
读取 < 请求，返回读取数
读取=0,请求>0读到文件末尾，返回0
读取失败（比如无效文件描述），返回-1
```

