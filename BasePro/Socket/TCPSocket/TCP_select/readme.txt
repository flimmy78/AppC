1.这个程序依然不能立即释放端口
2.selcet函数:
   select，poll，epoll都是IO多路复用的机制。所谓I/O多路复用机制，就是说通过一种机制，可以监视多个描述符，一旦某个描述符就绪（一般是读就绪或者写就绪），能够通知程序进行相应的读写操作。但select，poll，epoll本质上都是同步I/O，因为他们都需要在读写事件就绪后自己负责进行读写，也就是说这个读写过程是阻塞的，而异步I/O则无需自己负责进行读写，异步I/O的实现会负责把数据从内核拷贝到用户空间。
   1）实现socket的非阻塞通信。
   2）if(FD_ISSET(sock,&fds)) //测试sock是否可读，即是否网络上有数据 
3.fd_set:
  select()机制中提供一fd_set的数据结构，实际上是一long类型的数组，每一个数组元素都能与一打开的文件句柄（不管是socket句柄，还是其他文件或命名管道或设备句柄）建立联系，建立联系的工作由程序员完成，当调用select()时，由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程哪一socket或文件发生了可读或可写事件。
4. fd_set用法：
  1)FD_ZERO(&set); /*将set清零使集合中不含任何fd*/
  2)FD_SET(fd, &set); /*将fd加入set集合*/
  3)FD_CLR(fd, &set); /*将fd从set集合中清除*/
  4)FD_ISSET(fd, &set); /*在调用select()函数后，用FD_ISSET来检测fd在fdset集合中的状态是否变化返回整型，当检测到fd状态发生变化时返回真，否则，返回假（0）*/


      
