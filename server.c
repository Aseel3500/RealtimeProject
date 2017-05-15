#include "conf.h"
#define KEY 500
#define SHMSZ 2
void sendMessage(int workerPID,int qid);
void handler();


int fd = 0;
int numConnections = 0;
int main() {
    int numWorkers,portNumber;

    struct sockaddr_in srv;
//    struct sockaddr_in cli;
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
        printf("%d\n",fileData[i]);

    numWorkers = fileData[0];
    numConnections = fileData[1];
    portNumber = fileData[2];
    int *workersPIDs = (int *)malloc(numWorkers*sizeof(int));
    int *shmIds = (int *)malloc(numWorkers*sizeof(int));

    /*socket creation*/

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
    printf("after binding the socket with fd %d\n",fd);

    /*workers creation*/
    for(int j=0;j<numWorkers;j++){
        int pid = fork();




        if(!pid){//child
//            old_handler = signal (SIGINT, handler);
            handler();
//            printf("request Accepted by child %d\n",getpid());
//           while(1);



//            printf("child %d\n",getpid());
//            int qid = open_MQ(KEY);
////            printf("qid child %d\n",qid);
//            struct msgbuf msg;
//            strcpy(msg.a,"je");
//            printf("fd %d\n",fd);
//            while(1){
//                receive_msg_MQ(qid,&msg,getpid());
//                msgrcv(qid,&msg,sizeof(msg)-sizeof(long),getpid(),0);
//                int newfd; /* returned by accept() */
//                int cli_len = sizeof(cli);
//                //newfd= (int*)malloc(sizeof(int));
////                printf("fd %d\n",fd);
//                newfd = accept(fd, (struct sockaddr*) &cli, &cli_len);
//                if(newfd < 0) {
//                    perror("accept"); exit(1);
//                }
//                printf("the message is %s in child %d\n",msg.a, getpid());
//            }



            return 0;

        }
        else{ //parent
//            printf("parent %d with child %d\n",getpid(),pid);
            workersPIDs[j] = pid;
        }

//        execlp();
    }
//    for(int k=0;k<numWorkers;k++){
//        printf("worker #%d is %d\n",k,workersPIDs[k]);
//    }
    printf("after forking \n");



    /*Shared memory creation for each worker*/
    for(int l =0 ;l<numWorkers;l++){
        int key = workersPIDs[l];

        if ((shmIds[l]= shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            exit(1);
        }
        int *shm_base_address;

        /* * Now we attach the segment to our data space. */
        if((shm_base_address =(int *)shmat(shmIds[l],NULL,0)) == (char *) -1) {
            perror("shmat");
            exit(1);
        }
        *shm_base_address = numConnections;
//        printf("shm %d\n",*shm_base_adderess);
    }

    /*semaphores creation*/




    /*check if there's data to read from the general socket*/
    int m=0;
    while(1){

        fd_set mainReadfd;
        FD_ZERO(&mainReadfd);
        FD_SET(fd,&mainReadfd);
        if(select(fd+1,&mainReadfd,NULL,NULL,NULL)<0){
            perror("select");
            exit(1);
        }
//        printf("%d\n",m++);
        if(FD_ISSET(fd,&mainReadfd)){ // send msg to one of the workers to accept the request
//            printf("there's data on the main socket\n");
            int workerIndex = rand() % numWorkers;
//            workerIndex = 0;

            int workerPID = workersPIDs[workerIndex];
            //kill(workerPID,SIGINT);
            int qid = msgget(KEY,0666|IPC_CREAT);
            sendMessage(workerPID,qid);
            sleep(1);
        }

        
    }













//    }

}
void sendMessage(int workerPID,int qid){
    struct msgbuf p;

//    printf("qid parent %d\n",qid);
    strcpy(p.a,"ahln");
    printf("we send message to child %d\n",workerPID);
    p.type = workerPID;
    p.pid = getpid();

    if(msgsnd(qid,&p,sizeof(p)-sizeof(long),0)<0)
    {
        perror("error in sending to MQ");
        exit(1);
    }
//    printf("message is sent to worker\n");
}


int open_MQ(key_t key){

    return msgget(key,0666|IPC_CREAT);
}
int send_msg_MQ(int qid, struct msgbuf *msgp){
    int sz;
    sz= sizeof(msgp)-sizeof(long);
    return msgsnd(qid,msgp,sz,0);
}

int receive_msg_MQ(int qid, struct msgbuf *msgp,int type){
    int sz;
    sz= sizeof(msgp)-sizeof(long);

    return msgrcv(qid,msgp,sz,type,0);
}

void remove_MQ(int qid){
    msgctl(qid,IPC_RMID,NULL);
}


void handler() {
//    signal (SIGINT, handler);
//    printf("Signal handled\n");
    struct sockaddr_in cli;
    int cli_len = sizeof(cli);
    int newfd; /* returned by accept() */
    int nbytes;
    char buffer[512];
    int next = 0;
    int  sd;
    int max_sd;
    fd_set readfds;
    struct msgbuf received_msg;
    int *shm_base_address;
    int shmId, semid;
//    int newfds[100];
    int *newfds =(int *)malloc(sizeof(int)* numConnections);
    static struct sembuf acquire = {0, -1, SEM_UNDO},
            release = {0, 1, SEM_UNDO};
//    acquire.sem_num = 0;
//    release.sem_num = 0;
//    struct sembuf sbuf;

    /*shm creation and attaching*/
    if ((shmId= shmget(getpid(), SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if((shm_base_address =(int *)shmat(shmId,NULL,0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }




     for (int i = 0; i < numConnections; i++)
        {
            newfds[i] = 0;
        }
//    printf("child %d\n",getpid());
    while(1){
        int qid = msgget(KEY,0666|IPC_CREAT);
//        printf("before receive\n");
        if(msgrcv(qid,&received_msg,0,getpid(),0)<0)
            printf("there's msg\n");
        if(msgrcv(qid,&received_msg,sizeof(received_msg)-sizeof(long),getpid(),0)<0){
            perror("error in receiving MQ");
            exit(1);
        }


//        printf("the message is %s in child %d\n",received_msg.a, getpid());

        FD_ZERO(&readfds);
        FD_SET(fd,&readfds);
        max_sd = fd;
//        printf("before for\n");
/* Now use FD_SET to initialize other newfd’s that have already been returned by accept() */
        for(int i=0;i<numConnections;i++){
            sd =  newfds[i] ;
            FD_SET(newfds[i] ,&readfds);
            if(newfds[i] > max_sd)
            {
                max_sd = newfds[i];
            }
        }
       /// max_sd = 100;
//        printf("after for\n");
       // printf("max sd %d\n", max_sd);
        select(max_sd+1, &readfds, NULL, NULL, NULL);

//       if(FD_ISSET(fd, &readfds)) {
//           printf("inside the isset of fd\n");
           if ((newfds[next++] = accept(fd, (struct sockaddr *) &cli, &cli_len)) < 0) {
               perror("accept");
               exit(1);
           }
           else {
               /*semaphore access*/
               if ( (semid = semget(getpid(), 1, IPC_CREAT | 0666)) < 0 ) {
                   perror("semaphore get");
                   exit(1);
               }

               else{
                   printf(" smid %d\n", semid);
                   semctl(semid, 0, SETVAL, 1);
                   /* Initialize the semaphore. */
                   //sbuf.sem_num = 1;
               }

               printf("before acquire \n");
               /* acquire shared memory */
               if(semop(semid, &acquire, 1) < 0 ) {
                   perror("sem");
                   exit(1);
               }
               else
               {
                   printf("acquire done\n");
               }
               /*Access the shared memory to decrement the number of connection of the worker*/
               /* Now we attach the segment to our data space. */
               *shm_base_address = --(*shm_base_address);
               if ( semop(semid, &release, 1) <  0) {
                   perror("semop");
                   exit(1);
               }
               printf("shm after accept %d\n",*shm_base_address);

//               printf("new fd of acceptance %d\n",newfds[next-1]);


               nbytes = read(newfds[next - 1], &buffer, sizeof(buffer));
               printf("%s buffer value from client\n", buffer);
//               printf("%d next value\n", next);
//               snprintf(buffer, sizeof(buffer), "%d", getpid());

               /*Open the requested file and send it to the client*/
               FILE* fp;
               fp=fopen(buffer,"r");
               char line[512];
               if(fp<0){
                   perror("error in openning file\n");
               }
               while(fgets(line, sizeof(line), fp) != NULL){
                   printf("%s",line);
                   strcpy(buffer,line);
                   if (write(newfds[next-1], &buffer, strlen(buffer))< 0) {
                       perror("write");
                       exit(1);
                   }
               }

               strcpy(buffer,"*");
               buffer[2] = '\0';
               write(newfds[next-1], &buffer, strlen(buffer));


//               if (write(newfds[next-1], &buffer, strlen(buffer) < 0)) {
//                   perror("write");
//                   exit(1);
//               }

               printf("Response is sent from worker %d\n",getpid());

               *shm_base_address = ++(*shm_base_address);
               printf("shm after response %d\n",*shm_base_address);
               close(newfds[next-1]);

            }


//            printf("wow\n");
//           for (int i = 0; i < 100; i++) {
//               if (FD_ISSET(newfds[i], &readfds)) {
//                   printf("%s buffer value from client\n", buffer);
//                   if ((nbytes = read(newfds[i], &buffer, sizeof(buffer))) < 0) {
//                       perror("read error");
//                       exit(1);
//                   } else {
//                       write(newfds[i], &buffer, strlen(buffer) < 0);
//                       printf("%s buffer value\n", buffer);
//                       newfds[i] = 0;
//                    printf("Response is sent from worker %d\n",getpid());
//                       close(newfds[i]);
//                   }
//               }
//           }

           if (next == 99) {
               next = 0;
           }

    }
}




