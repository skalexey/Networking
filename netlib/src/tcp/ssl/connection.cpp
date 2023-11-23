#include <tcp/ssl/connection.h>
#include <asio/ssl/rfc2818_verification.hpp>
#include <utils/Log.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>

SET_LOG_VERBOSE(true)
LOG_TITLE("ssl/connection")
LOG_PREFIX("[ssl/connection]: ");
LOG_POSTFIX("\n");

namespace anp
{
	namespace tcp
	{
		namespace ssl
		{
			ssl::connection::connection(asio::io_context& io_ctx)
				: base(io_ctx)
				, m_ssl_ctx(asio::ssl::context::sslv23)
				, m_socket(io_ctx, m_ssl_ctx)
			{
				init();
			}

			ssl::connection::connection(asio::io_context& io_ctx, asio::ip::tcp::socket soc, int id)
				: base(io_ctx, std::move(soc), id)
				, m_ssl_ctx(asio::ssl::context::sslv23)
				, m_socket(std::move(soc), m_ssl_ctx)
			{
				init();
			}

			void ssl::connection::on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep, const tcp::endpoint_t& ep_high)
			{
				LOG_DEBUG("on_connect(" << ec << ", " << ep.address().to_string() << ", " << ep.port() << ", " << ep_high.host << ")");
				if (!ec)
				{
					LOCAL_VERBOSE("Connected!");
					boost::certify::set_server_hostname(m_socket.ssl_soc(), ep_high.host);
					boost::certify::sni_hostname(m_socket.ssl_soc(), ep_high.host);
					m_socket.ssl_soc().async_handshake(asio::ssl::stream_base::handshake_type::client, [self = this, ep, ep_high](const asio::error_code& ec) {
						if (!ec)
					 	{
					 		LOCAL_VERBOSE("Handshaked!");
					 		self->base::on_connect(ec, ep, ep_high);
					 	}
					 	else
					 	{
					 		LOCAL_VERBOSE("Handshake failed with code " << ec << ", message: " << ec.message());
					 	}
					 });
				}
				else
				{
					LOCAL_VERBOSE("Failed to connect: " << ec.message());
				}
			}

			void connection::init()
			{
				boost::certify::enable_native_https_server_verification(m_ssl_ctx);
			}
		}
	}
}
