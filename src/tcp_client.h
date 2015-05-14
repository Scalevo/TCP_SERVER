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
#include "std_msgs/Float64.h"
#include "std_msgs/Float64MultiArray.h"
#include "std_msgs/String.h"

#include "sensor_msgs/Imu.h"

#include "tcp_server/ScalevoWheels.h"

#include "scalevo_msgs/Starter.h"

//#include <Eigen/Dense>
#include <eigen3/Eigen/Dense>


using namespace std;

/**
    TCP Client class
*/
class tcp_client
{
private:
  

  ros::NodeHandle n_;
  ros::Subscriber sub;
  ros::Publisher pub;

// Object Parameters
  std::string msg_type_;
  int direction;
  std::string topic;

// Message Values
  std::vector<double> values;
  std::string header;
  std::string header2;

public:
  tcp_client(int dir,std::string top,ros::NodeHandle n,std::string msg_type);
  bool send_data(string data);
  string receive_bytes(int);
  void CallbackF64(const std_msgs::Float64::ConstPtr& msg);
  void CallbackF64MA(const std_msgs::Float64MultiArray::ConstPtr& msg);
  void CallbackS(const std_msgs::String::ConstPtr& msg);

  void parser(std::string s);
  void publish();

  static int sock;
  static std::string address;
  static int port;
  static struct sockaddr_in server;

};

#endif
