#include <iostream>
#include <http/http_client.h>
#include <utils/string_utils.h>
#include <utils/networking/http.h>
#include "HTTPClient.h"
#include <DMBCore.h>
#include <utils/log.h>
LOG_TITLE("HTTPClient");

namespace
{
	anp::tcp::endpoint_t ep{ "google.com", 80 };

	dmb::Model cfg_model;
	fs::path cfg_path = "config.json";

	vl::Object& get_cfg_data()
	{
		if (!cfg_model.IsLoaded())
			if (!cfg_model.Load(cfg_path.string()))
			{
				auto& data = cfg_model.GetContent().GetData();
				data.Set("host", ep.host);
				data.Set("port", ep.port);
				data.Set("initial_command", "");
				LOG_WARNING("There is no config in " << cfg_path.string() << ". I will create it for you.");
				if (!cfg_model.Store(cfg_path.string(), { true }))
					LOG_ERROR("Can't create config file");
			}
		return cfg_model.GetContent().GetData();
	}
}

int main()
{
	LOG("Launch");
	auto& cfg = get_cfg_data();

	anp::http_client c;
	// Connect
	std::string host = cfg["host"].AsString().Val();
	int port = cfg["port"].AsNumber().Val();
	ep = {host, port};
	// Setup the input logic
	try
	{
		std::string msg = cfg["initial_command"].AsString().Val();
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
			else if (msg == "upload_file_test")
			{
				ep = {"srv.vllibrary.net", 80};
				utils::http::upload_file_with_auth<anp::http::url_uploader>("test2.txt", ep, "/v/h.php", "skalexey", "3834057684");
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
