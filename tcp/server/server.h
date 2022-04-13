// server.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <cstddef>
#include <string>
#include <functional>
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include "common.h"

namespace anp
{
	namespace tcp
	{
		class server
		{
		public:
			server(int port);
			void set_on_receive(const on_data_cb& cb);

		private:
			on_data_cb m_on_receive;
		};
	}
}
