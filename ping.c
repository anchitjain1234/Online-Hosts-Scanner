//
// Created by anchit on 22/11/15.
//

#include "main.h"

void ping(struct sockaddr_in *addr,int pid,struct protoent *proto)
{
    const int val=255;
    int sd, cnt=1,j,i;
    struct packet pckt;
    struct sockaddr_in r_addr;
    unsigned char buf[1024];

    sd = socket(PF_INET, SOCK_RAW, proto->p_proto);
    if ( sd < 0 )
    {
        perror("socket");
        return;
    }
    //set ttl
    if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
        perror("Error in Set TTL option");

    //set nonblocking
    if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 )
        perror("Error in Request nonblocking I/O");

    //set broadcast permission
    int broadcastperm=1;
    if(setsockopt(sd,SOL_SOCKET,SO_BROADCAST,&broadcastperm,sizeof(broadcastperm))<0)
        perror("Error in broadcast permission\n");

    //will send icmp echo request 10 times, 1 per second
    for (j=0;j<10;j++)
    {
        int len=sizeof(r_addr);
        if ( recvfrom(sd,buf, sizeof(buf), 0, (struct sockaddr*)&r_addr,(socklen_t *) &len) > 0 )
        {
            fillshm(buf);
        }

        bzero(&pckt, sizeof(pckt));

        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = pid;

        for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
            pckt.msg[i] = i+'0';

        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = cnt++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));

        if ( sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 )
            perror("Error in sending echo request");
        sleep(1);
    }
}

void runping()
{
    struct hostent *hname;
    struct sockaddr_in addr;

    int pid;
    struct protoent *proto;
    char ip_prefix[20],ipadd[20];
    //gets ip prefix for e.g. if ip is 172.17.14.10, below function will return 172.17.14.
    getip_prefix(getip(),ip_prefix);
    int i,j,t=0,ct2=0;

    char str[3];

    //fill the array with some value other than 0 as icmp response type which would be stored in the array
    // would be zero.
    initialize(shmarr);

    printf("Scanning online hosts\n");

    //ip_prefix is getting some random string appended in the end. To filter it out this loop is created.
    for(i=0;i<strlen(ip_prefix);i++)
    {
        t++;
        if(ip_prefix[i]=='.')
            ct2++;
        if(ct2==3)
            break;
    }

    for(i=1;i<MAXHOSTS;i++)
    {
        //Will ping all the host ips in different processes to speed up.
        if(fork()==0)
        {
            bzero(ipadd,20);
            bzero(str,3);

            sprintf(str,"%d",i);

            for(j=0;j<t;j++)
                ipadd[j]=ip_prefix[j];


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
            //  sleep(10);
            continue;
        }
    }
    //make parent sleep
    sleep(10);

    printf("Online hosts\n");
    int ct=0;

    //since ip_prefix is getting appended with random text.
    bzero(ipadd,20);
    for(j=0;j<t;j++)
        ipadd[j]=ip_prefix[j];

    for(i=1;i<MAXHOSTS;i++)
    {
        if(shmarr[i]==0)
        {
            printf("%s%d\n",ipadd,i );
            ct++;
        }
    }
    printf("Total %d hosts online\n",ct);

    //wait for all the childs to exit
    wait(0);
}