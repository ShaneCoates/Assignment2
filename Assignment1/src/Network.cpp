#include <ImGui.h>

#include "Network.h"

Network::Network()
{
	m_maxClients = MAX_CLIENTS;
	m_port = DEFAULT_PORT;
	m_ipAddress = DEFAULT_IP;
	m_clientName = DEFAULT_NAME;
	m_initialized = false;
}

Network::~Network()
{
	EndConnection();
	m_peer->DeallocatePacket(m_packet);
	RakPeerInterface::DestroyInstance(m_peer);
}

void Network::InitializeServer()
{
	if (m_port != DEFAULT_PORT && m_ipAddress != DEFAULT_IP)
	{
		InitializeServer(m_port, m_ipAddress);
	}
	else if (m_port != DEFAULT_PORT && m_ipAddress == DEFAULT_IP)
	{
		InitializeServer(m_port, DEFAULT_IP);
	}
	else if (m_port == DEFAULT_PORT && m_ipAddress != DEFAULT_IP)
	{
		InitializeServer(DEFAULT_PORT, m_ipAddress);
	}
	else
	{
		InitializeServer(DEFAULT_PORT, DEFAULT_IP);
	}	
}

void Network::InitializeServer(unsigned int _port, char* _ipAddress)
{
	SocketDescriptor sd(_port, 0);
	m_peer->Startup(m_maxClients, &sd, 1);

	m_initialized = true;
	m_isServer = true;
	
	ImGui::LogCustomConsole("Initializing server...\n");
	m_peer->SetMaximumIncomingConnections(m_maxClients);
	ImGui::LogCustomConsole("Server initialized.\n");
}

void Network::InitializeClient()
{
	InitializeClient(DEFAULT_NAME, DEFAULT_PORT, DEFAULT_IP);
}

void Network::InitializeClient(char* _clientName)
{
	InitializeClient(_clientName, DEFAULT_PORT, DEFAULT_IP);
}

void Network::InitializeClient(char* _clientName, unsigned int _port, char* _ipAddress)
{
	SocketDescriptor sd;
	m_peer->Startup(1, &sd, 1);
	m_isServer = false;
	m_peer->Connect(_ipAddress, _port, 0, 0);

	m_clientName = _clientName;
	m_initialized = true;
}

void Network::InitThread()
{
	if (m_isServer)
	{
		serverThread = std::thread([&](){UpdateServer();});
	}
	else if (!m_isServer)
	{
		clientThread = std::thread([&](){UpdateClient();});
	}
	else
	{
		ImGui::LogCustomConsole("Tried to start uninitialized server!");
		return;
	}
}

void Network::EndConnection()
{
	if (!m_initialized) return;

	if (m_isServer)
	{
		BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_SERVER_END);
		m_peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_systemAddress, false);
		m_running = false;
		if (serverThread.joinable()) {
			serverThread.join();
		}
	}
	else if (!m_isServer)
	{
		BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_CLIENT_END);
		m_peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_systemAddress, false);
		m_running = false;
		if (serverThread.joinable()) {
			serverThread.join();
		}
	}
	else
	{
		ImGui::LogCustomConsole("Tried to end uninitialized server!");
		return;
	}
}

void Network::SendServerMessage(char* _message)
{
	BitStream bs;
	bs.Write((RakNet::MessageID)ID_SERVER_BROADCAST);

	char buffer[256];
	sprintf(buffer, "SERVER: %s", _message);

	bs.Write(buffer);
	m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_RAKNET_GUID, true);
}

void Network::SendClientMessage(char* _message)
{
	BitStream bs;
	bs.Write((RakNet::MessageID)ID_CLIENT_MESSAGE);

	char buffer[256];
	sprintf(buffer, "%s: %s", m_clientName, _message);
	bs.Write(buffer);
	m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_systemAddress, false);
}

void Network::UpdateServer()
{
	static std::mutex thisMutex;
	thisMutex.lock();

	m_running = true;

	while (m_running)
	{
		for (m_packet = m_peer->Receive(); m_packet; m_peer->DeallocatePacket(m_packet), m_packet = m_peer->Receive())
		{
			switch (m_packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				{														 
					ImGui::LogCustomConsole("Another client has disconnected.\n");
				} break;
			case ID_REMOTE_CONNECTION_LOST:
				{											  
					ImGui::LogCustomConsole("Another client has lost the connection.\n");
				} break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				{
					ImGui::LogCustomConsole("Another client has connected.\n");
				} break;
			
			case ID_NEW_INCOMING_CONNECTION:
				{
					ImGui::LogCustomConsole("A connection is incoming!");
				} break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				{
					ImGui::LogCustomConsole("The server is full.\n");
				} break;
			case ID_DISCONNECTION_NOTIFICATION:
				{
					ImGui::LogCustomConsole("A client has disconnected.\n");
				} break;
			case ID_CONNECTION_LOST:
				{
					ImGui::LogCustomConsole("A client lost the connection.\n");
				} break;
			case ID_CLIENT_CONNECT:
				{
					 //Server only update --
					 RakNet::RakString rs;
					 RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
					 bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					 bsIn.Read(rs);

					 char buffer[256];
					 sprintf(buffer, "%s connected to the server.\n", rs.C_String());

					 ImGui::LogCustomConsole(buffer);
				} break;
			case ID_CLIENT_UPDATE:
				{
					//RakNet::BitStream bsIn(packet->data, packet->length, false);
					//bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					//bsIn.Read(rs);

					//printf("X = %f, Y = %f\n", rs.x, rs.y);

					//serverVector = rs;
				} break;
			case ID_CLIENT_MESSAGE:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);

					char buffer[256];
					sprintf(buffer, "%s\n", rs.C_String());
					ImGui::LogCustomConsole(buffer);

					BitStream bs;
					bs.Write((RakNet::MessageID)ID_SERVER_BROADCAST);
					bs.Write(rs);

					m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);
				} break;
			case ID_SERVER_END:
				{
					m_running = false;
				}
				break;
			default:
				char buffer[256];
				sprintf(buffer, "Message with identifier &i has arrived.\n", m_packet->data[0]);
				ImGui::LogCustomConsole(buffer);
				break;
			}
		}
	}

	thisMutex.unlock();
}

void Network::UpdateClient()
{
	static std::mutex thisMutex;
	thisMutex.lock();

	m_running = true;

	while (m_running)
	{
		for (m_packet = m_peer->Receive(); m_packet; m_peer->DeallocatePacket(m_packet), m_packet = m_peer->Receive())
		{
			switch (m_packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					ImGui::LogCustomConsole("Our connection request has been accepted.\n");					
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_CLIENT_CONNECT);
					bsOut.Write(m_clientName);
					m_peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_packet->systemAddress, false);
					m_systemAddress = m_packet->systemAddress;
				} break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				{
					ImGui::LogCustomConsole("The server is full.\n");
				} break;
			case ID_SERVER_BROADCAST:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);

					if (rs.Find(m_clientName) == std::string::npos)
						ImGui::LogCustomConsole((char*)rs.C_String());

					//if (!rs.)
					//	printf("%s\n", rs.C_String());
				}break;
			}
		}
	}

	thisMutex.unlock();
}