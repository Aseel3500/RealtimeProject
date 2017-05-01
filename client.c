#define SERVER_PORT 5000
#include "conf.h"

void main(int argc, char *argv[]){

    int fd,nbytes;
    struct sockaddr_in srv;
    char buffer[50];


    if(argc<2){
        perror("No parameters passed!");
        exit(2);
    }
//    "/folderName/fileName"
    strcpy(buffer,argv[1]);

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
/* connect: use the Internet address family */
    srv.sin_family = AF_INET;
/* connect: socket ‘fd’ to port 80 */
    srv.sin_port = htons(SERVER_PORT);
/* connect: connect to IP Address “128.2.35.50” */
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(fd, (struct sockaddr*) &srv, sizeof(srv)) < 0) {
        perror("connect");
        exit(1);
    }


    if((nbytes = write(fd, &buffer, sizeof(buffer))) < 0) {

        perror("write error");
        exit(1);
    }






}