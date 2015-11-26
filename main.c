//
// Created by anchit on 22/11/15.
//

//Subnet netmask is assumed to be 255.255.255.0 . For other netmask code will not work.

//see intermediate function definitions in ping.c and utilities.c

#include "main.h"

int main(int argc, char *argv[])
{
    //Check for root permissions
    if(getuid()!=0)
    {
        perror("Root permissions are required to run the program\n");
        exit(-1);
    }

    if(argc !=2)
    {
        fprintf(stderr,"Usage\n");
        fprintf(stderr,"%s -u :- To find hosts by using UDP broadcast\n",argv[0] );
        fprintf(stderr,"%s -p :- To find hosts by pinging them individually\n",argv[0] );
        exit(-1);
    }
    else
    {
        if(strcmp(argv[1],"-p")==0)
        {

            //Creating shared memory for storing the icmp response for the requested ip. Host part of the ip would be used
            //as array index.
            shmid=shmget(IPC_PRIVATE,255*sizeof(int),0);
            shmarr=(int *)shmat(shmid,NULL,0);

            runping();
        }
        else if(strcmp(argv[1],"-u")==0)
            sendudpbroadcast(5678,"random");
        else
        {
            fprintf(stderr, "Invalid arguments\n" );
        }
    }
    return 0;
}