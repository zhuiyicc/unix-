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
#include <limits.h>
#include <poll.h>
#include <time.h>
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
            pollfd client[10];
            client[0].fd = listenfd;
            client[0].events = POLLRDNORM;
            for(i = 1;i < 10;++i) client[i].fd = -1;
            for(;;){
                int nready = poll(client,10,-1);
                if(client[0].revents & POLLRDNORM){
                    if((clifd = accept(listenfd,(sockaddr *)&cliaddr,&clilen))<0){
                        err_quit("accept error");
                    }
                    for(i = 1;i < 10;++i){
                        if(client[i].fd == -1){
                            client[i].fd = clifd;
                            client[i].events = POLLRDNORM;
                            break;
                        }
                    }
                    if(i == 10){
                        err_quit("too many client");
                    }
                    if(--nready <= 0) continue;
                }
                for(i = 1;i < 10;++i){
                    if((clifd = client[i].fd) < 0) continue;
                    if(client[i].revents & (POLLRDNORM | POLLERR)){
                        if((n = read(clifd,buff,maxlen))<0){
                            if(errno == ECONNRESET){
                                printf("connect is reset by client\n");
                                closeSock(clifd);
                                client[i].fd = -1;
                            }else err_quit("read error");
                        }else if(n == 0){
                            printf("client is close\n");
                            closeSock(clifd);
                            client[i].fd = -1;
                        }else{
                            buff[n] = '\0';
                            write(clifd,buff,n);
                        }
                    }
                    if(--nready <= 0) break;
                }
            }
        }
};
int main(){
    server ser;
    return 0;
}
