#ifndef GAMETCP_H
#define GAMETCP_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "tabuleiro.h"

#define ECHOMAX 500

int create_server(int receivePort);
int create_client(char *host, int port);
void tcp_send_data();
void tcp_read_data();

#endif
