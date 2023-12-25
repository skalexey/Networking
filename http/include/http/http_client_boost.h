// http_client_boost.h

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <boost/beast/http.hpp>
#include <tcp/fwd.h>
#include <anp/common.h>
#include <http/headers.h>
#include <utils/data_receiver_base.h>
#include "headers_parser.h"
#include "http_client_base.h"

namespace anp
{
	class http_client_boost : public virtual http_client_base
	{
		using base = http_client_base;

	public:
		enum erc : int
		{
			beast_error = http_client_base::erc::erc_count,
			count
		};

		~http_client_boost();

		void query_async(
			const tcp::endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const http::headers_t& m_headers = http::headers_t(),
			const anp::sock_data_t& body = {}
		) override;

	// http_client_base overrides
	protected:
		bool on_packet_receive(const std::vector<http_data_t>& data, std::size_t sz, int id) override;
		const http::headers_t& get_headers() const override {
			return m_headers;
		}
		int get_status() const override {
			return m_status;
		}

	// http_client_interface overrides
	protected:
		void reset(client_type c = client_type::http) override;
		
	// Data
	private:
		http::headers_t m_headers;
		int m_status = -1;
		boost::beast::http::response<boost::beast::http::dynamic_body> m_response;
	};
}
