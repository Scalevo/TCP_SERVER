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




/* Portnummer */
#define PORT 200

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

    std_msgs::String msg;

    msg.data =echo_buffer;

    ros::NodeHandle n;
    ros::Publisher chatter_pub = n.advertise<std_msgs::String>("serverIMU", 1000);

    chatter_pub.publish(msg);

}

/* Die Funktion gibt den aufgetretenen Fehler aus und
 * beendet die Anwendung. */
static void error_exit(char *error_message) {

    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));

    exit(EXIT_FAILURE);
}


void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  //ROS_INFO("I heard: [%s]", msg->data.c_str());
}

int main( int argc, char *argv[]) {
    struct sockaddr_in server, client;

    int sock, fd;

    unsigned int len;


      ros::init(argc, argv, "serverIMU");

      ros::NodeHandle n;
      ros::Publisher chatter_pub = n.advertise<std_msgs::String>("serverIMU", 1000);
      ros::Subscriber chatter_sub = n.subscribe("serverIMU", 1000, chatterCallback);

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

/**
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
/**
        close(fd);
    }
**/

    int count = 0;
      while (ros::ok())
      {


        len = sizeof(client);
        fd = accept(sock, (struct sockaddr*)&client, &len);
        if (fd < 0)
            error_exit("Fehler bei accept");
        printf("Bearbeite den Client mit der Adresse: %s\n",
           inet_ntoa(client.sin_addr));
        /* Daten vom Client auf dem Bildschirm ausgeben */
        echo( fd );

        /* Schließe die Verbindung. */

        close(fd);

        ros::spinOnce();

        loop_rate.sleep();
        ++count;
      }

    return EXIT_SUCCESS;
}
