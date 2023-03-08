// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <cstddef>
#include <memory>
#include <thread>
#include <string>
#include <common/asio_predefs.h>
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <common/common.h>
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
			bool is_connected() { return !!m_connection; }
			void send(const std::string& msg);
			void set_on_receive(const data_cb& cb);
			void set_on_connect(const error_cb& cb);
			void set_on_close(const utils::void_cb& cb);

		private:
			void on_connection_close();

		private:
			std::unique_ptr<asio::io_context> m_ctx;
		#ifdef __cpp_lib_jthread
			std::jthread m_thr_ctx;
		#else
			std::thread m_thr_ctx;
		#endif
			std::unique_ptr<asio::io_context::work> m_idle_work;
			std::unique_ptr<anp::tcp::connection> m_connection;
			data_cb m_on_receive;
			error_cb m_on_connect;
			utils::void_cb m_on_close;
			std::thread::id m_ctx_thread_id;
		};
	}
}
