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
#include "sensor_msgs/Imu.h"


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
    std::string msg_type_;

    std::vector<float> values;


public:
    tcp_client();
    tcp_client(int dir,std::string top,ros::NodeHandle n,std::string msg_type);
    bool send_data(string data);
    string receive_bytes(int);
    void Callback(const std_msgs::Float32::ConstPtr& msg);
    void parser(std::string s);

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
  std::ostringstream buff;
  buff<<msg->data;
  std::string data = buff.str();
  send_data(data);
}







tcp_client::tcp_client(int dir,std::string top,ros::NodeHandle n,std::string msg_type): direction(dir), topic(top),n_(n),msg_type_(msg_type)
{
   sock_t = sock;
    if (direction==1)	// Send to MyRio
	{
	  sub = n_.subscribe(topic,10, &tcp_client::Callback, this); 
	}

    else if (direction==2) // Get from MyRio
	{
 
	int count = 0;
	 ros::Rate loop_rate(1);
	    while(ros::ok())
	    {
	    //parser("IMU:198,132,3123,23,1498,12348,666");
	    ROS_INFO("BLABLABLAB %d",count);
  	    std::string test;
	    test = receive_bytes(1024);
   	    std::cout << test << std::endl;
	    parser(test);

	    ros::spinOnce();
	    loop_rate.sleep();
	    count++;
	    }
	} 

}

void tcp_client::parser(std::string s) {

    values.clear();

    std::string delimiter = ":";
    std::string delimiter2 = ",";
    size_t pos = 0;
    size_t pos2 = 0;
    int count=0;
    std::string header;
    std::string parameter_x;
    double imu_values[9];

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
    header = s.substr(0, pos);
    std::cout << " message header: " << header << std::endl;
    //std::cout << " s zum ersten: " << s << std::endl;
    s.erase(0, pos + delimiter.length());
    }
    

    if (header==topic) 
    {
    while ((pos2 = s.find(delimiter2)) != std::string::npos)
    {
    //count++;
    parameter_x = s.substr(0, pos2);
    values.push_back(atof (parameter_x.c_str()));
    ROS_INFO("value: %f",values[count]);
    count++;
    //std::cout << " s zum zweiten: " << s << std::endl;
    s.erase(0, pos2 + delimiter2.length());
    }

    values.push_back(atof (parameter_x.c_str()));
    ROS_INFO("value: %f",values[count]);
    ROS_INFO("VECTOR SIZE: %zu ",values.size());
    }



    if(msg_type_== "IMU")
{
	pub = n_.advertise<sensor_msgs::Imu>(topic,1);
	sensor_msgs::Imu msg;

	msg.header.stamp = ros::Time::now();
	msg.header.frame_id="IMU_frame";
	msg.orientation.x=values[0];
	msg.orientation.y=values[1];
	msg.orientation.z=values[2];
	msg.orientation.w=values[3];
	msg.linear_acceleration.x=values[4];
	msg.linear_acceleration.y=values[5];
	msg.linear_acceleration.z=values[6];

 	pub.publish(msg);    
}
	else if(msg_type_== "Float")
{
	
}
	else if(msg_type_==  "FloatArray")
{

}
	else if(msg_type_==  "Encoder")
{

}
	else if(msg_type_== "CMD")
{

}
	else
{
  //ROS_WARN("Uncorrect msg_type: %s",msg_type_);
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
    for (int i=4;i<how_big2+4;i++) {
    reply += buffer[i];
}

   return reply;
}

int main(int argc , char  **argv)
{
    ros::init(argc, argv, "tcp_server");
    ros::NodeHandle n;

    string host="192.168.10.105";    //string host="localhost";

    //connect to host
    conn(host , 4000);

    tcp_client beta(1,"/beta",n,"Float");
    tcp_client IMU(2,"IMU",n,"IMU");

    //done

    return 0;
}

