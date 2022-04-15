// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <cstddef>
#include <memory>
#include <thread>
#include <string>
#ifndef _WIN32_WINNT
	#ifdef _WIN32
		#define _WIN32_WINNT 0x0A00
	#endif
#endif
#ifndef ASIO_STANDALONE
	#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include "common.h"
#include <tcp/connection.h>

namespace anp
{
	namespace tcp
	{
		class client
		{
		public:
			client() = default;
			~client();
			// All operations are asynchronouse
			bool connect(const std::string& host, int port);
			// disconnect() Should be called from the thread that called connect()
			void disconnect();
			inline bool is_connected() { return !!m_connection; }
			void send(const std::string& msg);
			void set_on_receive(const on_client_data_cb& cb);

		private:
			std::unique_ptr<asio::io_context> m_ctx;
			std::jthread m_thr_ctx;
			std::unique_ptr<asio::io_context::work> m_idle_work;
			std::unique_ptr<anp::tcp::connection> m_connection;
			on_client_data_cb m_on_receive;
		};
	}
}
