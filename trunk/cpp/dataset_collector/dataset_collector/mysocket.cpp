﻿#include "global.h"
#include "mysocket.h"
#include "windows.h"
#include "botinterface.h"

bool mysocket::WSARunning = false;
int mysocket::nr_open_sockets = false;

mysocket::mysocket(int id, int PortNo, char *IPAddress, int buffersize, botinterface *i)
{
	bool result;
	this->id = id;
	this->i = i;
	this->buffersize = buffersize;

	connected = false;

	result = open(PortNo, IPAddress);

	if (result)
	{
		connected = true;
		Sleep(10);
		start_listener();
	}
}


mysocket::~mysocket(void)
{
	close();
}


int mysocket::send(char *message)
{
	return ::send(s, message, strlen(message), 0);
}


static DWORD WINAPI receive_thread(void* Param)
{
	mysocket* This = (mysocket*) Param; 

	int bytes;
	bytes = SOCKET_ERROR;
	char *cServerMessage;
	cServerMessage = new char[This->buffersize];

	while(bytes = recv(This->s, cServerMessage, This->buffersize, 0))
	{
		if(bytes == SOCKET_ERROR)
		{
			return 0;
		}
    
		if (bytes == 0 || bytes == WSAECONNRESET)
		{
			return 0;
		}

		if(bytes < 1)
		{
			Sleep(300);
			continue;
		}

		This->i->socket_callback(This->id, cServerMessage, bytes);

		//Sleep(1); // Don't consume too much CPU power.
	}

	return 0;
}

int mysocket::start_listener(void)
{
	DWORD ThreadID;
	h = CreateThread( NULL, 0, receive_thread, (void*) this, 0, &ThreadID);

	return 1;
}


bool mysocket::open(int PortNo, char* IPAddress)
{
	if (!WSARunning)
		winsock_start();

     //Fill out the information needed to initialize a socket�

    SOCKADDR_IN target; //Socket address information

    target.sin_family = AF_INET; // address family Internet

    target.sin_port = htons (PortNo); //Port to connect on

    target.sin_addr.s_addr = inet_addr (IPAddress); //Target IP


    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket

    if (s == INVALID_SOCKET)
    {
        return false; //Couldn't create the socket

    }  

    //Try connecting...


    if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
    {
        return false; //Couldn't connect

    }
    else
	{
		nr_open_sockets++;
        return true; //Success
	}
}

//CLOSECONNECTION � shuts down the socket and closes any connection on it

void mysocket::close()
{
    //Close the socket if it exists
    if (s) {
        closesocket(s);
		connected = false;

		if (--nr_open_sockets <= 0)
			winsock_stop();
	}
}

bool mysocket::winsock_start()
{
	if (WSARunning)
		return false;

   //Start up Winsock

    WSADATA wsadata;

    int error = WSAStartup(0x0202, &wsadata);

    //Did something happen?

    if (error)
        return false;

    //Did we get the right Winsock version?

    if (wsadata.wVersion != 0x0202)
    {
        WSACleanup(); //Clean up Winsock

        return false;
    }

	return true;
}


void mysocket::winsock_stop()
{
	if (WSARunning && nr_open_sockets <= 0) {
		WSACleanup();
		nr_open_sockets = 0;
	}
}