/**
    C++ client example using sockets
*/
#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent

#include <signal.h> //crtl+c

#include <stdlib.h>
#include <stdio.h>

#include <stdlib.h>
#include <stdio.h>

  #include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


using namespace std;





/**
    TCP Client class
*/
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
	

public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    string receive(int);
	void not_first();
bool first;	




	
};

tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
first=true;
}

/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }

    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    cout<<"Connected\n";
    return true;
}

/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
 

/**
int length=data.length();
char bytes[4];
//unsigned long n = 175;
 bytes[0] = (length >> 24) & 0xFF;
 bytes[1] = (length >> 16) & 0xFF;
 bytes[2] = (length >> 8) & 0xFF;
 bytes[3] = length & 0xFF;

char str[data.length()+4];

strcpy( str, bytes );
strcat( str, data.c_str() );

**/
   //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
 //if( send(sock , str , strlen( str) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send\n";

    return true;
}

void tcp_client::not_first(){
	first=false;
}
/**
    Receive data from the connected host
*/
string tcp_client::receive(int size=512)
{
    char buffer[512];
    string reply;
string blabla, blabla1,blabla2, blabla3, blabla4;
int n,tcp,datasize;
	
	if(first == true){
	not_first();	
		for(int i=0;i<1000;i++){
		cout<<"ich bin da1 "<<i<< " \n";
			blabla1=buffer[i];
			blabla2=buffer[i+1];
			if(blabla1=="/" && blabla2=="0" ){
				not_first();	
			}
			//blabla+=buffer[i];
		}
	}else{
		cout<<"ich bin da2";
		for(int iii=0;iii<100;iii++){
/**
			 wer+=buffer[0];
			wer+=buffer[1];
			if(!isdigit(wer)){
				
			//cout<<"es ist keine zahl "<<wer	<<" blablablabal"<<"\n";


			}

**/

				blabla3=buffer[iii];
				blabla4=buffer[iii+1];
				if(blabla3=="/" && blabla4=="0"){
					//not_first();	
					//cout<<blabla;
				iii=101;
				//blabla = blabla.substr (0,blabla.length()-1);
	 	tcp=tcp+(n/1500)+1; 
	    printf("[TCP]  %d Bytes empfangen -> %d Pakete\n\n", n, tcp); 
				return blabla+"\0";
				}
			//cout<<buffer[i+1]<< " ";
			blabla+=buffer[iii];
		

		}
	}

    //Receive a reply from the server
	n=recv(sock , buffer , sizeof(buffer) , 0);
    if( n < 0)
    {
        puts("recv failed");
	//exit(1);	
    }




    //reply = buffer;
 	tcp=tcp+(n/1500)+1; 
    printf("[TCP]  %d Bytes empfangen -> %d Pakete\n\n", n, tcp); 
	return blabla+"\0";

}


void my_handler(int s){
           

	printf("Caught signal %d\n",s);
         exit(1); 

}



int main(int argc , char *argv[])
{
    tcp_client c;
    string host="192.168.10.106";
	string port;
string console,s;


	std::string delimiter = ":";
    std::string delimiter2 = ",";
    size_t pos = 0;
    size_t pos2 = 0;
    int count=0;
    std::string header;
    std::string parameter_x;
    double imu_values[9];


    cout<<"Enter hostname : ";
    //cin>>host;
    cout<<"Enter Port : ";
    //cin>>port;
	
    //connect to host
    c.conn(host , 4000);

    //send some data
    //c.send_data("GET / HTTP/1.1\r\n\r\n");

    //send some data


 	signal (SIGINT,my_handler);


	
    c.send_data("98098098098098098098098098098098");


while(1){
  s="";
  //receive and echo reply
  cout<<"----------------------------\n\n";

  console=c.receive(1024);



  cout<< console << "\n";




    cout<<"\n\n----------------------------\n\n";
  cout<<"sleep(1)"<<"\n";
  time_t t = time(0);  // t is an integer type
  cout << t << " seconds since 01-Jan-1970\n";
  sleep(1);
	}


    //done
    return 0;
}
