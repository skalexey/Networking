// TCPClient.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cstdlib>
#include <tcp/client.h>
#include <tcp/ssl/client.h>
#include <utils/string_utils.h>
#include "TCPClient.h"
#include <utils/log.h>
LOG_TITLE("TCPClient");

void init_client(anp::tcp::client_base& client)
{
	client.set_on_receive([](auto& data, auto size, auto id) {
		std::cout << "Data received: " << std::endl;
		for (int i = 0; i < size; i++)
			std::cout << data[i];
		std::cout << std::endl;
		return true;
	});
}

int main()
{
	LOG("Launch");
	// Create a client
	std::unique_ptr<anp::tcp::client_base> c = std::make_unique<anp::tcp::client>();

	// Set what to do on data received
	init_client(*c);

	// Connect
	std::string host = "127.0.0.1";
	int port = 123;
	c->connect(host, port);

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
				c->connect(host, port);
			}
			else if (msg == "reconnect")
				c->connect(host, port);
			else if (msg == "disconnect")
				c->disconnect();
			else if (msg == "http")
				c->send("GET /vl/ HTTP/1.1\r\n"
					"Host: 127.0.0.1\r\n"
					"Connection: close\r\n\r\n");
			else if (msg == "get")
			{
				if (c->is_connected())
					c->disconnect();
				std::cout << "\n" << "Host: ";
				std::cin >> host;
				std::cout << "\n" << "Port: ";
				std::cin >> port;
				std::cout << "\n";
				std::cout << "\n" << "Remote path: ";
				std::string path;
				std::cin >> path;
				std::cout << "\n";
				c->connect(host, port, [&c, path, host](const std::error_code& ec) {
					LOG("Connected");
					LOG("Send request");
					c->send(SSTREAM("GET " << path << " HTTP/1.1\r\n"
						"Host: " << host << "\r\n"
						"Connection: close\r\n\r\n")
					);
				});
			}
			else if (msg == "exit")
				break;
			else if (msg == "send")
			{
				std::cout << "Message: ";
				std::cin >> msg;
				c->send(msg);
				LOG("Sent '" << msg << "'");
			}
			else if (msg == "ssl")
			{
				c = std::make_unique<anp::tcp::ssl::client>();
				init_client(*c);
			}
			else if (msg == "ssltest")
			{
				c = std::make_unique<anp::tcp::ssl::client>();
				init_client(*c);
				c->connect("srv.vllibrary.net", 443, [&c](const std::error_code& ec) {
					std::cout << "Connected\n";
					std::cout << "Send request\n";
					c->send(SSTREAM("GET /v/a.php?u=skalexey&t=33 HTTP/1.1\r\n"
						"Host: srv.vllibrary.net\r\n"
						"Connection: close\r\n\r\n")
						, [&c](const std::vector<char>& response) {
							LOG("Response received");
						}
					);
				});
			}
		}
	}
	catch (...)
	{
		LOG("Exception caught");
	}

	return 0;
}
