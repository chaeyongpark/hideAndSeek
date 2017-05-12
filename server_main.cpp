#include "Server.h"

int main()
{
	srand(time(NULL));
	Server* server = new Server();
	server->randGen();
	server->Init();
}