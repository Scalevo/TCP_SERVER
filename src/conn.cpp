#include "conn.h"
/**
    Connect to a host on a certain port number
*/

bool conn(string address , int port)
{
    //create socket if it is not already created

    if(tcp_client::sock == -1)
    {
        //Create socket
        tcp_client::sock = socket(AF_INET , SOCK_STREAM , 0);
        if (tcp_client::sock == -1)
        {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */   }

    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            tcp_client::server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

    //plain ip address
    else
    {
        tcp_client::server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    tcp_client::server.sin_family = AF_INET;
    tcp_client::server.sin_port = htons( port );

    //Connect to remote server
    if (connect(tcp_client::sock , (struct sockaddr *)&tcp_client::server , sizeof(tcp_client::server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
        exit(0);
    }

    cout<<"Connected\n";
    tcp_client::address = address;
    tcp_client::port = port;
    return true;
}