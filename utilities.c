//
// Created by anchit on 22/11/15.
//

#include "main.h"

//gives the host part of ip address.
int last3(char *ip)
{
    int i,j=0,ct=0;
    char num[3];
    bzero(num,3);
    for(i=0;i<strlen(ip);i++)
    {
        if(ip[i]=='.')
        {
            ct++;
            continue;
        }
        if(ct==3)
        {
            num[j++]=ip[i];
        }
    }
    return atoi(num);
}

//checksum for icmp
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result =(unsigned short) ~sum;
    return result;
}

//Change the value of the host in shared memory array to the icmp response.
void fillshm(void *buf)
{
    struct iphdr *ip = buf;
    struct icmphdr *icmp = buf+ip->ihl*4;

    char *ia=inet_ntoa(*(struct in_addr *)&ip->saddr);
    shmarr[last3(ia)]=icmp->type;
}

//initialize the shared memory array
void initialize(int *arr)
{
    int i;
    for(i=0;i<255;i++)
        arr[i]=5;
}

//get the host ip address.
char* getip(char *interface)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

//get prefix of ip address.
void getip_prefix(char *ipadd,char *prefix)
{
    bzero(prefix,sizeof(prefix));
    int i,ct=0;

    for(i=0;i<strlen(ipadd);i++)
    {
        prefix[i]=ipadd[i];
        if(ipadd[i]=='.')
            ct++;
        if(ct==3)
            return;
    }
}