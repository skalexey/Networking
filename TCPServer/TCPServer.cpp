// TCPServer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <tcp/server.h>
#include "TCPServer.h"

int main()
{
	std::cout << "TCPServer" << std::endl;

	anp::tcp::server s;
	s.set_on_receive([](auto& data, auto size, auto id) {
		std::cout << "Received from " << id << ":" << std::endl;
		for (int i = 0; i < size; i++)
			std::cout << data[i];
		std::cout << std::endl;
		return true;
	});
	int port = 123;
	s.start(port);
	// Setup the input logic
	try
	{
		std::string msg;
		while (true)
		{
			std::cout << " > ";
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
			else if (msg == "send")
			{
				std::cout << "Message: ";
				std::cin >> msg;
				s.send(msg);
				std::cout << "Sent '" << msg << "'\n";
			}
			else if (msg == "sf")
			{
				std::cout << "Send file input form...\n";
				std::string m = 
					"<!DOCTYPE html>"
					"<html lang=\"en\">"
					"<head>"
					"<meta charset=\"utf-8\"/>"
					"<title>upload</title>"
					"</head>"
					"<body>"
					"<form action=\"http://127.0.0.1:8080\" method=\"post\" enctype=\"multipart/form-data\">"
					"<p><input type=\"text\" name=\"text1\" value=\"text default\">"
					"<p><input type=\"text\" name=\"text2\" value=\"a&#x03C9;b\">"
					"<p><input type=\"file\" name=\"file1\">"
					"<p><input type=\"file\" name=\"file2\">"
					"<p><input type=\"file\" name=\"file3\">"
					"<p><button type=\"submit\">Submit</button>"
					"</form>"
					"</body>"
					"</html>"
				;
				s.send(m);
				std::cout << "Sent: \n'" << m << "'\n";
			}
		}
	}
	catch (...)
	{
		std::cout << "Exception caught\n";
	}
	return 0;
}
