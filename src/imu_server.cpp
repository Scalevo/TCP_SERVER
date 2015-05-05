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
#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>
#include <ctype.h>

//#include"conio.h"
#include"string.h"
#include"math.h"


// ros includes
#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include "sensor_msgs/Imu.h"

#include "tcp_server/ScalevoWheels.h"

//#include <Eigen/Dense>
#include <eigen3/Eigen/Dense>


using namespace std;

int main(int argc , char  **argv)
{
    ros::init(argc, argv, "tcp_imu");

    ros::NodeHandle m;

    ros::Publisher chatter_pub_imu = m.advertise<sensor_msgs::Imu>("tcp_imu", 1000);

    ros::Rate loop_rate(10);

    int count = 0;
    while (ros::ok())
    {
      sensor_msgs::Imu msg;

      float values[20];
      //wrong coordinate systems: correction
      //angles not unit vector
      float x_angle_ori=values[1];
      float y_angle_ori=values[0];
      float z_angle_ori=-values[2];
      Eigen::Vector3d angle_ori(x_angle_ori,x_angle_ori,x_angle_ori);


      float x_accel=values[4];
      float y_accel=values[3];
      float z_accel=-values[5];

      float x_angle_vel=values[7];
      float y_angle_vel=values[6];
      float z_angle_vel=-values[8];
      //conversion from angle to unit vector
      float Ax[3][3] =
      {
        {  1,  0,  0 },
        {  0, cos(x_angle_ori), sin(x_angle_ori) },
        { 0, -sin(x_angle_ori), cos(x_angle_ori) }
      };

      float Ay[3][3] =
      {
        {  cos(y_angle_ori),  0,  -sin(y_angle_ori) },
        {  0, 1, 0 },
        { sin(y_angle_ori), 0, cos(y_angle_ori) }
      };

      float Az[3][3] =

      {
        {  cos(z_angle_ori),  sin(z_angle_ori),  0 },
        {  -sin(z_angle_ori), cos(z_angle_ori), 0 },
        { 0, 0, 1 }
      };


      Eigen::Matrix3d Ax(3,3);
      Ax << 1,  0,  0 ,
        0, cos(x_angle_ori), sin(x_angle_ori) ,
       0, -sin(x_angle_ori), cos(x_angle_ori);
      std::cout << Ax << std::endl;


      Eigen::Matrix3d Ay(3,3);
      Ay << 1,  0,  0 ,
        0, cos(x_angle_ori), sin(x_angle_ori) ,
       0, -sin(x_angle_ori), cos(x_angle_ori);
      std::cout << Ax << std::endl;


      Eigen::Matrix3d Az(3,3);
      Azz << 1,  0,  0 ,
        0, cos(x_angle_ori), sin(x_angle_ori) ,
       0, -sin(x_angle_ori), cos(x_angle_ori);
      std::cout << Ax << std::endl;

      std::cout << "p*r:\n" << Azz*angle_ori << std::endl;


      float UV_angle_ori [3];
      //multiply[][];


      msg.header.stamp = ros::Time::now();
      msg.header.frame_id="IMU_frame";
      msg.orientation.x=values[0];
      msg.orientation.y=values[1];
      msg.orientation.z=values[2];
      msg.orientation.w=values[3];
      msg.linear_acceleration.x=values[4];
      msg.linear_acceleration.y=values[5];
      msg.linear_acceleration.z=values[6];

      cout<<msg<<"\n";

      /**




    sensor_msgs::Imu msgImu;

    float Zahl = rand()*M_PI;
    msgImu.header.stamp = ros::Time::now();
    //msgImu.orientation_covariance[0]=Zahl;
    // ***Whats frame_id?***
    msgImu.header.frame_id="IMU_sim";
    msgImu.orientation.x=0;//cos(Zahl);
    msgImu.orientation.y=0;//sin(Zahl);
    msgImu.orientation.z=0;
    msgImu.orientation.w=1.0;
    msgImu.linear_acceleration.x=0.002;
    msgImu.linear_acceleration.y=0.001;
    msgImu.linear_acceleration.z=9.81;

    std::cout << msgImu;
    chatter_pub_imu.publish(msgImu);
**/
      ros::spinOnce();

      loop_rate.sleep();
      ++count;
    }


    return 0;
}

