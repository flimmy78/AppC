#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define BUFSIZE 128
#define SERVER_PORT 8899

static int TcpCom()
{
  int s = 0;
  int fd = 0;
  int len = 0;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  socklen_t sin_size = 0;
  char buf[BUFSIZE] = {0};

  sin_size = sizeof (struct sockaddr_in);
  bzero(&my_addr, sizeof (my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port = htons (SERVER_PORT);

  if ((s = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {   
      perror ("socket");
      close (s);
      return -1; 
    }   
  if (bind (s, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) < 0)
    {
      perror ("bind");
      close (s);
      return -1;
    }
    while (1)
    {
		printf("kkkkkkkkk\n");
        listen (s, 5);
        if ((fd = accept (s, (struct sockaddr *) &remote_addr, &sin_size)) < 0)
        {
            perror ("accept");
            close (fd);
			continue;
        }
        printf ("accept client %s\n", inet_ntoa (remote_addr.sin_addr));
        len = send (fd, "Welcome to my server!", 21, 0);
        if(len < 0)
        {
            close (fd);
			continue;
        }
		memset(buf,0,sizeof(buf));
        len = recv (fd, buf, BUFSIZE, 0);
        if(len > 0)
        {
            buf[len] = '\0';
            printf ("Server recieve:%s\n", buf);
            if (send (fd, buf, len, 0) < 0)
            {
                perror ("write");
                close (fd);
				continue;
            }
        }
		close(fd);
  }
  close (fd);
  close (s);
  return 0;
}
void *pthKey()
{
	TcpCom();
	return NULL;
}
int main (int argc, char *argv[])
{
#if 1
	pthread_t pthid;
    if(pthread_create(&pthid,NULL,pthKey,NULL)!=0) 
    {   
        printf("Create thread error!\n");
        exit(1);
    }
	while(1)
	{
		sleep(1);
	}
#endif 

	return 0;
}
