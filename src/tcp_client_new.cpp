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
#include <stdio.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>


#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//#include"conio.h"
#include"stdio.h"
#include"string.h"
#include"math.h"


// ros includes
#include <ros/ros.h>
#include "std_msgs/Float32.h"


using namespace std;

/**
    TCP Client class
*/
class tcp_client
{
private:
  
    int direction;
    std::string topic;
 
    ros::Subscriber sub;
    ros::Publisher pub;

    ros::NodeHandle n_;

public:
    tcp_client();
    tcp_client(int dir,std::string top,ros::NodeHandle n);
    bool send_data(string data);
    string receive_bytes(int);
    void Callback(const std_msgs::Float32::ConstPtr& msg);

    static int sock;
    int sock_t;
    static std::string address;
    static int port;
    static struct sockaddr_in server;

};

 int tcp_client::sock = -1;
 int tcp_client::port = 0;
 std::string tcp_client::address = "";
 struct sockaddr_in tcp_client::server;

tcp_client::tcp_client()
{
  //  sock = -1;
  //  port = 0;
  //  address = "";
}

void tcp_client::Callback(const std_msgs::Float32::ConstPtr& msg)
{
//float test = msg->data;
  //std::string data = (string)msg->data;
  string= DATA + topic + msg->data;
  //send_data(data);
}

tcp_client::tcp_client(int dir,std::string top,ros::NodeHandle n): direction(dir), topic(top),n_(n)
{
   sock_t = sock;
    if (direction==1)	// Send to MyRio
	{
	  sub = n_.subscribe(topic,10, &tcp_client::Callback, this); 
	}

    else if (direction==2) // Get from MyRio
	{
	  
	}
}

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
    }

    cout<<"Connected\n";
    tcp_client::address = address;
    tcp_client::port = port;
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

/**
  cout <<"das ist length: "<<length<<"\n\n";
  int size =strlen(data.c_str());
  cout <<"das ist size: "<<size<<"\n\n";
  char buffer[size+1];
  buffer[0]=(int)size;
  cout <<"das ist  buffer[0]: "<<buffer[0]<<"\n\n";
  int how_big[4];
  int how_big2=0;
  for(int ii=0;ii<4;ii++)
  {
    cout <<"das ist str: "<<(int)str[ii]<<"\n\n";
  }
/**
for(int ii=0;ii<4;ii++)
{
  cout<<(int)buffer[ii]<<" ";
  how_big[ii]=(int)buffer[ii];
  how_big2=how_big[ii];
  //std::cout << std::dec<< how_big[ii] << '\n';
}
**/

    //Send some data
    //if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    if( send(sock_t , str , length+4 , 0) < 0)
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
    char array[1024]="";
    int how_big[4];
    int how_big2=0;


    //sizeof(buffer)--> 1024
    //Receive a reply from the server
    if( recv(sock_t , buffer , sizeof(buffer) , 0) < 0)
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
    cout<<"grösse: "<<how_big2<<"\n";


    cout<<"Data recv:string  ";
    for(int i=4;i<how_big2+4;i++)
    {
      cout<<buffer[i];
    }
    cout<<"\n";


    //reply=receive_string(s);
    //int s = *(int*)&buffer[0];
    //reply = buffer;
    return reply;
}

int main(int argc , char *argv[])
{
    ros::init(argc, argv, "tcp_server");
    ros::NodeHandle n;

    string host="192.168.10.100";
    //string host="localhost";

    //connect to host
    conn(host , 4000);

    tcp_client beta(1,"/talker_test",n);

    //done
    ros::spin();
    return 0;
}
