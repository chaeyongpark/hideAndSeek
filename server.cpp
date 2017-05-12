#include "Server.h"

#include <iostream>
#include <string>

#define BUF_SIZE 512
#define SERVERPORT 9979

using namespace std;

DWORD __stdcall startMethodInThread(LPVOID arg)
{
	if (!arg)
		return 0;
	Server *class_ptr = (Server*)arg;
	class_ptr->ThreadForEachClient((LPVOID)class_ptr->getClassParam());
	return 1;
}

Server::Server()
{
	class_param = (THREAD_PARAMETERS*)malloc(sizeof(THREAD_PARAMETERS));
}

void Server::ErrorHandling(char * message)
{
	cout << message << endl;
	exit(0);
}

DWORD Server::ThreadForEachClient(LPVOID pvoid)
{
	int str_lllen;
	char buf[BUF_SIZE];
	THREAD_PARAMETERS* para = (THREAD_PARAMETERS*)pvoid;
	SOCKET clnt_sock = para->client_socket;
	PACKET* checker = (PACKET*)malloc(sizeof(PACKET));
	int i,j;
	while (1)
	{
		cout << "before receiving from socket" << endl;
		while ((str_lllen = recv(clnt_sock, buf, BUF_SIZE, 0)) != 0)
		{
			if (str_lllen == SOCKET_ERROR)
			{
				cout << "recv() ERROR: " << WSAGetLastError() << endl;
				for (i = 0; i < client_list.size(); i++)
				{
					closesocket(client_list[i]->client_socket);
					client_list.clear();
					buffer_store.clear();
					//if (client_list[i] == para)
						//client_list.erase(client_list.begin() + i);
				}
				return 0;
			}
			para->packet = (PACKET*)malloc(sizeof(PACKET));
			memcpy(para->packet, buf, sizeof(PACKET));
			//para->packet = (PACKET*)buf;
			if (para->packet->id == -1)
			{
				cout << "initializing id value" << endl;
				para->packet->id = client_list.size() - 1;
				if (para->packet->id == 0) {
					para->packet->state = 1;
					para->packet->pos = coord[0];
				}
				else {
					para->packet->state = rand()%3+2;
					para->packet->pos = coord[1];
				}
			}
			cout << "===key:" << para->packet->key << "  state:" << para->packet->state << endl;
			if (para->packet->key == 'g')
			{
				para->packet->key = '!';
				cout << "====game start called by tagger===" << endl;
			}
			else if (para->packet->key == 'h')
			{
				para->packet->key = '?';
				cout << "====gun shot called by tagger===" << endl;
			}
			else if (para->packet->key == 't')
			{
				para->packet->key = '@';
				cout << "==========killed by tagger===========" << endl;
			}

			// 만약 새로운 id라면
			if (buffer_store.size() != client_list.size()) {
				char *new_buf = (char*)malloc(sizeof(char)*BUF_SIZE);
				memcpy(new_buf, para->packet, sizeof(PACKET));
				buffer_store.push_back(new_buf);
			}
			else {
				memcpy(buffer_store[para->packet->id], para->packet, sizeof(PACKET));
			}
			cout << "client_socket in the buffer:" << para->packet->client_socket << " id in the buffer:" << para->packet->id <<
				"  position in the buffer (x,y) : (" << para->packet->pos.x << "," << para->packet->pos.y << ")" << endl;
			for (i = 0; i < client_list.size(); i++)
			{
				if (client_list[i]->client_socket == clnt_sock)
					client_list[i]->packet = para->packet;
			}
			cout << client_list.size() << endl;
			bool isStart = false;
			bool isShoot = false;
			for (i = 0; i < client_list.size(); i++)
			{
				for (j = 0; j < client_list.size(); j++)
				{
					memcpy(checker, buffer_store[j], sizeof(PACKET));
					cout << "-------------------------" << endl;
					cout << "before sending to client-" << " client iter:"<< i << " buffer iter:" << j << 
						" client socket value:" << checker->client_socket << " client ID :" << checker->id <<
						" client x:"<< checker->pos.x << " client y:" << checker->pos.y << " key before change :" << checker->key << endl;
					if (send(client_list[i]->client_socket, buffer_store[j], sizeof(PACKET), 0) == SOCKET_ERROR)
					{
						cout << WSAGetLastError() << endl;
						ErrorHandling("send() ERROR! in ThreadForEachClient");
					}
					if (checker->key == '!')
					{
						cout << "deleting exclamation mark" << endl;
						isStart = true;
					}
					if (checker->key == '?')
					{
						cout << "deleting question mark" << endl;
						isShoot = true;
					}
					cout << "-------------------------" << endl;
				}
			}
			if (isStart)
			{
				for (j = 0;j < client_list.size();j++)
				{
					memcpy(checker, buffer_store[j], sizeof(PACKET));
					if (checker->key == '!')
					{
						checker->key = 'G';
						memcpy(buffer_store[j], checker, sizeof(PACKET));
					}
				}
				isStart = false;
			}
			if (isShoot)
			{
				for (j = 0;j < client_list.size();j++)
				{
					memcpy(checker, buffer_store[j], sizeof(PACKET));
					if (checker->key == '?')
					{
						checker->key = 'H';
						memcpy(buffer_store[j], checker, sizeof(PACKET));
					}
				}
				isShoot = false;
			}
		}
	}
	return 0;
}

