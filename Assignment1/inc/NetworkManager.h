#ifndef _NETWORK_MANAGER_H_
#define	_NETWORK_MANAGER_H_

#define	DEFAULT_PORT 3180
#define	DEFAULT_IP "127.0.0.1"
#define DEFAULT_NAME "Player"
#define MAX_CLIENTS 16

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <list>
#include <string>
using namespace RakNet;
class NetworkManager {
public:
	enum ServerMessage {
		ID_CLIENT_CONNECT = ID_USER_PACKET_ENUM + 1,
		ID_CLIENT_TURN = ID_USER_PACKET_ENUM + 2,
		ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 3,
		ID_SERVER_BROADCAST = ID_USER_PACKET_ENUM + 4,
		ID_SERVER_END = ID_USER_PACKET_ENUM + 5,
		ID_CLIENT_END = ID_USER_PACKET_ENUM + 6,
		ID_CLIENT_LIST = ID_USER_PACKET_ENUM + 7,
		ID_CLIENT_LIST_WIPE = ID_USER_PACKET_ENUM + 8
	}
	;
	NetworkManager();
	~NetworkManager();

	void InitializeServer(unsigned int _port = DEFAULT_PORT, char* _ipAddress = DEFAULT_IP);
	void InitializeClient(char* _clientName = DEFAULT_NAME, unsigned int _port = DEFAULT_PORT, char* _ipAddress = DEFAULT_IP);

	void Update(double _dt);

	void TerminateConnection();
	
	void Send(char* _message);

	bool IsInitialized() { return m_initialized; }

	char* m_clientName;

	std::list<char*> m_clients;
protected:
private:
	
	void UpdateServer();
	void UpdateClient();

	bool m_initialized;
	bool m_isServer;
	bool m_running;

	RakPeerInterface* m_peer = RakPeerInterface::GetInstance();
	Packet* m_packet;
	SystemAddress m_systemAddress;

	unsigned int m_maxClients;
	unsigned int m_port;
	char* m_ipAddress;

	unsigned int m_clientCount;
};

#endif