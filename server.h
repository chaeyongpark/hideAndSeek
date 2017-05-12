#pragma once
#include <vector>
#include <winsock2.h>
#include <ctime>
#include <cstdlib>
#define PLAYER_SIZE 60
#define NUM_OBSTACLE 20

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
}PACKET;

typedef struct obstacle {
	int num;
	struct position pos[NUM_OBSTACLE];
	int type[NUM_OBSTACLE];
}OBSTACLE;

typedef struct thread_parameters {
	SOCKET client_socket;
	PACKET* packet;
}THREAD_PARAMETERS;

class Server {
private:
	vector<THREAD_PARAMETERS*> client_list;
	vector<char*> buffer_store;
	THREAD_PARAMETERS* class_param;
	void ErrorHandling(char* message);
	struct position coord[NUM_OBSTACLE + 2];
	int obstacleType[NUM_OBSTACLE];

public:
	Server();
	DWORD WINAPI ThreadForEachClient(LPVOID pvoid);
	void Init();
	THREAD_PARAMETERS* getClassParam();
	void randGen();
};