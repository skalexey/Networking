// http_client_boost.cpp

#include <istream>
#include <sstream>
#include <exception>
#include <string_view>
#include <map>
#include <string>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <utils/profiler.h>
#include <utils/data_receiver_file.h>
#include <utils/data_receiver_memory.h>
#include <boost/beast.hpp>
#include <boost/beast/core/ostream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <http/chunked_data_receiver.h>
#include <tcp/client.h>
#include <tcp/ssl/client.h>
#include <http/http_client_boost.h>

LOG_PREFIX("[http_client_boost]: ");
LOG_POSTFIX("\n");
SET_LOCAL_LOG_DEBUG(true);

namespace beast = boost::beast;     // from <boost/beast.hpp>
//namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>

namespace
{
	const std::string empty_string;
}

namespace anp
{
	http_client_boost::~http_client_boost()
	{
		LOG_DEBUG("http_client_boost::~http_client_boost");
	}

	bool http_client_boost::on_packet_receive(const std::vector<http_data_t>& data, std::size_t sz, int id)
	{
		const http_data_t* payload = data.data();
		std::size_t payload_size = sz;
		
		// TODO: reduce copying
		std::istringstream response_stream(std::string(data.data(), sz));
		// TODO: implement
		//response_stream >> m_response;

		// if (m_response.)
		m_status = m_response.result_int();
		// TODO: implement receive finish check. Maybe just check if the status is 200?
		const http_data_t* body_data = nullptr;// TODO: make this work: m_response.body().cdata();
		if (auto on_response = get_on_response())
			return on_response(get_headers(), body_data, m_response.body().size(), get_status());

		return true;
	}

	void http_client_boost::query_async(
		const tcp::endpoint_t& endpoint,
		const std::string& method,
		const std::string& query,
		const http_response_cb& on_receive,
		const http::headers_t& headers,
		const anp::sock_data_t& body
	)
	{
		int version = 11;
			
		beast::http::request<beast::http::string_body> beast_req{utils::str_tolower(method) == "get" ? beast::http::verb::get : beast::http::verb::post, query, version};
        beast_req.set(beast::http::field::host, endpoint.host);
        beast_req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		//beast_req
		//beast::http::write(get_ssl_soc(), beast_req);
		std::map<std::string, std::string> def_headers{
			{ "Host", endpoint.host },
			{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Gecko/20100101 Firefox/120.0" },
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
			// { "Accept-Encoding", "gzip, deflate, br" },
			{ "Accept-Language", "en-US,en;q=0.5" },
			// { "Accept-Encoding", "gzip, deflate, br" },
			// { "Origin", "https://" + endpoint.host },
			{ "Connection", "keep-alive" },
			// { "Referer", "https://srv.vllibrary.net/s/j.php" },

		};

		auto add_header = [&](const std::string& name, const std::string& value) {
			// TODO: beastify
			//beast_req.add_header(name, value);
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
		
		// TODO: support binary data
		//std::ostringstream oss;
		//oss << beast_req;
		//std::string req = oss.str();
		//LOG_DEBUG("Beast request: '" << req << "'");
		//oss.clear();

		//request_async(
		//	endpoint
		//	, req
		//	, on_receive
		//);
	}

	void http_client_boost::reset(client_type c)
	{
		LOG_DEBUG("http_client_boost::reset()");
		base::reset(c);
	}
}