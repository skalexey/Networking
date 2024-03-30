// client_base.cpp : Defines the entry point for the application.
//

#include <cassert>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <chrono>
#include <asio/ts/buffer.hpp>
#include "tcp/client_base.h"
#include <utils/log.h>
#include <utils/profiler.h>
SET_LOCAL_LOG_LEVEL(verbose)
LOG_TITLE("tcp/client_base")

using namespace std::chrono_literals;

namespace anp
{
	namespace tcp
	{
		using namespace boost;
		struct error_category_user : public std::error_category
		{
			const char* name() const noexcept override {
				return "user";
			}
			std::string message(int _Errval) const override {
				return "This is a error category for every non-standard error represented as std::error_code";
			}
		};

		static error_category_user error_category_user_instance;

		enum erc : int {
			no_error = 0
			, undefined_exception
		};

		struct error_code_exception : public std::error_code
		{
			error_code_exception(const std::exception& ex) 
				: std::error_code(erc::undefined_exception, error_category_user_instance)
				, m_ex(ex) {}
			const std::exception& get_exception() const { return m_ex; }
			operator bool() const { return true; }
			
		private:
			std::exception m_ex;
		};

		client_base::~client_base()
		{
			LOG_DEBUG("client_base::~client_base");
			if (is_connected())
				disconnect();
			else
				LOG_DEBUG("Already disconnected");
			free_resources();
			assert(!m_thr_ctx.joinable() && "The thread should have been terminated in disconnect()");
		}

		bool client_base::connect(const std::string& host, int port, const anp::error_cb& on_connect)
		{
			if (is_connected())
				disconnect();
			LOCAL_VERBOSE("Connect to '" << host << ":" << port << " ...");
			LOCAL_VERBOSE("	Create resources");
			asio::error_code ec;
			m_ctx = std::make_unique<asio::io_context>();
			m_idle_work = std::make_unique<asio::io_context::work>(*m_ctx);
#ifdef __cpp_lib_jthread
			m_thr_ctx = std::jthread([&] {
#else
			m_thr_ctx = std::thread([&] {
#endif
				try
				{
					m_ctx->run();
				}
				catch (std::exception& ex)
				{
					LOG_ERROR("Exception in context thread caught: '" << ex.what() << "'");
				}
				catch (...)
				{
					LOG_ERROR("Exception in context thread caught");
				}
				LOCAL_VERBOSE("Context thread finished");
			});
			m_ctx_thread_id = m_thr_ctx.get_id();
			try
			{
				m_connection = make_connection();
				if (m_on_receive)
					m_connection->subscribe_on_receive(this, m_on_receive);
				if (m_on_connect)
					m_connection->add_on_connect(m_on_connect);
				m_connection->set_on_close(std::bind(&client_base::on_connection_close, this));
				LOCAL_VERBOSE("	Connect the socket");
				m_connection->connect({host, port}, [self = this, on_connect](const std::error_code& ec) {
					if (ec)
					{
						LOCAL_VERBOSE("Error during connection");
						self->disconnect();
					}
					else
					{
						if (on_connect)
							on_connect(ec);
					}
				});
			}
			catch (std::exception& e)
			{
				LOCAL_WARNING("Can't make endpoint from the given address. Error msg: '" << e.what() << "'");
				m_on_connect(error_code_exception(e));
				return false;
			}
			return true;
		}

		void client_base::on_connection_close()
		{
			if (m_on_close)
			{
				LOCAL_VERBOSE(" Call user callback");
				m_on_close();
			}

		}

		// Should be called from the thread that called connect()
		void client_base::disconnect()
		{
			PROFILE_TIME("client_base::disconnect()");

			if (is_connected())
			{
				LOG_VERBOSE("this_thread_id: " << std::this_thread::get_id() << ", m_ctx_thread_id: " << m_ctx_thread_id);
				auto job = [self = this]()
				{
					self->m_connection->close();
					LOCAL_VERBOSE(" Stop the context");
					self->m_ctx->stop();
					LOCAL_VERBOSE("		Context stopped");
				};
				if (std::this_thread::get_id() != m_ctx_thread_id)
					asio::post(*m_ctx, job);
				else
					job();

				free_resources();
			}
			else
			{
				LOCAL_VERBOSE("Disconnect called while already disconnected");
			}
		}

		void client_base::free_resources()
		{
			try
			{
				if (m_thr_ctx.joinable())
					if (std::this_thread::get_id() == m_ctx_thread_id)
						m_thr_ctx.detach();
					else // Wait for the job to complete
					{
						LOG_VERBOSE("Wait for the context thread to finish");
						m_thr_ctx.join();
						LOG_VERBOSE("Joined the context thread");
					}
			}
			catch (std::system_error& e)
			{
				LOG_ERROR("Error while freeing resources: '" << e.what() << "'");
			}

			assert(!is_connected() && "It should have been disconnected from the job");

			if (m_idle_work)
			{
				LOG_VERBOSE("Reset the idle work");
				m_idle_work.release();
			}
			if (m_connection)
			{
				LOG_VERBOSE("Reset the connection");
				m_connection.release();
				LOG_VERBOSE("Connection has been reset");
			}
			
			if (m_ctx)
			{
				if (std::this_thread::get_id() != m_ctx_thread_id)
					LOG_VERBOSE("Release the context");
				else
					LOG_VERBOSE("Release the context from its thread");
				m_ctx.release();
			}
			LOCAL_VERBOSE("	Resources destroyed");
		}

		bool client_base::is_connected() const
		{
			if (m_connection)
				return m_connection->is_connected();
			return false;
		}

		void client_base::send(const anp::sock_data_t& msg, const response_cb& on_response)
		{
			LOCAL_DEBUG("Send data: \n'" << (const char*)msg.data() << "'\n\n");

			if (!m_connection)
			{
				LOCAL_WARNING("send called while disconnected");
				return;
			}
			m_connection->send(msg, on_response);
		}

		void client_base::set_on_receive(const data_cb& cb)
		{
			m_on_receive = cb;
			if (m_connection) {
				m_connection->subscribe_on_receive(this, m_on_receive);
			}
		}

		void client_base::add_on_connect(const error_cb& cb)
		{
			m_on_connect = cb;
			if (m_connection)
				m_connection->add_on_connect(m_on_connect);
		}

		void client_base::set_on_close(const utils::void_cb& cb)
		{
			m_on_close = cb;
		}
	}
}
