#include "NetworkManager.h"
#include "imgui.h"

NetworkManager::NetworkManager() {
	m_maxClients = MAX_CLIENTS;
	m_port = DEFAULT_PORT;
	m_ipAddress = DEFAULT_IP;
	m_clientName = DEFAULT_NAME;
	m_initialized = false;
	m_clientCount = 0;
	m_clients.resize(MAX_CLIENTS, "");
	m_sendClientList = false;
	m_hasMoved = false;
	m_allocatedBlackController = false;
	m_allocatedRedController = false;
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
	m_hasMoved = false;
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
			char* pName = new char[strlen(rakString.C_String())];
			strcpy(pName, rakString.C_String());
			m_clients.push_front(pName);
			m_clients.pop_back();
			char buffer[256];
			sprintf(buffer, "%s connected to the server.\n", rakString.C_String());

			ImGui::LogCustomConsole(buffer);
			break;
		} case ID_CLIENT_LIST: {
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rakString);
			char* pName = new char[strlen(rakString.C_String())];
			strcpy(pName, rakString.C_String());
			m_clients.push_front(pName);
			m_clients.pop_back();
			break;
		} case ID_CLIENT_LIST_WIPE: {
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
			rakString.Clear();
			m_clients.clear();
			m_clients.resize(MAX_CLIENTS, "");
			break;
		} case ID_CLIENT_TURN: {
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_moveFrom);
			bsIn.Read(m_moveTo);
			m_hasMoved = true;
			break;
		} case ID_ALLOCATE_SIDE: {
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_allocatedBlackController);
			bsIn.Read(m_allocatedRedController);
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
			BitStream bsWipe;
			char buffer[256];
			char outBuffer[256];

			bs.Read(rakString);

			sprintf(buffer, "%s connected to the server.\n", rakString.C_String());

			char* pName = new char[strlen(rakString.C_String())];
			strcpy(pName, rakString.C_String());
			m_clients.push_front(pName);
			m_clients.pop_back();
			ImGui::LogCustomConsole(buffer);
			std::list<char*>::const_iterator pos;
			bsWipe.Write((RakNet::MessageID)ID_CLIENT_LIST_WIPE);
			m_peer->Send(&bsWipe, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);

			for (pos = m_clients.begin(); pos != m_clients.end(); pos++) {
				if (*pos != "") {
					BitStream bsClientList;
					sprintf(outBuffer, "%s\n", *pos);

					bsClientList.Write((RakNet::MessageID)ID_CLIENT_LIST);
					bsClientList.Write(outBuffer);
					m_peer->Send(&bsClientList, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);
				}
			}
			BitStream bsAllocateSide;
			if (m_allocatedBlackController) {
				m_allocatedRedController = true;
			}
			else {
				m_allocatedBlackController = true;
			}
			bsAllocateSide.Write((RakNet::MessageID)ID_ALLOCATE_SIDE);
			bsAllocateSide.Write(m_allocatedBlackController);
			bsAllocateSide.Write(m_allocatedRedController);
			m_peer->Send(&bsAllocateSide, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);
			
			break;
		} case ID_CLIENT_TURN:{
			RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
			m_peer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_RAKNET_GUID, true);

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_moveFrom);
			bsIn.Read(m_moveTo);

			char buffer[256];
			sprintf(buffer, "[%f][%f], [%f][%f]\n", m_moveFrom.x, m_moveFrom.y, m_moveTo.x, m_moveTo.y);
			ImGui::LogCustomConsole(buffer);
			m_hasMoved = true;
			
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

void NetworkManager::SendMove(glm::vec2 _from, glm::vec2 _to) {
	if (!m_initialized || m_isServer) return;

	BitStream bs;
	bs.Write((RakNet::MessageID)ID_CLIENT_TURN);
	
	bs.Write(_from);
	bs.Write(_to);
	
	m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_RAKNET_GUID, true);
}

bool NetworkManager::HasMoved(glm::vec2& _from, glm::vec2& _to) {
	if (!m_hasMoved) return false;

	_from = m_moveFrom;
	_to = m_moveTo;
	return true;
}