// http_client.h

#pragma once

#include <memory>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <tcp/fwd.h>
#include <common/common.h>
#include <utils/data_receiver_base.h>
#include "headers_parser.h"
#include "query.h"

namespace anp
{
	class http_client
	{
	public:
		using data_t = char;
		using http_response_cb = std::function<bool(
			const headers_t&		// HTTP Headers
			, const data_t*			// Buffer
			, std::size_t			// Size
			, int					// HTTP Status
		)>;

		enum erc : int
		{
			unknown = -1,
			no_error = 0,
			connection_error,
			connection_process_error,
			parse_headers_error,
			count
		};

		enum receive_mode : int
		{
			memory_full_payload,
			memory_tcp_packet,
			file
		};

		struct endpoint_t
		{
			std::string host;
			int port;
		};

		http_client();
		~http_client();

		int query(
			const endpoint_t& endpoint,
			const query_t& query,
			const http_response_cb& on_receive = http_response_cb()
		);

		void query_async(
			const endpoint_t& endpoint,
			const query_t& query,
			const http_response_cb& on_receive = http_response_cb()
		);

		int request(
			const endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		);

		void request_async(
			const endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		);

		int errcode() {
			return m_error_code.load();
		}

		// Alternative interface
		int query(
			const endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const headers_t& m_headers = headers_t(),
			const std::string& body = ""
		);

		void query_async(
			const endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const headers_t& m_headers = headers_t(),
			const std::string& body = ""
		);
		// End of Alternative interface
		
		void wait();

		int notify(int ec);

		void set_receive_file(const std::string& file_path);

	protected:
		std::string parse_header(const std::string& response, const std::string& header);

	protected:
		virtual void on_before_notify(int ec) {};
		virtual void on_notify(int ec) {};
		void reset();
		virtual void on_reset() {};

	private:
		std::atomic<int> m_error_code = erc::unknown;
		std::mutex m_cv_mtx;
		std::unique_lock<std::mutex> m_cv_ul;
		std::condition_variable m_cv;
		std::unique_ptr<tcp::client> m_client;
		headers_parser m_headers_parser;
		receive_mode m_receive_mode = receive_mode::memory_full_payload;
		std::string m_file_path;
		std::unique_ptr<utils::data::receiver_base<data_t>> m_data_receiver;
	};
}
