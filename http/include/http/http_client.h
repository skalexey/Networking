// http_client.h

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <tcp/fwd.h>
#include <anp/common.h>
#include <utils/data_receiver_base.h>
#include <http/headers_parser.h>
#include <http/http_client_base.h>

namespace anp
{
	class http_client : public http_client_base
	{
		using base = http_client_base;

	public:
		enum erc : int
		{
			parse_headers_error = int(http_client_base::erc::erc_count),
			data_receiver_error,
			erc_count
		};

		~http_client();
		
		void query_async(
			const tcp::endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const http::headers_t& m_headers = http::headers_t(),
			const std::string& body = ""
		) override;

		// MSVC fix.
		// This function is actually inherited from http_client_interface and should be visible
		void query_async(
			const tcp::endpoint_t& endpoint,
			const query_t& query,
			const http_response_cb& on_receive = http_response_cb()
		) override {
			base::query_async(endpoint, query, on_receive);
		}

	// http_client_base overrides
	protected:
		bool on_packet_receive(const std::vector<http_data_t>& data, std::size_t sz, int id) override;
		const http::headers_t& get_headers() const override {
			return m_headers_parser.headers();
		}
		int get_status() const override {
			return m_headers_parser.status();
		}

	// http_client own interface
	protected:
		std::string parse_header(const std::string& response, const std::string& header);

	// http_client_interface overrides
	protected:
		void reset(client_type c = client_type::http) override;
		
	// Data
	protected:
		std::unique_ptr<utils::data::receiver_base<http_data_t>> m_data_receiver;

	private:
		headers_parser m_headers_parser;
	};
}
