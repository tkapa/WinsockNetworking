//Drawserver Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996)

using namespace std;
#include "Packet.h"
#include <winsock2.h>
#include <mswsock.h>
#include <iostream>

//Init Variables
WSADATA wsaData;
SOCKET sendSocket;
sockaddr_in sendAddress;

void ReadInType() {
	char buffer[10000];
	int readLength = sizeof(sendAddress);
	int result = recvfrom(sendSocket,
		buffer,
		10000,
		0,
		(SOCKADDR*)&sendAddress,
		&readLength);
	if (result == SOCKET_ERROR) {
		cout << "Result Error " << WSAGetLastError() << endl;
	}
	//Recast buffer as a packet
	Packet *p = (Packet *)buffer;

	//Decide which packet has been sent
	switch (p->type) {
		case 1: {
			PacketPlayerInformation *pp = (PacketPlayerInformation *)p;
			cout << "Server Received Player Information" << endl;
			break;
		}

		case 2: {
			PacketAnnouncement *pa = (PacketAnnouncement *)p;
			cout << "Server Received a Client Announcemesnt" << endl;
			break;
		}

		case 3: {
			PacketAcknowledgement *pac = (PacketAcknowledgement *)p;
			cout << "Received Acknowledgement from the server" << endl;
			break;
		}
	}
}

//Wait for a response
void Selection() {
	int waiting;
	do {
		fd_set checkSockets;
		checkSockets.fd_count = 1;
		checkSockets.fd_array[0] = sendSocket;

		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 10;

		waiting = select(NULL, &checkSockets, NULL, NULL, &t);

		if (waiting > 0) {
			ReadInType();
		}
	} while (waiting);
}

void main() {

	//Init WSA
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "WSAStartup kill" << std::endl;
		return;
	}

	//Make send address
	sendAddress.sin_family = AF_INET;
	sendAddress.sin_port = htons(1234);
	//127.0.0.1
	sendAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Make a socket
	sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSocket == SOCKET_ERROR) {
		std::cout << "Socket Make Kill" << std::endl;
		return;
	}

	//Resist windows shitter function
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	WSAIoctl(sendSocket,
		SIO_UDP_CONNRESET,
		&bNewBehavior,
		sizeof(bNewBehavior),
		NULL,
		0,
		&dwBytesReturned,
		NULL,
		NULL);
	/*
	//Set socket to broadcast
	BOOL bOptVal = TRUE;
	setsockopt(sendSocket,
		SOL_SOCKET,
		SO_BROADCAST,
		(const char *)&bOptVal,
		sizeof(BOOL));*/

	PacketAnnouncement p;
	p.type = Packet::e_ClientAnnouncement;
	int result = sendto(
		sendSocket,
		(const char *)&p,
		sizeof(p),
		0,
		(SOCKADDR*)&sendAddress,
		sizeof(sendAddress));
	cout << result << endl;

	Selection();

	closesocket(sendSocket);

	system("Pause");
	WSACleanup();
	return;
}