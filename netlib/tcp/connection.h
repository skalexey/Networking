// connection.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <memory>
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

namespace anp
{
	namespace tcp
	{
		class connection
		{
			friend class server;
		public:
			connection(asio::io_context& ctx);
			connection(asio::io_context& ctx, asio::ip::tcp::socket socket, int id);
			inline bool is_connected() const { return m_soc.is_open(); };
			void connect(const asio::ip::tcp::resolver::results_type& ep, const asio_operation_cb& on_result = nullptr);
			void close();
			void send(const std::string& msg);
			void set_on_receive(const on_client_data_cb& cb);
			inline int get_id() { return m_id; }

		private:
			void ReadAsync();

		private:
			asio::io_context& m_ctx;
			asio::ip::tcp::socket m_soc;
			on_client_data_cb m_on_receive;
			int m_id = -1;
		};
		typedef std::shared_ptr<connection> connection_ptr;
	}
}
