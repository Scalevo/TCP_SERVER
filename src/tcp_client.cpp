#include "tcp_client.h"
#include <tf/transform_broadcaster.h>
#include "geometry_msgs/Quaternion.h"


#define PI 3.14159265



//Initialize static variables of tcp_client object.

int tcp_client::sock = -1;
int tcp_client::port = 0;
std::string tcp_client::address = "";
struct sockaddr_in tcp_client::server;
std::string string_message="";


tcp_client::tcp_client(int dir,std::string top,ros::NodeHandle n,std::string msg_type): direction(dir), topic(top),n_(n),msg_type_(msg_type)
{

  firstTime=true; // correction of IMU angle at beginning, this variable should be true here, DO NOT change its value!!!!!
  z_angle_ori_old=0;

    if (direction==1) // Send to MyRio
  {
    std::string _topic = "/" + topic;
    if (msg_type_ == "Float64")   {sub = n_.subscribe(topic,10, &tcp_client::CallbackF64, this);}
    else if (msg_type == "Float64MultiArray") {sub = n_.subscribe(topic,10, &tcp_client::CallbackF64MA, this);}
    else if (msg_type == "String") {sub = n_.subscribe(topic,10, &tcp_client::CallbackS, this);
}
  }

    else if (direction==2) // Get from MyRio
  {
    //not needed anymore: parsing within the loop in the main.cpp
  } 
}

// Callback for Float64
void tcp_client::CallbackF64(const std_msgs::Float64::ConstPtr& msg)
{
  if (sub.getNumPublishers() > 0) {
    std::ostringstream buff;
    buff<<msg->data;
    std::string data;
    data = "DATA:";
    data += topic;
    data += ",";
    data += buff.str();
    send_data(data);
  }
}

// Callback for Float64MultiArray
void tcp_client::CallbackF64MA(const std_msgs::Float64MultiArray::ConstPtr& msg)
{
  if (sub.getNumPublishers() > 0) {
    std::string data;
    data = "DATA:";
    data += topic;
    data += ",";
    for (int i=0; i < msg->data.size(); i++) {
      std::ostringstream buff;
      buff<<msg->data[i];
      data += buff.str();
      data += ",";
    }
    send_data(data);
  }
}

