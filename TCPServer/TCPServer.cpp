// TCPServer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <tcp/server.h>
#include "TCPServer.h"

int main()
{
	std::cout << "TCPServer" << std::endl;

	anp::tcp::server s;
	s.set_on_receive([&](auto& data, auto size, auto id) {
		std::cout << "Received from " << id << ":" << std::endl;
		for (int i = 0; i < size; i++)
			std::cout << data[i];
		std::cout << std::endl;
	});
	int port = 123;
	s.start(port);
	// Setup the input logic
	try
	{
		std::string msg;
		while (true)
		{
			std::cin >> msg;
			if (msg == "start")
			{
				std::cout << "Port: ";
				std::cin >> port;
				std::cout << std::endl;
				s.start(port);
			}
			else if (msg == "restart")
			{
				if (s.is_active())
					s.stop();
				s.start(port);
			}
			else if (msg == "stop")
				s.stop();
			else if (msg == "exit")
			{
				s.stop();
				break;
			}
		}
	}
	catch (...)
	{
		std::cout << "Exception catched\n";
	}
	return 0;
}
