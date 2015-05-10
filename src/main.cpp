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
    tcp_client stair_parameters(1,"stair_parameters",n,"Float64MultiArray");      //List of parameters:
    tcp_client distance_first(1,"distance_first",n,"Float64MultiArray");          //TODO create just one object?
    tcp_client distance_last(1,"distance_last",n,"Float64MultiArray");            //TODO create just one object?
    tcp_client set_vel(1,"set_vel",n,"Float64MultiArray");                        //set velocity with angle from x and speed in x direction
    tcp_client set_pos(1,"set_pos",n,"Float64MultiArray");                        //set position with angle from x and distance from the here
    //tcp_client velocity(1,"velocity",n,"Float64MultiArray");                    //is a duplicate from set_vel

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
      std::string receivedString= lambda.receive_bytes(1024);
      lambda.parser(receivedString);
      encoder.parser(receivedString);
      imu.parser(receivedString);

      ros::spinOnce();
      loop_rate.sleep();
      count++;
     }
  }

  //ros::spin();
  return 0;
}

