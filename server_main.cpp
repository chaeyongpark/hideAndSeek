#include "server.h"

int main()
{
	Server* server = new Server();
	server->randGen();
	server->Init();
}