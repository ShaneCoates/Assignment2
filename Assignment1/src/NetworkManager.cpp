#include "NetworkManager.h"
#include "imgui.h"

NetworkManager::NetworkManager() {
	m_maxClients = MAX_CLIENTS;
	m_port = DEFAULT_PORT;
	m_ipAddress = DEFAULT_IP;
	m_clientName = DEFAULT_NAME;
	m_initialized = false;
	//m_clients.reserve(MAX_CLIENTS);
	m_clientCount = 0;
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		m_clients[i] = "";
	}
}

NetworkManager::~NetworkManager() {
	TerminateConnection();
	m_peer->DeallocatePacket(m_packet);
	RakPeerInterface::DestroyInstance(m_peer);
}

void NetworkManager::InitializeServer(unsigned int _port, char* _ipAddress) {
	ImGui::LogCustomConsole("Initializing server...\n");
	SocketDescriptor sd(_port, 0);
	m_peer->Startup(m_maxClients, &sd, 1);
	m_initialized = true;
	m_isServer = true;
	
	m_peer->SetMaximumIncomingConnections(m_maxClients);
	ImGui::LogCustomConsole("Server initalized.\n");
}
void NetworkManager::InitializeClient(char* _clientName, unsigned int _port, char* _ipAddress) {
	SocketDescriptor sd;
	m_peer->Startup(1, &sd, 1);
	m_isServer = false;
	m_peer->Connect(_ipAddress, _port, 0, 0);
	m_clientName = _clientName;
	m_initialized = true;
}

void NetworkManager::Update(double _dt) {
	if (m_initialized) {
		if (m_isServer) {
			UpdateServer();
		} else {
			UpdateClient();
		}
	}
}

void NetworkManager::UpdateClient() {
	RakNet::BitStream bs;
	RakNet::RakString rakString;

	for (m_packet = m_peer->Receive(); m_packet; m_peer->DeallocatePacket(m_packet), m_packet = m_peer->Receive())
	{
		switch (m_packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED: {
			ImGui::LogCustomConsole("Our connection request has been accepted.\n");
			bs.Write((RakNet::MessageID)ID_CLIENT_CONNECT);
			bs.Write(m_clientName);
			m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_packet->systemAddress, false);
			m_systemAddress = m_packet->systemAddress;
			break;
		}case ID_NO_FREE_INCOMING_CONNECTIONS: {
			ImGui::LogCustomConsole("The server is full.\n");
			break;
		}case ID_SERVER_BROADCAST: {
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rakString);
			if (rakString.Find(m_clientName) == std::string::npos) {
				ImGui::LogCustomConsole((char*)rakString.C_String());
			}
			break;
		}case ID_CLIENT_CONNECT: {
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rakString);
			m_clients[m_clientCount] = rakString.C_String();
			++m_clientCount;
			//m_clients.push_back((char*)rakString.C_String());
			char buffer[256];
			sprintf(buffer, "%s connected to the server.\n", rakString.C_String());

			ImGui::LogCustomConsole(buffer);
			break;
		}
		}
	}
}

void NetworkManager::UpdateServer() {

	for (m_packet = m_peer->Receive(); m_packet; m_peer->DeallocatePacket(m_packet), m_packet = m_peer->Receive())
	{

		switch (m_packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:{
			ImGui::LogCustomConsole("A client has disconnected.\n");
			break;
		} case ID_REMOTE_CONNECTION_LOST:{
			ImGui::LogCustomConsole("A client has lost the connection.\n");
			break;
		} case ID_REMOTE_NEW_INCOMING_CONNECTION:{
			ImGui::LogCustomConsole("A client has connected.\n");
			break;
		} case ID_NEW_INCOMING_CONNECTION:{
			ImGui::LogCustomConsole("A connection is incoming!");
			break;
		} case ID_NO_FREE_INCOMING_CONNECTIONS:{
			ImGui::LogCustomConsole("The server is full.\n");
			break;
		} case ID_DISCONNECTION_NOTIFICATION:{
			ImGui::LogCustomConsole("A client has disconnected.\n");
			break;
		} case ID_CONNECTION_LOST:{
			ImGui::LogCustomConsole("A client lost the connection.\n");
			break;
		} case ID_CLIENT_CONNECT:{
			//Server only update --
			RakNet::BitStream bs(m_packet->data, m_packet->length, false);

			bs.IgnoreBytes(sizeof(RakNet::MessageID));

			RakString rakString;
			BitStream bsOut;
			char buffer[256];
			char outBuffer[256];

			bs.Read(rakString);

			sprintf(buffer, "%s connected to the server.\n", rakString.C_String());
			//m_clients.push_back((char*)rakString.C_String());
			ImGui::LogCustomConsole(buffer);

			sprintf(outBuffer, "%s\n", rakString.C_String());
			
			bsOut.Write((RakNet::MessageID)ID_CLIENT_CONNECT);
			bsOut.Write(rakString);

			m_peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);
			break;
		} case ID_CLIENT_TURN:{
			//RakNet::BitStream bsIn(packet->data, packet->length, false);
			//bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			//bsIn.Read(rs);

			//printf("X = %f, Y = %f\n", rs.x, rs.y);

			//serverVector = rs;
			break;
		} case ID_CLIENT_MESSAGE:{
			RakNet::BitStream bs(m_packet->data, m_packet->length, false);

			bs.IgnoreBytes(sizeof(RakNet::MessageID));
			RakString rakString;
			BitStream bsOut;
			char buffer[256];

			bs.Read(rakString);

			sprintf(buffer, "%s\n", rakString.C_String());
			ImGui::LogCustomConsole(buffer);

			bsOut.Write((RakNet::MessageID)ID_SERVER_BROADCAST);
			bsOut.Write(rakString);

			m_peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);
			break;
		} case ID_SERVER_END: {
			ImGui::LogCustomConsole("Server has shutdown.\n");
			break;
		} default: {	
			char buffer[256];

			sprintf(buffer, "Message with identifier &i has arrived.\n", m_packet->data[0]);
			ImGui::LogCustomConsole(buffer);
			break;
		}
		}
	}
}

void NetworkManager::TerminateConnection() {
	if (!m_initialized) return;

	if (m_isServer) {
		BitStream bs;
		bs.Write((RakNet::MessageID)ID_SERVER_END);
		m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_systemAddress, false);
	} else {
		BitStream bs;
		bs.Write((RakNet::MessageID)ID_CLIENT_END);
		char buffer[256];
		sprintf(buffer, "%s", m_clientName);
		bs.Write(buffer);
		m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_systemAddress, false);
	}
}

void NetworkManager::Send(char* _message) {
	if (!m_initialized) return;
	BitStream bs;
	char buffer[256];

	if (m_isServer) {
		bs.Write((RakNet::MessageID)ID_SERVER_BROADCAST);
		sprintf(buffer, "SERVER: %s", _message);
	} else {
		bs.Write((RakNet::MessageID)ID_CLIENT_MESSAGE);
		sprintf(buffer, "%s: %s", m_clientName, _message);
	}
	bs.Write(buffer);
	m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_RAKNET_GUID, true);
}