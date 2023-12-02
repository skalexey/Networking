// http_client.cpp

#include <exception>
#include <string_view>
#include <map>
#include <string>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <utils/profiler.h>
#include <utils/data_receiver_file.h>
#include <utils/data_receiver_memory.h>
#include <http/chunked_data_receiver.h>
#include <tcp/client.h>
#include <tcp/ssl/client.h>
#include <http/http_client.h>

LOG_PREFIX("[http_client]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);

namespace
{
	const std::string empty_string;
}

namespace anp
{
	http_client::http_client()
		: m_cv_ul(m_cv_mtx)
	{
	}

	// Define destructor in cpp because of the incompleteness of tcp/client type in the header
	http_client::~http_client()
	{
		LOG_DEBUG("http_client::~http_client");
	}

	std::string http_client::parse_header(const std::string& response, const std::string& header)
	{
		std::string what = utils::format_str("%s: ", header.c_str());
		auto res = response.find(what);
		if (res != std::string::npos)
		{
			auto res2 = response.find("\r", res);
			if (res2 != std::string::npos)
			{
				auto s = response.substr(res + what.size(), res2 - res - what.size());
				LOG_DEBUG(header << ": " << s);
				return s;
			}
		}
		return "";
	}

	int http_client::request(
		const tcp::endpoint_t& endpoint
		, const std::string& request
		, const http_response_cb& on_receive
	)
	{
		request_async(endpoint, request, on_receive);
		wait();
		return m_error_code;
	}

	void http_client::request_async(
		const tcp::endpoint_t& endpoint
		, const std::string& request
		, const http_response_cb& on_receive
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


		if (m_client->connect(endpoint.host, endpoint.port))
		{
			LOG_DEBUG("Set on_receive task");
			m_client->set_on_receive([=](
				const std::vector<http_data_t>& data
				, std::size_t sz
				, int id
			) -> bool {
				LOG_DEBUG("\nReceived packet " << id << " with " << sz << " bytes:");
				// C++17 string_view constructor
				std::string_view s(&*data.begin(), sz);
				LOG_DEBUG(s);

				const http_data_t* payload = data.data();
				std::size_t payload_size = sz;

				if (m_headers_parser.error_code() <= headers_parser::in_progress)
				{
					int parse_result = m_headers_parser.parse(s);
					if (parse_result > 0)
					{
						LOG("Headers parsing error: " << parse_result << "");
						notify(erc::parse_headers_error);
						return false;
					}
					else if (parse_result == headers_parser::done)
					{
						payload = data.data() + m_headers_parser.cursor();
						payload_size = sz - m_headers_parser.cursor();
						LOG_DEBUG("HTTP response headers parsed. Status: " << m_headers_parser.status());
						switch (m_receive_mode)
						{
							case receive_mode::file:
								if (m_headers_parser.transfer_encoding() == headers_parser::transfer_encoding_type::chunked)
									m_data_receiver = std::make_unique<http::chunked_data_receiver<utils::data::receiver_file<http_data_t>>>(m_headers_parser.content_length(), m_file_path);
								else
									m_data_receiver = std::make_unique<utils::data::receiver_file<http_data_t>>(m_headers_parser.content_length(), m_file_path);
								break;

							case receive_mode::memory_full_payload:
							{
								if (m_headers_parser.transfer_encoding() == headers_parser::transfer_encoding_type::chunked)
									m_data_receiver = std::make_unique<http::chunked_data_receiver<utils::data::receiver_memory<http_data_t>>>();
								else
								{
									assert(m_headers_parser.content_length() >= 0);
									m_data_receiver = std::make_unique<utils::data::receiver_memory<http_data_t>>(m_headers_parser.content_length());
								}
								break;
							}
						}
					}
				}

				if (on_receive) // Pass control to the user callback
				{
					switch (m_receive_mode)
					{
						case receive_mode::memory_tcp_packet:
							PROFILE_TIME(return on_receive(m_headers_parser.headers(), payload, payload_size, m_headers_parser.status()));
							break;

						case receive_mode::memory_full_payload:
						case receive_mode::file:
							if (m_data_receiver)
							{
								m_data_receiver->receive(payload, payload_size);
								auto error = m_data_receiver->error_code();
								if (error > 0)
								{
									LOG_ERROR("Data receiver error: " << error << ": '" << m_data_receiver->error_message() << "'");
									notify(erc::data_receiver_error);
									return false;
								}
								if (m_data_receiver->full())
									PROFILE_TIME(return on_receive(
											m_headers_parser.headers()
											, m_data_receiver->data()
											, m_data_receiver->size()
											, m_headers_parser.status()
										)
									);
							}
							break;
					}
				}

				notify(erc::no_error);
				return true;
			});
		}
		else
		{
			m_error_code = erc::connection_error;
		}
	}

	void http_client::wait() {
		LOG_DEBUG("Wait end of response...");
		m_cv.wait(m_cv_ul);
		LOG_DEBUG("Response received.");
		if (m_client->is_connected())
			m_client->disconnect();
	}

	

	void http_client::query_async(
		const tcp::endpoint_t& endpoint,
		const std::string& method,
		const std::string& query,
		const http_response_cb& on_receive,
		const headers_t& headers,
		const std::string& body
	)
	{
		std::string req = utils::format_str(
			"%s %s HTTP/1.1\r\n"
			, utils::str_toupper(method).c_str()
			, query.c_str()
		);
		std::map<std::string, std::string> def_headers{
			{ "Connection", "keep-alive" },
			{ "Host", endpoint.host }
		};

		auto add_header = [&](const std::string& name, const std::string& value) {
			req += header({ name, value }).to_string();
		};

		for (auto&& h : headers.data())
		{
			add_header(h.first, h.second.value);
			auto it = def_headers.find(h.first);
			if (it != def_headers.end())
				def_headers.erase(it);
		}

		for (auto&& [n, v] : def_headers)
			add_header(n, v);
		
		req += "\r\n";
		req += body;

		request_async(
			endpoint
			, req
			, on_receive
		);
	}

	int http_client::notify(int ec)
	{
		on_before_notify(ec);
		m_error_code = ec;
		m_cv.notify_one();
		on_notify(ec);
		return m_error_code;
	}

	void http_client::set_receive_file(const fs::path &file_path)
	{
		m_receive_mode = receive_mode::file;
		m_file_path = file_path;
	}

	bool http_client::remove_received_file()
	{
		if (!m_file_path.empty())
			return utils::file::remove(m_file_path);
		return false;
	}

	void http_client::reset(client_type c)
	{
		LOG_DEBUG("http_client::reset()");
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
		m_data_receiver.reset();
		m_headers_parser.reset();
		// TODO: clean this too
		//m_file_path.clear();
		//receive_mode m_receive_mode = receive_mode::memory_full_payload;
		on_reset();
	}
}