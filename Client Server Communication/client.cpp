#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>


using namespace std;

int main(int argv,char **arg)
{
	
	if(argv<3)					//checking for missing arguments
	{
		cout<<"[-] missing arguments "<<endl;
		exit(1);
	}
	
	string ip_address(arg[1]),port_no(arg[2]);

	if(ip_address!="172.0.0.1")
	{
		cout<<"[-] IP Address Must be loopback address otherwise it can't connect to server"<<endl;
		cout<<"[-] SERVER IP Address is by default LOOPBACK address by problem statement"<<endl;
		exit(1);
	}

	int port;
	int clientfd,connected;						//declaration of file descriptor
	try{
		port=stoi(port_no);
	}
	catch(...)					//checking wheather given port number is valide or not.
	{
		cout<<"[-] invalid port number "<<endl;
		exit(1);
	}

	struct sockaddr_in server_address;							
	memset((struct sockaddr_in*)&server_address,'\0',sizeof(server_address));

	server_address.sin_family=AF_INET;							//Assigning the socket address parameters to input values
	server_address.sin_port=htons(port);
	server_address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

	cout<<"[*] IP Address    PORT No.    Protocol     Message  "<<endl; 
	cout<<"-----------------------------------------------------------------"<<endl;
	clientfd=socket(AF_INET,SOCK_STREAM ,0);														//function call for socket creation
	if(clientfd<0)
	{
		
		cout<<"[-] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :   TCP    Error "<<strerror(errno)<<endl;
		exit(1);
	}
	cout<<"[+] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Socket created"<<endl;

	
	connected=connect(clientfd,(struct sockaddr*)&server_address,sizeof(server_address));				//call to make connection with server
	if(connected<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	
	cout<<"[+] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Connection Esatablished"<<endl;
	char buf[]="1 13 req udp";
	

	int ret=send(clientfd,buf,strlen(buf),0);												//sending Type 1 Message ON TCp 
	if(ret<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	
	
	cout<<"[+] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Type 1 Message send(UDP Port no. req)."<<endl;

	char msg[100];
	memset(msg,'\0',sizeof(msg));
	ret=recv(clientfd,msg,100,0);												//waiting to receive Type 2 Msg from server
	
	if(ret<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}

	string msg_str(msg);
	
	int msg_len=stoi(msg_str.substr(2,4)),rec_port_no=stoi(msg_str.substr(5));
	cout<<"[+] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Type 2 Message Received "<<endl;

	cout<<"Message :- "<<msg<<endl;
	ret=close(clientfd);									//closing the tcp connection from client side
	if(ret<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	
	cout<<"[+] "<<inet_ntoa(server_address.sin_addr)<<" -----"<<port<<"   :    TCP\t   Connection Closed"<<endl;

	//**************************phase 2 ****************//


	struct sockaddr_in server_address2;			
	int clientfd2;
	memset((struct sockaddr_in*)&server_address2,'\0',sizeof(server_address2));
	server_address2.sin_family=AF_INET;
	server_address2.sin_port=htons(rec_port_no);					//socket address parameter initialization for UDP
	server_address2.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

	clientfd2=socket(PF_INET,SOCK_DGRAM ,0);				// new socket creation for UDP connection	
	if(clientfd2<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	cout<<"[+] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Socket created"<<endl;

	socklen_t len=sizeof(server_address2);	
	char buffer_udp[]="3 26 message on udp connection";		

	bool ch=true;								//sending the type 3 msg on UDP connection
	char ch1='n';
	do{
		ret=sendto(clientfd2,buffer_udp,300,0,(struct sockaddr*)&server_address2,len);
		if(ret<0)
		{
			cout<<"[-] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
			exit(1);
		}
		
		cout<<"[+] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Type 3 Message send."<<endl;

		char msg_type4[300];
		memset(msg_type4,'\0',sizeof(msg_type4));				//initializing the msg buffer to null
		len=sizeof(server_address2);							
		ret=recvfrom(clientfd2,msg_type4,300,0,(struct sockaddr*)&server_address2,&len);		//waiting to receive msg from server
		if(ret<0)					//if server not send any msg or any other cause of no respose is reported here
		{
			cout<<"[-] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
			exit(1);
		}
		
		cout<<"[+] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Type 4 Message Received."<<endl;

		cout<<"Message :- "<<msg_type4<<endl;
		cout<<endl;																//asking from user to continue on udp connection or not 
		cout<<"[+] Action : Do You want to continue to send data type y/n : ";
		cin>>ch1;

		if(!(ch1=='Y' || ch1=='y'))
		{
			ch=false;
		}

	}while(ch);

	string str="exit";					//sending the exit signal to the server.
	ret=sendto(clientfd2,str.c_str(),str.length(),0,(struct sockaddr*)&server_address2,len);
	if(ret<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	ret=close(clientfd2);				//closing the udp connection from client side. 
	if(ret<0)
	{
		cout<<"[-] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	
	cout<<"[+] "<<inet_ntoa(server_address2.sin_addr)<<" -----"<<rec_port_no<<"   :    UDP\t   Connection Closed."<<endl;

return 0;
}
