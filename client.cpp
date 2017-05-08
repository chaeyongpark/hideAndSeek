#include "client.h"

DWORD __stdcall startMethodInClass(LPVOID arg)
{
	if (!arg)
		return 0;
	Client* class_ptr = (Client*)arg;
	class_ptr->ThreadFunction((LPVOID)&class_ptr->getParam());
}

// Compare function along client ID
bool cmp(const PACKET *a, const PACKET *b) {
	return a->id < b->id;
}

// Constructor 
Client::Client() {
	is_update = -1;
	is_g = 0;
	is_h = 0;
}

// Init Client 
bool Client::init(char *_server_address, u_int _server_port, struct position _p)
{
	char buf[BUFSIZE];
	int obstacle_num;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	param.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (param.sock == INVALID_SOCKET)
		return false;

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(_server_address);
	server_addr.sin_port = htons(_server_port);

	if (connect(param.sock, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
		return false;
	/*
	recv(param.sock, buf, sizeof(PACKET), 0);
	memcpy(&obstacle_num, buf, sizeof(int));
	for (int i = 0; i < obstacle_num; i++) {
		struct position *p = (struct position *)malloc(sizeof(struct position));
		memcpy(p, buf + sizeof(int) + i * sizeof(struct position), sizeof(struct position));
		obstacle.push_back(p);
	}*/

	PACKET *packet = (PACKET *)malloc(sizeof(PACKET));
	packet->client_socket = param.sock;
	packet->id = -1;
	packet->pos = _p;

	param.packets.push_back(packet);
	CreateThread(NULL, 0, startMethodInClass, this, 0, NULL);
	//CreateThread(NULL, 0, ThreadFunction, (LPVOID)&param, 0, NULL);
	
	sendPacket(_p, 'a');

	return true;
}

// Send packet function
bool Client::sendPacket(struct position _pos, char _key)
{
	char buf[BUFSIZE];
	PACKET send_packet;

	send_packet.client_socket = param.packets[0]->client_socket;
	send_packet.pos = _pos;
	send_packet.key = _key;
	send_packet.id = param.packets[0]->id;

	memcpy(buf, &send_packet, sizeof(PACKET));
	if (send(param.sock, buf, sizeof(PACKET), 0) == SOCKET_ERROR)
		return false;

	return true;
}

// End Socket
void Client::close() {
	closesocket(param.sock);
	WSACleanup();
}

// Thread
DWORD Client::ThreadFunction(LPVOID pvoid) {
	int retval;
	int i;
	char buf[BUFSIZE] = { 0 };
	
	//PARAMETER* dup_param = ((PARAMETER *)pvoid);
	
	while (1) {
		PACKET *tmp_packet = (PACKET*)malloc(sizeof(PACKET));
		/* Receive data */
		retval = recv(param.sock, buf, sizeof(PACKET), 0);

		if (retval == SOCKET_ERROR) {
			cout << "SOCKET_ERROR" << endl;
			cout << WSAGetLastError() << endl;
		}
		else if (retval == 0) {
			cout << "retval == 0" << endl;
		}
		memcpy(tmp_packet, buf, sizeof(PACKET));

		cout << "       " <<tmp_packet->key << "       " << endl;
		
		if (tmp_packet->key == '!') {
			cout << "smRlavy" << endl;
			is_g++;
			continue;
		}
		else if (tmp_packet->key == '?') {
			is_h++;
			continue;
		}

		for (i = 0; i < param.packets.size(); i++) {
			

			cout << param.packets[i]->client_socket << endl;
			cout << tmp_packet->client_socket << endl;
			cout << param.packets[i]->id << endl;
			cout << tmp_packet->id << endl;
			cout << param.packets[i]->pos.x << " " << param.packets[i]->pos.y << endl;
			cout << tmp_packet->pos.x << " " << tmp_packet->pos.y << endl;
			
			if (param.packets[i]->client_socket == tmp_packet->client_socket) {
				param.packets[i]->id = tmp_packet->id;
			}				
			if (param.packets[i]->id == tmp_packet->id) {
				param.packets[i]->pos = tmp_packet->pos;
				param.packets[i]->state = tmp_packet->state;
				if (i == param.packets.size() - 1)
					is_update++;
				break;
			}
		}
		
		if (i == param.packets.size())
			param.packets.push_back(tmp_packet);
	}

	return 0;
}

// Get parameter
PARAMETER Client::getParam() {
	return param;
}

// Sort client along ID
void Client::sortClient() {
	sort(param.packets.begin(), param.packets.end(), cmp);
}

// Get is_update
int Client::getIsUpdate() {
	return is_update;
}

// Get is_update
void Client::setIsUpdate() {
	is_update--;
}

// Get is_g
int Client::getIsG() {
	return is_g;
}

// initial is_g
void Client::setIsG() {
	is_g = 0;
}

// get is_h
int Client::getIsH() {
	return is_h;
}

// initial is_h
void Client::setIsH() {
	is_h = 0;
}
