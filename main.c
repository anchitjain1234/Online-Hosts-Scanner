//
// Created by anchit on 22/11/15.
//

//Subnet netmask is assumed to be 255.255.255.0 . For other netmask code will not work.

//see intermediate function definitions in ping.c and utilities.c

#include "main.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Invalid arguments. Usage: %s <interface>\n", argv[0]);
        return -1;
    }
    struct hostent *hname;
    struct sockaddr_in addr;

    int pid;
    struct protoent *proto;
    char ip_prefix[20],ipadd[20];
    //gets ip prefix for e.g. if ip is 172.17.14.10, below function will return 172.17.14.
    getip_prefix(getip(argv[1]),ip_prefix);
    int i,j;

    char str[3];

    //Check for root permissions
    if(getuid()!=0)
    {
        perror("Root permissions are required to run the program\n");
        exit(-1);
    }

    //Creating shared memory for storing the icmp response for the requested ip. Host part of the ip would be used
    //as array index.
    int shmid;
    shmid=shmget(IPC_PRIVATE,255*sizeof(int),0);
    shmarr=(int *)shmat(shmid,NULL,0);

    //fill the array with some value other than 0 as icmp response type which would be stored in the array
    // would be zero.
    initialize(shmarr);

    printf("Scanning online hosts\n");

    for(i=1;i<MAXHOSTS;i++)
    {
        //Will ping all the host ips in different processes to speed up.
        if(fork()==0)
        {
            bzero(ipadd,20);
            bzero(str,3);

            sprintf(str,"%d",i);
            strcpy(ipadd,ip_prefix);

            int t=(int)strlen(ip_prefix);

            //Creating host ip address by appending host part to the ip prefix.
            for(j=0;j<strlen(str);j++)
                ipadd[t+j]=str[j];

            //pid required for icmp id
            pid = getpid();
            proto = getprotobyname("ICMP");
            hname = gethostbyname(ipadd);

            bzero(&addr, sizeof(addr));
            addr.sin_family = (sa_family_t )hname->h_addrtype;
            addr.sin_port = 0;
            addr.sin_addr.s_addr = (in_addr_t )*(long*)hname->h_addr;

            //now ping the host.
            ping(&addr,pid,proto);
            exit(0);
        }
        else
        {
            // if(i%100==0)
            // 	sleep(10);
            continue;
        }
    }
    //make parent sleep
    sleep(10);

    printf("Online hosts\n");
    int ct=0;

    for(i=1;i<MAXHOSTS;i++)
    {
        if(shmarr[i]==0)
        {
            printf("%s%d\n",ip_prefix,i );
            ct++;
        }
    }
    printf("Total %d hosts online\n",ct);

    //wait for all the childs to exit
    wait(0);

    return 0;
}