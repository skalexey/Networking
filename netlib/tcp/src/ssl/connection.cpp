#include <tcp/ssl/connection.h>
#include <asio/ssl/rfc2818_verification.hpp>
#include <utils/Log.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>
#include <tcp/ssl/root_certificates.hpp>

//#include <utils/envar.h>

SET_LOG_VERBOSE(true)
LOG_TITLE("ssl/connection")
LOG_PREFIX("[ssl/connection]: ");
LOG_POSTFIX("\n");

namespace
{
	std::vector<char> buf(20 * 1024);
}

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

			void ssl::connection::on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep)
			{
				LOG_DEBUG("on_connect(" << ec << ", " << ep.address().to_string() << ", " << ep.port());
				if (!ec)
				{
					LOCAL_VERBOSE("Connected!");
					//socket().set_option(asio::ip::tcp::no_delay(true));
					//m_socket.set_verify_callback(asio::ssl::rfc2818_verification("vllibrary.net"));
					//detail::load_root_certificates(m_ssl_ctx, ec);
					//boost::certify::enable_native_https_server_verification(m_ssl_ctx);
					if (!SSL_set_tlsext_host_name(m_socket.ssl_soc().native_handle(), "srv.vllibrary.net"))
					{
						throw asio::system_error(
							::ERR_get_error(), asio::error::get_ssl_category());
					}
					//m_socket.handshake(asio::ssl::stream<asio::ip::tcp::socket>::client);
					m_socket.ssl_soc().async_handshake(asio::ssl::stream_base::handshake_type::client, [self = this, ep](const asio::error_code& ec) {
						if (!ec)
					 	{
					 		LOCAL_VERBOSE("Handshaked!");
					 		self->base::on_connect(ec, ep);
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
				//auto env = X509_get_default_cert_dir_env();
				//const char* dir = getenv(env);
				//int setenv_result = setenv(env, "C:\\Program Files\\Common Files\\SSL\\certs");
				//if (setenv_result != 0)
				//{
				//	LOG_ERROR("setenv(SSL_CERT_DIR, ...) error: " << setenv_result);
				//}
				
				//if (!dir)
					//dir = X509_get_default_cert_dir();

				//m_ssl_ctx.set_default_verify_paths();
				//m_ssl_ctx.load_verify_file("C:\\Users\\skoro\\certs\\ca3.pem");
				//m_ssl_ctx.set_verify_mode(asio::ssl::verify_peer);
				
				m_socket.ssl_soc().set_verify_mode(asio::ssl::verify_peer);
				//m_ssl_ctx.set_options(asio::ssl::context::single_dh_use);
				//m_ssl_ctx.set_options(asio::ssl::context::default_workarounds);
				//m_ssl_ctx.set_options(asio::ssl::context::no_sslv2);
				//m_ssl_ctx.set_options(asio::ssl::context::no_sslv3);
				//m_ssl_ctx.set_options(asio::ssl::context::tlsv12);
				m_socket.ssl_soc().set_verify_callback([](bool preverified, asio::ssl::verify_context& ctx) {
					// The verify callback can be used to check whether the certificate that is
					// being presented is valid for the peer. For example, RFC 2818 describes
					// the steps involved in doing this for HTTPS. Consult the OpenSSL
					// documentation for more details. Note that the callback is called once
					// for each certificate in the certificate chain, starting from the root
					// certificate authority.
					// In this example we will simply print the certificate's subject name.
					char subject_name[256];
					X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
					X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
					std::cout << "Verifying " << subject_name << "\n";
					return true;
				});
			}
		}
	}
}
