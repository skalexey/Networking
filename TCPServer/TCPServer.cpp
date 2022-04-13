// TCPServer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <tcp/server/server.h>
#include "TCPServer.h"

using namespace std;

int main()
{
	cout << "TCPServer" << endl;

	anp::tcp::server s(123);
	s.set_on_receive([&](auto& data) {
		cout << "Data received: " << data << std::endl;
	});

	return 0;
}
