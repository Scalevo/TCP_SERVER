#include "tcp_client.h"



 int tcp_client::sock = -1;
 int tcp_client::port = 0;
 std::string tcp_client::address = "";
 struct sockaddr_in tcp_client::server;


tcp_client::tcp_client(int dir,std::string top,ros::NodeHandle n,std::string msg_type): direction(dir), topic(top),n_(n),msg_type_(msg_type)
{

    if (direction==1)	// Send to MyRio
	{
	  sub = n_.subscribe(topic,10, &tcp_client::Callback, this); 
	}

    else if (direction==2) // Get from MyRio
	{
 
    int count = 0;
    ros::Rate loop_rate(20);
    while(ros::ok())
    {
      //ROS_INFO("BLABLABLAB %d",count);
      
      parser(receive_bytes(1024));
      //parser("DATA:Stair_parameters:0.17,.3");

      ros::spinOnce();
      loop_rate.sleep();
      count++;
	   }
	} 

}

void tcp_client::Callback(const std_msgs::Float64::ConstPtr& msg)
{
  std::ostringstream buff;
  buff<<msg->data;
  std::string data = buff.str();
  send_data(data);
}


void tcp_client::parser(std::string s) {

    values.clear();

    //ROS_INFO("vector cleaned");

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
    //std::cout << " message header: " << header << std::endl;
    //std::cout << " s zum ersten: " << s << std::endl;
    s.erase(0, pos + delimiter.length());
    }

    if(header=="DATA"){
      //cout <<"blablabla\n";
      pos = s.find(delimiter2);
      header2 = s.substr(0, pos);
      //ROS_INFO("header2: %s",header2.c_str());
      //std::cout << " message header: " << header << std::endl;
      //std::cout << " s zum ersten: " << s << std::endl;
      s.erase(0, pos + delimiter2.length());
    }

    //ROS_INFO("here core dumped");
    //ROS_INFO("topic: %s",topic.c_str());

    //FIXME sometimes "Segmentation fault (core dumped)" an dieser Stelle
    //FIXME topic hat leerschlag davor, wieso??...
    std::string _topic=" "+topic;
    //cout <<"_topic"<<_topic<<"\n";
    //cout <<"header2"<<header2<<"\n";


    if (header2 ==_topic )
    {
      ROS_INFO("topic: %s",_topic.c_str());
      ROS_INFO("header2: %s",header2.c_str());

    while ((pos2 = s.find(delimiter2)) != std::string::npos)
    {
    parameter_x = s.substr(0, pos2);
    //std::cout << " parameter_: " <<count<<" :"<< parameter_x << std::endl;

    values_parameter[count]=atof (parameter_x.c_str());
    values.push_back(atof (parameter_x.c_str()));
    ROS_INFO("value_%i: %f",count,values[count]);
    count++;
    //std::cout << " s zum zweiten: " << s << std::endl;
    s.erase(0, pos2 + delimiter2.length());
    }
    parameter_x = s.substr(0, pos2);

    values_parameter[count]=atof (parameter_x.c_str());
    values.push_back(atof (parameter_x.c_str()));

    //ROS_INFO("value_%i: %f",count,values[count]);
    ROS_INFO("VECTOR SIZE: %zu ",values.size());

    publish();

    }

    ROS_INFO("Message has been parsed.");
    /**else if (header2=="CMD")
    {

    }else if (header2=="CMD")
    {

    }else
    {
      ROS_ERROR("NO MESSAGE HEADER RECOGNIZED");
    }
**/


}

void tcp_client::publish() {

      if(msg_type_== "IMU" && header2=="IMU")
    {
      ROS_INFO("ich bin drin: %s",msg_type_.c_str());

      pub = n_.advertise<sensor_msgs::Imu>(topic,5);
      sensor_msgs::Imu msg;

      //wrong coordinate systems: correction
      //angles not unit vector
      float x_angle_ori=values[1];
      float y_angle_ori=values[0];
      float z_angle_ori=-values[2];
      //cout <<"msg : "<<"blabla"<<"\n";

      float x_accel=values[4];
      float y_accel=values[3];
      float z_accel=-values[5];

      float x_angle_vel=values[7];
      float y_angle_vel=values[6];
      float z_angle_vel=-values[8];
      //conversion from angle to unit vector

      /**
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
**/

      msg.header.stamp = ros::Time::now();
      msg.header.frame_id="odom";

      msg.orientation.x=x_angle_ori;
      msg.orientation.y=y_angle_ori;
      msg.orientation.z=z_angle_ori;
      msg.orientation.w=1;

      msg.angular_velocity.x=x_accel;
      msg.angular_velocity.y=y_accel;
      msg.angular_velocity.z=z_accel;

      msg.linear_acceleration.x=x_angle_vel;
      msg.linear_acceleration.y=x_angle_vel;
      msg.linear_acceleration.z=x_angle_vel;


      //cout<<msg<<"\n";
      pub.publish(msg);
    }
    else if(msg_type_== "Float64")
    {
      pub = n_.advertise<std_msgs::Float64>(topic,5);
      std_msgs::Float64 msg;

      msg.data = values[0];

      pub.publish(msg);
      ROS_INFO("Topic %s has been published.",topic.c_str());

    }
    else if(msg_type_==  "Float64MultiArray")
    {
      pub = n_.advertise<std_msgs::Float64MultiArray>(topic,5);
      std_msgs::Float64MultiArray msg;

      for(int i=0;i<values.size();i++) msg.data.push_back(values[i]);
      
      pub.publish(msg);
      ROS_INFO("Topic %s has been published.",topic.c_str());

    }
    else if(msg_type_==  "Encoder")
    {
      cout<< "bin i msg type Encoder";

      /*
      pub = n_.advertise<tcp_server::ScalevoWheels>(topic,1);
      tcp_server::ScalevoWheels msg;

      msg.header.stamp = ros::Time::now();
      //msg.header.frame_id="IMU_frame";
      msg.travel[0]=values[0];
      msg.travel[1]=values[1];
      msg.speed[0]=values[2];
      msg.speed[1]=values[3];
      msg.travel_tracks[0]=values[0];
      msg.travel_tracks[1]=values[1];
      msg.speed_tracks[0]=values[2];
      msg.speed_tracks[1]=values[3];

      cout<<msg<<"\n";
      pub.publish(msg);

      */
    }
    else if(msg_type_== "CMD")
    {

    }
    else
    {
      ROS_WARN("Uncorrect msg_type: %s",msg_type_.c_str());
    }
}



/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
    ROS_INFO("msg_send: %s",data.c_str());

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
    //cout<<"Data send: "<<str<<"\n";

    return true;
}

/**
    Receive data from the connected host
*/
string tcp_client::receive_bytes(int size=512)
{
    char buffer[size+1];
    string reply;
    int how_big[4];
    int how_big2=0;


    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        //puts("recv failed");
        ROS_ERROR("recv failed!!");

    }
    buffer[size] = 0;

    //cout<<"Data recv:bytes  ";
    for(int ii=0;ii<4;ii++)
    {
      //cout<<(int)buffer[ii]<<" ";
      how_big[ii]=(int)buffer[ii];
      how_big2=how_big[ii];
    }

    //cout<<"\n";


    //    cout<<"Data recv:string  ";

    /**
    for(int i=4;i<how_big2+4;i++)
    {
      cout<<buffer[i];
    }
    **/
    for (int i=4;i<how_big2+4;i++) {
      reply += buffer[i];
    }
    cout<<"\n";
    //ROS_INFO("msg_recv: %s",reply.c_str());

   return reply;
}