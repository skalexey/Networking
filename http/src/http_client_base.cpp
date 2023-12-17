// http_client_base.cpp

#include <exception>
#include <string_view>
#include <map>
#include <string>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <utils/profiler.h>
#include <utils/file_utils.h>
#include <tcp/client.h>
#include <tcp/ssl/client.h>
#include <http/http_client.h>

LOG_PREFIX("[http_client_base]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);

namespace anp
{
	http_client_base::http_client_base()
		: m_cv_ul(m_cv_mtx)
	{
	}

	// Define destructor in cpp because of the incompleteness of tcp/client type in the header
	http_client_base::~http_client_base()
	{
		LOG_DEBUG("http_client_base::~http_client_base");
	}

	int http_client_base::request(
		const tcp::endpoint_t& endpoint
		, const std::string& request
		, const http_response_cb& on_receive
	)
	{
		request_async(endpoint, request, on_receive);
		wait();
		return m_error_code;
	}

	void http_client_base::request_async(
		const tcp::endpoint_t& endpoint
		, const std::string& request
		, const http_response_cb& on_response
	)
	{
		auto type = endpoint.port == 443 ? client_type::https : client_type::http;
		reset(type);

		LOG_VERBOSE("request: " << request);

		m_client->set_on_close([&] {
			notify(m_error_code);
		});

		m_client->add_on_connect([self = this, request](const std::error_code& ec) {
			if (!ec)
			{
				LOG_DEBUG("Send...");
				self->m_client->send(request);
			}
			else
			{
				LOG_ERROR("Error during connection: " << ec);
				self->notify(connection_process_error);
			}
		});

		set_on_response(on_response);

		if (m_client->connect(endpoint.host, endpoint.port))
		{
			LOG_DEBUG("Set on_receive task");
			m_client->set_on_receive([=, self = this](
				const std::vector<http_data_t>& data
				, std::size_t sz
				, int id
			) -> bool {
				LOG_DEBUG("\nReceived packet " << id << " with " << sz << " bytes:");
				// C++17 string_view constructor
				std::string_view s(&*data.begin(), sz);
				LOG_DEBUG(s);

				if (m_on_response) // Pass control to the user callback
				{
					switch (m_receive_mode)
					{
					case http_client_interface::receive_mode::memory_tcp_packet:
							PROFILE_TIME(return m_on_response(get_headers(), data.data(), sz, get_status()));
							break;
						default:
							break;
					}
				}

				return self->on_packet_receive(data, sz, id);
			});
		}
		else
		{
			m_error_code = erc::connection_error;
		}
	}

	void http_client_base::wait() {
		LOG_DEBUG("Wait for end of response...");
		m_cv.wait(m_cv_ul);
		LOG_DEBUG("Response received.");
		if (m_client->is_connected())
			m_client->disconnect();
	}

	int http_client_base::notify(int ec)
	{
		on_before_notify(ec);
		m_error_code = ec;
		m_cv.notify_one();
		on_notify(ec);
		return m_error_code;
	}

	void http_client_base::set_receive_file(const fs::path &file_path)
	{
		m_receive_mode = receive_mode::file;
		m_file_path = file_path;
	}

	bool http_client_base::remove_received_file()
	{
		if (!m_file_path.empty())
			return utils::file::remove(m_file_path);
		return false;
	}

	void http_client_base::reset(client_type c)
	{
		LOG_DEBUG("http_client_base::reset()");
		m_error_code = erc::unknown;
		switch (c)
		{
			case client_type::http:
				m_client = std::make_unique<tcp::client>();
				break;
			case client_type::https:
				m_client = std::make_unique<tcp::ssl::client>();
				break;
			default:
				throw std::exception("Unknown client type");
		}
		// TODO: clean this too
		//m_file_path.clear();
		//receive_mode m_receive_mode = receive_mode::memory_full_payload;
		on_reset();
	}
}