void Server::Init()
{
	WSADATA wsa_data;
	SOCKET serv_sock, clnt_sock;
	SOCKADDR_IN serv_addr, clnt_addr;
	int sz_clnt_addr;
	OBSTACLE obstacle;

	obstacle.num = NUM_OBSTACLE;
	memcpy(obstacle.pos, &coord[2], sizeof(struct position) * NUM_OBSTACLE);
	memcpy(obstacle.type, obstacleType, sizeof(int) * NUM_OBSTACLE);
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
		ErrorHandling("WSAStartup() error!");

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == INVALID_SOCKET)
		ErrorHandling("socket() error!");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVERPORT);

	if (bind(serv_sock, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		ErrorHandling("bind() Error!");

	if (listen(serv_sock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling("listen() Error!");

	cout << "Server initialized" << endl;

	sz_clnt_addr = sizeof(clnt_addr);
	while (1)
	{
		THREAD_PARAMETERS *parameter = (THREAD_PARAMETERS *)malloc(sizeof(THREAD_PARAMETERS));
		clnt_sock = accept(serv_sock, (SOCKADDR*)&clnt_addr, &sz_clnt_addr);
		if (clnt_sock == INVALID_SOCKET)
			ErrorHandling("accept() Error!");
		else
			cout << "New client connected, Client socket value:" << clnt_sock << endl;
		if (send(clnt_sock, (char*)&obstacle, sizeof(OBSTACLE), 0) == SOCKET_ERROR)
		{
			cout << WSAGetLastError() << endl;
			ErrorHandling("send() ERROR! in Init");
		}
		//parameter->sample = (SAMPLE*)malloc(sizeof(SAMPLE));
		parameter->client_socket = clnt_sock;
		cout << "Before pushback Socket value:" << clnt_sock << endl;
		client_list.push_back(parameter);
		class_param = parameter;
		CreateThread(NULL, 0, startMethodInThread, this, 0, NULL);
	}
	closesocket(serv_sock);
	WSACleanup();

}

THREAD_PARAMETERS * Server::getClassParam()
{
	return class_param;
}

void Server::randGen() {
	struct position r;
	int num = 0;
	
	while (1) {
		bool check_duplication =false ;
		r = { rand() % 16 * PLAYER_SIZE + PLAYER_SIZE / 2, rand() % 9 * PLAYER_SIZE + PLAYER_SIZE / 2 };
		for (int i = 0; i < num; i++) {
			if (r.x == coord[i].x && r.y == coord[i].y)
				check_duplication = true;
		}
		if (check_duplication)
			continue;
		coord[num] = r;
		
		if (num < NUM_OBSTACLE)
			obstacleType[num] = rand() % 3;
		
		num++;
		if (num == NUM_OBSTACLE + 2)
			break;
	}
}

