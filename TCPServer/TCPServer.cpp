// TCPServer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <tcp/server.h>
#include "TCPServer.h"

int main()
{
	std::cout << "TCPServer" << std::endl;

	anp::tcp::server s(123);
	s.set_on_receive([&](auto& data, auto size, auto id) {
		std::cout << "Received from " << id << ":" << std::endl;
		for (int i = 0; i < size; i++)
			std::cout << data[i];
		std::cout << std::endl;
	});
	s.start();
	// Setup the input logic
	try
	{
		std::string msg;
		while (true)
		{
			std::cin >> msg;
			if (msg == "start")
				s.start();
			else if (msg == "stop")
				s.stop();
			else if (msg == "exit")
				break;
		}
	}
	catch (...)
	{
		std::cout << "Exception catched\n";
	}
	return 0;
}
