#include <iostream>
#include <http/http_client.h>
#include <utils/string_utils.h>
#include "HTTPClient.h"
#include <utils/Log.h>
LOG_POSTFIX("\n");
LOG_PREFIX("[HTTPClient]: ");

int main()
{
	LOG("Launch");
	anp::http_client c;
	// Connect
	std::string host = "srv.vllibrary.net";
	int port = 443;
	anp::tcp::endpoint_t ep{ host, port };

	// Setup the input logic
	try
	{
		std::string msg;// = "ssltest";
		while (true)
		{
			std::cout << " > ";
			if (msg.empty())
				std::cin >> msg;
			if (msg == "endpoint")
			{
				std::cout << "\n" << "Host: ";
				std::cin >> host;
				std::cout << "\nPort: ";
				std::cin >> port;
				std::cout << "\n";
				ep = {host, port};
			}
			else if (msg == "http")
				ep = {"google.com", 80};
			else if (msg == "get")
			{
				anp::query_t q;
				q.method = "GET";
				c.query(ep, q);
			}
			else if (msg == "exit")
				break;
			else if (msg == "ssl")
			{
				ep = {"srv.vllibrary.net", 443};
			}
			else if (msg == "ssltest")
			{
				ep = {"srv.vllibrary.net", 443};
				anp::query_t q;
				q.method = "GET";
				c.query(ep, q);
			}
			msg.clear();
		}
	}
	catch (...)
	{
		LOG("Exception caught\n");
	}

	return 0;
}
