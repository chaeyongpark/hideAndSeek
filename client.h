#pragma once
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>

#define BUFSIZE 512

using namespace std;

struct position {
	int x;
	int y;
};

typedef struct packet {
	SOCKET client_socket;
	char key;
	int id;
	int state;
	struct position pos;
} PACKET;

typedef struct parameter {
	SOCKET sock;
	SOCKADDR_IN serveraddr;
	vector<PACKET *> packets;
} PARAMETER;

typedef struct obstacle_packet {
	struct position pos;
	int type;
} OBSTACLE_PACKET;

class Client {
private:
	WSADATA wsa;
	SOCKADDR_IN server_addr;
	PARAMETER param;
	vector<OBSTACLE_PACKET*> obstacle;
	int is_update;
	int is_g;
	int is_h;
	int is_t;

public:
	Client();
	bool init(char *_server_address, u_int _server_port, struct position _p);
	bool sendPacket(struct position _pos, char _key, int my_id);
	void close();
	DWORD WINAPI ThreadFunction(LPVOID pvoid);
	PARAMETER getParam();
	void sortClient();
	int getIsUpdate();
	void setIsUpdate();
	int getIsG();
	void setIsG();
	int getIsH();
	void setIsH();
	int getIsT();

	vector<OBSTACLE_PACKET*> getObstaclePos();
};
