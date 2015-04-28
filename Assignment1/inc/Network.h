#ifndef NETWORK_H
#define NETWORK_H

#define	DEFAULT_PORT 3180
#define	DEFAULT_IP "127.0.0.1"
#define DEFAULT_NAME "Player"
#define MAX_CLIENTS 16

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>

#include <thread>
#include <mutex>

using namespace RakNet;
class Network
{
public:

	Network();
	~Network();

	enum ServerMessages
	{
		ID_CLIENT_CONNECT = ID_USER_PACKET_ENUM + 1,
		ID_CLIENT_UPDATE = ID_USER_PACKET_ENUM + 2,
		ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 3,
		ID_SERVER_BROADCAST = ID_USER_PACKET_ENUM + 4,
		ID_SERVER_END = ID_USER_PACKET_ENUM + 5,
		ID_CLIENT_END = ID_USER_PACKET_ENUM + 6
	};

	void InitializeServer();
	void InitializeServer(unsigned int _port, char* _ipAddress);

	void InitializeClient();
	void InitializeClient(char* _clientName);
	void InitializeClient(char* _clientName, unsigned int _port, char* _ipAddress);

	void InitThread();
	void EndConnection();

	bool Initialized(){ return m_initialized; }

	void SendServerMessage(char* _serverMessage);
	void SendClientMessage(char* _serverMessage);

	Network* GetInstance();

	char* m_clientName;

private:

	void UpdateServer();
	void UpdateClient();

	bool m_isServer;
	bool m_initialized;
	bool m_running;

	RakPeerInterface* m_peer = RakPeerInterface::GetInstance();
	Packet* m_packet;
	SystemAddress m_systemAddress;

	std::thread serverThread;
	std::thread clientThread;

	unsigned int m_maxClients;
	unsigned int m_port;
	char* m_ipAddress;
};

#endif NETWORK_H