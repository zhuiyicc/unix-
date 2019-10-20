#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
using namespace std;
#define MAXCLIENT 10
void err_quit(const char *s){
    perror(s);
    exit(0);
}
class server{
    public:
        server(int port=6666){
            createSock();
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
            servaddr.sin_port = htons(port);
            if((bind(listenfd,(sockaddr *)&servaddr,sizeof(servaddr)))<0){
                err_quit("bind error");
            }
            if((listen(listenfd,MAXCLIENT)) < 0){
                err_quit("listen error");
            }
            handleRWAC();
            closeSock(listenfd);
        }
    private:
        sockaddr_in servaddr;
        int listenfd,client[MAXCLIENT];
        void createSock(){
            if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
                err_quit("socket create");
            }
        }
        void closeSock(int sockfd){
            close(sockfd);
        }
        void handleRWAC(){
            sockaddr_in cliaddr;
            socklen_t clilen = sizeof(cliaddr);
            int maxfd = listenfd;
            int clifd,i,n,maxlen = 1024;
            char buff[1024];
            fd_set fset;
            for(i = 0;i < MAXCLIENT;++i) client[i] = -1;
            FD_ZERO(&fset);
            FD_SET(listenfd,&fset);
            for(;;){
                int nready = select(maxfd+1,&fset,NULL,NULL,NULL);
                if(FD_ISSET(listenfd,&fset)){
                    if((clifd = accept(listenfd,(sockaddr *)&cliaddr,&clilen))<0){
                        err_quit("accept error");
                    }
                    puts("some body is connect");
                    for(i = 0;i < MAXCLIENT;++i){
                        if(client[i] == -1){
                            client[i] = clifd;
                            break;
                        }
                    }
                    if(i == MAXCLIENT){
                        err_quit("client is too many");
                    }
                    FD_SET(clifd,&fset);
                    if(clifd > maxfd) maxfd = clifd;
                    if(--nready <= 0) continue;
                }
                for(i = 0;i < MAXCLIENT;++i){
                    if(client[i] != -1 && FD_ISSET(client[i],&fset)){
                        clifd = client[i];
                        if((n = read(clifd,buff,maxlen))==0){
                            puts("somebody is close");
                            closeSock(clifd);
                            FD_CLR(clifd,&fset);
                            client[i] = -1;
                        }else{
                            buff[n] = '\0';
                            printf("%s\n",buff);
                            write(clifd,buff,n);
                        }
                        if(--nready <= 0) break;
                    }
                }
            }
        }
};
int main(){
    server ser;
    return 0;
}
