#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>
#include <iostream>
#include <string>
#include "ros/time.h"
#include <sick_tim/sick_tim_common_tcp.h>





void chatterCallback(const std_msgs::String::ConstPtr& msg)
{

  std::cout<<" \n "<<"ich bin bei callback"<<" \n ";
  std::cout<< *msg;

  if(*msg.data=="CMD:sick-tim12,on"){

  }
  if(*msg.data=="CMD:sick-tim12,off"){

  }
  if(*msg.data=="CMD:sick-tim3,on"){

  }
  if(*msg.data=="CMD:sick-tim3,off"){

  }
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "sick_sensor_switch");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("scainfo", 1000, chatterCallback);

  ros::spin();

  return 0;
}
