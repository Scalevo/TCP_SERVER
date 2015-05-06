#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

/**
    C++ client example using sockets
*/
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


// ros includes
#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include "sensor_msgs/Imu.h"

#include "tcp_server/ScalevoWheels.h"

//#include <Eigen/Dense>
#include <eigen3/Eigen/Dense>


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
    tcp_client(int dir,std::string top,ros::NodeHandle n,std::string msg_type);
    bool send_data(string data);
    string receive_bytes(int);
    void Callback(const std_msgs::Float32::ConstPtr& msg);
    void parser(std::string s);

    static int sock;
    static std::string address;
    static int port;
    static struct sockaddr_in server;

};

#endif