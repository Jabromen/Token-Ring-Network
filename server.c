#include "udpsockets.h"

int main (int argc, char** argv) {
    
    // Check if enough arguments
    if (argc < 3) {
        printf("ERROR: Not enough arguments. Use format:\n"
               "\"portNum numberOfHosts\"\n");
        exit(EXIT_FAILURE);
    }

    // Parse command line arguments
    u_short my_port  = (u_short) atoi(argv[1]);
    int     numHosts =           atoi(argv[2]);

    // Create UDP socket
    udpsocket_t *sckt = initUdpSocket(my_port);

    if (!sckt)
        exit(EXIT_FAILURE);

    // Array the will hold the address and port for each host joining
    struct sockaddr_in hostAddrs[numHosts];

    // Buffer to receive and send messages
    char buffer[128];
    // Number of bytes received
    int recvlen;

    // Loop until the specified number of join requests are made
    int i = 0;
    while (i < numHosts) {
        // Clear buffer and receive message
        memset(buffer, 0, 128);
        recvlen = receiveMessage(buffer, 128, sckt);

        // Check if received message is a join request
        if (recvlen > 0 && !strcmp(buffer, "JOIN")) {
            // If so, add host to address array
            hostAddrs[i++] = sckt->remaddr;
            printf("Host #%d Joined\n", i);
        }
    }

    // Send peer address and port to first 'numHosts - 1' hosts
    for (i = 0; i < numHosts - 1; i++) {
        memset(buffer, 0, 128);
        makeAddrString(buffer, "PEER", &sckt->myaddr, &hostAddrs[i + 1]);
        sendto(sckt->fd, buffer, strlen(buffer), 0, (struct sockaddr *) &hostAddrs[i], sckt->addrlen);
    }

    // Send peer address and port to last host
    memset(buffer, 0, 128);
    makeAddrString(buffer, "PEER", &sckt->myaddr, &hostAddrs[0]);
    sendto(sckt->fd, buffer, strlen(buffer), 0, (struct sockaddr *) &hostAddrs[i], sckt->addrlen);

    // Tell the first host that joined to create and send the token
    memset(buffer, 0, 128);
    strcpy(buffer, "GO");
    sendto(sckt->fd, buffer, strlen(buffer), 0, (struct sockaddr *) &hostAddrs[0], sckt->addrlen);

    exit(EXIT_SUCCESS);
}