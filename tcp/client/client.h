// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <cstddef>
#include <string>
#include <functional>
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include "common.h"

namespace anp
{
	namespace tcp
	{
		class client
		{
		public:
			bool connect(const std::string& host, int port);
			void disconnect();
			size_t send(const std::string& msg);
			void set_on_receive(const on_data_cb& cb);

		private:
			on_data_cb mOnReceive;
		};
	}
}
