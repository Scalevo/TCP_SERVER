/* server.c */

#include "ros/ros.h"

#include "std_msgs/String.h"

#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

/* Headerfiles für UNIX/Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <sensor_msgs/Imu.h>
#include <iostream>
#include <string>
#include "ros/time.h"
#include <math.h>

#include <stdlib.h>


/* Portnummer */
#define PORT 2000

/* Puffer für eingehende Nachrichten */
#define RCVBUFSIZE 1024


   static void echo( int );

static void error_exit(char *errorMessage);

/* Die Funktion gibt Daten vom Client auf stdout aus,
 * die dieser mit der Kommandozeile übergibt. */



static void echo(int client_socket)
{
    char echo_buffer[RCVBUFSIZE];
    int recv_size;
    time_t zeit;

    if((recv_size =
            recv(client_socket, echo_buffer, RCVBUFSIZE,0)) < 0)
        error_exit("Fehler bei recv()");
    echo_buffer[recv_size] = '\0';
    time(&zeit);
    printf("Nachrichten vom Client : %s \t%s",
            echo_buffer, ctime(&zeit));

    std::string s = echo_buffer;

    std::string delimiter = ":";
    std::string delimiter2 = ",";
    size_t pos = 0;
    size_t pos2 = 0;
    int count=0;
    std::string header;
    std::string parameter_x;
    double imu_values[9];
    //here the string will be separeted into 2 parts
    //1. # Bytes and MSG Code

    while ((pos = s.find(delimiter)) != std::string::npos) {
    header = s.substr(0, pos);
    std::cout << " message header: " << header << std::endl;
    //std::cout << " s zum ersten: " << s << std::endl;
    s.erase(0, pos + delimiter.length());
    }


    std::cout << " alle_parameter: " << s << std::endl;
    //2. parametes divided by comma
    while ((pos2 = s.find(delimiter2)) != std::string::npos) {
    //count++;
    parameter_x = s.substr(0, pos2);
    imu_values[count]=atof (parameter_x.c_str());
    std::cout << " parameter_" << count++ << " " << parameter_x << std::endl;
    //std::cout << " s zum zweiten: " << s << std::endl;
    s.erase(0, pos2 + delimiter2.length());
    }
    std::cout << " parameter_" << count << " " << s << std::endl;



    ros::NodeHandle m;
    ros::Publisher chatter_pub_imu = m.advertise<sensor_msgs::Imu>("serverIMU", 1000);

    sensor_msgs::Imu msgImu;
    float Zahl = rand()*M_PI;
    msgImu.header.stamp = ros::Time::now();
    //msgImu.orientation_covariance[0]=Zahl;

    msgImu.header.frame_id="IMU";

    //die sind noch nicth implementiert *****
    msgImu.orientation.x=imu_values[1];
    msgImu.orientation.y=imu_values[1];
    msgImu.orientation.z=imu_values[1];
    msgImu.orientation.w=imu_values[1];

    //die sind jetzt richtig in m/s²
    msgImu.linear_acceleration.x=imu_values[3];
    msgImu.linear_acceleration.y=imu_values[4];
    msgImu.linear_acceleration.z=imu_values[5];

    //die sind jetzt richtig in rad/s
    msgImu.angular_velocity.x =imu_values[6];
    msgImu.angular_velocity.y=imu_values[7];
    msgImu.angular_velocity.z=imu_values[8];


    std::cout << msgImu;
    chatter_pub_imu.publish(msgImu);

}

/* Die Funktion gibt den aufgetretenen Fehler aus und
 * beendet die Anwendung. */
static void error_exit(char *error_message) {

    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));

    exit(EXIT_FAILURE);
}

int main( int argc, char *argv[]) {
    struct sockaddr_in server, client;

    int sock, fd;

    unsigned int len;

      ros::init(argc, argv, "serverIMU");
      ros::NodeHandle n;
      ros::Publisher chatter_pub = n.advertise<sensor_msgs::Imu>("serverIMU", 1000);
      ros::Rate loop_rate(1000);

    /* Erzeuge das Socket. */
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        error_exit("Fehler beim Anlegen eines Sockets");

    /* Erzeuge die Socketadresse des Servers. */
    memset( &server, 0, sizeof (server));
    /* IPv4-Verbindung */
    server.sin_family = AF_INET;
    /* INADDR_ANY: jede IP-Adresse annehmen */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Portnummer */
    server.sin_port = htons(PORT);

    /* Erzeuge die Bindung an die Serveradresse
     * (genauer: an einen bestimmten Port). */
    if(bind(sock,(struct sockaddr*)&server, sizeof( server)) < 0)
        error_exit("Kann das Socket nicht \"binden\"");

    /* Teile dem Socket mit, dass Verbindungswünsche
     * von Clients entgegengenommen werden. */
    if(listen(sock, 5) == -1 )
         error_exit("Fehler bei listen");

    printf("Server bereit - wartet auf Anfragen ...\n");
    /* Bearbeite die Verbindungswünsche von Clients
     * in einer Endlosschleife.
     * Der Aufruf von accept() blockiert so lange,
     * bis ein Client Verbindung aufnimmt. */


    for (;;) {
        len = sizeof(client);
        fd = accept(sock, (struct sockaddr*)&client, &len);
        if (fd < 0)
            error_exit("Fehler bei accept");
        printf("Bearbeite den Client mit der Adresse: %s\n",
           inet_ntoa(client.sin_addr));
        /* Daten vom Client auf dem Bildschirm ausgeben */
        echo( fd );

        //Schließe die Verbindung.
        close(fd);
    }


    int count = 0;
      while (ros::ok())
      {

        ros::spinOnce();
        loop_rate.sleep();
        ++count;
      }

    return EXIT_SUCCESS;
}
