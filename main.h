//
// Created by anchit on 22/11/15.
//

#ifndef HOSTS_HOSTS_H
#define HOSTS_HOSTS_H

#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/wait.h>

#define PACKETSIZE	64
#define MAXHOSTS 255
int *shmarr;

struct packet
{
    struct icmphdr hdr;
    char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

//defined in utilities.c
int last3(char *ip);
unsigned short checksum(void *b, int len);
void fillshm(void *buf);
void initialize(int *arr);
char* getip();
void getip_prefix(char *ipadd,char *prefix);

//defined in ping.c
void ping(struct sockaddr_in *addr,int pid,struct protoent *proto);

#endif //HOSTS_HOSTS_H
