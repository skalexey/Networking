// server.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <unordered_map>
#include <thread>
#include <anp/common/asio_predefs.h>
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <anp/common.h>
#include <tcp/connection.h>

namespace anp
{
	namespace tcp
	{
		class server
		{
		public:
			server();
			bool is_active() const;
			bool start(int port);
			void stop();
			void set_on_receive(const data_cb& cb);
			bool send(const anp::sock_data_t& msg, int conn_id = -1);

		protected:
			void WaitClientConnection();
			virtual bool on_client_connect(const connection_ptr& connection);

		private:
			std::unique_ptr<asio::io_context> m_ctx;
			std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;
			std::thread m_thr_ctx;
			data_cb m_on_receive;
			std::unordered_map<int, connection_ptr> m_connections;
			int m_conn_id = 0;
		};
	}
}
