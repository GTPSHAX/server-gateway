#pragma once

#include <BaseApp.h>

#include <string>

#include <enet/enet.h>

#include <utils/Utils.h>

struct PacketVariant
{
private:
	int index = 0 , len = 0;
	BYTE* packet_data = new BYTE[ 61 ];
public:
	PacketVariant(int delay = 0 , int NetID = -1);
	~PacketVariant();

	PacketVariant* Insert ( std::string a );
	PacketVariant* Insert ( int a );
	PacketVariant* Insert ( unsigned int a );
	PacketVariant* Insert ( float a );
	PacketVariant* Insert ( float a , float b );
	PacketVariant* Insert ( float a , float b , float c );
	void CreatePacket ( ENetPeer* peer );
};