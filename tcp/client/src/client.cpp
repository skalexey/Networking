// client.cpp : Defines the entry point for the application.
//

#include "client.h"

using namespace std;
namespace anp
{
	namespace tcp
	{
		bool client::connect(const std::string& host, int port)
		{
			return false;
		}

		void client::disconnect()
		{
		}

		size_t client::send(const std::string& msg)
		{
			return size_t();
		}

		void client::set_on_receive(const on_data_cb& cb)
		{
		}
	}
}
