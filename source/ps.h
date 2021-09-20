#ifndef PS_H
#define PS_H

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

#define MAXDATASIZE 1024

typedef struct PowerSupply {
    int sockfd;
    char ip[INET6_ADDRSTRLEN];
    int port;
    char buf[MAXDATASIZE];
} PowerSupply;

PowerSupply *ps_init(char *ip, int port);
int ps_connect(PowerSupply *ps);
int ps_ready(PowerSupply *ps);
int ps_query(PowerSupply *ps, char *cmd, char *resp, int maxlen);
void ps_close(PowerSupply *ps);
void ps_free(PowerSupply *ps);

#endif
