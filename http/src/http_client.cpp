// http_client.cpp

#include <exception>
#include <string_view>
#include <map>
#include <string>
#include <tcp/client.h>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <utils/profiler.h>
#include <tcp/client.h>
#include "http_client.h"

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
		, m_client(std::make_unique<tcp::client>())
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
		const endpoint_t& endpoint
		, const std::string& request
		, const http_response_cb& on_receive
	)
	{
		request_async(endpoint, request, on_receive);
		wait();
		return m_error_code;
	}

	void http_client::request_async(
		const endpoint_t& endpoint
		, const std::string& request
		, const http_response_cb& on_receive
	)
	{
		reset();

		m_client->set_on_close([&] {
			notify(m_error_code);
		});

		m_client->set_on_connect([=, this](const std::error_code& ec) {
			if (!ec)
			{
				LOG_DEBUG("Send...");
				m_client->send(request);
			}
			else
			{
				LOG_ERROR("Error during connection.");
				notify(connection_process_error);
			}
		});


		if (m_client->connect(endpoint.host, endpoint.port))
		{
			LOG_DEBUG("Set on_receive task");
			m_client->set_on_receive([=](
				const std::vector<data_t>& data
				, std::size_t sz
				, int id
			) -> bool {
					LOG_DEBUG("\nReceived packet " << id << " with " << sz << " bytes:");
					std::string_view s(data.begin(), data.begin() + sz);
					LOG_DEBUG(s);

					const data_t* payload = data.data();
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
									m_data_receiver = std::make_unique<data_receiver_file<data_t>>(m_headers_parser.content_length());
									break;

								case receive_mode::memory_full_payload:
									m_data_receiver = std::make_unique<data_receiver_memory<data_t>>(m_headers_parser.content_length());
									break;
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
									if (m_data_receiver->receive(payload, payload_size))
										PROFILE_TIME(return on_receive(
												m_headers_parser.headers()
												, m_data_receiver->data().data()
												, m_data_receiver->data().size()
												, m_headers_parser.status()
											)
										);
								break;
						}
					}
					
				return true;
				}
			);
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

	int http_client::query(
		const endpoint_t& endpoint,
		const std::string& method,
		const std::string& query,
		const http_response_cb& on_receive,
		const headers_t& m_headers,
		const std::string& body
	)
	{
		query_async(endpoint, method, query, on_receive, m_headers, body);
		wait();
		return m_error_code;
	}

	void http_client::query_async(
		const endpoint_t& endpoint,
		const std::string& method,
		const std::string& query,
		const http_response_cb& on_receive,
		const headers_t& m_headers,
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

		for (auto&& h : m_headers.data)
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

	void http_client::reset()
	{
		LOG_DEBUG("http_client::reset()");
		m_error_code = erc::unknown;
		m_client = std::make_unique<anp::tcp::client>();
		m_data_receiver.reset();
		m_headers_parser.reset();
		on_reset();
	}
	
	// Begin of headers_t
	std::string http_client::headers_t::to_string() const
	{
		std::string r;
		for (auto&& h : m_data)
			r += h.second.to_string();
		return r;
	}

	http_client::headers_t::operator std::string() const
	{
		return to_string();
	}

	void http_client::headers_t::add(const http_client::header& h)
	{
		m_data.add(h.name, h);
	}

	const std::string& http_client::headers_t::get(const std::string& name) const
	{
		if (auto it = m_data.find(name))
			return (*it).second.value;
		return empty_string;
	}
	// End of headers_t

	// Begin of header
	std::string http_client::header::to_string() const
	{
		return utils::format_str("%s: %s\r\n", name.c_str(), value.c_str());
	}

	http_client::header::operator std::string() const
	{
		return to_string();
	}
	// End of header
	
	int http_client::headers_parser::parse(const std::string_view& s)
	{
		auto on_content_length_changed = [=] {
			if (!m_content_length.has_value())
				return false;
			if (m_content_length.value() == 0)
			{
				m_content_length.reset();
				return false;
			}
			// Download is going to begin
			return true;
		};

		auto on_headers_received = [&] {
			auto& sc = m_headers.get("Content-Length");
			if (!sc.empty())
			{
				try
				{
					m_content_length = std::atoi(sc.c_str());
				}
				catch (...)
				{
					LOG_ERROR("Can't parse content length '" << sc << "'");
					return erc::parse_size_error;
				}

				if (!on_content_length_changed())
				{
					return erc::receive_size_error;
				}
			}
			else
			{
				LOG_ERROR("No content length received from the server");
				return erc::no_content_length;
			}
			return erc::done;
		};

		m_cursor = 0; // Cursor

		do
		{
			// Parse headers
			auto p = s.find("\r\n", m_cursor); // Header end position
			if (p != std::string::npos)
			{
				if (p == 0 && m_last_data.empty())
				{
					// Headers block finished
					if (auto r = on_headers_received())
					{
						LOG_DEBUG("Headers parsing error: " << r);
						return m_error_code = r;
					}
					m_cursor = p + 2;
					m_last_data.clear();
					return erc::done;
				}
				else
				{
					// Header
					std::string_view h(s.begin() + m_cursor, s.begin() + p);
					auto colon_p = h.find_first_of(":", m_cursor);
					if (colon_p != std::string::npos)
					{
						auto n = h.substr(0, colon_p); // Header name
						auto v = h.substr(colon_p);
						m_headers.add({ n, v });
					}
					else
					{
						// Parse HTTP status
						auto hp = h.find("HTTP/1");
						if (hp != std::string::npos)
						{
							auto sp = h.find_first_of(' '); // Begin of status
							if (sp != std::string::npos)
							{
								auto spe = h.find_first_of(' ', sp + 1);// End of status
								if (spe != std::string::npos)
								{
									try
									{
										m_status = std::atoi(h.substr(sp + 1, spe - sp).data());
									}
									catch (const std::exception& ex)
									{
										LOG_ERROR("Can't parse HTTP status");
										return m_error_code = erc::status_parse_error;
									}
								}
							}
						}
					}
					m_cursor = p + 2;
				}
			}
			else
			{
				m_last_data = s.substr(m_cursor);
				return erc::in_progress;
			}
		} while (true);

		assert(false, "Should not be here");
		return erc::count;
	}

	void http_client::headers_parser::reset()
	{
		m_error_code = erc::initial;
		m_cursor = 0;
		m_status = -1;
		m_content_length.reset();
		m_headers.clear();
		m_last_data.clear();
	}
}