/**
    C++ client example using sockets
*/
#include<iostream>  //cout
#include<stdio.h>   //printf
#include<string.h>  //strlen
#include<string>    //string
#include<sys/socket.h>  //socket
#include<arpa/inet.h>   //inet_addr
#include<netdb.h>   //hostent
#include <sstream>

#include <signal.h> //crtl+c
#include <stdlib.h>
#include <unistd.h>

#include <stdlib.h>
#include <ctype.h>
#include"math.h"
#include "ros/ros.h"

#include "ros/ros.h"
#include "std_msgs/String.h"


using namespace std;

string new_msg;
string old_msg;

/**
    TCP Client class
*/
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;

public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    string receive_bytes(int);
    string receive_string(int);

};

tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}

/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }

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
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    cout<<"Connected\n";
    return true;
}

/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{

  int length=data.length();
  char bytes[5];
  //unsigned long n = 175;
   bytes[0] = (length >> 24) & 0xFF;
   bytes[1] = (length >> 16) & 0xFF;
   bytes[2] = (length >> 8) & 0xFF;
   bytes[3] = length & 0xFF;
   bytes[4] = 0;
  char str[length+5];

  memcpy( str, bytes, 4 );
  memcpy( &str[4], data.c_str(), length );

    //Send some data
    if( send(sock , str , length+4 , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send: "<<str<<"\n";

    return true;
}

/**
    Receive data from the connected host
*/
string tcp_client::receive_bytes(int size=512)
{
    char buffer[size+1];
    string reply;
    int how_big[4];
    int how_big2=0;

    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }
    buffer[size] = 0;


    cout<<"Data recv:bytes  ";
    for(int ii=0;ii<4;ii++)
    {
      cout<<(int)buffer[ii]<<" ";
      how_big[ii]=(int)buffer[ii];
      how_big2=how_big[ii];

      //std::cout << std::dec<< how_big[ii] << '\n';
    }

    cout<<"\n";

    cout<<"Data recv:string  ";
    for(int i=4;i<how_big2+4;i++)
    {
      cout<<buffer[i];
    }
    cout<<"\n";

    //reply = buffer;
    return reply;
}

void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}


int main(int argc, char **argv)
{
    tcp_client c;
    string host="192.168.10.100";
    //string host="localhost";

    //connect to host
    c.conn(host , 4000);
    int count=0;


    ros::init(argc, argv, "tcp_client");

    ros::NodeHandle nodeHandle;

    ros::Subscriber sub_cmd = nodeHandle.subscribe("scalevo_pos", 1000, chatterCallback);

    while(){
      cout<<"\n\n";
      cout<<"--------------du bist am Anfang--------------\n";
      /**
      stringstream ss;
      ss << count;
      string str = ss.str();
      count++;
      cout<<"----------------------------\n";
      string blabla="DATA:beta,87.2154";
      **/


      //send some data
      c.send_data("Data to be send");

      //receive and echo reply
      cout<<"----------------------------\n";
      cout<<c.receive_bytes(1024);
      cout<<"----------------------------\n";

      cout<<"--------------du bist am schluss--------------\n";
      sleep(0.1);
    }

      ros::spin();


    //done
    return 0;
}
