// http_client_interface.cpp

#include <http/http_client_interface.h>

namespace anp
{
	int http_client_interface::query(
		const tcp::endpoint_t& endpoint,
		const query_t& q,
		const http_response_cb& on_receive
	)
	{
		return query(endpoint, q.method, q.uri(), on_receive, q.headers, q.body);
	}

	void http_client_interface::query_async(
		const tcp::endpoint_t& endpoint,
		const query_t& query,
		const http_response_cb& on_receive
	)
	{
		query_async(endpoint, query.method, query.uri(), on_receive, query.headers, query.body);
	}

	int http_client_interface::query(
		const tcp::endpoint_t& endpoint,
		const std::string& method,
		const std::string& query,
		const http_response_cb& on_receive,
		const http::headers_t& headers,
		const std::string& body
	)
	{
		query_async(endpoint, method, query, on_receive, headers, body);
		wait();
		return errcode();
	}
}