//Server
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996)

using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <iostream>
#include "Packet.h"

//Init Variables
WSADATA wsaData;
SOCKET receiveSocket;
sockaddr_in receiveAddress;

void ReadInType() {
	char buffer[10000];
	int readLength = sizeof(receiveAddress);
	int result = recvfrom(receiveSocket,
		buffer,
		10000,
		0,
		(SOCKADDR*)&receiveAddress,
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
			cout << "Server Received a Client Announcement" << endl;

			//Send an acknowledgement back
			PacketRequest pan;
			pan.type = Packet::e_ServerAcknowledgement;
			int result;
			result = sendto(
				receiveSocket,
				(const char *)&pan,
				sizeof(pan),
				0,
				(SOCKADDR*)&receiveAddress,
				sizeof(receiveAddress));
			cout << result << endl;
			break;
		}

		case 3: {
			PacketRequest *pac = (PacketRequest *)p;
			break;
		}

		case 4: {
			PacketInformation *pi = (PacketInformation *)p;
			cout << "Received user and password from client" << endl;
			cout << pi->username << "\n" << pi->password << endl;;
		}
	}
}

void Selection() {
	int waiting;
	do {
		fd_set checkSockets;
		checkSockets.fd_count = 1;
		checkSockets.fd_array[0] = receiveSocket;

		struct timeval t;
		t.tv_sec = 100;
		t.tv_usec = 100;

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
	receiveAddress.sin_family = AF_INET;
	receiveAddress.sin_port = htons(1234);
	receiveAddress.sin_addr.s_addr = INADDR_ANY;

	//Make a socket
	receiveSocket = socket(AF_INET, SOCK_DGRAM, 0);

	//Resist windows shitter function
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	WSAIoctl(receiveSocket,
		SIO_UDP_CONNRESET,
		&bNewBehavior,
		sizeof(bNewBehavior),
		NULL,
		0,
		&dwBytesReturned,
		NULL,
		NULL);

	if (bind(receiveSocket, (SOCKADDR*)&receiveAddress, sizeof(receiveAddress)) == SOCKET_ERROR) {
		std::cout << "Bind Kill " << WSAGetLastError() << std::endl;
		return;
	}

	Selection();

	system("Pause");
	WSACleanup();
	return;
}