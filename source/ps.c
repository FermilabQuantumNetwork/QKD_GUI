#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include "ps.h"

PowerSupply *ps_init(char *ip, int port)
{
    PowerSupply *ps = (PowerSupply *) malloc(sizeof(PowerSupply));

    if (!ps) return NULL;

    strcpy(ps->ip, ip);
    ps->port = port;
    ps->sockfd = -1;

    return ps;
}

int ps_connect(PowerSupply *ps)
{
    int numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (ps->sockfd != -1)
        ps_close(ps);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ps->ip, ps->port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((ps->sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(ps->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(ps->sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    return 0;
}

int ps_query(PowerSupply *ps, char *cmd, char *resp)
{
    int numbytes = 0;

    while (1) {
        if ((numbytes = recv(ps->sockfd, ps->sockfd, ps->resp+numbytes, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            return -1;
        }

        if (resp[numbytes-1] == '\n')
            break
    }

    return 0;
}

int ps_close(PowerSupply *ps)
{
    close(ps->sockfd);
    ps->sockfd = -1;
}

int ps_close(PowerSupply *ps)
{
    if (ps->sockfd != -1)
        close(ps->sockfd);
}

void ps_free(PowerSupply *ps)
{
    if (ps) {
        ps_close(ps);
        free(ps);
    }
}
