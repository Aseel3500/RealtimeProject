#define SERVER_PORT 5000
#include "conf.h"

void main(int argc, char *argv[]){

    int fd,nbytes;
    struct sockaddr_in srv;
    char buffer[512];
    printf(">>");
    scanf("%s",buffer);


//    if(argc<2){
//        perror("No parameters passed!");
//        exit(2);
//    }

//    "/folderName/fileName"
//    strcpy(buffer,argv[1]);

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
    //sleep(3);


    if((nbytes = write(fd, &buffer, sizeof(buffer))) < 0) {

        perror("write error");
        exit(1);
    }
    char readBuffer[512];
    while(1){
        if((nbytes = read(fd, &readBuffer, sizeof(readBuffer))) < 0){
            perror("read error");
        }
        printf("%s\n",readBuffer);
        if (strchr(readBuffer, '*') != NULL)
        {
            break;
        }
//        char tmp[512];
//        strcpy(tmp,"*");
//        if(strcmp(tmp,readBuffer) == 0) {
//            break;
//        }
    }
    close(fd);

//
//    char readBuffer[512];
//
//    if ((nbytes = read(fd, &readBuffer, sizeof(readBuffer))) < 0) {
//        perror("read error");
//        exit(1);
//    } else{
//        printf("%s buffer value from server\n", readBuffer);
//        close(fd);
//    }






}