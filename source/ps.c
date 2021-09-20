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

#include <netdb.h>

void *get_in_addr(struct sockaddr *sa) {
  return sa->sa_family == AF_INET
    ? (void *) &(((struct sockaddr_in*)sa)->sin_addr)
    : (void *) &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char _port[6];

    if (ps->sockfd != -1)
        ps_close(ps);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    sprintf(_port,"%i",ps->port);
    if ((rv = getaddrinfo(ps->ip, _port, &hints, &servinfo)) != 0) {
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

int ps_query(PowerSupply *ps, char *cmd, char *resp, int maxlen)
{
    int numbytes = 0;
    int sent = 0;
    int recieved = 0;

    int len = strlen(cmd);

    while (sent < len) {
        if ((numbytes = send(ps->sockfd, cmd+numbytes, len, 0)) == -1) {
            perror("recv");
            return -1;
        }

        sent += numbytes;
    }

    while (1) {
        if ((numbytes = recv(ps->sockfd, resp+numbytes, maxlen-recieved-1, 0)) == -1) {
            perror("recv");
            return -1;
        }

        recieved += numbytes;

        if (resp[recieved-1] == '\n')
            break;
    }

    return 0;
}

void ps_close(PowerSupply *ps)
{
    if (ps->sockfd != -1) {
        close(ps->sockfd);
        ps->sockfd = -1;
    }
}

void ps_free(PowerSupply *ps)
{
    if (ps) {
        ps_close(ps);
        free(ps);
    }
}
