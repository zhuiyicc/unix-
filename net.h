#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERV_PORT 6666
#define SA sockaddr
#define MAXLINE 1024
int Socket(int domain, int type, int protocol)
{
    int sock = socket(domain,type,protocol);
    if(sock == -1)
        perror("create socket error");
    return sock;
}
void Bind(int fd,SA *addr,socklen_t len)
{
    if(bind(fd,addr,len) < 0)
        perror("bind error");
}

