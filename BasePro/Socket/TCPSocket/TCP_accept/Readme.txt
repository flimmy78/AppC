1. 这是使用socket套接字实现最简单TCP通信的过程。
2. 这个程序还很不完善，比如
   1）关闭服务端的程序时，绑定的端口并不能立即释放，当服务端程序再次启动时，将会导致绑定端口失败！
