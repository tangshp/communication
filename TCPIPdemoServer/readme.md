#TCPIOdemoServer
1 accept read都是阻塞型
2     char buffer[89]="hello";
      printf("%s\n",buffer);//buffer是指针

3 模拟一个client
```c
$nc 127.0.0.1 8765
```
