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
	is_t = 0;
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
	
	recv(param.sock, buf, sizeof(OBSTACLE_PACKET)*20 + sizeof(int), 0);
	memcpy(&obstacle_num, buf, sizeof(int));
	for (int i = 0; i < obstacle_num; i++) {
		OBSTACLE_PACKET *obs = (OBSTACLE_PACKET*)malloc(sizeof(OBSTACLE_PACKET));
		struct position *p = (struct position*)malloc(sizeof(struct position));
		memcpy(p, buf + sizeof(int) + i * sizeof(struct position), sizeof(struct position));
		obs->pos = *p;
		free(p);

		obstacle.push_back(obs);
	}

	for (int i = 0; i < obstacle_num; i++) {
		int type;
		memcpy(&type, buf + sizeof(int) + 20 * sizeof(struct position) + i * sizeof(int), sizeof(int));
		obstacle[i]->type = type;
	}

	PACKET *packet = (PACKET *)malloc(sizeof(PACKET));
	packet->client_socket = param.sock;
	packet->id = -1;
	packet->pos = _p;

	param.packets.push_back(packet);
	CreateThread(NULL, 0, startMethodInClass, this, 0, NULL);
	
	sendPacket(_p, 'a', packet->id);

	return true;
}

// Send packet function
bool Client::sendPacket(struct position _pos, char _key, int my_id)
{
	char buf[BUFSIZE];
	PACKET send_packet;
	
	if (my_id == -1) {
		send_packet.client_socket = param.packets[0]->client_socket;
		send_packet.id = param.packets[0]->id;
	}
	else {
		send_packet.client_socket = param.packets[my_id]->client_socket;
		send_packet.id = param.packets[my_id]->id;
	}
	send_packet.pos = _pos;
	send_packet.key = _key;	

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

		cout << "Receive key: " << tmp_packet->key << endl;
		
		if (tmp_packet->key == '!') {
			is_g++;
			continue;
		}
		else if (tmp_packet->key == '?') {
			is_h++;
			continue;
		}
		else if (tmp_packet->key == '@') {
			is_t = 1;
			continue;
		}

		for (i = 0; i < param.packets.size(); i++) {
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
	cout << "SORT SIZE: " << param.packets.size() << endl;
	sort(param.packets.begin(), param.packets.end(), cmp);
	cout << "SORT SIZE: " << param.packets.size() << endl;
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

int Client::getIsT() {
	return is_t;
}

vector<OBSTACLE_PACKET*> Client::getObstaclePos() {
	return obstacle;
}
