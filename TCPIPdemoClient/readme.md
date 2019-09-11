#TCPIOdemoClient
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
