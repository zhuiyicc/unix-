#include <iostream>
#include "net.h"
using namespace std;
int main(){
    int servfd;
    sockaddr_in servaddr,cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    char buff[MAXLINE];
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servfd = Socket(AF_INET,SOCK_DGRAM,0);
    Bind(servfd,(SA *)&servaddr,sizeof(servaddr));
    for(;;){
        int n = recvfrom(servfd,buff,MAXLINE,0,(SA *)&cliaddr,&clilen);
        if(n == -1)
            perror("recv error");
        sendto(servfd,buff,MAXLINE,0,(SA *)&cliaddr,clilen);
    }
    return 0;
}
