﻿// http_client.h

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
#include "http_client_interface.h"

namespace anp
{
	class http_client : public virtual http_client_interface
	{
	public:
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

		http_client();
		~http_client();

		int request(
			const endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		) override;

		void request_async(
			const endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		) override;

		int errcode() override {
			return m_error_code.load();
		}

		void query_async(
			const endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const headers_t& m_headers = headers_t(),
			const std::string& body = ""
		) override;
		
		void wait() override;

		int notify(int ec) override;

		void set_receive_file(const fs::path& file_path) override;

		const fs::path& get_file_path() const override {
			return m_file_path;
		}

	protected:
		std::string parse_header(const std::string& response, const std::string& header);

	protected:
		void reset() override;
		
	protected:
		std::unique_ptr<utils::data::receiver_base<http_data_t>> m_data_receiver;

	private:
		std::atomic<int> m_error_code = erc::unknown;
		std::mutex m_cv_mtx;
		std::unique_lock<std::mutex> m_cv_ul;
		std::condition_variable m_cv;
		std::unique_ptr<tcp::client> m_client;
		headers_parser m_headers_parser;
		receive_mode m_receive_mode = receive_mode::memory_full_payload;
		fs::path m_file_path;
	};
}
