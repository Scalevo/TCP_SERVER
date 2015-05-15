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
  int numberOfTries=500;
  while(tries<numberOfTries && isConnected==false){
    tries++;
    isConnected=conn(host,port);
    if(isConnected==false){
      ROS_WARN("connection failed for the %d time",tries);
      ROS_WARN("Trying again in 1 sec");
      if(tries==numberOfTries){
        ROS_ERROR("BAUAB ES FUNKTIONIERT NOED... \n ISCH DE MYRIO AM LAUFE?? \n ODER BISCH DU IM SCALAN??");
      }
    }
    sleep(1);
  }


  if(isConnected){
    //ROS --> MyRIO---------------------------------------------------------------
    tcp_client scalevo_cmd(1,"scalevo_cmd",n,"String");                           // cmd to myRIO like Assume_safe_pos, push linMot etc.
    tcp_client scalevo_msg(1,"scalevo_msg",n,"String");                           // msg to myRIO programm ready etc.
    tcp_client scalevo_err(1,"scalevo_err",n,"String");                           // err to myRIO like File not found etc.
    
    tcp_client beta(1,"beta",n,"Float64");                                        // Angle on the stairs
    tcp_client stair_parameters(1,"stair_parameters",n,"Float64MultiArray");      //List of parameters:
    tcp_client distance(1,"distance",n,"Float64MultiArray");          		        //Distance to steps (first_right,first_left,last_right,last_left)
    tcp_client set_vel(1,"set_vel",n,"Float64MultiArray");                        //set velocity with angle from x and speed in x direction
    tcp_client set_pos(1,"set_pos",n,"Float64MultiArray");                        //set position with angle from x and distance from the here


    //MyRIO --> ROS---------------------------------------------------------------
    tcp_client imu(2,"IMU",n,"IMU");
    tcp_client encoder(2,"Encoder",n,"lasertech::ScalevoWheels");
    tcp_client lambda(2,"Lambda",n,"Float64MultiArray");
    tcp_client scainfo(2,"scainfo",n,"String");                                   // MSG, ERR, INFO etc from myRIO is published here

    // Services Start and Stop
    // Scadaption
    tcp_client distance_first(2,"distance_first",n,"scalevo_msgs::Starter");	    //start and stop analysis of distance to first step
    tcp_client distance_last(2,"distance_last",n,"scalevo_msgs::Starter");	      //start and stop analysis of distance to last step
	
    // Localtech
    tcp_client localtechWatchdog(2,"watchdog",n,"scalevo_msgs::Starter");         // ich hab ein watchdog gemacht, der mir die einzelne services startet je nach Gebrauch... sonst musste ich 10 services machen... ein File nur für Services eigentlich viel Besser...
//    tcp_client posEstimator(2,"posEstimator",n,"scalevo_msgs::Starter");


    // Scalaser
    tcp_client align_wheelchair(2,"align_wheelchair",n,"scalevo_msgs::Starter");


    //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64MultiArray");
    //tcp_client stair_parameters(2,"Stair_parameters",n,"Float64");

    int count = 0;
    ros::Rate loop_rate(100);
    while(ros::ok())
    {
      std::string receivedString= lambda.receive_bytes(1024);

      lambda.parser(receivedString);
      encoder.parser(receivedString);
      imu.parser(receivedString);
      distance_first.parser(receivedString);
      distance_last.parser(receivedString);
      scainfo.parser(receivedString);
      align_wheelchair.parser(receivedString);
      localtechWatchdog.parser(receivedString);

      ros::spinOnce();
      loop_rate.sleep();
      count++;
     }
  }

  //ros::spin();
  return 0;
}

