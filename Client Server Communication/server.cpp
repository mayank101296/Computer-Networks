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
    if(argv<2)
    {
        cout<<"[-] missing arguments "<<endl;
		exit(1);
    }

    string port_no(arg[1]);
	int port;
	try{
		port=stoi(port_no);
	}
	catch(...)
	{
		cout<<"[-] invalid port number "<<endl;
		exit(1);
	}

	cout<<"[*] IP Address    PORT No.    Protocol     Message  "<<endl; 
	cout<<"-----------------------------------------------------------------"<<endl;
    int serverfd,val;

	struct sockaddr_in sdd;
    memset((struct sockaddr*)&sdd,'\0',sizeof(sdd));

    sdd.sin_family=AF_INET;
    sdd.sin_port=htons(port);
    sdd.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    serverfd=socket(AF_INET,SOCK_STREAM,0);
    if(serverfd<0)
	{
		cout<<"[-] "<<inet_ntoa(sdd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	
	cout<<"[+] "<<inet_ntoa(sdd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Socket created."<<endl;
    
	socklen_t len=sizeof(sdd);
	val=bind(serverfd,(struct sockaddr*)&sdd,len);
	if(val<0)
	{
		cout<<"[-] "<<inet_ntoa(sdd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		
		exit(1);
	}
   
	cout<<"[+] "<<inet_ntoa(sdd.sin_addr)<<" -----"<<port<<"   :    TCP\t   binding completed."<<endl;
	val=listen(serverfd,10);
	if(val<0)
	{
		cout<<"[-] "<<inet_ntoa(sdd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
	}
	
	cout<<"[+] "<<inet_ntoa(sdd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Listening...."<<endl;
	int tcp_clientfd;
	struct sockaddr_in tcp_clientadd;
	len=sizeof(tcp_clientadd);
	memset((struct sockaddr*)&tcp_clientadd,'\0',sizeof(tcp_clientadd));
	
	while(1)
	{
		tcp_clientfd=accept(serverfd,(struct sockaddr*)&tcp_clientadd,&len);
		if(tcp_clientfd<0)
		{
			cout<<"[-] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		exit(1);
		}

		
		cout<<"[+] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Connected to new client."<<endl;
	
		if(fork()==0)
		{
			srand(time(0));
			close(serverfd);
			char buf[100];
			memset(buf,'\0',sizeof(buf));
			val=recv(tcp_clientfd,buf,100,0);
			if(val<0)
			{
				cout<<"[-] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		
				exit(1);
			}
			
			cout<<"[+] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Message Received "<<endl;
	
			cout<<"Message :- "<<buf<<endl;

			memset(buf,'\0',sizeof(buf));
			int udp_port=(rand()%48001)+1200;				//make it random in range(1200, 50000)

			string str="2 10 "+to_string(udp_port);
			
			
			struct sockaddr_in udp_seradd;
			memset((struct sockaddr_in*)&udp_seradd,'\0',sizeof(udp_seradd));
			udp_seradd.sin_family=AF_INET;
			udp_seradd.sin_port=htons(udp_port);
			udp_seradd.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
		
			int upd_fd;

			upd_fd=socket(PF_INET,SOCK_DGRAM,0);
			int val_bind=bind(upd_fd,(struct sockaddr*)&udp_seradd,sizeof(udp_seradd));

			val=send(tcp_clientfd,str.c_str(),str.length(),0);
			if(val<0)
			{
				cout<<"[-] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
		
				exit(1);
			}
			
			cout<<"[+] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Type 2 Message Send To Client."<<endl;
	
			int ret=close(tcp_clientfd);
			if(ret<0)
			{
				cout<<"[-] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Error "<<strerror(errno)<<endl;
				exit(1);
			}
			
			cout<<"[+] "<<inet_ntoa(tcp_clientadd.sin_addr)<<" -----"<<port<<"   :    TCP\t   Connection Closed."<<endl;
	
			
			/*********************phase 2*****************************/

			
			if(upd_fd<0)
			{
				cout<<"[-] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
				exit(1);
			}
			cout<<"[+] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Socket Created."<<endl;
	
			
			if(val_bind<0)
			{
				cout<<"[-] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
				exit(EXIT_FAILURE);
			}
		
			cout<<"[+] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Binding Completed."<<endl;
	
			len=sizeof(udp_seradd);
			memset(buf,'\0',sizeof(buf));
			string exit_msg="no";
			do
			{
				val=recvfrom(upd_fd,buf,100,0,(struct sockaddr*)&udp_seradd,&len);
				if(val<0)
				{
					cout<<"[-] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
				
					exit(1);
				}
				exit_msg=buf;
				if(exit_msg!="exit")
				{
					
					cout<<"[+] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Type 3 Message Received."<<endl;
	
					cout<<"Message :- "<<buf<<endl;
					memset(buf,'\0',sizeof(buf));
					str="4 10 UDP message received successfully";
					val=sendto(upd_fd,str.c_str(),str.length(),0,(struct sockaddr*)&udp_seradd,sizeof(udp_seradd));
					if(val<0)
					{
						cout<<"[-] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
						exit(1);
					}
					
					cout<<"[+] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Type 4 Message Send To Client."<<endl;
	
				}

			}while(exit_msg!="exit");


			val=close(upd_fd);
			if(val<0)
			{
				cout<<"[-] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Error "<<strerror(errno)<<endl;
				exit(1);
			}
			
			cout<<"[+] "<<inet_ntoa(udp_seradd.sin_addr)<<" -----"<<udp_port<<"   :    UDP\t   Connection Closed."<<endl;
	

			break;

		}
		
		

	}
	


return 0;
}
