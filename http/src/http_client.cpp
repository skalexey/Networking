// http_client.cpp

#include <sstream>
#include <exception>
#include <string_view>
#include <map>
#include <string>
#include <utils/log.h>
#include <utils/string_utils.h>
#include <utils/profiler.h>
#include <utils/data_receiver_file.h>
#include <utils/data_receiver_memory.h>
#include <http/chunked_data_receiver.h>
#include <http/http_client.h>

LOG_TITLE("http_client");
SET_LOCAL_LOG_LEVEL(debug);

namespace anp
{
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

	bool http_client::on_packet_receive(const std::vector<http_data_t>& data, std::size_t sz, int id)
	{
		const http_data_t* payload = data.data();
		std::size_t payload_size = sz;

		if (m_headers_parser.error_code() <= headers_parser::in_progress)
		{
			std::string_view s(&*data.begin(), sz);
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
				switch (get_receive_mode())
				{
					case receive_mode::file:
						try
						{
							if (m_headers_parser.transfer_encoding() == headers_parser::transfer_encoding_type::chunked)
								m_data_receiver = std::make_unique<http::chunked_data_receiver<utils::data::receiver_file<http_data_t>>>(m_headers_parser.content_length(), get_file_path());
							else
								m_data_receiver = std::make_unique<utils::data::receiver_file<http_data_t>>(m_headers_parser.content_length(), get_file_path());
						}
						catch (const std::exception& ex)
						{
							LOG_ERROR("Failed to create a data receiver of type " << get_receive_mode() << ". Content length: " << m_headers_parser.content_length() << ". File path: '" << get_file_path() << "'");
						}
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

		bool result = true;
		bool response_received = false;
		const http_data_t* received_data = nullptr;
		std::size_t received_data_size = 0;

		switch (get_receive_mode())
		{
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
					{
						response_received = true;
						received_data = m_data_receiver->data();
						received_data_size = m_data_receiver->size();
					}
				}
				else
				{
					// We don't accumulate data in the case of no data receiver, so return on the first received TCP packet.
					// TODO: check if the server closes the connection automatically and implement auto response on connection close.
					response_received = true;
				}
				if (response_received)
				{
					if (auto on_response = get_on_response()) // Pass control to the user callback
						PROFILE_TIME(result = on_response(
							get_headers()
							, received_data
							, received_data_size
							, get_status()
						)
						);
					if (errcode() == erc::unknown) // User may already called notify
						notify(result ? erc::no_error : erc::user_error);
					return result;
				}
				break;
			default:
				break;
		}
		return true;
	}

	void http_client::query_async(
		const tcp::endpoint_t& endpoint,
		const std::string& method,
		const std::string& query,
		const http_response_cb& on_receive,
		const http::headers_t& headers,
		const anp::sock_data_t& body
	)
	{
		std::string req = utils::format_str(
			"%s %s HTTP/1.1\r\n"
			, utils::str_toupper(method).c_str()
			, query.c_str()
		);

		std::map<std::string, std::string> def_headers{
			{ "Host", endpoint.host },
			{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Gecko/20100101 Firefox/120.0" },
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
			// { "Accept-Encoding", "gzip, deflate, br" },
			{ "Accept-Language", "en-US,en;q=0.5" },
			// { "Origin", "https://" + endpoint.host },
			{ "Connection", "keep-alive" },

		};

		auto add_header = [&](const std::string& name, const std::string& value) {
			req += http::header({ name, value }).to_string();
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

		anp::sock_data_t req_bin;
		req_bin.insert(req_bin.end(), req.begin(), req.end());
		req_bin.insert(req_bin.end(), body.begin(), body.end());

		request_async(
			endpoint
			, req_bin
			, on_receive
		);
	}

	void http_client::reset(client_type c)
	{
		LOG_DEBUG("http_client::reset()");
		m_data_receiver.reset();
		m_headers_parser.reset();
		base::reset(c);
	}
}