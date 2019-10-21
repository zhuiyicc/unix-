#include <iostream>
#include "net.h"
using namespace std;
int main(int argc,char **argv){
    int clifd;
    char buff[MAXLINE];
    sockaddr_in servaddr;
    clifd = Socket(AF_INET,SOCK_DGRAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    //connect(clifd,(SA *)&servaddr,sizeof(servaddr));
    while(fgets(buff,MAXLINE,stdin) != NULL){
        //使用connect时可以不使用sendto，recvfrom指明目的端和对端。
        //write(sockfd,buff,strlen(buff));
        //read(sockfd,buff,MAXLINE);
        sendto(clifd,buff,MAXLINE,0,(SA *)&servaddr,sizeof(servaddr));
        int n = recvfrom(clifd,buff,MAXLINE,0,NULL,NULL);
        buff[n] = '\0';
        fputs(buff,stdout);
    }
    return 0;
}
