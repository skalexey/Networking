// TCPClient.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cstdlib>
#include <tcp/client.h>
#include "TCPClient.h"

int main()
{
	std::cout << "TCPClient" << std::endl;
	// Create a client
	anp::tcp::client c;

	// Set what to do on data received
	c.set_on_receive([](auto& data, auto size, auto id) {
		std::cout << "Data received: " << std::endl;
		for (int i = 0; i < size; i++)
			std::cout << data[i];
		std::cout << std::endl;
		return true;
	});

	// Connect
	std::string host = "127.0.0.1";
	int port = 123;
	c.connect(host, port);

	// Setup the input logic
	try
	{
		std::string msg;
		while (true)
		{
			std::cout << " > ";
			std::cin >> msg;
			if (msg == "connect")
			{
				std::cout << "\n" << "Host: ";
				std::cin >> host;
				std::cout << "\nPort: ";
				std::cin >> port;
				std::cout << "\n";
				c.connect(host, port);
			}
			else if (msg == "reconnect")
				c.connect(host, port);
			else if (msg == "disconnect")
				c.disconnect();
			else if (msg == "http")
				c.send("GET /vl/ HTTP/1.1\r\n"
					"Host: 127.0.0.1\r\n"
					"Connection: close\r\n\r\n");
			else if (msg == "exit")
				break;
			else if (msg == "send")
			{
				std::cout << "Message: ";
				std::cin >> msg;
				c.send(msg);
				std::cout << "Sent '" << msg << "'\n";
			}
		}
	}
	catch (...)
	{
		std::cout << "Exception catched\n";
	}

	return 0;
}
