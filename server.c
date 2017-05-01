
#include "conf.h"

int main() {
    int numWorkers,numConnections,portNumber;
    int fd;
    struct sockaddr_in srv;
    struct sockaddr_in cli;
    /*read from file*/
    int fileData[3];
    int i=0;
    FILE* fp;
    fp=fopen("conf.txt","r");
    char line[10];
    while(fgets(line, sizeof(line), fp) != NULL){
        fileData[i++] = atoi(line);
    }

    for(i=0;i<3;i++)
        printf("%d",fileData[i]);
    numWorkers = fileData[0];
    numConnections = fileData[1];
    portNumber = fileData[2];



/* socket descriptor */
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    srv.sin_family = AF_INET; /* use the Internet addr family */
    srv.sin_port = htons(portNumber); /* bind socket ‘fd’ to port 80*/
    /* bind: a client may connect to any of my addresses */

    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(fd, (struct sockaddr*) &srv, sizeof(srv)) < 0) {
        perror("bind error");
        exit(1);
    }

    if(listen(fd, 1000) < 0) {
        perror("listen");
        exit(1);
    }

    /*workers creation*/
    for(int j=0;j<numWorkers;j++){
        fork();
//        execlp();
    }








}


