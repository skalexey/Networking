// client_base.cpp : Defines the entry point for the application.
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include <chrono>
#include <asio/ts/buffer.hpp>
#include "tcp/client_base.h"
#include <utils/Log.h>
#include <utils/profiler.h>
SET_LOG_VERBOSE(true)
SET_LOG_DEBUG(true)
LOG_TITLE("tcp_client_base")
LOG_PREFIX("[client_base]: ");
LOG_POSTFIX("\n");
using namespace std::chrono_literals;

namespace anp
{
	namespace tcp
	{
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
				m_connection->close();
			assert(std::this_thread::get_id() != m_ctx_thread_id);
			if (m_thr_ctx.joinable())
				m_thr_ctx.join();
			m_ctx.reset();
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
					std::cout << "Exception in context thread cathced: '" << ex.what() << "'\n";
				}
				catch (...)
				{
					std::cout << "Exception in context thread cathced\n";
				}
			});
			m_ctx_thread_id = m_thr_ctx.get_id();
			try
			{
				asio::ip::tcp::resolver resolver(*m_ctx);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
				m_connection = make_connection();
				m_connection->set_on_receive(m_on_receive);
				m_connection->add_on_connect(m_on_connect);
				m_connection->set_on_close(std::bind(&client_base::on_connection_close, this));
				LOCAL_VERBOSE("	Connect the socket");
				m_connection->connect(endpoints, [self = this, on_connect](const std::error_code& ec) {
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
				if (std::this_thread::get_id() != m_ctx_thread_id)
					asio::post(*m_ctx, [self = this] {
						self->m_connection->close();
					});
				else
					m_connection->close();

				LOCAL_VERBOSE("	Stop the context");
				m_ctx->stop();
				LOCAL_VERBOSE("		Context stopped");
				try
				{
					if (std::this_thread::get_id() != m_ctx_thread_id)
					{
						if (m_thr_ctx.joinable())
						{
							LOCAL_VERBOSE("		Join the thread...");
							m_thr_ctx.join();
							LOCAL_VERBOSE("		Thread is joined");
						}
						else
						{
							LOG_VERBOSE("Thread is not joinable");
						}
					}
					else
					{
						LOG_VERBOSE("Won't join the thread cause we are already in that thread");
					}
				}
				catch (std::system_error& e)
				{
					LOG_ERROR("Error while disconnecting: " << e.what());
				}

				m_idle_work.reset();
				m_connection.reset(nullptr);
				// TODO: check if everything is ok in the ELSE case of this IF
				if (std::this_thread::get_id() != m_ctx_thread_id)
				{
					LOG_VERBOSE("Reset the context");
					m_ctx.reset();
				}
				else
				{
					LOG_VERBOSE("Won't reset the context in this thread");
				}
				LOCAL_VERBOSE("	Resources destroyed");

			}
			else
			{
				LOCAL_VERBOSE("Disconnect called while already disconnected");
			}
		}

		void client_base::send(const std::string& msg)
		{
			LOCAL_DEBUG("Send data: \n'" << msg << "'\n\n");

			if (!m_connection)
			{
				LOCAL_WARNING("send called while disconnected");
				return;
			}
			m_connection->send(msg);
		}

		void client_base::set_on_receive(const data_cb& cb)
		{
			m_on_receive = cb;
			if (m_connection)
				m_connection->set_on_receive(m_on_receive);
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
