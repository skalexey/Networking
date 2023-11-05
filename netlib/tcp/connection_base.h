// connection_base.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vector>
#include <atomic>
#include <memory>
#include <common/asio_predefs.h>
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <common/common.h>

namespace anp
{
	namespace tcp
	{
		class connection_base
		{
			friend class server;
		public:
			~connection_base();
			connection_base(asio::io_context& ctx);
			connection_base(asio::io_context& ctx, asio::ip::tcp::socket socket, int id);
			bool is_connected() const { return m_soc.is_open(); };
			void connect(const asio::ip::tcp::resolver::results_type& ep, const asio_operation_cb& on_result = nullptr);
			void close();
			void send(const std::string& msg);
			void set_on_receive(const data_cb& cb);
			void add_on_connect(const error_cb& cb);
			void set_on_close(const utils::void_cb& cb);
			int get_id() { return m_id; }
			asio::ip::tcp::socket& socket() { return m_soc; }
			const asio::ip::tcp::socket& get_socket() const { return m_soc; }

		protected:
			virtual void on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep);

		private:
			void read_async();

		private:
			asio::io_context& m_ctx;
			asio::ip::tcp::socket m_soc;
			data_cb m_on_receive;
			std::vector<error_cb> m_on_connect;
			utils::void_cb m_on_close;
			int m_id = -1;
			std::atomic<bool> m_wait_read = false;
		};
		typedef std::shared_ptr<connection_base> connection_base_ptr;
	}
}
