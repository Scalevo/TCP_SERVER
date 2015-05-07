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
  tcp_client stair_parameters(1,"/stair_parameters",n,"Float64MultiArray");


  //MyRIO --> ROS
  //tcp_client IMU(2,"IMU",n,"IMU");
  tcp_client encoder(2,"Encoder",n,"lasertech::ScalevoWheels");
  tcp_client lambda(2,"Lambda",n,"Float64MultiArray");
  //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64MultiArray");
  //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64");

// If you uncomment this....... -> tcp_client::tcp_client()

    int count = 0;
    ros::Rate loop_rate(100);
    while(ros::ok())
    {
      //ROS_INFO("BLABLABLAB %d",count);
      
      lambda.parser(lambda.receive_bytes(1024));
      encoder.parser(encoder.receive_bytes(1024));

      //parser("DATA:Stair_parameters:0.17,.3");

      ros::spinOnce();
      loop_rate.sleep();
      count++;
     }

  //ros::spin();
  return 0;
}

