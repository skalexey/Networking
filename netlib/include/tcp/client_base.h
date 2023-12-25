#pragma once

#include <cstddef>
#include <memory>
#include <thread>
#include <string>
#include <anp/common/asio_predefs.h>
#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <anp/common.h>
#include <tcp/connection_base.h>

namespace anp
{
	namespace tcp
	{
		class client_base
		{
		public:
			~client_base();
			// All operations are asynchronouse
			bool connect(const std::string& host, int port, const anp::error_cb& cb = nullptr);
			// disconnect() Should be called from the thread that called connect()
			void disconnect();
			bool is_connected() const;
			void send(const anp::sock_data_t& msg, const response_cb& on_response = nullptr);
			void set_on_receive(const data_cb& cb);
			void add_on_connect(const error_cb& cb);
			void set_on_close(const utils::void_cb& cb);

		protected:
			virtual std::unique_ptr<anp::tcp::connection_base> make_connection() = 0;

			const asio::io_context& get_io_context() const { return *m_ctx; }
			asio::io_context& io_context() { return *m_ctx; }

		private:
			void on_connection_close();

		private:
			std::unique_ptr<asio::io_context> m_ctx = nullptr;
		#ifdef __cpp_lib_jthread
			std::jthread m_thr_ctx;
		#else
			std::thread m_thr_ctx;
		#endif
			std::unique_ptr<asio::io_context::work> m_idle_work = nullptr;
			std::unique_ptr<anp::tcp::connection_base> m_connection = nullptr;
			data_cb m_on_receive;
			error_cb m_on_connect;
			utils::void_cb m_on_close;
			std::thread::id m_ctx_thread_id;
		};
	}
}
