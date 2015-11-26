//
// Created by anchit on 26/11/15.
//

#include "main.h"

void display(void *buf, int bytes)
{
	int i;
	struct iphdr *ip = buf;
	struct icmphdr *icmp = buf+ip->ihl*4;

	// printf("----------------\n");
	// for ( i = 0; i < bytes; i++ )
	// {
	// 	if ( !(i & 15) ) printf("\n%X:  ", i);
	// 	printf("%X ", ((unsigned char*)buf)[i]);
	// }
	// printf("\n");
	// printf("IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s ",
	// 	ip->version, ip->ihl*4, ntohs(ip->tot_len), ip->protocol,
	// 	ip->ttl, inet_ntoa(*(struct in_addr *)&ip->saddr));
	// printf("dst=%s\n", inet_ntoa(*(struct in_addr *)&ip->daddr));

	printf("ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n",
			icmp->type, icmp->code, ntohs(icmp->checksum),
			icmp->un.echo.id, icmp->un.echo.sequence);
	if(icmp->type == 3 && icmp->code==3)
		printf("Port unreachable ICMP packet\n");
	else
		printf("Other ICMP packet\n\n");
	// if ( icmp->un.echo.id == pid )
	// {
	// 	printf("ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n",
	// 		icmp->type, icmp->code, ntohs(icmp->checksum),
	// 		icmp->un.echo.id, icmp->un.echo.sequence);
	// }
}

void listenicmp()
{
	struct protoent *proto=getprotobyname("ICMP");

	char recvbuff[100];
	struct sockaddr_in clientaddr;
	int clientlen=sizeof(clientaddr);

	int icmp_sock=socket(PF_INET, SOCK_RAW, proto->p_proto);
	if(icmp_sock<0)
	{
		perror("ICMP sock error");
		exit(-1);
	}

	for(;;)
	{
		int n = recvfrom(icmp_sock, recvbuff, 100, 0,(struct sockaddr *) &clientaddr, &clientlen);

	if (n < 0)
	      perror("ERROR in recvfrom");
	  	else
	  	{
	  		printf("received reply from %s:%d\n",inet_ntoa(clientaddr.sin_addr),(int)ntohs(clientaddr.sin_port));
	  		display(recvbuff,100);
	  		// sleep(3);
	  	}
	}
}

void sendudpbroadcast(in_port_t port,char *message)
{
	// in_port_t port=htons((in_port_t) atoi(argv[1]));
	struct sockaddr_storage deststorage;
	memset(&deststorage,0,sizeof(deststorage));
	size_t addrsize=0;

	char recvbuff[100];
	struct sockaddr_in clientaddr;
	int clientlen=sizeof(clientaddr);

	struct sockaddr_in *da=(struct sockaddr_in *) &deststorage;
	da->sin_family=AF_INET;
	da->sin_port=port;
	da->sin_addr.s_addr=INADDR_BROADCAST;

	addrsize=sizeof(struct sockaddr_in);
	struct sockaddr *destadd=(struct sockaddr *)&deststorage;
	size_t msglen=strlen(message);

	int sock=socket(destadd->sa_family,SOCK_DGRAM,IPPROTO_UDP);

	int broadcastperm=1;
	if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&broadcastperm,sizeof(broadcastperm))<0)
	{
		perror("setsocketopt failed in udp\n");
		exit(-1);
	}

	if(fork()==0)
		listenicmp();
	else
	{
		for(;;)
		{
			printf("sending broadcast\n");

			ssize_t nbytes=sendto(sock,message,msglen,0,destadd,addrsize);

			if(nbytes<0) 
				perror("sendto failed");
			else if(nbytes!=msglen)
				perror("size diff in udp sendto");

			sleep(1);
		}
	}
}
