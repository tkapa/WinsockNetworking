#pragma once
#pragma pack(push, 1)
class Packet {
public:
	enum {
		e_Player = 1, //Client to server, is a player structure
		e_ClientAnnouncement, //Client to server, first packet sent
		e_ServerAcknowledgement, //Server to client, an acknowledgement of the client from server 
		e_ClientInformation
	};
	int type;
};

class PacketPlayerInformation : public Packet {
public:
	char name[16];
	int level;
	int experience;
	float attack;
	float defense;
};

class PacketAnnouncement : public Packet {

};

class PacketRequest : public Packet {
	
};

class PacketInformation : public Packet {

public:
	char username[15];
	char password[10];
};
#pragma pack(pop)