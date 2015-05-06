#ifndef CONN_H
#define CONN_H

#include <iostream>  //cout
#include <stdio.h>   //printf
#include <string.h>  //strlen
#include <string>    //string
#include <sys/socket.h>  //socket
#include <arpa/inet.h>   //inet_addr
#include <netdb.h>   //hostent
#include <sstream>

#include <signal.h> //crtl+c
#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>
#include <ctype.h>

//#include"conio.h"
#include "string.h"
#include "math.h"

#include "tcp_client.h"



bool conn(string address , int port);

#endif 