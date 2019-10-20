#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;
void err_quit(const char *s){
    perror(s);
    exit(0);
}
class client{
    public:
        client(const char *s = "127.0.0.1",int port = 6666){
            createSock();
            memset(&servaddr,0,sizeof(servaddr));
            servaddr.sin_addr.s_addr = inet_addr(s);
            servaddr.sin_port = htons(port);
            servaddr.sin_family = AF_INET;
            tryConnect();
            handleRWByselect();
            closeSock();
        }
    private:
        sockaddr_in servaddr;//连接服务器端的地址和端口
        int sockfd;
        void createSock(){
            if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
                err_quit("socket create");
            }
        }
        void closeSock(){
            close(sockfd);
        }
        void handleRWByselect(){
            int filefd = fileno(stdin),maxlen = 1024,n;
            int maxfdp1 = max(filefd,sockfd) + 1,stdineof = 0;
            char buff[1024];
            fd_set fset;
            for(;;){
                FD_ZERO(&fset);
                if(!stdineof) FD_SET(filefd,&fset);
                FD_SET(sockfd,&fset);
                select(maxfdp1,&fset,NULL,NULL,NULL);
                if(FD_ISSET(sockfd,&fset)){
                    if((n = read(sockfd,buff,maxlen)) == 0){
                        if(stdineof) return;
                        else{
                            err_quit("server terminate");
                        }
                    }
                    buff[n] = '\0';
                    write(filefd,buff,n);
                }
                if(!stdineof && FD_ISSET(filefd,&fset)){
                    if((n = read(filefd,buff,maxlen)) == 0){
                        stdineof = 1;
                        shutdown(sockfd,SHUT_WR);
                        FD_CLR(filefd,&fset);
                        continue;
                    }
                    buff[n] = '\0';
                    write(sockfd,buff,n);
                }
            }
        }
        void tryConnect(){
            if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
                err_quit("connect error");
            }
        }
};
int main(){
    client cli;
    return 0;
}
