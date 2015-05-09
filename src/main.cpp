// ros includes
#include <ros/ros.h>
#include "tcp_client.h"
#include "conn.h"


using namespace std;


int main(int argc , char  **argv)
{
  ros::init(argc, argv, "tcp_server");
  ros::NodeHandle n;
  int tries=0;
  string host;
  int port;

  bool isConnected=false;

  n.param<std::string>("/tcp_server/hostname",host,"192.168.10.100");
  n.param("/tcp_server/port",port,4000);

  //connect to host
  while(tries<5 && isConnected==false){
    tries++;
    isConnected=conn(host,port);
    if(isConnected==false){
      ROS_WARN("connection failed for the %d time",tries);
      ROS_WARN("Trying again in 0.5 sec");
      if(tries==5){
        ROS_ERROR("BAUAB ES FUNKTIONIERT NOED... \n ISCH DE MYRIO AM LAUFE?? \n ODER BISCH DU IM SCALAN??");
      }
    }

    sleep(0.5);
  }


  if(isConnected){
    //ROS --> MyRIO
    tcp_client beta(1,"beta",n,"Float64");                                        // Angle on the stairs
    tcp_client cmd(1,"scalevo_cmd",n,"String");                                   // cmd to myRIO like Assume_safe_pos, push linMot etc.
    tcp_client stair_parameters(1,"stair_parameters",n,"Float64MultiArray");
    tcp_client distance_first(1,"distance_first",n,"Float64MultiArray");
    tcp_client distance_last(1,"distance_last",n,"Float64MultiArray");
    tcp_client set_vel(1,"set_vel",n,"Float64MultiArray");
    tcp_client set_pos(1,"set_pos",n,"Float64MultiArray");

    //MyRIO --> ROS
    tcp_client imu(2,"IMU",n,"IMU");
    tcp_client encoder(2,"Encoder",n,"lasertech::ScalevoWheels");
    tcp_client lambda(2,"Lambda",n,"Float64MultiArray");
    //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64MultiArray");
    //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64");

    int count = 0;
    ros::Rate loop_rate(1000);
    while(ros::ok())
    {
      //ROS_INFO("BLABLABLAB %d",count);
      std::string receivedString= lambda.receive_bytes(1024);
      lambda.parser(receivedString);
      encoder.parser(receivedString);
      imu.parser(receivedString);

      //parser("DATA:Stair_parameters:0.17,.3");

      ros::spinOnce();
      loop_rate.sleep();
      count++;
     }

  }




// If you uncomment this....... -> tcp_client::tcp_client()



  //ros::spin();
  return 0;
}

