// ros includes
#include <ros/ros.h>
#include "tcp_client.h"
#include "conn.h"


using namespace std;


int main(int argc , char  **argv)
{
  ros::init(argc, argv, "tcp_server");
  ros::NodeHandle n;

  string host;
  int port;

  n.param<std::string>("/tcp_server/hostname",host,"192.168.10.100");
  n.param("/tcp_server/port",port,4000);

  //connect to host
  conn(host,port);

  //ROS --> MyRIO
  //tcp_client beta(1,"/beta",n,"Float64");
  //tcp_client cmd(1,"/scalevo_cmd",n,"ArrayString");

  //MyRIO --> ROS
  //tcp_client IMU(2,"IMU",n,"IMU");
  //tcp_client encoder(2,"Encoder",n,"lasertech::ScalevoWheels");
  tcp_client lambda(2,"Lambda",n,"Float64MultiArray");
  //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64MultiArray");
  //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64");

  return 0;
}

