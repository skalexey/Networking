// http_client_20.cpp

#include <string_view>
#include <map>
#include <string>
#include <utils/log.h>
#include <utils/string_utils.h>
#include <utils/profiler.h>
//#include <http/http_client_20.h>

LOG_TITLE("http_client_20");
SET_LOCAL_LOG_LEVEL(debug);

namespace anp
{
	//http_client_20::~http_client_20()
	//{
	//	LOG_DEBUG("http_client_20::~http_client_20");
	//}

	//void http_client::query_async(
	//	const tcp::endpoint_t& endpoint,
	//	const std::string& method,
	//	const std::string& query,
	//	const http_response_cb& on_receive,
	//	const headers_t& headers,
	//	const anp::sock_data_t& body
	//)
	//{
	//	std::string req = utils::format_str(
	//		"PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n"
	//		"\0\0\0\4\0\0\0\0\0\0\0\0\0\0\0\0"
	//		"%s %s HTTP/2\r\n"
	//		, utils::str_toupper(method).c_str()
	//		, query.c_str()
	//	);

	//	std::map<std::string, std::string> def_headers{
	//		{ "Host", endpoint.host },
	//		{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Gecko/20100101 Firefox/120.0" },
	//		{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
	//		{ "Accept-Language", "en-US,en;q=0.5" },
	//		// { "Accept-Encoding", "gzip, deflate, br" },
	//		// { "Origin", "https://" + endpoint.host },
	//		{ "Connection", "keep-alive" },

	//	};

	//	auto add_header = [&](const std::string& name, const std::string& value) {
	//		req += header({ name, value }).to_string();
	//	};

	//	for (auto&& h : headers.data())
	//	{
	//		add_header(h.first, h.second.value);
	//		auto it = def_headers.find(h.first);
	//		if (it != def_headers.end())
	//			def_headers.erase(it);
	//	}

	//	for (auto&& [n, v] : def_headers)
	//		add_header(n, v);
	//	
	//	req += "\r\n";
	//	req += body;

	//	request_async(
	//		endpoint
	//		, req
	//		, on_receive
	//	);
	//}
}