// Callback for String
void tcp_client::CallbackS(const std_msgs::String::ConstPtr& msg)
{
  if(sub.getNumPublishers() > 0){
    std::ostringstream buffer2;
    buffer2<<msg->data;
    std::string data2;

    if(topic=="scalevo_cmd"){
      data2="CMD:";
//      data2 += ",";       // not conform with google drive doc
      data2 += buffer2.str();
      send_data(data2);
      ROS_WARN("CMD SEND %s", data2.c_str());
    }
    if(topic=="scalevo_msg"){
      data2="MSG:";
//      data2 += ",";     // not conform with google drive doc
      data2 += buffer2.str();
      send_data(data2);
    }
    if(topic=="scalevo_err"){
      data2="ERR:";
//      data2 += ",";     // not conform with google drive doc
      data2 += buffer2.str();
      send_data(data2);
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

    std::string parameter_x;
    float values_parameter[12];

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
      header = s.substr(0, pos);
      //ROS_INFO("header1: %s",header.c_str());
      s.erase(0, pos + delimiter.length());
    }

    if((header=="DATA") || (header=="SRV")){
      pos = s.find(delimiter2);
      header2 = s.substr(0, pos);
      //ROS_INFO("header2: %s",header2.c_str());
      s.erase(0, pos + delimiter2.length());
    }

    //ROS_INFO("topic: %s",topic.c_str());
    //FIXME topic hat leerschlag davor, wieso??...
    std::string _topic=" "+topic;

    if ((header == "DATA") && (header2 ==_topic) )
    {
      while ((pos2 = s.find(delimiter2)) != std::string::npos)
      {
        parameter_x = s.substr(0, pos2);
        values_parameter[count]=atof (parameter_x.c_str());
        values.push_back(atof (parameter_x.c_str()));
        //ROS_INFO("value_%i: %f",count,values[count]);
        count++;
      s.erase(0, pos2 + delimiter2.length());
      }
      parameter_x = s.substr(0, pos2);

      values_parameter[count]=atof (parameter_x.c_str());
      values.push_back(atof (parameter_x.c_str()));

      //ROS_INFO("value_%i: %f",count,values[count]);
      //ROS_INFO("VECTOR SIZE: %zu ",values.size());
      publish();
        //ROS_INFO("Message has been parsed.");
    } 
    if ((header=="SRV") && (header2 == topic) )
    {
  //ROS_INFO("string: %s",s.c_str());
  ros::ServiceClient client = n_.serviceClient<scalevo_msgs::Starter>(header2);
    scalevo_msgs::Starter srv;
  //ROS_INFO("service initialised");

  pos2 = s.find(delimiter2);
  parameter_x = s.substr(0, pos2);  
  if (atoi(parameter_x.c_str()) == 1) {srv.request.on = true;}
  else {srv.request.on = false;}
  s.erase(0, pos2 + delimiter2.length());
  //ROS_INFO("I got %s",parameter_x.c_str());

  pos2 = s.find(delimiter2);
  parameter_x = s.substr(0, pos2);  
  if (atoi(parameter_x.c_str()) == 1) {srv.request.up = true;}
  else {srv.request.up = false;}
  s.erase(0, pos2 + delimiter2.length());
  //ROS_INFO("I got2 %s",parameter_x.c_str());

  if (client.call(srv))
    {
    ROS_INFO("Server %s started",header2.c_str());
    }
    else
    {
    ROS_ERROR("Server %s did not start",header2.c_str());
    }
    }

    if((header=="MSG")||(header=="ERR")){
      string_message=s;
      publish();
    }

/**else if (header2=="CMD")
    {

    }else
    {
      ROS_ERROR("NO MESSAGE HEADER RECOGNIZED");
    }
**/

}

void tcp_client::publish()
{
  if(msg_type_== "IMU")// && header2=="IMU")
  {
    pub = n_.advertise<sensor_msgs::Imu>("IMU",5);
    sensor_msgs::Imu msg;

    //wrong coordinate systems: correction
    float x_angle_ori;      			//Roll
    float y_angle_ori;      			//Pitch
    float z_angle_ori;     			//Yaw
	
    // making sure, yaw is 0 at the beginning
    if (firstTime){
        z_angle_ori_old=-values[2];
        firstTime=false;
    }

    x_angle_ori=values[1];      		//Roll
    y_angle_ori=values[0];      		//Pitch
    z_angle_ori=-values[2]-z_angle_ori_old;     //Yaw

    float x_accel=values[4];
    float y_accel=values[3];
    float z_accel=-values[5];

    float x_angle_vel=-values[7];
    float y_angle_vel=values[6];
    float z_angle_vel=values[8];

    msg.header.stamp = ros::Time::now();
    msg.header.frame_id="odom";

    // transforming angles into quaternions for imu message
    tf::Quaternion q_tf;
    q_tf.setRPY(x_angle_ori,y_angle_ori,z_angle_ori);
    geometry_msgs::Quaternion q_qm;
    tf::quaternionTFToMsg(q_tf, q_qm);

    msg.orientation=q_qm;
    msg.orientation_covariance[0]=x_angle_ori;
    msg.orientation_covariance[1]=y_angle_ori;
    msg.orientation_covariance[2]=z_angle_ori;

    msg.angular_velocity.x=x_angle_vel;
    msg.angular_velocity.y=y_angle_vel;
    msg.angular_velocity.z=z_angle_vel;

    msg.linear_acceleration.x=x_accel;
    msg.linear_acceleration.y=y_accel;
    msg.linear_acceleration.z=z_accel;

    pub.publish(msg);
    ROS_INFO_ONCE("Topic %s has been published.",topic.c_str());
    }
    else if(msg_type_== "Float64")
    {
      pub = n_.advertise<std_msgs::Float64>(topic,5);
      std_msgs::Float64 msg;

      msg.data = values[0];

      pub.publish(msg);
      ROS_INFO_ONCE("Topic %s has been published.",topic.c_str());

    }
    else if(msg_type_==  "Float64MultiArray") 	// Lambdas etc. 
    {
      pub = n_.advertise<std_msgs::Float64MultiArray>(topic,5);
      std_msgs::Float64MultiArray msg;

      for(int i=0;i<values.size();i++) msg.data.push_back(values[i]);
      
      pub.publish(msg);
      ROS_INFO_ONCE("Topic %s has been published.",topic.c_str());

    }
    else if(msg_type_==  "tcp_server::ScalevoWheels") // Encoder data
    {

      pub = n_.advertise<tcp_server::ScalevoWheels>(topic,1);
      tcp_server::ScalevoWheels msg;

      msg.header.stamp = ros::Time::now();
      msg.header.frame_id="odom";

      msg.travel[0]=values[0]*2*PI;
      msg.travel[1]=values[1]*2*PI;

      msg.speed[0]=values[2]*2*PI/60;           // RTM to rad/s
      msg.speed[1]=values[3]*2*PI/60;           // RPM to rad/s

      msg.travel_tracks[0]=values[4];           // Achtung nicht integriert... [0,1]--> nicht getestet
      msg.travel_tracks[1]=values[5];           // Achtung nicht integriert....[0,1] --> nicht getestet

      pub.publish(msg);
      ROS_INFO_ONCE("Topic %s has been published.",topic.c_str());
    }
    else if(msg_type_== "String")			// CMD, MSG, ERR Messages from myRIO
    {

      pub = n_.advertise<std_msgs::String>(topic,1);
      std_msgs::String msg;
      msg.data=string_message;
      ROS_INFO("Messages in scainfo: %s",msg.data.c_str());

      pub.publish(msg);
      ROS_INFO_ONCE("Topic %s has been published.",topic.c_str());
    }
    else						// unknown stuff
    {
      ROS_ERROR("Incorrect msg_type: %s",msg_type_.c_str());
    }
}

//Send data to the connected host
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

    //Send some data
    if( send(sock , str , length+4 , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    ROS_INFO_ONCE("msg_send: %s",data.c_str());
    return true;
}


//Receive data from the connected host
string tcp_client::receive_bytes(int size=512)
{
    char buffer[size+1];
    string reply;
    int how_big[4];
    int how_big2=0;

    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        ROS_ERROR("recv failed!!");
    }
    buffer[size] = 0;

    for(int ii=0;ii<4;ii++)
    {
      how_big[ii]=(int)buffer[ii];
      how_big2=how_big[ii];
    }

    for (int i=4;i<how_big2+4;i++) {
      reply += buffer[i];
    }
    ROS_INFO_ONCE("msg_recv: %s",reply.c_str());

   return reply;
}
