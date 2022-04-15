// server.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vector>
#include <thread>
#include <common/asio_predefs.h>
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <common/common.h>
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
			void set_on_receive(const on_client_data_cb& cb);

		protected:
			void WaitClientConnection();
			virtual bool OnClientConnect(const connection_ptr& connection);

		private:
			std::unique_ptr<asio::io_context> m_ctx;
			std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;
			std::thread m_thr_ctx;
			on_client_data_cb m_on_receive;
			std::vector<connection_ptr> m_connecions;
			int m_conn_id = 0;
		};
	}
}
