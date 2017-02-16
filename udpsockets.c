#include "udpsockets.h"

udpsocket_t *initUdpSocketClient(const char *hostname, u_short dest_port, u_short my_port) {

	// Allocate memory for udp sockets struct
	udpsocket_t *sckt;

	if (!(sckt = initUdpSocket(my_port)))
		return NULL;

	// Get destination ipv4 address
	struct hostent *host = gethostbyname(hostname);

	if (!host) {
		fprintf(stderr, "Cannot obtain address of %s\n", hostname);
		closeSocket(sckt);
		return NULL;
	}

	// Set destination socket's type, address, and port
	memset((char *) &sckt->destaddr, 0, sizeof(sckt->destaddr));
	sckt->destaddr.sin_family = AF_INET;
	memcpy((void *) &sckt->destaddr.sin_addr, (void *) host->h_addr, host->h_length);
	sckt->destaddr.sin_port = htons(dest_port);

	return sckt;
}

udpsocket_t *initUdpSocket(u_short my_port) {

	// Allocate memory for udp sockets struct
	udpsocket_t *sckt = (udpsocket_t *) malloc(sizeof(udpsocket_t));

	if (!sckt) {
		fprintf(stderr, "Malloc failed: udpsocket_t\n");
		return NULL;
	}

	// Create udp socket
	if ((sckt->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Cannot create socket");
		closeSocket(sckt);
		return NULL;
	}

	// Set my socket's type, address, and port
	memset((char *) &sckt->myaddr, 0, sizeof(sckt->myaddr));
	sckt->myaddr.sin_family = AF_INET;
	sckt->myaddr.sin_port = htons(my_port);

	// Get my ipv4 address
	char myname[32];
	gethostname(myname, 32);
	struct hostent *host = gethostbyname(myname);

	memcpy((void *) &sckt->myaddr.sin_addr, (void *) host->h_addr, host->h_length);

	// Bind socket
	if (bind(sckt->fd, (struct sockaddr *) &sckt->myaddr, sizeof(sckt->myaddr)) < 0) {
		perror("Bind Failed");
		closeSocket(sckt);
		return NULL;
	}

	sckt->addrlen = sizeof(sckt->myaddr);

	return sckt;
}

void setUdpDestination(u_long dest_addr, u_short dest_port, udpsocket_t *sckt) {

	// Set destination socket's type, address, and port
	memset((char *) &sckt->destaddr, 0, sizeof(sckt->destaddr));
	sckt->destaddr.sin_family      = AF_INET;
	sckt->destaddr.sin_addr.s_addr = dest_addr;
	sckt->destaddr.sin_port        = dest_port;
}

void makeAddrString(char *buffer, const char *prefix, struct sockaddr_in *myaddr, struct sockaddr_in *destaddr) {

	sprintf(buffer, "%s %u %u %u %u", prefix,
		                              myaddr->sin_addr.s_addr, 
	                                  myaddr->sin_port,
	                                  destaddr->sin_addr.s_addr, 
	                                  destaddr->sin_port);
}

int parseMessage(char *message, addrport_t *ap) {

	char *token[6];

	int i = 0;
	token[i] = strtok(message, " ");

	while (token[i] != NULL) {
		token[++i] = strtok(NULL, " ");
	}

	if (i == 5 && !strcmp(token[0], "PEER")) {
		ap->oldaddr = (unsigned int)  atoi(token[1]);
		ap->oldport = (u_short) atoi(token[2]);
		ap->newaddr = (unsigned int)  atoi(token[3]);
		ap->newport = (u_short) atoi(token[4]);

		return 1;
	}

	return 0;
}

int checkDestination(udpsocket_t *sckt, addrport_t *ap) {

	if (sckt->destaddr.sin_addr.s_addr != ap->oldaddr)
		return 0;

	if (sckt->destaddr.sin_port != ap->oldport)
		return 0;

	setUdpDestination(ap->newaddr, ap->newport, sckt);

	return 1;
}

int sendMessage(const char *message, udpsocket_t *sckt) {

	if (sendto(sckt->fd, message, strlen(message), 0, (struct sockaddr *) &sckt->destaddr, sckt->addrlen) < 0) {
		fprintf(stderr, "Sendto failed\n");
		return -1;
	}

	return 0;
}

int receiveMessage(char *buffer, const int buffSize, udpsocket_t *sckt) {

	return recvfrom(sckt->fd, buffer, buffSize, 0, (struct sockaddr *) &sckt->remaddr, &sckt->addrlen);
}

void closeSocket(udpsocket_t *sckt) {

	close(sckt->fd);
	free(sckt);
}
