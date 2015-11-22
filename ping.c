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